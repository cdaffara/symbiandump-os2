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
@SETLOCAL
@rem This batch file will run the tests automatically.
@IF "%~1"=="" goto ERROR
@IF "%~2"=="" goto ERROR
@FIND "startupmode 1" %EPOCROOT%EPOC32\data\epoc.ini >NUL
@if errorlevel 1 goto ADDLINE
goto BODY

:ADDLINE
echo startupmode 1 >> %EPOCROOT%EPOC32\data\epoc.ini
goto BODY

:BODY
@if exist \textshell.bat call \textshell.bat
@REM FIRST TEST PRESETUP
@if exist %EPOCROOT%EPOC32\winscw\c\private\10202be9\romversion\romversion_info.txt erase %EPOCROOT%EPOC32\winscw\c\private\10202be9\romversion\romversion_info.txt

@REM FIRST TEST SYSLIB-CENTRALREPOSITORY-CT-400[2|3|4]
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_txtem2.exe call %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_txtem2.exe

@REM SECOND TEST PRESETUP SYSLIB-CENTRALREPOSITORY-CT-4005
@if exist  %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt move %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt  %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_temp.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_modified.cre copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_modified.cre %EPOCROOT%EPOC32\winscw\c\private\10202be9\persists\10033333.cre
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\sw_modified1.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\sw_modified1.txt %EPOCROOT%EPOC32\winscw\c\private\10202be9\romversion\romversion_info.txt

@REM SECOND TEST
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_txtem1.exe call %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_txtem1.exe

@REM SECOND TEST POST CLEANUP
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_temp.txt move %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_temp.txt  %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt
goto EOF

:ERROR
@echo "please supply the release winscw and build type [udeb|urel]"

:EOF


