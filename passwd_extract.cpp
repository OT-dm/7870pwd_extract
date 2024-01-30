#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <minizip/unzip.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include "ed25519/src/ed25519.h"

#include "passwd_extract.h"
#include "cmd_line.h"

using namespace std;

namespace
{

//https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
int decrypt(const unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext)
{

   if(get_cmd_line_params().verbose)
   {
      cout << "ciphertext: "; print_hex(ciphertext,ciphertext_len);
      cout << "ciphertext_len: " << ciphertext_len << endl;
      cout << "key: "; print_hex(key,16);
      cout << "iv: " ; print_hex(iv,16);
   }

    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
    {
       throw std::runtime_error("Can't create and initialise the cipher context");
    }
    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
    {
        throw std::runtime_error("Can't initialise the decryption operation");
    }
    EVP_CIPHER_CTX_set_padding(ctx, 0);//disable padding
    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    {
        throw std::runtime_error("Can't decrypt");
    }
    plaintext_len = len;
    if(get_cmd_line_params().verbose)
    {
      cout << "plaintext: "; print_hex(plaintext,plaintext_len);
      cout << "plaintext as text: "; cout << string((const char*)plaintext,plaintext_len);
    }

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
    {
        throw std::runtime_error("Can't finalize decryption");
    }
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

//private key hardcoded in lsec6318update, replaced with search in binary - may help if key changed in future
/*const unsigned char private_key_1[]={0xd0,0x4c,0x24,0x90,0xc9,0x5b,0xf6,0x39,0xa8,0x20,0x22,0x2d,0xa7,0x0f,0x05,0xf7,
                                     0x76,0xa5,0x4e,0xa9,0xe5,0x91,0x21,0x98,0x17,0x8b,0x72,0xea,0xdd,0x5a,0xb0,0x62,
                                     0x3d,0x59,0xe2,0x3e,0x62,0x7f,0x2e,0x5b,0x0e,0x04,0x68,0xf4,0xa6,0x9e,0x5b,0xdb,
                                     0x36,0x85,0x2b,0x82,0x58,0x2b,0xa7,0xa1,0xc8,0x77,0x32,0x62,0xe0,0x25,0xbf,0xcf};
*/
void extract_priv_key(const string& lsec6318path, unsigned char * private_key, size_t private_key_size )
{
   //nice pattern in lsec6318update just after private key!!
   const unsigned char pattern[]={0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x08,0x00,0x00,0x00,
                                  0x10,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x80,0x00,0x00,0x00};
   
   const size_t buffer_size=8*1024*1024;
   unique_ptr<unsigned char[]> buffer(new unsigned char[buffer_size]);

   ifstream lsec_file(lsec6318path, ifstream::binary);
   lsec_file.read(reinterpret_cast<char*>(buffer.get()),buffer_size);
   size_t read_bytes=lsec_file.gcount();
   lsec_file.close();

   unsigned char * pattern_buf_ptr = search( buffer.get(), buffer.get()+read_bytes, pattern, pattern+sizeof(pattern) );

   if( pattern_buf_ptr && buffer.get()<(pattern_buf_ptr-private_key_size) )
   {
      copy(pattern_buf_ptr-private_key_size,pattern_buf_ptr,private_key);
   }
   else
   {
      throw std::runtime_error("Private key not found in: " + lsec6318path);
   }   
   if(get_cmd_line_params().verbose)
   {
      cout << "Private key found in: " << lsec6318path << endl;
      print_hex(private_key,private_key_size);
      //print_hex(private_key_1,sizeof(private_key_1));
   }
}

//uVar2 = GetPasswd(&uStack_e0,uStack_e8 & 0xffffffff,param_1 + 0x10);
void print_passwd (const unsigned char * comment, int length, unzFile zipfile)
{
   unsigned char shared_secret[32] = {0};
   unsigned char private_key[64]={0};
   int ret = 0;

   extract_priv_key(get_cmd_line_params().lsec6318_path,private_key,sizeof(private_key));

   ed25519_key_exchange(shared_secret, comment /*other_public_key*/, private_key);
      
   if(get_cmd_line_params().verbose)
   {
      cout << "shared_secret: ";
      print_hex(shared_secret,32);
   }

   unsigned char iv[16]={0};//initialization vector
   unsigned char decrypted_text[1024] = "";
   int decrypted_len = 0;

   decrypted_len = decrypt(comment+0x30/*ciphertext*/, length-0x30/*ciphertext_len*/, shared_secret/*key*/, iv, decrypted_text);
   decrypted_text[decrypted_len] = '\0';

   if(get_cmd_line_params().verbose)
   {
      cout << "decrypted_password = <" << decrypted_text << ">" << endl;
   }
   
   ret = 0;
   if( (ret = ed25519_verify(decrypted_text+48,decrypted_text+16,32,comment)) != 0) //verify digital signature on decrypted text
   {
      if(get_cmd_line_params().verbose)
      {
         cout << "ed25519_verify signature of decrypted text successful" << endl;
      }
      cout << "password: " << string(reinterpret_cast<const char*>(decrypted_text)+16,32) << endl;
   }
   else
   {
      cout << "ed25519_verify signature failed ret=<" << ret << "> raw decrypted text =<" << decrypted_text << ">" << endl;
   }
}
}; //namespace

void print_hex(const unsigned char* byte_str, int len)
{
   const char szHexDigits[] = "0123456789abcdef" ;

   string hex;
   hex.reserve(len*2);

   int i = 0;
   for (const unsigned char* it = byte_str; i < len ; it++, i++)
   {
      hex+= szHexDigits[*it >> 4];
      hex+= szHexDigits[*it & 0x0F];
   }
   cout <<hex<< endl;
}

void print_passwd (const string & zip_path)
{
   unzFile zipfile = unzOpen64( zip_path.c_str() );
   if ( zipfile == NULL )
   {
        throw std::runtime_error("Cant open zip file " + zip_path);
   }

   try
   {
      char comment[1025] = "";
      int comment_len = 0;
      if ((comment_len = unzGetGlobalComment(zipfile, comment, sizeof(comment))) >= 0)
      {
         if (get_cmd_line_params().verbose)
         {
            cout << "zip global comment text =<" << comment << "> comment_len =<" << comment_len << ">" << endl;
            cout << "zip global comment HEX: ";
            print_hex(reinterpret_cast<const unsigned char *>(comment), comment_len);
         }
         print_passwd(reinterpret_cast<const unsigned char *>(comment), comment_len, zipfile);
      }
      else
      {
         cout << "Cant get global comment from zip file " << zip_path << " comment_len:" << std::to_string(comment_len) << endl;
      }
   }
   catch(...)
   {
     unzClose(zipfile);
     throw;   
   }
   unzClose(zipfile);
}
