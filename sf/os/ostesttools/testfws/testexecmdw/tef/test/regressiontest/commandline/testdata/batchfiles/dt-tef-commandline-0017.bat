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

testexecute c:\testdata\scripts\te_regseltestcase_duplicate_testcasename.script -tci DT-TEF-SelTestCase-0003
copy c:\logs\testexecute\te_regseltestcase_duplicate_testcasename.htm   c:\logs\testexecute\dt-tef-command-line-0017_1.htm
testexecute c:\testdata\scripts\te_regseltestcase_duplicate_testcasename.script -tcx DT-TEF-SelTestCase-0005
copy c:\logs\testexecute\te_regseltestcase_duplicate_testcasename.htm   c:\logs\testexecute\dt-tef-command-line-0017_2.htm
