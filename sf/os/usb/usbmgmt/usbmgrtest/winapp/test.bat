REM Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
REM All rights reserved.
REM This component and the accompanying materials are made available
REM under the terms of "Eclipse Public License v1.0"
REM which accompanies this distribution, and is available
REM at the URL "http://www.eclipse.org/legal/epl-v10.html".
REM 
REM Initial Contributors:
REM Nokia Corporation - initial contribution.
REM 
REM Contributors:
REM 
REM Description:
REM 
REM 

usbcheck 0 0 1060 8444 293 0 "SMSC" "USB 2 Flash Media Device" >log.txt
if ERRORLEVEL 1 goto bad
echo !!!!
goto end

:bad

echo ???

:end

