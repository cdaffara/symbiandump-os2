// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TA_ARRAY.CPP
// 
//

#include <e32test.h>
#include <basched.h>
#include <f32file.h>
#include <centralrepository.h>

#include <bassnd.h>
#include "BASSNDUID.h"
#include "BASSNDPriorityDecoder.h"

LOCAL_D RTest test(_L("T_SSND"));
RFs TheFs;
TBool TestingOOM = EFalse;

const TUid KRing1={100};
const TUid KRing2={101};
const TUid KAlarm1={201};
const TUid KAlarm2={202};
const TUid KMessage1={203};

_LIT(KSndNameWarble,"c:\\system\\data\\warble.wav");
_LIT(KSndNameMozart,"c:\\system\\data\\mozart.wav");
_LIT(KSndNameBach,"c:\\system\\data\\bach.wav");
_LIT(KSndNameVivaldi,"c:\\system\\data\\vivaldi.wav");

#define TEST2(err, cond) \
	{\
	if (TestingOOM && err == KErrNoMemory) \
		{\
		User::LeaveNoMemory();\
		}\
	test(cond);\
	}

// Delete all entries in sound table
LOCAL_C void ClearSoundTableL()
	{
	CRepository *repository = CRepository::NewLC(KSystemSoundRepositoryUID);
	repository->Reset();
	CleanupStack::PopAndDestroy( repository );
	}

LOCAL_C void TestSetSounds1L()
	{
	TBaSystemSoundType sndType(KSystemSoundRingUID,KRing1);
	TBaSystemSoundInfo sndInfo(sndType,KSndNameWarble());
	BaSystemSound::SetSoundL(TheFs,sndInfo);
	}

LOCAL_C void TestSetSounds2L()
	{
	TBaSystemSoundType sndType(KSystemSoundRingUID,KRing2);
	TBaSystemSoundInfo sndInfo(sndType,KSndNameMozart());
	BaSystemSound::SetSoundL(TheFs,sndInfo);
	}

LOCAL_C void TestSetSounds3L()
	{
	TBaSystemSoundType sndType(KSystemSoundAlarmUID,KAlarm1);
	TBaSystemSoundInfo sndInfo(sndType,KSndNameBach());
	sndInfo.iVolume=100;
	sndInfo.iPriority=TBaSoundPriorityEncoder(16).Int();
	BaSystemSound::SetSoundL(TheFs,sndInfo);
	}

