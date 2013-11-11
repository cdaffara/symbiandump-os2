// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



#include "ctestbase.h"


EXPORT_C void CTestBase::MemoryLeakTestL(CTestBase& aTest, CIpuTestHarness& aTestHarness)
	{
	// set aTestHarness to be in memoryleak mode
	aTestHarness.SetTestMode(CIpuTestHarness::ETestModeMemoryLeak);

	TInt ii=0;
	_LIT(KMemoryLeakMessage, "Memory Leak Testing on allocation %d\n");
	FOREVER
		{
		aTestHarness.LogIt(KMemoryLeakMessage, ii);
		__UHEAP_MARK;
		__UHEAP_FAILNEXT(ii);
		TRAPD(err,aTest.DoTestsL());
		__UHEAP_MARKEND;
		User::Heap().Check();
		__UHEAP_RESET;
		// Break out if the test passes successfully;
		if (err == KErrNone)
			break;
		if (err != KErrNoMemory)
			{
			// set aTestHarness back to normal mode
			aTestHarness.SetTestMode(CIpuTestHarness::ETestModeNormal);			
			User::LeaveIfError(err);
			}
		++ii;
		}
	// set aTestHarness back to normal mode
	aTestHarness.SetTestMode(CIpuTestHarness::ETestModeNormal);
	}
