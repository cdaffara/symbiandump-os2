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
//

#include <e32test.h>
#include <f32file.h>
#include "ImplementationInformation.h"


RTest TheTest(_L("t_ImplementationInformation.exe"));

// functions for checking results
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
        TheTest(EFalse, aLine);
		}
	}

static  void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
        RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}


#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-1664
@SYMTestCaseDesc	    This test case tests the SetVendorId and VendorId methods. the vendor
						Id value is checked.
@SYMTestPriority 	    High
@SYMTestActions  	    Checks that the VID of the implementation is 0 right after a  CImplementationInformation
						is created. call SetVendorId, then call VendorId to check that the returned VID is the same as
						the value set in SetVendorId method.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ6111
*/
void TestImplementationInformationL()
	{

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-1664 Start to create CImplementationInformation object..... "));
	__UHEAP_MARK;

	//create CImplementationInformation object

	TDriveUnit drive(EDriveC);
	TUid uid1 =	{0x00000000};

	CImplementationInformation* implementation = CImplementationInformation::NewL(uid1,
																		  1,
																		  (HBufC*)NULL,
																		  (HBufC8*)NULL,
																		  (HBufC8*)NULL,
																		  drive,
																		  EFalse,
																		  EFalse);
	CleanupStack::PushL(implementation);

	TVendorId vid1 = implementation->VendorId();
	// Vid should be 0 after initialization.
	TEST(vid1.iId == 0);
	TVendorId vid2(0x70000001);
	// Set Vid
	implementation->SetVendorId(vid2);

	TVendorId vid3 = implementation->VendorId();
	//check VID.
	TEST(vid3.iId == 0x70000001);
	//clean up.
	CleanupStack::PopAndDestroy(implementation);
	__UHEAP_MARKEND;
	}

// out of memory test
void TestImplementationInformationOOML()
	{
	TheTest.Next(_L("CImplementationInformation Out of Memory Test....."));
	TInt processHandlesS = 0;
	TInt threadHandlesS = 0;
	TInt processHandlesE = 0;
	TInt threadHandlesE = 0;
	RThread().HandleCount(processHandlesS, threadHandlesS);
	for(TInt count=1;;++count)
		{
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, count);
		__UHEAP_MARK;

		TRAPD(err,TestImplementationInformationL());
		if(err == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if(err == KErrNone)
			{
			__UHEAP_MARKEND;
			RDebug::Print(_L("The test succeeded at heap failure rate=%d.\n"), count);
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			TEST2(err, KErrNone);
			}
		__UHEAP_RESET;
		}
	__UHEAP_RESET;
	RThread().HandleCount(processHandlesE, threadHandlesE);
	TEST(processHandlesS == processHandlesE);
	TEST(threadHandlesS == threadHandlesE);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TheTest.Title();
	TheTest.Start(_L("Starting tests..."));

	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(err2,TestImplementationInformationL());
	TRAP(err2, TestImplementationInformationOOML());
	TheTest(err2==KErrNone);

	delete cleanup;


	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;
	return(0);
	}

