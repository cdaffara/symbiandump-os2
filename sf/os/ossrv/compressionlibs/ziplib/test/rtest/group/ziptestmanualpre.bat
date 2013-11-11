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
REM This should be run prior to running the ZipTestManual test for ezlib on the Emulator (the test does not run on hardware).

copy /y \epoc32\RELEASE\WINSCW\UREL\Z\resource\plugins\cp850.RSC	\epoc32\winscw\c\test\data\cp850.RSC
copy /y \epoc32\RELEASE\WINSCW\UDEB\Z\resource\plugins\cp850.RSC	\epoc32\winscw\c\test\data\cp850.RSC

if exist \epoc32\winscw\c\Resource\Plugins\cp850.RSC del \epoc32\winscw\c\Resource\Plugins\cp850.RSC
del /Q \epoc32\winscw\c\private\10009D8F\*.*
del \epoc32\RELEASE\WINSCW\UREL\Z\resource\plugins\cp850.RSC
del \epoc32\RELEASE\WINSCW\UDEB\Z\resource\plugins\cp850.RSC

REM textshell.bat should only be called during an ONB.
if exist \textshell.bat call \textshell.bat
	
