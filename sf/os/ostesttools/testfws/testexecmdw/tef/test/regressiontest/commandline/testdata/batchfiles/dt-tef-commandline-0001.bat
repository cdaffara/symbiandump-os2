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

testexecute c:\testdata\scripts\te_regseltestcase.script -tci DT-TEF-SelTestCase-0001
copy c:\logs\testexecute\te_regseltestcase.htm   c:\logs\testexecute\dt-tef-command-line-0001_1.htm
testexecute c:\testdata\scripts\te_regseltestcase.script -tci DT-TEF-SelTestCase-0003,DT-TEF-SelTestCase-0009
copy c:\logs\testexecute\te_regseltestcase.htm   c:\logs\testexecute\dt-tef-command-line-0001_2.htm
testexecute c:\testdata\scripts\te_regseltestcase.script -tcx DT-TEF-SelTestCase-0005
copy c:\logs\testexecute\te_regseltestcase.htm   c:\logs\testexecute\dt-tef-command-line-0001_3.htm
testexecute c:\testdata\scripts\te_regseltestcase.script -tcx DT-TEF-SelTestCase-0007,DT-TEF-SelTestCase-0008
copy c:\logs\testexecute\te_regseltestcase.htm   c:\logs\testexecute\dt-tef-command-line-0001_4.htm