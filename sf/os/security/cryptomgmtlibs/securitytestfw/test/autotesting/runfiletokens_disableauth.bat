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

copy /s z:\tkeystore\ c:\tkeystore\
copy /s z:\tcertstore\ c:\tcertstore\
copy /s z:\ttesttools\ c:\ttesttools\
copy /s z:\system\data\test1certstore.dat c:\system\data\test1certstore.dat
copy /s c:\ttesttools\data\certclients.dat c:\private\101f72a6\certclients.dat

TESTEXECUTE c:\ttesttools\scripts\sec-filetokens-tools.script
move c:\logs\testexecute\sec-filetokens-tools.htm e:\testresults\sec-filetokens-tools.htm

del c:\private\101f72a6\certclients.dat
del c:\system\data\test1certstore.dat

del /s c:\ttesttools\
del /s c:\tcertstore\
del /s c:\tkeystore\

rem TCERTAPPS
copy /s z:\tcertapps\ c:\tcertapps\
t_certapps c:\tcertapps\scripts\script1.script c:\logs\testexecute\tcertapps.htm
move c:\logs\testexecute\tcertapps.htm e:\testresults\tcertapps.htm
t_certapps c:\tcertapps\scripts\apipolicing.script c:\logs\testexecute\tcertapps-apipolicing.htm
move c:\logs\testexecute\tcertapps-apipolicing.htm e:\testresults\tcertapps-apipolicing.htm
del /s c:\tcertapps\


REM t_keystore
copy /s z:\tkeystore\ c:\tkeystore\
t_keystore c:\tkeystore\scripts\tkeystore.script c:\logs\testexecute\tkeystore.htm
move c:\logs\testexecute\tkeystore.htm e:\testresults\tkeystore.htm
t_keystore c:\tkeystore\scripts\authobjects.script c:\logs\testexecute\tkeystore_authobjects.htm
move c:\logs\testexecute\tkeystore_authobjects.htm e:\testresults\tkeystore_authobjects.htm
t_keystore c:\tkeystore\scripts\exportscript.script c:\logs\testexecute\tkeystore_export.htm
move c:\logs\testexecute\tkeystore_export.htm e:\testresults\tkeystore_export.htm
t_keystore c:\tkeystore\scripts\reload.script c:\logs\testexecute\tkeystore_reload.htm
move c:\logs\testexecute\tkeystore_reload.htm e:\testresults\tkeystore_reload.htm
t_keystore c:\tkeystore\scripts\oldserver_newfeature.script c:\logs\testexecute\oldserver_newfeature.htm
move c:\logs\testexecute\oldserver_newfeature.htm e:\testresults\oldserver_newfeature.htm
del /s c:\tkeystore\
