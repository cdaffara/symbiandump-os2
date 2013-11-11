@echo off
rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem
@SEtLOCAL
@rem This bat file creates roms
@if not exist %EPOCROOT%EPOC32\data\Z\resource\versions\sw.txt goto ERROR

@IF "%~1"=="" goto ERRORA
@IF "%~1"=="1" goto SETUPA
@IF "%~1"=="2" goto SETUPB
GOTO ERRORA

:SETUPA
@call buildrom -D_FULL_DEBUG -nofm -D_NAND2 -DROMORIG h4hrp techview CenRepFotaTvRTests.iby 
GOTO EOF

:SETUPB
@call buildrom -D_FULL_DEBUG -nofm -D_NAND2 -DROMFLASHING h4hrp techview CenRepFotaTvRTests.iby  
GOTO EOF

:ERROR
echo "Missing sw.txt file"
GOTO EOF

:ERRORA
echo "Need to pass in number arguments [1-2]"

:EOF
