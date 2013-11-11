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

@rem This call is so that the batch file can be used unaltered by the ONB
@if exist \textshell.bat call \textshell.bat

@if not exist %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\featregtest\featreg.cfg goto notBuilt
@if not exist %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\featregtest\featreg.cfg[2-0] goto notBuilt
@if not exist %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\featregtest\featreg.cfg[3-0] goto notBuilt
@if exist %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\private\102744ca\featreg.tmp.cfg goto TmpFilesExist

@rem Save original featreg files
@rename %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\private\102744ca\featreg.cfg featreg.tmp.cfg
@rename %EPOCROOT%EPOC32\RELEASE\WINSCW\UREL\Z\private\102744ca\featreg.cfg featreg.tmp.cfg

@rem Copy over new featreg files
@copy /y %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\featregtest\featreg.cfg %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\private\102744ca\featreg.cfg
@copy /y %EPOCROOT%EPOC32\RELEASE\WINSCW\UREL\Z\featregtest\featreg.cfg %EPOCROOT%EPOC32\RELEASE\WINSCW\UREL\Z\private\102744ca\featreg.cfg
@copy /y %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\featregtest\featreg.cfg[2-0] %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\private\102744ca\featreg.cfg[2-0]
@copy /y %EPOCROOT%EPOC32\RELEASE\WINSCW\UREL\Z\featregtest\featreg.cfg[2-0] %EPOCROOT%EPOC32\RELEASE\WINSCW\UREL\Z\private\102744ca\featreg.cfg[2-0]
@copy /y %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\featregtest\featreg.cfg[3-0] %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\private\102744ca\featreg.cfg[3-0]
@copy /y %EPOCROOT%EPOC32\RELEASE\WINSCW\UREL\Z\featregtest\featreg.cfg[3-0] %EPOCROOT%EPOC32\RELEASE\WINSCW\UREL\Z\private\102744ca\featreg.cfg[3-0]
@goto :EOF

:TmpFilesExist
@echo Tmp files exist %0 already called
@goto :EOF

:notBuilt
@echo %EPOCROOT%EPOC32\RELEASE\WINSCW\UDEB\Z\featregtest\featreg.cfg doesn't exist, test files not exported
