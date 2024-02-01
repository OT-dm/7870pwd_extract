7870pwd_extract is utility to extract password from vendor specific portion(file AllAppUpdateA13.bin) of FYT UIS 7870 firmware.

## 1. Build.

### 1.1 Pre-requisites:
- Linux 64bit (tested on WSL2 instance of Ubuntu 22.04)
- gcc/g++ 11 or higher (tested with 11)
- gnu make
- openssl (tested with openssl3) + openssl dev package (sudo apt install libssl-dev)
- minizip (sudo apt install libminizip-dev)

Also includes ed25519 implementation from https://github.com/orlp/ed25519/tree/master


### 1.2 Build
```
    cd 7870pwd_extract
    make
```
## 2. Usage.

### 2.1 Run utility.
    
    7870pwd_extract -z <path_to_AllAppUpdateA13.bin> -l <path_to_lsec6318update> [-v]
    
    Where -v is optional parameter which enables verbose mode

Example:
```
./7870pwd_extract -z /mnt/d/tmp/7870_2000x1200_20240111_LiJun/AllAppUpdateA13.bin -l /mnt/d/tmp/7870_2000x1200_20240111_LiJun/lsec6318update
password: b8eb271f597885f48b8d237259aa72d0
```

### 2.2 Test that password good with your favorite archiver.

Example:
```
7z t /mnt/d/tmp/7870_2000x1200_20240111_LiJun/AllAppUpdateA13.bin -p b8eb271f597885f48b8d237259aa72d0
Scanning the drive for archives:
1 file, 641257863 bytes (612 MiB)

Testing archive: /mnt/d/tmp/7870_2000x1200_20240111_LiJun/AllAppUpdateA13.bin
--
Path = /mnt/d/tmp/7870_2000x1200_20240111_LiJun/AllAppUpdateA13.bin
Type = zip
Physical Size = 641257863
Comment = |c\'Ë+¹EÞxÌÅò´òÀ?»wØ+´=wÐ'

No files to process
Everything is Ok

Files: 0
Size:       0
Compressed: 641257863
```
