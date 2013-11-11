@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem
@echo off
@echo This script requries OpenSSL which may be freely downloaded from www.openssl.org
pause

rem Create the files
..\dergen.pl --in pkcs12simple.txt				--out pkcs12simple.p12
..\dergen.pl --in pkcs12_certonly.txt 			--out pkcs12_certonly.p12
..\dergen.pl --in pkcs12_crl.txt 				--out pkcs12_crl.p12
..\dergen.pl --in pkcs12_keyonly.txt 			--out pkcs12_keyonly.p12
..\dergen.pl --in pkcs12_nomaciter.txt 	 		--out pkcs12_nomaciter.p12
..\dergen.pl --in pkcs12_secret.txt 		 	--out pkcs12_secret.p12
..\dergen.pl --in pkcs12_twocerts.txt		 	--out pkcs12_twocerts.p12
..\dergen.pl --in pkcs12_unencrypted.txt 		--out pkcs12_unencrypted.p12

rem Parse the files using openssl
openssl pkcs12 -info -nodes -password pass:password -in pkcs12simple.p12 		> pkcs12simple.out 2<&1
openssl pkcs12 -info -nodes -password pass:password -in pkcs12_certonly.p12 	> pkcs12_certonly.out 2<&1
openssl pkcs12 -info -nodes -password pass:password -in pkcs12_crl.p12 			> pkcs12_crl.out 2<&1
openssl pkcs12 -info -nodes -password pass:password -in pkcs12_keyonly.p12 		> pkcs12_keyonly.out 2<&1
openssl pkcs12 -info -nodes -password pass:password -in pkcs12_nomaciter.p12 	> pkcs12_nomaciter.out 2<&1
openssl pkcs12 -info -nodes -password pass:password -in pkcs12_secret.p12 		> pkcs12_secret.out 2<&1
openssl pkcs12 -info -nodes -password pass:password -in pkcs12_twocerts.p12 	> pkcs12_twocerts.out 2<&1
openssl pkcs12 -info -nodes -password pass:password -in pkcs12_unencrypted.p12 	> pkcs12_unencrypted.out 2<&1

rem Create files that are not supported by OpenSSL
..\dergen.pl --in pkcs12_nested.txt				--out pkcs12_nested.p12
..\dergen.pl --in pkcs12_publickeyintegrity.txt --out pkcs12_publickeyintegrity.p12
