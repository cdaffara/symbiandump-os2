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
// Tests for TBaSystemSoundType and TBaSystemSoundInfo class
// 
//

#include <e32test.h>
#include <basched.h>
#include <f32file.h>
#include <s32file.h>
#include <bassnd.h>

RTest test(_L("T_SSND1"));
RFs TheFs;

const TUid KRing1 = {100};
const TUid KRing2 = {101};

_LIT(KSndNameWarble,"c:\\system\\data\\warble.wav");
_LIT(KSndNameMozart,"c:\\system\\data\\mozart.wav");

/**
@SYMTestCaseID          SYSLIB-BAFL-UT-1707
@SYMTestCaseDesc        Testing the functions of TBaSystemSoundType and TBaSystemSoundInfo class
                        Tests for TBaSystemSoundInfo::SetTone(),TBaSystemSoundInfo::SetFixedSequenceNumber(),TBaSystemSoundInfo::TBaSystemSoundInfo(TTone) and TBaSystemSoundType::operator==() function
@SYMTestPriority        Medium
@SYMTestActions         Compare two system sound types, set the tone and fixed sequence number
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void TestSoundsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-UT-1707 Setting sounds "));

	TBaSystemSoundType sndType1(KSystemSoundRingUID,KRing1);
	TBaSystemSoundInfo sndInfo1(sndType1,KSndNameWarble());
	BaSystemSound::SetSoundL(TheFs,sndInfo1);
	TBaSystemSoundType sndType2(KSystemSoundRingUID,KRing2);
	TBaSystemSoundInfo sndInfo2(sndType2,KSndNameMozart());
	BaSystemSound::SetSoundL(TheFs,sndInfo2);

	TBaSystemSoundType sndType3(KSystemSoundRingUID,KRing1);
	TBaSystemSoundInfo sndInfo3(sndType3,KSndNameWarble());
	BaSystemSound::SetSoundL(TheFs,sndInfo3);

	// Comparing two system sound types

	test(!(sndType1==sndType2));
	test(sndType1==sndType3);

	// Setting with TTone

	TBaSystemSoundInfo::TTone tone(120,3400);
	TBaSystemSoundType sndType4;
	TBaSystemSoundInfo sndInfo4(sndType4,tone);

	TBaSystemSoundInfo::TTone tone1(140,3200);

	sndInfo4.SetFixedSequenceNumber(1);
	TInt seqNo = sndInfo4.FixedSequenceNumber();
	test(seqNo==1);
	sndInfo4.SetTone(tone1);
	test(!(sndType4==sndType3));
	}

void DoTestsL()
	{
	TheFs.Connect();
	CleanupClosePushL(TheFs);

	TestSoundsL();

	CleanupStack::PopAndDestroy(); // TheFs
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
	test.Start(_L("Testing system sounds"));
	TRAPD(err,DoTestsL());
	test(err==KErrNone);
	test.End();
	test.Close();
	delete cleanup;

	__UHEAP_MARKEND;

	return(0);
	}
