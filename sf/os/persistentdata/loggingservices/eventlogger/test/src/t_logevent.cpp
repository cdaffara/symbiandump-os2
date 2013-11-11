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
#include <s32mem.h>

RTest TheTest(_L("t_logevent"));

TBool CompareEvents(const CLogEvent& aLeft, const CLogEvent& aRight)
	{
	TBool ret = aLeft.Data() == aRight.Data();
	ret = ret && aLeft.Description() == aRight.Description();
	ret = ret && aLeft.Direction() == aRight.Direction();
	ret = ret && aLeft.Duration() == aRight.Duration();
	ret = ret && aLeft.DurationType() == aRight.DurationType();
	ret = ret && aLeft.EventType() == aRight.EventType();
	ret = ret && aLeft.Flags() == aRight.Flags();
	ret = ret && aLeft.Id() == aRight.Id();
	ret = ret && aLeft.Link() == aRight.Link();
	ret = ret && aLeft.Number() == aRight.Number();
	ret = ret && aLeft.RemoteParty() == aRight.RemoteParty();
	ret = ret && aLeft.Status() == aRight.Status();
	ret = ret && aLeft.Subject() == aRight.Subject();
	ret = ret && aLeft.Time() == aRight.Time();
	return ret;
	}

void TestStoreL(const CLogEvent& aEvent)
	{
//	const TInt size = aEvent.Size();

	//Store
	CBufFlat* buf = CBufFlat::NewL(0x100);
	CleanupStack::PushL(buf);
	RBufWriteStream write(*buf);
	write << aEvent;
	write.CommitL();

	//Restore
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	RBufReadStream read(*buf);
	read >> *event;

	TBool ret = CompareEvents(aEvent, *event);
	TEST(ret);

	CleanupStack::PopAndDestroy(2);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1008
@SYMTestCaseDesc	    Tests for writing different events to a store
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up TestStoreL() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEventL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1008 "));
	CLogEvent* event1 = CLogEvent::NewL();
	CleanupStack::PushL(event1);

	// Event Id
	TEST(event1->Id() == KLogNullId);
	TestStoreL(*event1);

	event1->SetId(0x12345678);
	TEST(event1->Id() == 0x12345678);
	TestStoreL(*event1);

	event1->SetId(KMaxTInt32);
	TEST(event1->Id() == KMaxTInt32);
	TestStoreL(*event1);

	event1->SetId(0);
	TEST(event1->Id() == 0);
	TestStoreL(*event1);

	// Event type
	event1->SetEventType(TUid::Null());
	TestStoreL(*event1);

	event1->SetEventType(TUid::Uid(0x12345678));
	TEST(event1->EventType() == TUid::Uid(0x12345678));
	TestStoreL(*event1);

	event1->SetEventType(TUid::Uid(KMaxTInt32));
	TEST(event1->EventType() == TUid::Uid(KMaxTInt32));
	TestStoreL(*event1);

	event1->SetEventType(TUid::Null());
	TEST(event1->EventType() == TUid::Null());
	TestStoreL(*event1);

	// Remote Party
	TEST(event1->RemoteParty() == KNullDesC);
	TestStoreL(*event1);

	HBufC* buf = TestUtils::CreateBufLC(KLogMaxRemotePartyLength / 2);
	event1->SetRemoteParty(buf->Des());
	TEST(event1->RemoteParty() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxRemotePartyLength);
	event1->SetRemoteParty(buf->Des());
	TEST(event1->RemoteParty() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxRemotePartyLength * 2);
	event1->SetRemoteParty(buf->Des());
	TEST(event1->RemoteParty() == buf->Des().Left(KLogMaxRemotePartyLength));
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	event1->SetRemoteParty(KNullDesC);
	TEST(event1->RemoteParty() == KNullDesC);
	TestStoreL(*event1);

	// Direction
	TEST(event1->Direction() == KNullDesC);
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxDirectionLength / 2);
	event1->SetDirection(buf->Des());
	TEST(event1->Direction() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxDirectionLength);
	event1->SetDirection(buf->Des());
	TEST(event1->Direction() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxDirectionLength * 2);
	event1->SetDirection(buf->Des());
	TEST(event1->Direction() == buf->Des().Left(KLogMaxDirectionLength));
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	event1->SetDirection(KNullDesC);
	TEST(event1->Direction() == KNullDesC);
	TestStoreL(*event1);

	// Time
	TTime time;

	time.UniversalTime();
	event1->SetTime(time);
	TEST(event1->Time() == time);
	TestStoreL(*event1);

	time.HomeTime();
	event1->SetTime(time);
	TEST(event1->Time() == time);
	TestStoreL(*event1);

	// Duration Type
	TEST(event1->DurationType() == KLogNullDurationType);

	event1->SetDurationType(0xf);
	TEST(event1->DurationType() == 0xf);
	TestStoreL(*event1);

	event1->SetDurationType(KMaxTInt8);
	TEST(event1->DurationType() == KMaxTInt8);
	TestStoreL(*event1);

	event1->SetDurationType(KLogNullDurationType);
	TEST(event1->DurationType() == KLogNullDurationType);
	TestStoreL(*event1);

	// Duration
	TEST(event1->Duration() == KLogNullDuration);

	event1->SetDuration(0x12345678);
	TEST(event1->Duration() == 0x12345678);
	TestStoreL(*event1);

	event1->SetDuration(KMaxTUint32);
	TEST(event1->Duration() == KMaxTUint32);
	TestStoreL(*event1);

	event1->SetDuration(KLogNullDuration);
	TEST(event1->Duration() == KLogNullDuration);
	TestStoreL(*event1);

	// Status
	TEST(event1->Status() == KNullDesC);

	buf = TestUtils::CreateBufLC(KLogMaxStatusLength / 2);
	event1->SetStatus(buf->Des());
	TEST(event1->Status() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxStatusLength);
	event1->SetStatus(buf->Des());
	TEST(event1->Status() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxStatusLength * 2);
	event1->SetStatus(buf->Des());
	TEST(event1->Status() == buf->Des().Left(KLogMaxStatusLength));
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	event1->SetStatus(KNullDesC);
	TEST(event1->Status() == KNullDesC);
	TestStoreL(*event1);

	// Subject
	TEST(event1->Subject() == KNullDesC);

	buf = TestUtils::CreateBufLC(KLogMaxSubjectLength / 2);
	event1->SetSubject(buf->Des());
	TEST(event1->Subject() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxSubjectLength);
	event1->SetSubject(buf->Des());
	TEST(event1->Subject() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxSubjectLength * 2);
	event1->SetSubject(buf->Des());
	TEST(event1->Subject() == buf->Des().Left(KLogMaxSubjectLength));
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	event1->SetSubject(KNullDesC);
	TEST(event1->Subject() == KNullDesC);
	TestStoreL(*event1);

	// Number
	TEST(event1->Number() == KNullDesC);

	buf = TestUtils::CreateBufLC(KLogMaxNumberLength / 2);
	event1->SetNumber(buf->Des());
	TEST(event1->Number() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxNumberLength);
	event1->SetNumber(buf->Des());
	TEST(event1->Number() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxNumberLength * 2);
	event1->SetNumber(buf->Des());
	TEST(event1->Number() == buf->Des().Left(KLogMaxNumberLength));
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	event1->SetNumber(KNullDesC);
	TEST(event1->Number() == KNullDesC);
	TestStoreL(*event1);

	// Contact
	TEST(event1->Contact() == KLogNullContactId);

	event1->SetContact(0x12345678);
	TEST(event1->Contact() == 0x12345678);
	TestStoreL(*event1);

	event1->SetContact(KMaxTInt32);
	TEST(event1->Contact() == KMaxTInt32);
	TestStoreL(*event1);

	event1->SetContact(KLogNullContactId);
	TEST(event1->Contact() == KLogNullContactId);
	TestStoreL(*event1);

	event1->SetContact(KMinTInt32);
	TEST(event1->Contact() == KMinTInt32);
	TestStoreL(*event1);

	// Link
	TEST(event1->Link() == KLogNullLink);

	event1->SetLink(0x12345678);
	TEST(event1->Link() == 0x12345678);
	TestStoreL(*event1);

	event1->SetLink(KMaxTUint32);
	TEST(event1->Link() == KMaxTUint32);
	TestStoreL(*event1);

	event1->SetLink(KLogNullLink);
	TEST(event1->Link() == KLogNullLink);

	// Description
	TEST(event1->Description() == KNullDesC);

	buf = TestUtils::CreateBufLC(KLogMaxDescriptionLength / 2);
	event1->SetDescription(buf->Des());
	TEST(event1->Description() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxDescriptionLength);
	event1->SetDescription(buf->Des());
	TEST(event1->Description() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	buf = TestUtils::CreateBufLC(KLogMaxDescriptionLength * 2);
	event1->SetDescription(buf->Des());
	TEST(event1->Description() == buf->Des().Left(KLogMaxDescriptionLength));
	CleanupStack::PopAndDestroy(); // buf
	TestStoreL(*event1);

	event1->SetDescription(KNullDesC);
	TEST(event1->Description() == KNullDesC);

	// Flags
	TEST(event1->Flags() == KLogNullFlags);
	event1->SetFlags(1);
	TEST(event1->Flags() == 1);
	TestStoreL(*event1);
	event1->SetFlags(2);
	TEST(event1->Flags() == 3);
	event1->SetFlags(4);
	TEST(event1->Flags() == 7);
	event1->SetFlags(8);
	TEST(event1->Flags() == KLogFlagsMask);
	event1->ClearFlags(8);
	TEST(event1->Flags() == 7);
	event1->ClearFlags(4);
	TEST(event1->Flags() == 3);
	event1->ClearFlags(2);
	TEST(event1->Flags() == 1);
	event1->ClearFlags(1);
	TEST(event1->Flags() == KLogNullFlags);

	event1->SetFlags(1);
	TEST(event1->Flags() == 1);
	event1->SetFlags(3);
	TEST(event1->Flags() == 3);
	event1->SetFlags(7);
	TEST(event1->Flags() == 7);
	event1->SetFlags(15);
	event1->SetFlags(KLogFlagsMask);
	TEST(event1->Flags() == KLogFlagsMask);
	event1->ClearFlags(KLogFlagsMask);
	TEST(event1->Flags() == KLogNullFlags);

	event1->SetFlags(0x5);
	TEST(event1->Flags() == 0x5);
	event1->SetFlags(0xA);
	TEST(event1->Flags() == KLogFlagsMask);
	event1->ClearFlags(0x5);
	TEST(event1->Flags() == 0xA);
	event1->ClearFlags(0xA);
	TEST(event1->Flags() == KLogNullFlags);

	// Data
	TEST(event1->Data() == KNullDesC8);

	HBufC8* buf8;

	buf8 = TestUtils::CreateBuf8LC(100);
	event1->SetDataL(buf8->Des());
	TEST(event1->Data() == buf8->Des());
	CleanupStack::PopAndDestroy();
	TestStoreL(*event1);

	buf8 = TestUtils::CreateBuf8LC(200);
	event1->SetDataL(buf8->Des());
	TEST(event1->Data() == buf8->Des());
	CleanupStack::PopAndDestroy();
	TestStoreL(*event1);

	buf8 = TestUtils::CreateBuf8LC(400);
	event1->SetDataL(buf8->Des());
	TEST(event1->Data() == buf8->Des());
	CleanupStack::PopAndDestroy();
	TestStoreL(*event1);

	event1->SetDataL(KNullDesC8);
	TEST(event1->Data() == KNullDesC8);

	// streaming
	TFileName storename = _L("c:\\T_LOGEVENT_DATA");
	TUid uid={0x12345678};

	// create a store
	theFs.Delete(storename);
	CDictionaryFileStore* store = CDictionaryFileStore::OpenLC(theFs, storename, uid);
	
	RDictionaryWriteStream write;
	RDictionaryReadStream read;

	event1->SetDataL(KNullDesC8);

	uid.iUid++;
	write.AssignL(*store, uid);
	write << event1->Data();
	write.CommitL();
	write.Close();

	read.OpenL(*store, uid);
	event1->SetDataL(read, 0);
	read.Close();

	TEST(event1->Data() == KNullDesC8);

	buf8 = TestUtils::CreateBuf8LC(100);
	event1->SetDataL(buf8->Des());

	uid.iUid++;
	write.AssignL(*store, uid);
	write.WriteL(event1->Data());
	write.CommitL();
	write.Close();

	read.OpenL(*store, uid);
	event1->SetDataL(read, 100);
	read.Close();

	TEST(event1->Data() == buf8->Des());
	CleanupStack::PopAndDestroy(); // buf8

	buf8 = TestUtils::CreateBuf8LC(200);
	event1->SetDataL(buf8->Des());

	uid.iUid++;
	write.AssignL(*store, uid);
	write.WriteL(event1->Data());
	write.CommitL();
	write.Close();

	read.OpenL(*store, uid);
	event1->SetDataL(read, 200);
	read.Close();

	TEST(event1->Data() == buf8->Des());
	CleanupStack::PopAndDestroy(); // buf8

	buf8 = TestUtils::CreateBuf8LC(400);
	event1->SetDataL(buf8->Des());

	uid.iUid++;
	write.AssignL(*store, uid);
	write.WriteL(event1->Data());
	write.CommitL();
	write.Close();

	read.OpenL(*store, uid);
	event1->SetDataL(read, 400);
	read.Close();

	TEST(event1->Data() == buf8->Des());
	CleanupStack::PopAndDestroy(2); // buf8, store

	// Copying
	event1->SetId(0x12345678);
	event1->SetEventType(TUid::Uid(0x12345678));

	buf = TestUtils::CreateBufLC(KLogMaxRemotePartyLength / 2);
	event1->SetRemoteParty(buf->Des());
	TEST(event1->RemoteParty() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf = TestUtils::CreateBufLC(KLogMaxDirectionLength / 2);
	event1->SetDirection(buf->Des());
	TEST(event1->Direction() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	event1->SetDurationType(0xf);
	TEST(event1->DurationType() == 0xf);

	event1->SetDuration(0x12345678);
	TEST(event1->Duration() == 0x12345678);

	buf = TestUtils::CreateBufLC(KLogMaxStatusLength / 2);
	event1->SetStatus(buf->Des());
	TEST(event1->Status() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf = TestUtils::CreateBufLC(KLogMaxSubjectLength / 2);
	event1->SetSubject(buf->Des());
	TEST(event1->Subject() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf = TestUtils::CreateBufLC(KLogMaxNumberLength / 2);
	event1->SetNumber(buf->Des());
	TEST(event1->Number() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	event1->SetContact(0x12345678);
	TEST(event1->Contact() == 0x12345678);

	event1->SetLink(0x12345678);
	TEST(event1->Link() == 0x12345678);

	buf = TestUtils::CreateBufLC(KLogMaxDescriptionLength / 2);
	event1->SetDescription(buf->Des());
	TEST(event1->Description() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf8 = TestUtils::CreateBuf8LC(200);
	event1->SetDataL(buf8->Des());
	TEST(event1->Data() == buf8->Des());
	CleanupStack::PopAndDestroy();

	CLogEvent* event2 = CLogEvent::NewL();
	CleanupStack::PushL(event2);
	TEST(!TestUtils::EventsEqual(*event1, *event2));

	event2->CopyL(*event1);
	TEST(TestUtils::EventsEqual(*event1, *event2));
	
	CleanupStack::PopAndDestroy(); // event2;

	event2 = CLogEvent::NewL();
	CleanupStack::PushL(event2);
	TEST(!TestUtils::EventsEqual(*event1, *event2));

	event1->CopyL(*event2);
	TEST(TestUtils::EventsEqual(*event1, *event2));

	CleanupStack::PopAndDestroy(2); // event1, event2;

	::DeleteDataFile(storename);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1009
@SYMTestCaseDesc	    Tests for CLogEvent::NewL(),SetDataL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for creation of log event on heap and 
                        test for setting  event specific data from the specified stream and try to read the data back.
                        Check for memory errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEventWithHeapFailL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1009 "));
#ifdef _DEBUG
	TInt failCount = 0;
#endif
	TInt error;
	TBool finished = EFalse;

	CLogEvent* event = NULL;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, event = CLogEvent::NewL());
		
		__UHEAP_RESET;

		if (error == KErrNone)
			{
			finished = ETrue;
			CleanupStack::PushL(event);
			}
		else
			TEST2(error, KErrNoMemory);
		}

	_LIT8(KDataTest1, "01234567890123456789");
	_LIT8(KDataTest2, "012345678901234567890123456789");

	finished = EFalse;
#ifdef _DEBUG
	failCount = 0;
#endif
	event->SetDataL(KNullDesC8);

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, event->SetDataL(KDataTest1));

		__UHEAP_RESET;

		if (error == KErrNone)
			{
			finished = ETrue;
			TEST(event->Data() == KDataTest1);
			}
		else
			{
			TEST2(error, KErrNoMemory);
			TEST(event->Data() == KNullDesC8);
			}
		}

	finished = EFalse;
#ifdef _DEBUG
	failCount = 0;
#endif
	event->SetDataL(KNullDesC8);

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, event->SetDataL(KDataTest2));

		__UHEAP_RESET;

		if (error == KErrNone)
			{
			finished = ETrue;
			TEST(event->Data() == KDataTest2);
			}
		else
			{
			TEST2(error, KErrNoMemory);
			TEST(event->Data() == KNullDesC8);
			}
		}

	finished = EFalse;
#ifdef _DEBUG
	failCount = 0;
#endif
	event->SetDataL(KNullDesC8);

	// Check we don't get any more failures
	__UHEAP_FAILNEXT(0);
	event->SetDataL(KDataTest2);
	TEST(event->Data() == KDataTest2);
	event->SetDataL(KDataTest1);
	TEST(event->Data() == KDataTest1);
	event->SetDataL(KNullDesC8);
	TEST(event->Data() == KNullDesC8);
	__UHEAP_RESET;

	// streaming
	TFileName storename = _L("c:\\T_BASIC_DATA");
	TUid uid={0x12345678};

	// create a store
	theFs.Delete(storename);
	CDictionaryFileStore* store = CDictionaryFileStore::OpenLC(theFs, storename, uid);
	
	RDictionaryWriteStream write;
	RDictionaryReadStream read;

	uid.iUid++;
	write.AssignL(*store, uid);
	write << KNullDesC8;
	write.CommitL();
	write.Close();

	read.OpenL(*store, uid);

#ifdef _DEBUG
	failCount = 0;
#endif
	finished = EFalse;

	__UHEAP_FAILNEXT(0);
	event->SetDataL(read, 0);
	__UHEAP_RESET;

	read.Close();	
	TEST(event->Data() == KNullDesC8);

	HBufC8* buf8 = TestUtils::CreateBuf8LC(100);

	uid.iUid++;
	write.AssignL(*store, uid);
	write.WriteL(buf8->Des());
	write.CommitL();
	write.Close();

	read.OpenL(*store, uid);

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, event->SetDataL(read, 100));

		__UHEAP_RESET;

		if (error == KErrNone)
			{
			TEST(event->Data() == buf8->Des());
			read.Close();
			finished = ETrue;
			}
		else
			{
			TEST2(error, KErrNoMemory);
			TEST(event->Data() == KNullDesC8);
			}
		}

	event->SetId(0x12345678);
	event->SetEventType(TUid::Uid(0x12345678));

	HBufC* buf = TestUtils::CreateBufLC(KLogMaxRemotePartyLength / 2);
	event->SetRemoteParty(buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf = TestUtils::CreateBufLC(KLogMaxDirectionLength / 2);
	event->SetDirection(buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	TTime time;

	time.UniversalTime();
	event->SetTime(time);
	TEST(event->Time() == time);

	event->SetDurationType(0xf);
	event->SetDuration(0x12345678);

	buf = TestUtils::CreateBufLC(KLogMaxStatusLength / 2);
	event->SetStatus(buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf = TestUtils::CreateBufLC(KLogMaxSubjectLength / 2);
	event->SetSubject(buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf = TestUtils::CreateBufLC(KLogMaxNumberLength / 2);
	event->SetNumber(buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	event->SetContact(0x12345678);
	event->SetLink(0x12345678);

	buf = TestUtils::CreateBufLC(KLogMaxDescriptionLength / 2);
	event->SetDescription(buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	event->SetFlags(0xA);

	buf8 = TestUtils::CreateBuf8LC(100);
	event->SetDataL(buf8->Des());
	TEST(event->Data() == buf8->Des());
	CleanupStack::PopAndDestroy(); // buf8

	CLogEvent* event1 = CLogEvent::NewL();
	CleanupStack::PushL(event1);

	CLogEvent* event2 = CLogEvent::NewL();
	CleanupStack::PushL(event2);

	TEST(TestUtils::EventsEqual(*event1, *event2));

	finished = EFalse;
#ifdef _DEBUG
	failCount = 0;
#endif

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, event1->CopyL(*event));

		__UHEAP_RESET;

		if (error == KErrNone)
			{
			TEST(!TestUtils::EventsEqual(*event1, *event2));
			TEST(TestUtils::EventsEqual(*event1, *event));
			finished = ETrue;
			}
		else
			{
			TEST2(error, KErrNoMemory);
			TEST(TestUtils::EventsEqual(*event1, *event2));
			TEST(!TestUtils::EventsEqual(*event1, *event));
			}
		}

	CleanupStack::PopAndDestroy(5); // buf8, store, event, event1, event2
	::DeleteDataFile(storename);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1010
@SYMTestCaseDesc	    Tests for CLogEvent::NewL(),CLogEvent::SetDataL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for setting event specific data read from the file and try to read the data back from the event.
                        Check for memory errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEventWithFileFailL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1010 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// streaming
	TFileName storename = _L("c:\\T_BASIC_DATA");
	TUid uid={0x12345678};

	// create a store
	theFs.Delete(storename);
	CDictionaryFileStore* store = CDictionaryFileStore::OpenLC(theFs, storename, uid);
	
	RDictionaryWriteStream write;
	RDictionaryReadStream read;

	uid.iUid++;
	write.AssignL(*store, uid);
	write << KNullDesC8;
	write.CommitL();
	write.Close();

	read.OpenL(*store, uid);

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	theFs.SetErrorCondition(KErrGeneral, 0);
	event->SetDataL(read, 0);
	theFs.SetErrorCondition(KErrNone, 10000);

	read.Close();
	TEST(event->Data() == KNullDesC8);

	HBufC8* buf8 = TestUtils::CreateBuf8LC(100);

	uid.iUid++;
	write.AssignL(*store, uid);
	write.WriteL(buf8->Des());
	write.CommitL();
	write.Close();

	while(!finished)
		{
		read.OpenL(*store, uid);
		theFs.SetErrorCondition(KErrGeneral, failCount++);

		TRAP(error, event->SetDataL(read, 100));

		theFs.SetErrorCondition(KErrGeneral, 10000);

		read.Close();

		if (error == KErrNone)
			{
			TEST(event->Data() == buf8->Des());
			finished = ETrue;
			}
		else
			{
			TEST2(error, KErrGeneral);
			TEST(event->Data() == KNullDesC8);
			}
		}

	CleanupStack::PopAndDestroy(3); // buf8, store, event
	::DeleteDataFile(storename);
	}

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logevent"));

	TheTest.Start(_L("Event"));
//	TestServerL();
	

	TestEventL();
	theLog.Write(_L8("Test 1 OK\n"));

	TheTest.Next(_L("Event with heap failure"));
	TestEventWithHeapFailL();
	theLog.Write(_L8("Test 2 OK\n"));

	TheTest.Next(_L("Event with file failure"));
	TestEventWithFileFailL();
	theLog.Write(_L8("Test 3 OK\n"));
	}

