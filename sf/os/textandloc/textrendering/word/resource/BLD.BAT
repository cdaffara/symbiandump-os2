@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
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

set _vgroup=word

if "%1"=="wins" set _mbmdir=\epoc32\release\wins\%2\z\system\apps\%_vgroup%
if "%1"=="WINS" set _mbmdir=\epoc32\release\wins\%2\z\system\apps\%_vgroup%
if "%1"=="marm" set _mbmdir=\epoc32\release\marm\%2
if "%1"=="MARM" set _mbmdir=\epoc32\release\marm\%2

bmconv bld.txt >NUL
copy %_vgroup%.mbm %_mbmdir%\%_vgroup%.mbw >NUL

if "%2"=="urel" copy %_vgroup%u.abw %_mbmdir%\%_vgroup%.abw >NUL
if "%2"=="UREL" copy %_vgroup%u.abw %_mbmdir%\%_vgroup%.abw >NUL
if "%2"=="UDEB" copy %_vgroup%u.abw %_mbmdir%\%_vgroup%.abw >NUL
if "%2"=="udeb" copy %_vgroup%u.abw %_mbmdir%\%_vgroup%.abw >NUL

if "%2"=="urel" set _inifile=wsiniu.ini
if "%2"=="UREL" set _inifile=wsiniu.ini
if "%2"=="udeb" set _inifile=wsiniu.ini
if "%2"=="UDEB" set _inifile=wsiniu.ini
if "%1"=="marm" copy %_inifile% %_mbmdir%\wsini.ini
if "%1"=="MARM" copy %_inifile% %_mbmdir%\wsini.ini
if "%1"=="wins" copy %_inifile% \epoc32\release\wins\%2\z\system\data\wsini.ini
if "%1"=="WINS" copy %_inifile% \epoc32\release\wins\%2\z\system\data\wsini.ini
fc4bat %_vgroup%.mbg \epoc32\include\%_vgroup%.mbg
if errorlevel==1 copy %_vgroup%.mbg \epoc32\include >NUL
del %_vgroup%.mbg >NUL
del %_vgroup%.mbm >NUL

set _vgroup=
set _mbmdir=
