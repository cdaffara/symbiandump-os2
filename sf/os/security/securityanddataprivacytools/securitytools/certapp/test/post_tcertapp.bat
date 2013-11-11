@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
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
set TARG=\epoc32\release\winscw\udeb\z\private\101f72a6
set BACKUP=\epoc32\release\winscw\udeb\z\private\101f72a6.pretcertapp
del /q %TARG%\*
move /y %BACKUP%\* %TARG%
rd %BACKUP%

REM
REM Restore UREL filecertstore
REM
set TARG=\epoc32\release\winscw\urel\z\private\101f72a6
set BACKUP=\epoc32\release\winscw\urel\z\private\101f72a6.pretcertapp
del /q %TARG%\*
move /y %BACKUP%\* %TARG%
rd %BACKUP%

REM
REM Delete system drive filecertstore
REM
del /q \epoc32\winscw\c\private\101f72a6\*


REM
REM Restore in swicertstore
REM

REM UDEB swicertstore
set TARG=\epoc32\release\winscw\udeb\z\resource
set BACKUP=\epoc32\release\winscw\udeb\z\resource.pretcertapp
del /q %TARG%\swicertstore*
move /y %BACKUP%\swicertstore* %TARG%
rd %BACKUP%

REM UREL swicertstore
set TARG=\epoc32\release\winscw\urel\z\resource
set BACKUP=\epoc32\release\winscw\urel\z\resource.pretcertapp
del /q %TARG%\swicertstore*
move /y %BACKUP%\swicertstore* %TARG%
rd %BACKUP%

REM Restore swicertstore unified certstore plugin
move \epoc32\release\winscw\udeb\swicertstoreplugin.dll.disabled \epoc32\release\winscw\udeb\swicertstoreplugin.dll
move \epoc32\release\winscw\urel\swicertstoreplugin.dll.disabled \epoc32\release\winscw\urel\swicertstoreplugin.dll

