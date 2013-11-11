// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32file.h>
#include "t_logutil2.h"

RTest TheTest(_L("t_logfilter"));

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0924
@SYMTestCaseDesc	    CLogFilter functionality test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for event,remote,direction,duration,status,contact,number,copying,flags types
                        Tests for copying of two filter objects and test for its equality.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestFilterL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0924 "));
	CLogFilter* filter1 = CLogFilter::NewL();
	CleanupStack::PushL(filter1);

	// Event type
	TEST(filter1->EventType() == TUid::Null());

	filter1->SetEventType(TUid::Uid(0x12345678));
	TEST(filter1->EventType() == TUid::Uid(0x12345678));

	filter1->SetEventType(TUid::Uid(KMaxTInt32));
	TEST(filter1->EventType() == TUid::Uid(KMaxTInt32));

	filter1->SetEventType(TUid::Null());
	TEST(filter1->EventType() == TUid::Null());

	// Remote Party
	TEST(filter1->RemoteParty() == KNullDesC);

	HBufC* buf = TestUtils::CreateBufLC(KLogMaxRemotePartyLength / 2);
	filter1->SetRemoteParty(buf->Des());
	TEST(filter1->RemoteParty() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf = TestUtils::CreateBufLC(KLogMaxRemotePartyLength);
	filter1->SetRemoteParty(buf->Des());
	TEST(filter1->RemoteParty() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	filter1->SetRemoteParty(KNullDesC);
	TEST(filter1->RemoteParty() == KNullDesC);

	// Direction
	TEST(filter1->Direction() == KNullDesC);

	buf = TestUtils::CreateBufLC(KLogMaxDirectionLength / 2);
	filter1->SetDirection(buf->Des());
	TEST(filter1->Direction() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf = TestUtils::CreateBufLC(KLogMaxDirectionLength);
	filter1->SetDirection(buf->Des());
	TEST(filter1->Direction() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	filter1->SetDirection(KNullDesC);
	TEST(filter1->Direction() == KNullDesC);

	// Duration Type
	TEST(filter1->DurationType() == KLogNullDurationType);

	filter1->SetDurationType(0xf);
	TEST(filter1->DurationType() == 0xf);

	filter1->SetDurationType(KMaxTInt8);
	TEST(filter1->DurationType() == KMaxTInt8);

	filter1->SetDurationType(0);
	TEST(filter1->DurationType() == 0);

	// Status
	TEST(filter1->Status() == KNullDesC);

	buf = TestUtils::CreateBufLC(KLogMaxStatusLength / 2);
	filter1->SetStatus(buf->Des());
	TEST(filter1->Status() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf = TestUtils::CreateBufLC(KLogMaxStatusLength);
	filter1->SetStatus(buf->Des());
	TEST(filter1->Status() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	filter1->SetStatus(KNullDesC);
	TEST(filter1->Status() == KNullDesC);

	// Contact
	TEST(filter1->Contact() == KLogNullContactId);

	filter1->SetContact(0x12345678);
	TEST(filter1->Contact() == 0x12345678);

	filter1->SetContact(KMaxTInt32);
	TEST(filter1->Contact() == KMaxTInt32);

	CLogFilter* filter2 = CLogFilter::NewL();
	CleanupStack::PushL(filter2);
	TEST(!TestUtils::FiltersEqual(*filter1, *filter2));

	filter1->SetContact(KLogNullContactId);
	TEST(filter1->Contact() == KLogNullContactId);

	filter1->SetContact(KMinTInt32);
	TEST(filter1->Contact() == KMinTInt32);

	// Number
	TEST(filter1->Number() == KNullDesC);

	buf = TestUtils::CreateBufLC(KLogMaxNumberLength / 2);
	filter1->SetNumber(buf->Des());
	TEST(filter1->Number() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf = TestUtils::CreateBufLC(KLogMaxNumberLength);
	filter1->SetNumber(buf->Des());
	TEST(filter1->Number() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	filter1->SetNumber(KNullDesC);
	TEST(filter1->Number() == KNullDesC);

	// Copying
	filter1->SetEventType(TUid::Uid(0x12345678));

	buf = TestUtils::CreateBufLC(KLogMaxRemotePartyLength / 2);
	filter1->SetRemoteParty(buf->Des());
	TEST(filter1->RemoteParty() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf = TestUtils::CreateBufLC(KLogMaxDirectionLength / 2);
	filter1->SetDirection(buf->Des());
	TEST(filter1->Direction() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	filter1->SetDurationType(0xf);
	TEST(filter1->DurationType() == 0xf);

	buf = TestUtils::CreateBufLC(KLogMaxStatusLength / 2);
	filter1->SetStatus(buf->Des());
	TEST(filter1->Status() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	filter1->SetContact(0x12345678);
	TEST(filter1->Contact() == 0x12345678);

	buf = TestUtils::CreateBufLC(KLogMaxNumberLength / 2);
	filter1->SetNumber(buf->Des());
	TEST(filter1->Number() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	// Flags
	TEST(filter1->Flags() == KLogNullFlags);
	filter1->SetFlags(1);
	TEST(filter1->Flags() == 1);
	filter1->SetFlags(2);
	TEST(filter1->Flags() == 3);
	filter1->SetFlags(4);
	TEST(filter1->Flags() == 7);
	filter1->SetFlags(8);
	TEST(filter1->Flags() == KLogFlagsMask);
	filter1->ClearFlags(8);
	TEST(filter1->Flags() == 7);
	filter1->ClearFlags(4);
	TEST(filter1->Flags() == 3);
	filter1->ClearFlags(2);
	TEST(filter1->Flags() == 1);
	filter1->ClearFlags(1);
	TEST(filter1->Flags() == KLogNullFlags);

	filter1->SetFlags(1);
	TEST(filter1->Flags() == 1);
	filter1->SetFlags(3);
	TEST(filter1->Flags() == 3);
	filter1->SetFlags(7);
	TEST(filter1->Flags() == 7);
	filter1->SetFlags(KLogFlagsMask);
	TEST(filter1->Flags() == KLogFlagsMask);
	filter1->ClearFlags(KLogFlagsMask);
	TEST(filter1->Flags() == KLogNullFlags);

	filter1->SetFlags(0x5);
	TEST(filter1->Flags() == 0x5);
	filter1->SetFlags(0xA);
	TEST(filter1->Flags() == KLogFlagsMask);
	filter1->ClearFlags(0x5);
	TEST(filter1->Flags() == 0xA);
	filter1->ClearFlags(0xA);
	TEST(filter1->Flags() == KLogNullFlags);

	// Test copying

	filter2->Copy(*filter1);
	TEST(TestUtils::FiltersEqual(*filter1, *filter2));
	
	CleanupStack::PopAndDestroy(); // filter2;

	filter2 = CLogFilter::NewL();
	CleanupStack::PushL(filter2);
	TEST(!TestUtils::FiltersEqual(*filter1, *filter2));

	filter1->Copy(*filter2);
	TEST(TestUtils::FiltersEqual(*filter1, *filter2));

	CleanupStack::PopAndDestroy(2); // filter1, filter2;
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0925
@SYMTestCaseDesc	    Filter with heap failure test
@SYMTestPriority 	    High
@SYMTestActions  	    Check for any memory and environment errors while creation
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestFilterWithHeapFailL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0925 "));
#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	CLogFilter* filter = NULL;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, filter = CLogFilter::NewL());

		__UHEAP_RESET;

		if (error == KErrNone)
			{
			TEST2(error, KErrNone);
			delete filter;
			finished = ETrue;
			}
		else
			TEST2(error, KErrNoMemory);
		}
	}

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logfilter"));

	TheTest.Start(_L("Filter"));
	TestFilterL();
	theLog.Write(_L8("Test 1 OK\n"));

	TheTest.Next(_L("Filter with heap failure"));
	TestFilterWithHeapFailL();
	theLog.Write(_L8("Test 2 OK\n"));
	}
