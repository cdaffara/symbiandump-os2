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
echo Create a certificate with the specified extension

SET CERT_PATH=%SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\testcertificates\certman\tx509\Data\extensions
rem Create certificate request
openssl req -newkey rsa:1024 -keyout %CERT_PATH%\tmp\key.pem -out %CERT_PATH%\tmp\%1_req.pem -subj "/O=Symbian/CN=Entity Cert 1 (%1)" -nodes -config openssl.cfg

rem Sign the request using the CA key and add the extension
openssl x509 -req -in %CERT_PATH%\tmp\%1_req.pem -inform pem -out %CERT_PATH%\certs\%1.der -outform DER -extfile ext\%1.cfg -CAkey %CERT_PATH%\ca1\private\cakey.pem -CA %CERT_PATH%\ca1\cacert.pem
