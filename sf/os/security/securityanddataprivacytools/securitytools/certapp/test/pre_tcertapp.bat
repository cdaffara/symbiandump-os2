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
md %BACKUP%
md %TARG%
del /q /s %BACKUP%\*
move /y %TARG%\* %BACKUP%
copy \epoc32\winscw\c\security_tools\tcertapp\SDP-Security_Tools-CertApp-CCDAT_0000.dat  %TARG%\certclients.dat
copy \epoc32\winscw\c\security_tools\tcertapp\SDP-Security_Tools-CertApp-FILEDAT_0000.dat %TARG%\cacerts.dat

REM
REM Backup and copy in UREL certclients and filecertstore
REM
set TARG=\epoc32\release\winscw\urel\z\private\101f72a6
set BACKUP=\epoc32\release\winscw\urel\z\private\101f72a6.pretcertapp
md %BACKUP%
md %TARG%
del /q /s %BACKUP%\*
move /y %TARG%\* %BACKUP%
copy \epoc32\winscw\c\security_tools\tcertapp\SDP-Security_Tools-CertApp-CCDAT_0000.dat  %TARG%\certclients.dat
copy \epoc32\winscw\c\security_tools\tcertapp\SDP-Security_Tools-CertApp-FILEDAT_0000.dat %TARG%\cacerts.dat

REM
REM Delete system drive filecertstore
REM
del /q /s \epoc32\winscw\c\private\101f72a6\*


REM
REM Backup and copy in swicertstore
REM

REM UDEB swicertstore
set TARG=\epoc32\release\winscw\udeb\z\resource
set BACKUP=\epoc32\release\winscw\udeb\z\resource.pretcertapp
md %BACKUP%
md %TARG%
del /q /s %BACKUP%\*
move /y %TARG%\swicertstore* %BACKUP%
copy \epoc32\winscw\c\security_tools\tcertapp\SDP-Security_Tools-CertApp-SWIDAT_0000.dat %TARG%\swicertstore.dat

REM UREL swicertstore
set TARG=\epoc32\release\winscw\urel\z\resource
set BACKUP=\epoc32\release\winscw\urel\z\resource.pretcertapp
md %BACKUP%
md %TARG%
del /q /s %BACKUP%\*
move /y %TARG%\swicertstore* %BACKUP%
copy \epoc32\winscw\c\security_tools\tcertapp\SDP-Security_Tools-CertApp-SWIDAT_0000.dat %TARG%\swicertstore.dat

REM
REM Delete writable swicertstore files
REM
del /q \epoc32\winscw\c\resource\swicertstore\dat\swicertstore*.dat

REM
REM Disable swicertstore unified certstore plugin
REM
move \epoc32\release\winscw\udeb\swicertstoreplugin.dll \epoc32\release\winscw\udeb\swicertstoreplugin.dll.disabled
move \epoc32\release\winscw\urel\swicertstoreplugin.dll \epoc32\release\winscw\urel\swicertstoreplugin.dll.disabled


