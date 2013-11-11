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

rmdir /s /q epoc32\wins\c\
rmdir /s /q epoc32\winscw\c\
rmdir /s /q epoc32\drive_d\
rmdir /s /q epoc32\drive_e\

xcopy /y/e backup\wins\c epoc32\wins\c\
xcopy /y/e backup\winscw\c epoc32\winscw\c\

rem xcopy /y/i/e/s/r/q \epoc32\winscw\c\tswi \epoc32\release\winscw\urel\z\tswi

xcopy /y/e backup\drive_d epoc32\drive_d\
xcopy /y/e backup\drive_e epoc32\drive_e\

rmdir /s /q backup

rem call \epoc32\release\wins\udeb\ced -dtextshell -- -i c:\auto.cfg
rem call \epoc32\release\winscw\udeb\ced -dtextshell -- -i c:\auto.cfg

