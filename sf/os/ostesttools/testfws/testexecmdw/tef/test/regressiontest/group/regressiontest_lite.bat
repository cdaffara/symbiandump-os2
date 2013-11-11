@rem
@rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

md c:\testdata\
md c:\testdata\scripts\
md c:\testdata\configs\
md c:\system\data\

copy z:\testdata\scripts\ c:\testdata\scripts\
copy z:\testdata\configs\ c:\testdata\configs\
attrib c:\testdata\configs\ -R
copy z:\system\data\testexecute_test.ini c:\system\data\testexecute_test.ini

move c:\system\data\testexecute.ini c:\system\data\testexecute.ini.bak
copy c:\system\data\testexecute_test.ini c:\system\data\testexecute.ini

testexecutelite c:\testdata\scripts\te_regscriptcommandsuite.script
testexecutelite c:\testdata\scripts\te_regsteptestsuite.script
testexecutelite c:\testdata\scripts\dt-tef-teststep-0018.script

testexecutelite c:\testdata\scripts\te_regblocktestsuite.script
testexecutelite c:\testdata\scripts\te_regblocktestsuite_neg.script
testexecutelite c:\testdata\scripts\te_regclienttest.script
testexecutelite c:\testdata\scripts\te_regunittest.script
testexecutelite c:\testdata\scripts\te_regconcurrenttestsuite.script
testexecutelite c:\testdata\scripts\te_regperformancetestsuite.script
testexecutelite c:\testdata\scripts\te_regutilitiestest.script
c:\testdata\configs\tefLoggertest.bat

del c:\system\data\testexecute.ini
move c:\system\data\testexecute.ini.bak c:\system\data\testexecute.ini
