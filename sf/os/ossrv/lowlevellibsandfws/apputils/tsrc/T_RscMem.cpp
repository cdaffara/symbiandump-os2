// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests CResourceFile class - memory allocation tests
// 
//

#include <e32test.h>
#include <barsc2.h>

LOCAL_D RTest test(_L("T_RSCMEM"));
LOCAL_D RFs TheFs;

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0484
@SYMTestCaseDesc        CResourceFile class test
                        Memory allocation tests
@SYMTestPriority        High
@SYMTestActions         Tests for the out of memory conditions
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOpenL(const TDesC &aFileName, TUint aFileOffset, TUint aFileSize)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0484 "));
	__UHEAP_RESET;

	for(TInt count = 1; ;count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, count);
		__UHEAP_MARK;

		CResourceFile* rsc_file = NULL;
		TRAPD(err, rsc_file = CResourceFile::NewL(TheFs, aFileName, aFileOffset, aFileSize));
		delete rsc_file;

		if(err == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if(err == KErrNone)
			{
			__UHEAP_MARKEND;
			break;
			}
		else
			User::Panic(_L("CResourceFile::NewL(), error "), err);
		}

	__UHEAP_RESET;
	}

LOCAL_C void DoTestsL()
    {
	CleanupClosePushL(TheFs);
	User::LeaveIfError(TheFs.Connect());

	::TestOpenL(_L("z:\\system\\data\\Trsc.rsc"), 0, 0);

	CleanupStack::PopAndDestroy(1, &TheFs);
    }

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
	test.Title();
	test.Start(_L("Testing CResourceFile::NewL()"));
    TRAPD(err, DoTestsL());
    test.Printf(_L("Error code is %d\n"), err);
    test(err == KErrNone);
    test.Next(_L("/n"));
	test.End();
    test.Close();
    delete cleanup;
    __UHEAP_MARKEND;
	return 0;
    }
