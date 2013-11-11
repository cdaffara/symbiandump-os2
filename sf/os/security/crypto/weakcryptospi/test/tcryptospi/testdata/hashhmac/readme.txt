*********************************
"HashGen.pl" README DOCUMENTATION

Paul Sinfield
v1.0
28/02/2007
*********************************

The hashgen.pl script generates Hash and Hmac expected Hexadecimal string values for a given source file and the specified secret key.

The following hash algorithms are supported:

HASH: MD2/MD4/MD5/SHA-1
HMAC: MD2/MD4/MD5/SHA-1


1. Before running the script file, please ensure that the following files have been copied into the "c:\apps\perl\site\lib\digest" directory:

HMAC-MD2.pm
HMAC-MD4.pm


2. The script takes 2 arguments as follows:

hashgen.pl <Source File Path> <Key>

e.g. hashgen.pl "C:\testdata\tcryptospi\data\LargeHash-SRC.dat" CryptoSpiValidKey

NOTE: Please ensure that speech marks ("") are placed around arguments that have spaces in them, such as file paths.


3. Once the script has been run, the results will be output a text file with the same name as the source but with a suffix "*_HASHES.txt", within the same directory.

e.g. LargeHash-SRC_HASHDATA.txt

An example of the files contents are displayed below:

****** HASH MD2/MD4/MD5/SHA1 ******

MD2: 359df8d2cc91db9c8ec0f61718b65a60
MD4: 826598b8fa06b7deafd255280319e38b
MD5: 201f59b84894f378e1cce73abea86ca6
SHA1: 21ef662c79cb3e972a3bb71937381b26e0daa2fc

****** HMAC MD2/MD4/MD5/SHA1 ******

HMAC-MD2: 50635299e47531b4913abf986c063054
HMAC-MD4: 9e1734fe8dae94d48a9b8fbdd1b55d3a
HMAC-MD5: 0db3d1f04967261a9761140f44ef0c4a
HMAC-SHA1: 101607c5a079b69c439f307e0590af588c9bf7e2

Key: CryptoSpiValidKey


*********************************
"HashGenSha2.pl" README DOCUMENTATION

*********************************

The hashgensha2.pl script generates Hash and Hmac expected Hexadecimal string values for a given source file and the specified secret key for Sha224, Sha256, Sha384 and Sha 512.

Before Running the script file, ensure that openssl is installed onto the system.

The script takes 2 arguments as follows:
hashgensha2.pl <Source File>  <Key>

e.g. hashgen.pl "LargeHash-SRC.dat" CryptoSpiValidKey

3. Once the script has been run, the results will be output a text file with the same name as the source but with a suffix "*_SHA2HASHDATA.txt", within the same directory.

e.g. largehash-src_SHA2HASHDATA.txt

An example of the files contents are displayed below:
****** HASH SHA224/SHA256/SHA384/SHA512 ******

SHA224(largehash-src.dat)= c363a255fc7db36aeecfcfa6f9790ecb75043a0f5ebe8cc79a3c7a6a
SHA256(largehash-src.dat)= 83b7bc6895df985de5543e8cd2b150232a4b9b4ba093c92fec725a924e4279aa
SHA384(largehash-src.dat)= 7785397e9094c917e76c0ed1cfdb59c073f5ec4d29c503c6c8f28bfde1ee46bd43f21ca7f49b754f6cc94ff37e50798c
SHA512(largehash-src.dat)= e93c8160c1a9888d8a03a8ffa97a3682597789b9e5250a0cffa0ac984c85e557356201068ef5fa3e968a7623597ba65c50248dab70233f68e547bbc03e58e441


****** HMAC SHA224/SHA256/SHA384/SHA512 ******

HMAC-SHA224(largehash-src.dat)= e811df3c4ef67a7ef57baffa7e796a5ec2bfec357eb8ef03dabc9c97
HMAC-SHA256(largehash-src.dat)= 7bdcc4bed110ccafd5b152de05af95441bce9a1e608abb7e46b10c2e7cee7dc9
HMAC-SHA384(largehash-src.dat)= 258d6725d583482bca882af8b8a18f9ec2c002047051beda50d447aedb7721ef0c4511c04a71f82641ce865a2e83dd21
HMAC-SHA512(largehash-src.dat)= 38e9456ce22cb3ba6a642bb9ca2d3c8722dad96397da1cfa6ff072eaf8cd661798be8f9b3af7f2b4e930fc4370351f891a2b938cb4d312faae13cccd06fc302c
