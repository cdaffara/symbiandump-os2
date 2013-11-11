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
cls
echo Note that this file should not be run.
echo It only exists to show how the certificate extensions were created.
pause
rmdir /S /Q ca1 tmp

mkdir ca1\private 
mkdir certs 
mkdir tmp 
echo "01" > ca1/serial

rem Generate root cert
openssl req -x509 -newkey rsa:1024 -keyout %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\testcertificates\certman\tx509\Data\extensions\ca1\private\cakey.pem -out ca1\cacert.pem -subj "/O=Symbian/CN=CA Root Cert" -nodes -config openssl.cfg
openssl x509 -in %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\testcertificates\certman\tx509\Data\extensions\ca1\cacert.pem -outform DER -out %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\testcertificates\certman\tx509\Data\extensions\certs\ca1-root.der 

REM Delete all AUTO generated files (some cfg and der files are created manually)
for %%i in (ext\*.txt) do del "ext\%%~ni.cfg" /F /Q
for %%i in (ext\*.cfg) do del "certs\%%~ni.der" /F /Q
for %%i in (ext\*.txt) do del "certs\%%~ni.der" /F /Q

for %%i in (ext\*.txt) do perl -w certextbuilder.pl "%%i" "ext\%%~ni.cfg"
for %%i in (ext\*.cfg) do call .\makecert.bat %%~ni

