@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

REM days default is 30 and generated on 13/03/2008

REM *** SEC-CERTMAN-DATETIME-001 ***
REM - Root Cert

openssl req -newkey rsa:1024 -keyform PEM -keyout datetime_0001_root_key.pem -subj "/O=Symbian/CN=datetime_0001_root" -nodes -config openssl.cfg -out datetime_0001_root_req.pem -outform PEM
openssl x509 -req -inform PEM -in datetime_0001_root_req.pem -signkey datetime_0001_root_key.pem -outform PEM -out datetime_0001_root.pem -days 60
openssl x509 -inform PEM -in datetime_0001_root.pem -outform DER -out datetime_0001_root.der



REM - End Entity

openssl req -newkey rsa:1024 -keyform PEM -keyout datetime_0001_endentity_key.pem -subj "/O=Symbian/CN=datetime_0001_ee" -nodes -config openssl.cfg -out datetime_0001_endentity_req.pem -outform PEM
openssl x509 -req -inform PEM -in datetime_0001_endentity_req.pem -CA datetime_0001_root.pem  -CAkey datetime_0001_root_key.pem -CAcreateserial -outform PEM -out datetime_0001_endentity.pem
openssl x509 -inform PEM -in datetime_0001_endentity.pem -outform DER -out datetime_0001_endentity.der



REM *** SEC-CERTMAN-CRITEXT-001 ***
REM - Root Cert

openssl req -newkey rsa:512 -nodes -out 001_rc.pem -keyout 001_rc.key.pem -config 001_rc.cfg -days 3650
openssl x509 -req -days 3650 -in 001_rc.pem -signkey 001_rc.key.pem -out 001_rc.pem -extfile 001_rc.cfg -extensions v3_ca
openssl x509 -inform PEM -in 001_rc.pem -outform DER -out 001_rc.der


REM - Intermediate 2 Cert

openssl req -newkey rsa:512 -nodes -out 001_im2.pem -keyout 001_im2.key.pem -config 001_im2.cfg -days 3650
openssl x509 -req -in 001_im2.pem -out 001_im2.pem -CA 001_rc.pem -CAkey 001_rc.key.pem -days 3650 -CAserial 001_rc.srl -CAcreateserial -extfile 001_im2.cfg -extensions v3_ca
openssl x509 -inform PEM -in 001_im2.pem -outform DER -out 001_im2.der


REM - Intermediate 1 Cert

openssl req -newkey rsa:512 -nodes -out 001_im1.pem -keyout 001_im1.key.pem -config 001_im1.cfg -days 3650
openssl x509 -req -in 001_im1.pem -out 001_im1.pem -CA 001_im2.pem -CAkey 001_im2.key.pem -days 3650 -extfile 001_im1.cfg -extensions v3_ca -CAserial 001_im2.srl -CAcreateserial 
openssl x509 -inform PEM -in 001_im1.pem -outform DER -out 001_im1.der


REM - End Entity Cert

openssl req -newkey rsa:512 -nodes -out 001_ee.pem -keyout 001_ee.key.pem -config 001_ee.cfg -days 3650
openssl x509 -req -in 001_ee.pem -out 001_ee.pem -CA 001_im1.pem -CAkey 001_im1.key.pem -days 3650 -extfile 001_ee.cfg -extensions v3_ca -CAserial 001_im1.srl -CAcreateserial 
openssl x509 -inform PEM -in 001_ee.pem -outform DER -out 001_ee.der


REM *** SEC-CERTMAN-CRITEXT-002 ***
REM - Root Cert 1

openssl req -newkey rsa:512 -nodes -out 002_rc1.pem -keyout 002_rc1.key.pem -config 002_rc1.cfg -days 3650
openssl x509 -req -days 3650 -in 002_rc1.pem -signkey 002_rc1.key.pem -out 002_rc1.pem
openssl x509 -inform PEM -in 002_rc1.pem -outform DER -out 002_rc1.der


REM - Root Cert 2

openssl req -newkey rsa:512 -nodes -out 002_rc2.pem -keyout 002_rc2.key.pem -config 002_rc2.cfg -days 3650
openssl x509 -req -days 3650 -in 002_rc2.pem -out 002_rc2.pem -CA 002_rc1.pem -CAkey 002_rc1.key.pem -days 3650 -CAserial 002_rc1.srl -CAcreateserial -extfile 002_rc2.cfg -extensions v3_ca
openssl x509 -inform PEM -in 002_rc2.pem -outform DER -out 002_rc2.der


