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

if not exist %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\private\102744CA\featreg.tmp.cfg goto noTmpFiles

rem Delete the test featreg files
del /f %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\z\private\102744ca\featreg.cfg
del /f %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\z\private\102744ca\featreg.cfg[2-0]
del /f %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\z\private\102744ca\featreg.cfg[3-0]
del /f %EPOCROOT%EPOC32\RELEASE\WINSCW\UREL\z\private\102744ca\featreg.cfg
del /f %EPOCROOT%EPOC32\RELEASE\WINSCW\UREL\z\private\102744ca\featreg.cfg[2-0]
del /f %EPOCROOT%EPOC32\RELEASE\WINSCW\UREL\z\private\102744ca\featreg.cfg[3-0]

rem Restore original featreg files
rename %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\z\private\102744ca\featreg.tmp.cfg featreg.cfg
rename %EPOCROOT%EPOC32\RELEASE\WINSCW\UREL\z\private\102744ca\featreg.tmp.cfg featreg.cfg
goto :EOF

:noTmpFiles
@echo No temp files exist, pre bat file hasn't been run or %0 already called