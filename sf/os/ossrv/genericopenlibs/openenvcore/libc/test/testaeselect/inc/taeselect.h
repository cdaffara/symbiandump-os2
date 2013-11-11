/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Description:
* Name        : tselect.h
*
*/

#ifndef __TESTAESELECT_H__
#define __TESTAESELECT_H__

#include <test/testexecutestepbase.h>
#include <e32svr.h>
#include <sys/aeselect.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

_LIT(KTestTimeout, "TestTimeout");
_LIT(KTestASelect, "TestASelect");
_LIT(KTestCancelASelect, "TestCancelASelect");
_LIT(KTestAselectRequestStatus,"TestAselectRequestStatus");
_LIT(KTestMultipleASelect, "TestMultipleASelect");
_LIT(KTestSelectwithEselect, "TestSelectwithEselect");
_LIT(KTestESelect, "TestESelect");
_LIT(KTestEselectRequestStatus, "TestEselectRequestStatus");
_LIT(KTestESelectTimeout, "TestESelectTimeout");
_LIT(KTestEselectInvalidInput_01,"TestEselectInvalidInput_01");
_LIT(KTestEselectInvalidInput_02,"TestEselectInvalidInput_02");

class CTestAESelect : public CTestStep
	{
public:
	~CTestAESelect(); 
	CTestAESelect(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
    TInt TestTimeout();
    TInt TestAselect();
    TInt TestCancelAselect();
    TInt TestAselectRequestStatus();    
    TInt TestMultipleAselect();
    TInt TestSelectwithEselect();
    TInt TestEselect();
    TInt TestEselectTimeout();
    TInt TestEselectRequestStatus();
    TInt TestEselectInvalidInput_01();
    TInt TestEselectInvalidInput_02();
	};

#endif // __TESTAESELECT_H__

