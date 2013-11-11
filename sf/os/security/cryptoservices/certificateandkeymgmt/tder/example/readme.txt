The following example creates a PKCS#12 file that is identical to a reference
file created using OpenSSL.


Limitations
***********
1) There is no support for the PKCS#12 PBE KDF. The derived keys must be calculated externally.

*** OpenSSL must be in your path for the example to work. ***

Reference PKCS12 file (root5ca.p12) details
******************************************
certificate: root5ca.pem
key: root5ca_key.pem
commands line: openssl pkcs12 -export -in root5ca.pem -inkey root5ca_key.pem -out root5ca.p12 -name "Root5CA" -password pass:password
password: password

The salt was extracted from root5ca.p12 and the iv and key were re-created using
the PKCS12 KDF. These crypto params could be re-used to create other examples
so long as the same ciphers are used.

Certificate
-----------
salt: 07EE43282EFFF9D4	
iter: 2048
key: 44f58eebbf
iv:	2b4076b4b266f90b

Private Key
-----------
salt: 17D32E39CDF9CCD6
iter: 2048
key: 3abe08c2e6a4782090258b75936f730c81165ad16a968401	
iv:	d23d26d5f997e21f

Mac
---
salt:  6F855D153C618D4C
iter: 2048
key: d9c9d4803178c611251ccefef608bda2bf7ec183

Running the example
*******************
From the example directory run
..\dergen.pl --binary --in pkcs12simple.txt --out pkcs12simple.p12

Viewing the data
openssl pkcs12 -in pkcs12simple.p12 -info -nodes -password pass:password

or double-click on the file in Explorer

Instructions for DERGEN.pl
**************************
see ..\syntax.txt