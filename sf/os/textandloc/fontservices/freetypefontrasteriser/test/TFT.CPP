/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <hal.h>
#include "TFT.H"
#include "testharness.h"


//-----------------
CTFtStep::CTFtStep()
	{
	SetTestStepName(KTFtStep);
	}

TVerdict CTFtStep::doTestStepL()
    {
#if defined(__SUPPORT_CPP_EXCEPTIONS__)
/**
@SYMTestCaseID GRAPHICS-FREETYPE-0002
*/
	SetTestStepID(_L("GRAPHICS-FREETYPE-0002"));
	TInt error = KErrNone;

	try	{
		TTrapHandler* trapHandler = User::MarkCleanupStack();

		__UHEAP_MARK;

		CTestHarness* test_harness = CTestHarness::NewL(this);
		test_harness->PerformTests();
		delete test_harness;

		__UHEAP_MARKEND;

		User::UnMarkCleanupStack(trapHandler);
		}
	catch (XLeaveException& leave)
		{
		error = leave.GetReason();
		}
	catch (...)
		{
		User::Invariant();
		}
	
	if(error != KErrNone)
		{
		SetTestStepResult(EFail);
		}

	RecordTestResultL();
#endif	//__SUPPORT_CPP_EXCEPTIONS__
	return TestStepResult();
    }
