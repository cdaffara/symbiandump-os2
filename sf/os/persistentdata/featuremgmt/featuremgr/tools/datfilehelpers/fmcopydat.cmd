// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

@echo off

if "%1"=="-h" goto help

if "%1"==""  (set srcdat=%EPOCROOT%epoc32\data\config\features.dat) else (set srcdat=%1)

echo Attempting to copy "%srcdat%" into both Emulator Z: drives...

copy /Y %srcdat% %EPOCROOT%epoc32\release\winscw\urel\z\private\10205054\features.dat
copy /Y %srcdat% %EPOCROOT%epoc32\release\winscw\udeb\z\private\10205054\features.dat
echo Done.

goto end

:help
echo on
@echo . 
@echo Installs the specified features.dat file into the FeatMgr emulator Z:
@echo directories. If no filename is supplied it attempts to install the one 
@echo from epoc32/data/config create by the romkit component features command.
@echo .
@echo usage: "fmcopydat [-h] [<features.dat filename>]"
@echo example: "fmcopydat features.dat"
@echo .

:end
