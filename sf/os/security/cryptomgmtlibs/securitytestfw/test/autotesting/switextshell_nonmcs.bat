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

REM Attempt to stop the execution in case it's accidentally started on a PC
ECHO About to run Security Team textshell Hardware Tests
pause

REM create testresults directory
md c:\testresults
md e:\testresults
copy z:\buildid.txt e:\
copy z:\buildid.txt e:\testresults

REM copy hosts file to redirect cam-ocspserver request to ban-ocspserver
md c:\private
md c:\private\10000882
copy z:\hosts c:\private\10000882\hosts

REM install automated commdb
copy z:\auto.cfg c:\auto.cfg
ced -i c:\auto.cfg

sntpclient -savings cbtime01.symbian.intra bantime01.symbian.intra

copy z:\auto.cfg c:\auto.cfg
attrib c:\auto.cfg -r

testexecute z:\tswi\tuiscriptadaptors\scripts\testupgrades_tshell.script
move c:\logs\testexecute\testupgrades_tshell.htm e:\testresults\testupgrades_tshell.htm

REM restore standard automated commdb for the rest of the tests
ced -i z:\auto.cfg

REM SwiConsole Tests
testexecute z:\tswi\tswiconsole\scripts\tswiconsole_tshell.script
move c:\logs\testexecute\tswiconsole_tshell.htm e:\testresults\tswiconsole_tshell.htm
md c:\tswi
md c:\tswi\tbackuprestore

REM SWI Observer Tests
testexecute z:\tswi\tswiobserver\scripts\testswiobserver_tshell.script
copy c:\logs\testexecute\testswiobserver_tshell.htm e:\testresults\testswiobserver_tshell.htm

testexecute z:\tswi\tdevcerts\scripts\devcerts_ocsp_tshell.script
move c:\logs\testexecute\devcerts_ocsp_tshell.htm e:\testresults\devcerts_ocsp_tshell.htm

rem Deletes files left by tswiconsole tests.
attrib c:\Documents\InstTest\file1.txt -r
del c:\Documents\InstTest\file1.txt

testexecute z:\tswi\tuiscriptadaptors\scripts\switermination_tshell.script
move c:\logs\testexecute\switermination_tshell.htm e:\testresults\switermination_tshell.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\switextshell_tshell.script
move c:\logs\testexecute\switextshell_tshell.htm e:\testresults\switextshell_tshell.htm

del c:\temp\*.*
attrib c:\tswi\*.* -r
del c:\tswi\*.*

attrib c:\logs\testexecute\*.* -r
del c:\logs\testexecute\*.*

