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
@IF "%~1"=="" goto ORIG
@if not exist %EPOCROOT%EPOC32\data\Z\resource\versions\sw.txt goto ERROR

echo "Building modified sw.txt rom"
@call buildrom -D_NAND2 -nofm h4hrp techview CentralRepositoryTechviewFlashPerformance.iby
GOTO EOF
 
:ORIG
echo "Building original rom"
@call buildrom -D_NAND2 -nofm -DROMORIG h4hrp techview CentralRepositoryTechviewFlashPerformance.iby 

GOTO EOF

:ERROR
echo "Missing sw.txt file"

:EOF