REM - Intermediate 2 Cert

openssl req -newkey rsa:512 -nodes -out 002_im2.pem -keyout 002_im2.key.pem -config 002_im2.cfg -days 3650
openssl x509 -req -in 002_im2.pem -out 002_im2.pem -CA 002_rc2.pem -CAkey 002_rc2.key.pem -days 3650 -CAserial 002_rc2.srl -CAcreateserial -extfile 002_im2.cfg -extensions v3_ca
openssl x509 -inform PEM -in 002_im2.pem -outform DER -out 002_im2.der


REM - Intermediate 1 Cert

openssl req -newkey rsa:512 -nodes -out 002_im1.pem -keyout 002_im1.key.pem -config 002_im1.cfg -days 3650
openssl x509 -req -in 002_im1.pem -out 002_im1.pem -CA 002_im2.pem -CAkey 002_im2.key.pem -days 3650 -extfile 002_im1.cfg -extensions v3_ca -CAserial 002_im2.srl -CAcreateserial 
openssl x509 -inform PEM -in 002_im1.pem -outform DER -out 002_im1.der


REM - End Entity Cert

openssl req -newkey rsa:512 -nodes -out 002_ee.pem -keyout 002_ee.key.pem -config 002_ee.cfg -days 3650
openssl x509 -req -in 002_ee.pem -out 002_ee.pem -CA 002_im1.pem -CAkey 002_im1.key.pem -days 3650 -extfile 002_ee.cfg -extensions v3_ca -CAserial 002_im1.srl -CAcreateserial 
openssl x509 -inform PEM -in 002_ee.pem -outform DER -out 002_ee.der


REM *** SEC-CERTMAN-CRITEXT-003 ***
REM - Root Cert

openssl req -newkey rsa:512 -nodes -out 003_rc.pem -keyout 003_rc.key.pem -config 003_rc.cfg -days 3650
openssl x509 -req -days 3650 -in 003_rc.pem -signkey 003_rc.key.pem -out 003_rc.pem -extfile 003_rc.cfg -extensions v3_ca
openssl x509 -inform PEM -in 003_rc.pem -outform DER -out 003_rc.der


REM - End Entity Cert

openssl req -newkey rsa:512 -nodes -out 003_ee.pem -keyout 003_ee.key.pem -config 003_ee.cfg -days 3650
openssl x509 -req -in 003_ee.pem -out 003_ee.pem -CA 003_rc.pem -CAkey 003_rc.key.pem -days 3650 -extfile 003_ee.cfg -extensions v3_ca -CAserial 003_rc.srl -CAcreateserial 
openssl x509 -inform PEM -in 003_ee.pem -outform DER -out 003_ee.der


REM *** SEC-CERTMAN-CRITEXT-004 ***
REM - using Root Cert from 003
REM - End Entity Cert

openssl req -newkey rsa:512 -nodes -out 004_ee.pem -keyout 004_ee.key.pem -config 004_ee.cfg -days 3650
openssl x509 -req -in 004_ee.pem -out 004_ee.pem -CA 003_rc.pem -CAkey 003_rc.key.pem -days 3650 -extfile 004_ee.cfg -extensions v3_ca -CAserial 003_rc.srl -CAcreateserial 
openssl x509 -inform PEM -in 004_ee.pem -outform DER -out 004_ee.der


REM *** SEC-CERTMAN-CRITEXT-009 to 022 ***
REM - Root Cert

openssl req -newkey rsa:512 -nodes -out 009_rc.pem -keyout 009_rc.key.pem -config 009_rc.cfg -days 3650
openssl x509 -req -days 3650 -in 009_rc.pem -signkey 009_rc.key.pem -out 009_rc.pem
openssl x509 -inform PEM -in 009_rc.pem -outform DER -out 009_rc.der


REM - End Entity Cert

openssl req -newkey rsa:512 -nodes -out 009_ee.pem -keyout 009_ee.key.pem -config 009_ee.cfg -days 3650
openssl x509 -req -in 009_ee.pem -out 009_ee.pem -CA 009_rc.pem -CAkey 009_rc.key.pem -days 3650 -extfile 009_ee.cfg -extensions v3_ca -CAserial 009_rc.srl -CAcreateserial 
openssl x509 -inform PEM -in 009_ee.pem -outform DER -out 009_ee.der
