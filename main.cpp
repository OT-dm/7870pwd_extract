#include <stdexcept>
#include <string>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <minizip/unzip.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include "ed25519/src/ed25519.h"

#include "cmd_line.h"
#include "passwd_extract.h"

using namespace std;

int main(int argc, const char* argv[])
try
{
   process_cmd_line(argc,argv);

   print_passwd(get_cmd_line_params().zip_path);
   
   return 0;
}
catch(const std::exception & exc)
{
   cout << exc.what() << endl;
   throw;
}
catch(...)
{
   return 1;
}