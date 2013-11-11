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
if "%1"=="" goto err
if "%2"=="" goto err
if "%2"=="rel" goto err
if "%2"=="REL" goto err
if "%2"=="deb" goto err
if "%2"=="DEB" goto err

if "%1"=="wins" set _destdir=\epoc32\release\wins\%2\z\system\templates\word\
if "%1"=="WINS" set _destdir=\epoc32\release\wins\%2\z\system\templates\word\
if "%1"=="marm" set _destdir=\epoc32\release\marm\%2\
if "%1"=="MARM" set _destdir=\epoc32\release\marm\%2\
set _lang=%3
if "%3"=="" set _lang=uk

goto %1

:wins
copy blank.uk %_destdir%"Blank document"
goto home

:marm
copy *.%_lang% %_destdir%*.%_lang%
goto home

:err
echo Usage: bld [plat] [var] [lang]
echo var should be udeb/urel
goto end

:home
echo Templates copied to %_destdir%

:end
set _destdir=
set _lang=
