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
cd \
call %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\test\autotesting\useautocfg.bat

rem call \epoc32\release\wins\udeb\ced -dtextshell -- -i c:\auto.cfg
rem call \epoc32\release\winscw\udeb\ced -dtextshell -- -i c:\auto.cfg

xcopy /y/e epoc32\wins\c backup\wins\c\
xcopy /y/e epoc32\winscw\c backup\winscw\c\

mkdir \epoc32\drive_e
mkdir \epoc32\drive_d

rem xcopy /y/i/e/s/r/q \epoc32\winscw\c\tswi \epoc32\release\winscw\udeb\z\tswi

xcopy /y/e \epoc32\drive_d \backup\drive_d\
xcopy /y/e \epoc32\drive_e \backup\drive_e\

copy /Y \epoc32\winscw\c\config_urel.bat \config_urel.bat
