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

xrem This script will build the source for verbose mode.
rem Verbose mode enable all Log points 

rem change attributes for "read only" files
call attrib ..\..\group\*.* -S -H -R -A

md backup
copy ..\..\group\*.* backup\
copy MacroInstrumenterTool\MacroInstrumenter.exe ..\..\group\MacroInstrumenter.exe

cd ..\..\group
CALL MacroInstrumenter.exe ULoggerClient.mmp __LIGHTLOGGER_ENABLED 1
CALL MacroInstrumenter.exe ULoggerClient.mmp __PERFORMANCE_TEST 1

CALL MacroInstrumenter.exe uloggercommand.mmp __LIGHTLOGGER_ENABLED 1
CALL MacroInstrumenter.exe uloggercommand.mmp __PERFORMANCE_TEST 1

CALL MacroInstrumenter.exe uloggerplugfile.mmp __LIGHTLOGGER_ENABLED 1
CALL MacroInstrumenter.exe uloggerplugfile.mmp __PERFORMANCE_TEST 1

rem compile code
call bldmake bldfiles
call abld reallyclean
call abld build

rem cleaning up
del MacroInstrumenter.exe
del /q *.*
cd ..\test\Utils
call copy /y backup\*.* ..\..\group\
del /q backup\*.*
call pause