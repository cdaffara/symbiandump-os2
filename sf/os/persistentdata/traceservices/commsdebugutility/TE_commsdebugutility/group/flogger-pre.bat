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
@echo on
call \textshell.bat

if not exist %EPOCROOT%epoc32\wins\c\logs\ md %EPOCROOT%epoc32\wins\c\logs\
attrib -r %EPOCROOT%epoc32\wins\c\logs\comsdbg.ini
copy %EPOCROOT%epoc32\wins\c\logs\comsdbg.ini %EPOCROOT%epoc32\wins\c\logs\comsdbg.ini.old
copy %EPOCROOT%epoc32\release\wins\udeb\z\TestData\Scripts\ts_comsdbg_file.ini %EPOCROOT%epoc32\wins\c\logs\ts_flogger.ini

if not exist %EPOCROOT%epoc32\winscw\c\logs\ md %EPOCROOT%epoc32\winscw\c\logs\
attrib -r %EPOCROOT%epoc32\winscw\c\logs\comsdbg.ini
copy %EPOCROOT%epoc32\winscw\c\logs\comsdbg.ini %EPOCROOT%epoc32\winscw\c\logs\comsdbg.ini.old
copy %EPOCROOT%epoc32\release\winscw\udeb\z\TestData\Scripts\ts_comsdbg_file.ini %EPOCROOT%epoc32\winscw\c\logs\ts_flogger.ini
