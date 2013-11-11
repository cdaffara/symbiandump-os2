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
md c:\system\data\
copy /s z:\system\data\test1certstore.dat c:\system\data\test1certstore.dat
copy /s z:\system\data\test2certstore.dat c:\system\data\test2certstore.dat
md c:\private\101f72a6\
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
t_certapps c:\tcertapps\scripts\script1.script  c:\testresults\tcertapps.htm
move c:\testresults\tcertapps.htm e:\testresults\tcertapps.htm
t_certapps c:\tcertapps\scripts\apipolicing.script c:\testresults\tcertapps-apipolicing.htm
move c:\testresults\tcertapps-apipolicing.htm e:\testresults\tcertapps-apipolicing.htm
del /s c:\tcertapps\


REM t_keystore
copy /s z:\tkeystore\ c:\tkeystore\
t_keystore c:\tkeystore\scripts\tkeystore.script c:\testresults\tkeystore.htm
move c:\testresults\tkeystore.htm e:\testresults\tkeystore.htm
t_keystore c:\tkeystore\scripts\authobjects.script c:\testresults\tkeystore_authobjects.htm
move c:\testresults\tkeystore_authobjects.htm e:\testresults\tkeystore_authobjects.htm
t_keystore c:\tkeystore\scripts\exportscript.script c:\testresults\tkeystore_export.htm
move c:\testresults\tkeystore_export.htm e:\testresults\tkeystore_export.htm
t_keystore c:\tkeystore\scripts\reload.script c:\testresults\tkeystore_reload.htm
move c:\testresults\tkeystore_reload.htm e:\testresults\tkeystore_reload.htm
del /s c:\tkeystore\
