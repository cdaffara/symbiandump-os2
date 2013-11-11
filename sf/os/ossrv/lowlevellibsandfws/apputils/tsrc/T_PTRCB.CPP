// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests for CPtrC8Array and TKeyDesC8Array class(Test Improvement activity)
// 
//

#include <e32test.h>
#include <badesca.h>

RTest test(_L("T_PTRCB"));

/**
@SYMTestCaseID          SYSLIB-BAFL-UT-1706
@SYMTestCaseDesc        Testing 8-bit descriptor arrays
						Tests for CPtrC8Array::MdcaCount(),CPtrC8Array::CopyL(),CPtrC8Array::operator =() functions
@SYMTestPriority        Medium
@SYMTestActions         Tests by adding descriptors to CDesC8Array and then copying to CPtrC8Array
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/

void DoTestsL()
	{
	test.Next(_L("Test for ptrC8 and desC8 arrays"));

	CPtrC8Array* ptrB = new(ELeave) CPtrC8Array(8);
	CDesC8Array* desB = new(ELeave) CDesC8ArrayFlat(8);

	desB->AppendL(_L8("one"));
	desB->AppendL(_L8("two"));
	desB->AppendL(_L8("three"));

	// calling CPtrC8Array::CopyL() with an overloaded '=' operator

	*ptrB = *desB;

	test(ptrB->MdcaCount()==desB->MdcaCount());
	test(ptrB->MdcaPoint(2)==desB->MdcaPoint(2));

	test.Next(_L("Test for covering case ECmpCollated8 for TKeyDesC8Array class"));

	TInt pos = desB->InsertIsqAllowDuplicatesL(_L8("hello"));
	test(desB->MdcaCount()==4);

	// Invoking TKeyDesC8Array::Compare(), covers ECmpCollated8 case inside switch statement

	desB->Sort(ECmpCollated8);

	delete(ptrB);
	delete(desB);
	}


TInt E32Main()
	{
    __UHEAP_MARK;

    CTrapCleanup* cleanup=CTrapCleanup::New();
	if(cleanup == NULL)
	{
		return KErrNoMemory;
	}
	test.Title();
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-UT-1706 Testing CPtrC8Array and TKeyDesC8Array class "));

    TRAPD(err,DoTestsL());
    test(err==KErrNone);
    test.End();
    test.Close();
    delete cleanup;

    __UHEAP_MARKEND;

	return(0);
    }
