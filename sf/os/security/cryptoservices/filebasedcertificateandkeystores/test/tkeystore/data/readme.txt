\keys directory contains the keys generated by OpenSSL
.txt files contain PKCS8 key data generated by OpenSSL
.001 files are the decoded version of that data

file descriptions:

pkcs8rsa.001 - pkcs8rsa2.001 are decoded PKCS8 representations of 512 bit RSA keys
pkcs8rsa3.001 is a decoded PKCS8 representation of a 1024 bit RSA key
pkcs8rsa4.001 is a decoded PKCS8 representation of a 2048 bit RSA key
pkcs8invalidrsa.001 is a decoded PKCS8 representation of a 2049 bit RSA key (too big!)

pkcs8dsa.001 - pkcs8dsa2.001 are decoded PKCS8 representations of 512 bit DSA keys
pkcs8dsa3.001 is a decoded PKCS8 representation of a 1024 bit DSA key
pkcs8dsa4.001 is a decoded PKCS8 representation of a 2048 bit DSA key
pkcs8invaliddsa.001 is a decoded PKCS8 representation of a 2049 bit DSA key (too big!)