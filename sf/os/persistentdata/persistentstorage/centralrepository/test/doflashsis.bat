@echo off
rem For flash test stub (centrepswiteststub.sis) see testexecute/SWI/data/dosis.bat
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
@echo on

rem Make and sign the RF1 package.
makesis RF1.pkg
makesis RF2.pkg
makesis RF3.pkg
makesis RF4.pkg

signsis -S RF1.sis techview_RF1.sis testexecute\SWI\data\certstore\centreproot.pem testexecute\SWI\data\certstore\centreproot.key
signsis -S RF2.sis techview_RF2.sis testexecute\SWI\data\certstore\centreproot.pem testexecute\SWI\data\certstore\centreproot.key
signsis -S RF3.sis techview_RF3.sis testexecute\SWI\data\certstore\centreproot.pem testexecute\SWI\data\certstore\centreproot.key
signsis -S RF4.sis techview_RF4.sis testexecute\SWI\data\certstore\centreproot.pem testexecute\SWI\data\certstore\centreproot.key

signsis -S RF1.sis rnd_RF1.sis testexecute\SWI\data\certstore\Nokia_RnDCert_02.der testexecute\SWI\data\certstore\Nokia_RnDCert_02.key
signsis -S RF2.sis rnd_RF2.sis testexecute\SWI\data\certstore\Nokia_RnDCert_02.der testexecute\SWI\data\certstore\Nokia_RnDCert_02.key
signsis -S RF3.sis rnd_RF3.sis testexecute\SWI\data\certstore\Nokia_RnDCert_02.der testexecute\SWI\data\certstore\Nokia_RnDCert_02.key
signsis -S RF4.sis rnd_RF4.sis testexecute\SWI\data\certstore\Nokia_RnDCert_02.der testexecute\SWI\data\certstore\Nokia_RnDCert_02.key

del /f RF1.sis
del /f RF2.sis
del /f RF3.sis
del /f RF4.sis