LOCAL_C void TestSetSounds4L()
	{
	TBaSystemSoundType sndType(KSystemSoundAlarmUID,KAlarm2);
	TBaSystemSoundInfo sndInfo(sndType,KSndNameVivaldi());
	sndInfo.iVolume=83;
	TBaSoundPriorityEncoder priority(-100,(TMdaPriorityPreference(0xFF030001)));
	sndInfo.iPriority=priority.Int();
	BaSystemSound::SetSoundL(TheFs,sndInfo);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-1286
@SYMTestCaseDesc        Tests for BaSystemSound::SetSoundL() function
@SYMTestPriority        High
@SYMTestActions         Tests for KErrArgument,when TSoundCategory is passed with out being set as a tone
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestSetSounds5L()
	{
	// Testing SetSoundL.
	// Should leave with KErrArgument when TSoundCategory of sndInfo
	// parameter is passed with out being set either as a tone, a file
    // or a sequence.
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1286 "));
	TBaSystemSoundInfo sndInfo;
	TRAPD(err,BaSystemSound::SetSoundL(TheFs,sndInfo));
	TEST2(err, err==KErrArgument);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-3395
@SYMTestCaseDesc        Tests for BaSystemSound::SetSound() function
@SYMTestPriority        High
@SYMTestActions         Set sounds of different types: file, tone and seq
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF095280
*/
LOCAL_C void TestSetSoundsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-3395 "));
	// split into stupidly small chunks to avoid _chkstk errors

	//set sounds
	TestSetSounds1L();
	TestSetSounds2L();
	TestSetSounds3L();
	TestSetSounds4L();
	TestSetSounds5L();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-1287
@SYMTestCaseDesc        Tests for BaSystemSound::GetSound() function
@SYMTestPriority        High
@SYMTestActions         Attempt for getting back sounds
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF095280
*/
LOCAL_C void TestGetSoundsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1287 "));
	TBaSystemSoundInfo info;

	//get sounds
	//should return warble
	TBaSystemSoundType sndType(KSystemSoundRingUID,KRing1);
	TInt e=BaSystemSound::GetSound(TheFs,sndType,info);
	TEST2(e, e==KErrNone);
	test(info.FileName()==KSndNameWarble);
	//should return mozart
	sndType=TBaSystemSoundType(KSystemSoundRingUID,KRing2);
	e=BaSystemSound::GetSound(TheFs,sndType,info);
	TEST2(e, e==KErrNone);
	test(info.FileName()==KSndNameMozart);
	//should return bach
	sndType=TBaSystemSoundType(KSystemSoundAlarmUID,KAlarm1);
	e=BaSystemSound::GetSound(TheFs,sndType,info);
	TEST2(e, e==KErrNone);
	test(info.FileName()==KSndNameBach);
	test(info.iVolume=100);
	TBaSoundPriorityDecoder priorityDecode(info.iPriority);
	test(priorityDecode.Priority()==16);
	test(priorityDecode.PriorityPreference()==EMdaPriorityPreferenceTimeAndQuality);
	//should rturn vivaldi
	sndType=TBaSystemSoundType(KSystemSoundAlarmUID,KAlarm2);
	e=BaSystemSound::GetSound(TheFs,sndType,info);
	TEST2(e, e==KErrNone);
	test(info.FileName()==KSndNameVivaldi);
	test(info.iVolume=83);
	priorityDecode=TBaSoundPriorityDecoder(info.iPriority);
	test(priorityDecode.Priority()==-100);
	test(priorityDecode.PriorityPreference()==(TMdaPriorityPreference(0xFF030001)));
	}

// ---------------------------------------------------
// OomTest
//
// Function to convert a test into an OOM test

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-1288
@SYMTestCaseDesc        Out of memory tests
@SYMTestPriority        High
@SYMTestActions         Turns a basic test function into OOM test.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF095280
@SYMREQ                 REQ0000
*/
LOCAL_C void OomTest( void (*testFuncL)() )
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1288 "));
	TInt error;
	TestingOOM = ETrue;
	TInt count = 0;

	do
		{
		User::__DbgMarkStart( RHeap::EUser );

		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		User::__DbgSetAllocFail( RHeap::EUser, RHeap::EFailNext, ++count );

		TRAP( error, (testFuncL)() );

		User::__DbgSetAllocFail( RHeap::EUser, RHeap::ENone, 1 );

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		User::__DbgMarkEnd( RHeap::EUser, 0 );
		} while( error == KErrNoMemory );

	TestingOOM = EFalse;

	_LIT( KTestFailed, "Out of memory test failure on iteration %d\n" );
	__ASSERT_ALWAYS( error == KErrNone, test.Panic( error, KTestFailed, count ) );

	test.Printf( _L( "Out of memory tests succeeded at heap failure rate of %i\n" ), count );
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-3397
@SYMTestCaseDesc        When a client requests a non-existing sound
                        which is in one of the system category,
						BaSystemSound returns the default sound for
						that system category.
@SYMTestPriority        High
@SYMTestActions         With the sound table empty, get sound in the
						alarm, message and ring categories.
@SYMTestExpectedResults The default sound for the category is returned.
@SYMDEF					DEF095280
*/
LOCAL_C void GetNonExistingSoundL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-3397 "));
	TBaSystemSoundInfo info;
	TInt ret(KErrNone);

	//get sound empty file
	TBaSystemSoundType sndType(KSystemSoundAlarmUID,KAlarm1);
	ret = BaSystemSound::GetSound(TheFs,sndType,info);
	TEST2(ret, ret==KErrNotFound);
	_LIT(KSystemSoundDefaultAlarm,"z:\\system\\SystemSounds\\alarm.wav");
	test(info.FileName() == KSystemSoundDefaultAlarm);

	sndType.iMajor = KSystemSoundMessageUID;
	sndType.iMinor = KMessage1;
	ret = BaSystemSound::GetSound(TheFs,sndType,info);
	TEST2(ret, ret==KErrNotFound);
	_LIT(KSystemSoundDefaultMessage,"z:\\system\\SystemSounds\\message.wav");
	test(info.FileName() == KSystemSoundDefaultMessage);

	sndType.iMajor = KSystemSoundRingUID;
	sndType.iMinor = KRing1;
	ret = BaSystemSound::GetSound(TheFs,sndType,info);
	TEST2(ret, ret==KErrNotFound);
	_LIT(KSystemSoundDefaultRing,"z:\\system\\SystemSounds\\ring.wav");
	test(info.FileName() == KSystemSoundDefaultRing);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-3396
@SYMTestCaseDesc        Test CBaSystemSoundArray APIs.
@SYMTestPriority        High
@SYMTestActions         Use CBaSystemSoundArray to retrieve an entire
						sound category and step through each sound in
						the array.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF095280
*/
LOCAL_C void GetSoundArrayL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-3396 "));
	CBaSystemSoundArray* sa= CBaSystemSoundArray::NewLC();
	sa->RestoreL(TheFs,KSystemSoundRingUID);

	TInt count = sa->Count();
	test(count==2);
	TBaSystemSoundInfo i0 = sa->At(0);
	test(i0.iType.iMinor==KRing1);
	test(i0.FileName()==KSndNameWarble);
	TBaSystemSoundInfo i1 = sa->At(1);
	test(i1.iType.iMinor==KRing2);
	test(i1.FileName()==KSndNameMozart);

	CleanupStack::PopAndDestroy(); // sa
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0441
@SYMTestCaseDesc        Tests for the functionality of TBaSystemSoundInfo
@SYMTestPriority        High
@SYMTestActions         Calls up BaSystemSound test functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void DoTestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0441 "));
	//delete all sounds
	ClearSoundTableL();

	test.Next(_L("Get non-existing sound"));
	GetNonExistingSoundL();

	test.Next(_L("Setting sounds"));
	TestSetSoundsL();

	test.Next(_L("Getting back sounds\n"));
	TestGetSoundsL();

	test.Next(_L("CBaSystemSoundArray access"));
	GetSoundArrayL();

	//delete all sounds
	ClearSoundTableL();

	// OOM test
	test.Next(_L("OOM Test get non-existing sound"));
	OomTest(GetNonExistingSoundL);

	test.Next(_L("OOM Test Setting sounds"));
	OomTest(TestSetSoundsL);

	test.Next(_L("OOM Test Getting back sounds"));
	OomTest(TestGetSoundsL);

	test.Next(_L("OOM Test CBaSystemSoundArray access"));
	OomTest(GetSoundArrayL);

	// cleanup
	ClearSoundTableL();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CActiveScheduler::Install(new CActiveScheduler);
	CTrapCleanup* cleanup=CTrapCleanup::New();
	test.Title();
//	test.RunManual();
	test.Start(_L("Testing system sounds"));
	TRAPD(err,DoTestsL());
	test(err==KErrNone);
	test.End();
	test.Close();
	delete cleanup;
	delete CActiveScheduler::Current();
	__UHEAP_MARKEND;
	return(0);
	}
