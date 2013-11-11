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

md e:\testresults\
testexecute z:\tups\tupsdb\scripts\tupsdb.script 
move c:\logs\testexecute\tupsdb.htm e:\testresults\tupsdb.htm

testexecute z:\tups\tupsdb\scripts\tupsdbperformance.script 
move c:\logs\testexecute\tupsdbperformance.htm e:\testresults\tupsdbperformance.htm

rem UPS policy library unit tests
testexecute z:\tups\tpolicies\scripts\tpolicycache.script
move c:\logs\testexecute\tpolicycache.htm e:\testresults\tpolicycache.htm
testexecute z:\tups\tpolicies\scripts\tpolicyreader.script
move c:\logs\testexecute\tpolicyreader.htm e:\testresults\tpolicyreader.htm
testexecute z:\tups\tpolicies\scripts\tserviceconfig.script
move c:\logs\testexecute\tserviceconfig.htm e:\testresults\tserviceconfig.htm

rem ups integration tests
rem Creates temporary folder for backup and restore tests.
md c:\tswi
md c:\tswi\tbackuprestore

testexecute z:\tups\integ\scripts\tups_integ.script
move c:\logs\testexecute\tups_integ.htm e:\testresults\tups_integ.htm

tmsgapp.exe
move c:\tmsgapp.log e:\testresults\tmsgapp.log
del c:\tmsgapp.log

attrib c:\logs\testexecute\*.* -r
del c:\logs\testexecute\*.*

rem Deletes temporary folder for backup and restore tests.
attrib c:\tswi\*.* -r
del c:\tswi\*.*

