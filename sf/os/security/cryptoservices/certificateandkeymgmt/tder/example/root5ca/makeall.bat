@rem
@rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

echo This script only needs to be run if you wish to modify this test data.
echo Press CTRL-C to exit
pause

rem blank the existing CA
del index.txt rand serial *.old *.attr *.pem *.req *.pk8 /S /Q
echo 01 > serial
type nul > index.txt

set CERT_PATH=%SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\testcertificates\certman\tder\example\root5ca

rem create a key and request
rem sign the request
rem revoke the signed certificate
rem generate a CRL
openssl genrsa -out %CERT_PATH%\ee_key.pem
openssl req -config openssl_ee.config -newkey rsa:1024 -key %CERT_PATH%\ee_key.pem -out ee.req 
openssl ca -config openssl.config -name Root5CA -in ee.req -extensions Signing_Extensions -extfile openssl.config -batch -out %CERT_PATH%\root5_ee.pem
openssl ca -config openssl.config -name Root5CA -revoke %CERT_PATH%\root5_ee.pem -crl_reason keyCompromise
openssl ca -config openssl.config -name Root5CA -gencrl -out %CERT_PATH%\root5crl.pem

rem convert everything to DER
openssl x509 -inform pem -outform der -in %CERT_PATH%\root5_ee.pem -out %CERT_PATH%\root5_ee.der
openssl rsa -inform pem -outform der -in %CERT_PATH%\ee_key.pem -out %CERT_PATH%\ee_key.der
openssl pkcs8 -topk8 -nocrypt -in ee_key.pem -outform der -out ee_key.pk8
openssl crl -inform pem -outform der -in %CERT_PATH%\root5crl.pem -out %CERT_PATH%\root5crl.der
