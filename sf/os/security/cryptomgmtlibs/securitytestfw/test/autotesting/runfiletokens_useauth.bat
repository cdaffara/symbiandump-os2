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
del /s c:\tkeystore\

del /s c:\private\101f72a6\keys.dat
del /s c:\system\data\keys.dat

REM t_keystore
copy /s z:\tkeystore\ c:\tkeystore\

TESTEXECUTE  c:\tkeystore\scripts\initialize.script
move c:\logs\testexecute\initialize.htm e:\testresults\initialize.htm

T_KEYSTORE c:\tkeystore\scripts\tkeystore_nouserregistered.script c:\logs\testexecute\tkeystore_nouserregistered.htm
T_KEYSTORE c:\tkeystore\scripts\tkeystore_nouserregistered.script c:\logs\testexecute\tkeystore_nouserregistered.htm
move c:\logs\testexecute\tkeystore_nouserregistered.htm e:\testresults\tkeystore_nouserregistered.htm


REM the execution of this code is a prerequisite for running the keystore tests against the test plugins.
TESTEXECUTE c:\tkeystore\scripts\reg_first_identity.script
move c:\logs\testexecute\reg_first_identity.htm e:\testresults\reg_first_identity.htm

REM These set of tests run when keystore has been enbled to use authentication server.
T_KEYSTORE c:\tkeystore\scripts\tkeystore_useauth.script	c:\logs\testexecute\tkeystore_useauth.htm
move c:\logs\testexecute\tkeystore_useauth.htm e:\testresults\tkeystore_useauth.htm

T_KEYSTORE c:\tkeystore\scripts\authobjects_useauth.script c:\logs\testexecute\tkeystore_authobjects_useauth.htm
move c:\logs\testexecute\tkeystore_authobjects_useauth.htm e:\testresults\tkeystore_authobjects_useauth.htm

T_KEYSTORE c:\tkeystore\scripts\exportscript_useauth.script c:\logs\testexecute\tkeystore_export_useauth.htm
move c:\logs\testexecute\tkeystore_export_useauth.htm c:\testresults\tkeystore_export_useauth.htm

T_KEYSTORE c:\tkeystore\scripts\reload_useauth.script c:\logs\testexecute\tkeystore_reload_useauth.htm
move c:\logs\testexecute\tkeystore_reload_useauth.htm e:\testresults\tkeystore_reload_useauth.htm

T_KEYSTORE c:\tkeystore\scripts\authenticate_useauth.script c:\logs\testexecute\tkeystore_authenticate_useauth.htm
move c:\logs\testexecute\tkeystore_authenticate_useauth.htm e:\testresults\tkeystore_authenticate_useauth.htm

T_KEYSTORE c:\tkeystore\scripts\tkeystore_authentication1.script c:\logs\testexecute\tkeystore_authentication1.htm
move c:\logs\testexecute\tkeystore_authentication1.htm e:\testresults\tkeystore_authentication1.htm

TESTEXECUTE c:\tkeystore\scripts\reg_second_identity.script
move c:\logs\testexecute\reg_second_identity.htm e:\testresults\reg_second_identity.htm

T_KEYSTORE c:\tkeystore\scripts\tkeystore_authentication2.script c:\logs\testexecute\tkeystore_authentication2.htm
move c:\logs\testexecute\tkeystore_authentication2.htm e:\testresults\tkeystore_authentication2.htm

del /s c:\tkeystore\
