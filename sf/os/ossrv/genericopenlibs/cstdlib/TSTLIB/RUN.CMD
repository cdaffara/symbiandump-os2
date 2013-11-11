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
@echo off
goto b%1
:b
:bdeb
set _where_=\Epoc32\Release\WINS\DEB
goto doit
:brel
set _where_=\Epoc32\Release\WINS\REL
goto doit
:budeb
set _where_=\Epoc32\Release\WINS\UDEB
goto doit
:burel
set _where_=\Epoc32\Release\WINS\UREL
:doit

echo \STDLIB\TSTLIB\STDLIB2K...
%_where_%\ESTLIB2K
if errorlevel 1 pause

echo \STDLIB\TSTLIB\TTIME...
%_where_%\TTIME
if errorlevel 1 pause

echo \STDLIB\TSTLIB\TFILES...
%_where_%\TFILES
if errorlevel 1 pause

echo \STDLIB\TSTLIB\THELLO...
%_where_%\THELLO
if errorlevel 1 pause

echo \STDLIB\TSTLIB\TMISC...
%_where_%\TMISC
if errorlevel 1 pause

echo \STDLIB\TSTLIB\TMISC2...
%_where_%\TMISC2
if errorlevel 1 pause

echo \STDLIB\TSTLIB\TDIRS...
%_where_%\TDIRS
if errorlevel 1 pause

echo \STDLIB\TSTLIB\TMTHREAD...
%_where_%\TMTHREAD
if errorlevel 1 pause

REM echo \STDLIB\TSTLIB\TLSTREAM...
REM %_where_%\TLSTREAM
REM if errorlevel 1 pause

echo \STDLIB\TSTLIB\TISTREAM...
%_where_%\TISTREAM
if errorlevel 1 pause

echo \STDLIB\TSTLIB\TPIPE2...
%_where_%\TPIPE2
if errorlevel 1 pause

echo \STDLIB\TSTLIB\TCANCEL...
%_where_%\TCANCEL
if errorlevel 1 pause

echo \STDLIB\TSTLIB\TWFILES...
%_where_%\TWFILES
if errorlevel 1 pause

echo \STDLIB\TSTLIB\TWDIRS...
%_where_%\TWDIRS
if errorlevel 1 pause

set _where_=

