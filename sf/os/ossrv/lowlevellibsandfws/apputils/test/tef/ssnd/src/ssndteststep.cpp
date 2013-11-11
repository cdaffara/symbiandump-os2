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

/**
 @file
 @internalComponent
*/

#include <e32property.h>
#include <connect/sbdefs.h>
#include <centralrepository.h>
#include <baflpan.h>
#include <bassnd.h>
#include "ssndteststep.h"
#include "BaSsndStore.h"
#include "BASSNDUID.h"

const TBaSystemSoundUid KLockoutTestSoundCategory = {288};

/** CResetDbStep constructor */
CResetDbStep::CResetDbStep()
	{
	SetTestStepName(KResetDb);
	}

/** Implementation of the pure virtual base class function.
This teststep does not really test anything. It serves as
a helper of other steps to clear the sound table at the 
beginning of a test.
*/
TVerdict CResetDbStep::doTestStepL()
	{
	CRepository* repository = CRepository::NewLC(KSystemSoundRepositoryUID);
	TESTL(repository != NULL);
	TEST(repository->Reset() == KErrNone);

	CleanupStack::PopAndDestroy(repository);
	return TestStepResult();
	}

//
// GetSound/SetSound
//
_LIT(KUnknownParm, "Err reading INI section %S, unknown value %S");

_LIT(KSoundCat, "Category");
_LIT(KEFile, "EFile");
_LIT(KESequence, "ESequence");
_LIT(KETone, "ETone");

_LIT(KSoundFileName, "Filename");
_LIT(KFixedSequence, "SequenceNum");
_LIT(KToneFrequency, "Frequency");
_LIT(KToneDuration, "Duration");

_LIT(KMajorUid, "MajorUid");
_LIT(KMinorUid, "MinorUid");

/** Read data from the TEF ini file to construct iSound. */
void CSsndBaseStep::ConstructSoundL()
	{
	TPtrC cat;
	TPtrC ssndFile;
	TInt toneFreq;
	TTimeIntervalMicroSeconds32 duration;
	TInt intdata;

	TESTL( GetStringFromConfig(ConfigSection(), KSoundCat, cat) );
	if (cat == KEFile())
		{
		TESTL( GetStringFromConfig(ConfigSection(), KSoundFileName, ssndFile) );
		TBaSystemSoundName filebuf(ssndFile);
		iSound.SetFileName(filebuf);
		}
	else if (cat == KESequence())
		{
		TESTL( GetIntFromConfig(ConfigSection(), KFixedSequence, intdata) );
		iSound.SetFixedSequenceNumber(intdata);
		}
	else if (cat == KETone())
		{
		TESTL( GetIntFromConfig(ConfigSection(), KToneFrequency, toneFreq) );
		TESTL( GetIntFromConfig(ConfigSection(), KToneDuration, intdata) );
		duration = intdata;
		TBaSystemSoundInfo::TTone tone(toneFreq, duration);
		iSound.SetTone(tone);
		}
	else
		{
		ERR_PRINTF3(KUnknownParm, &KSoundCat, &cat);
		TESTL(EFalse);
		}

	TESTL( GetIntFromConfig(ConfigSection(), KMajorUid, intdata) );
	iSound.iType.iMajor.iUid = intdata;
	TESTL( GetIntFromConfig(ConfigSection(), KMinorUid, intdata) );
	iSound.iType.iMinor.iUid = intdata;
	}

/** CGetSoundStep constructor */
CGetSoundStep::CGetSoundStep()
	{
	SetTestStepName(KGetSound);
	}

/** Implement the pure virtual base class function.
Read sound from repository and compare it against
the data specified in TEF ini file.
*/
TVerdict CGetSoundStep::doTestStepL()
	{
	ConstructSoundL();
	RFs dummy;

	TInt ret = BaSystemSound::GetSound(dummy, iSound.iType, iReadFromRep);
	if (ret != KErrNone)
		{
		// Do it this way for B&R test.
		SetTestStepError(ret);
		return EFail;
		}

	TBaSystemSoundInfo::TSoundCategory cat = iReadFromRep.SoundCategory();
	if (cat == TBaSystemSoundInfo::ETone)
		{
		TEST(iSound.Tone().iFrequency == iReadFromRep.Tone().iFrequency);
		TEST(iSound.Tone().iDuration == iReadFromRep.Tone().iDuration);
		}
	else if (cat == TBaSystemSoundInfo::ESequence)
		{
		TEST(iSound.FixedSequenceNumber() == iReadFromRep.FixedSequenceNumber());
		}
	else if (cat == TBaSystemSoundInfo::EFile)
		{
		TEST(iSound.FileName() == iReadFromRep.FileName());
		}
	return TestStepResult();
	}

/** CSetSoundStep constructor */
CSetSoundStep::CSetSoundStep()
	{
	SetTestStepName(KSetSound);
	}

/** Implementation of pure virtual base class method.
Call SetSoundL and return pass if KErrNone, fail otherwise.
*/
TVerdict CSetSoundStep::doTestStepL()
	{
	ConstructSoundL();
	RFs dummy;
	TRAPD(ret, BaSystemSound::SetSoundL(dummy, iSound));
	if (ret != KErrNone)
		{
		// SetTestStepError pass the error code back to TEF.
		// If the script is expecting this particular error code,
		// TEF will flip the EFail into EPass.
		SetTestStepError(ret);
		return EFail;
		}
	return EPass;
	}

/** CSystemSoundFileTestStep constructor */
CSystemSoundFileTestStep::CSystemSoundFileTestStep()
	{
	SetTestStepName(KSystemSoundFileTest);
	}

/** Implementation of pure virtual base class method.
Call the SystemSoundFile API and check the returned
filename is the old sound table file.
NB: the SystemSoundFile API has been deprecated.
*/
TVerdict CSystemSoundFileTestStep::doTestStepL()
	{
	_LIT(KExpectedName, ":\\system\\data\\syssnd.dat");
	TFileName fn = BaSystemSound::SystemSoundFile();
	TInt pos = fn.FindF(KExpectedName);
	TEST(pos != KErrNotFound);
	return TestStepResult();
	}

/** CGapTestStep constructor */
CGapTestStep::CGapTestStep()
	{
	SetTestStepName(KGapTest);
	}

/** Implementation of pure virtual base class method.
Create a setting at key 7 leaving a gap at key 3. 
Then call SetSoundL. The call should fail with KErrCorrupt.
*/
TVerdict CGapTestStep::doTestStepL()
	{
	const TInt32 KKeyWithGap = 7;
	const TInt KSomeInteger = 1;
	CRepository* rep = CRepository::NewLC(KSystemSoundRepositoryUID);
	TESTL(rep->Create(KKeyWithGap, KSomeInteger) == KErrNone);
	
	TUid minorUid = {KSomeInteger};
	TBaSystemSoundType soundId(KLockoutTestSoundCategory, minorUid);
	TBaSystemSoundInfo sound(soundId, KSomeInteger);

	RFs dummy;
	TRAPD(ret, BaSystemSound::SetSoundL(dummy, sound));
	TEST(ret == KErrCorrupt);

	CleanupStack::PopAndDestroy(rep);	
	return TestStepResult();
	}

/** CSoundFileNoPathTestStep constructor */
CSoundFileNoPathTestStep::CSoundFileNoPathTestStep()
	{
	SetTestStepName(KSoundFileNoPathTest);
	}

/** Implementation of pure virtual base class method.
Call SetSoundL with a wav filename which does not contain path.
BaSystemSound should panic.
*/
TVerdict CSoundFileNoPathTestStep::doTestStepL()
	{
// BaSystemSound only trigger EBafPanicSystemSoundNoPath on udeb.
// So on urel do it ourself.
#ifndef _DEBUG
	Panic(EBafPanicSystemSoundNoPath);
#else
	const TBaSystemSoundUid KMajorUid = {0x888};
	TBaSystemSoundType soundId(KMajorUid);

	// side bar to fill a code coverage gap in TTone::IsNull 
	TTimeIntervalMicroSeconds32 duration = 10;
	TBaSystemSoundInfo::TTone tone(0, duration);
	TBaSystemSoundInfo sound(soundId, tone);
	TBaSystemSoundInfo::TSoundCategory cat = sound.SoundCategory();
	TEST(cat == TBaSystemSoundInfo::ETone);
	
	// now back to the panic test
	_LIT(KSoundFileName, "rr.wav");
	TBaSystemSoundName filename(KSoundFileName);
	sound.SetFileName(filename);

	RFs dummy;
	// Following line should panic
	BaSystemSound::SetSoundL(dummy, sound);
#endif
	return TestStepResult();
	}

/** CCorruptSoundStep constructor */
CCorruptSoundStep::CCorruptSoundStep()
	{
	SetTestStepName(KCorruptSound);
	}

/** Implementation of pure virtual base class method.
Corrupt the soundtype, sound cat. and path length fields
in the sound data and verify GetSound returns KErrCorrupt.
*/
TVerdict CCorruptSoundStep::doTestStepL()
	{
	ConstructSoundL();
	RFs dummy;
	TInt ret(KErrNone);
	BaSystemSound::SetSoundL(dummy, iSound);
	
	// make a connection to CentRep to directly manipulate the
	// setting.
	CRepository* rep = CRepository::NewLC(KSystemSoundRepositoryUID);
	HBufC8* orighbuf8 = HBufC8::NewLC(sizeof(TBaSystemSoundInfo));
	TPtr8 origbuf8 = orighbuf8->Des();

	const TUint32 KKeyOfFirstSound = 3;
	ret = rep->Get(KKeyOfFirstSound, origbuf8);
	TESTL(ret == KErrNone);

	// need a running copy of the sound data
	HBufC8* temphbuf8 = orighbuf8->AllocLC();
	TPtr8 tempbuf8 = temphbuf8->Des();

	//Test corrupting the major sound Id
	tempbuf8[0] = 1;
	tempbuf8[1] = 2;
	tempbuf8[2] = 3;
	tempbuf8[3] = 4;
	TESTL(rep->Set(KKeyOfFirstSound, tempbuf8) == KErrNone);
	ret = BaSystemSound::GetSound(dummy, iSound.iType, iReadFromRep);
	TEST(ret == KErrCorrupt);

	//Test corrupting the sound cat. field
	//First restore original data.
	tempbuf8.Copy(origbuf8);
	const TInt KSoundCatLocation = 16;
	tempbuf8[KSoundCatLocation] = 255;
	tempbuf8[KSoundCatLocation+1] = 127;
	TESTL(rep->Set(KKeyOfFirstSound, tempbuf8) == KErrNone);
	ret = BaSystemSound::GetSound(dummy, iSound.iType, iReadFromRep);
	TEST(ret == KErrCorrupt);

	//Test corrupting the path length field
	//First restore original data.
	tempbuf8.Copy(origbuf8);
	const TInt KPathLengthLocation = 20;
	tempbuf8[KPathLengthLocation] = 255;
	tempbuf8[KPathLengthLocation+1] = 127;
	TESTL(rep->Set(KKeyOfFirstSound, tempbuf8) == KErrNone);
	ret = BaSystemSound::GetSound(dummy, iSound.iType, iReadFromRep);
	TEST(ret == KErrCorrupt);
	
	//Corruption of other fields are not detectable.

	//If we restore the original data in CentRep, GetSound should pass.
	tempbuf8.Copy(origbuf8);
	TESTL(rep->Set(KKeyOfFirstSound, tempbuf8) == KErrNone);
	ret = BaSystemSound::GetSound(dummy, iSound.iType, iReadFromRep);
	TEST(ret == KErrNone);

	CleanupStack::PopAndDestroy(temphbuf8);	
	CleanupStack::PopAndDestroy(orighbuf8);	
	CleanupStack::PopAndDestroy(rep);	
	return TestStepResult();
	}

/** Derive from CBaSsndStore to force collision between
concurrent set sound threads.
*/
class CCollisionSsndStore : public CBaSsndStore
	{
public:
	static CCollisionSsndStore* NewLC(TTimeIntervalMicroSeconds32 aDelay);
	virtual void KeyOfSoundTypeL(TBaSystemSoundType& aSoundType, TUint32& aKey) const;
	virtual void SetSoundInTransactionL(TBaSystemSoundType& aSoundType,
										const TDesC8& aDes8) const;
	inline TUint32 NumCollisions() const {return iNumCollisions;}
	inline TUint32 Key() const {return iKey;}

private:
	inline CCollisionSsndStore(const TTimeIntervalMicroSeconds32 aDelay)
		: iDelay(aDelay), iNumCollisions(0), iKey(NCentralRepositoryConstants::KUnspecifiedKey) {}
	inline void ConstructL() {CBaSsndStore::ConstructL();}
	TInt RecordKeyPosition(TBaSystemSoundType& aSoundType) const;

private:
	/** indicate how long to pause in the middle of the transaction.
	 The pause causes concurrent transactions to fail. */
	TTimeIntervalMicroSeconds32 iDelay;

	/** Record how many times transaction commit failed. Mutable because
	 SetSoundInTransactionL in production code is a const method. */
	mutable TUint32 iNumCollisions;

	/** Record the CentRep key used to store the sound. Mutable because
	 KeyOfSoundTypeL in production code is a const method. */
	mutable TUint32 iKey;
	};

/** static factory method to instantiate CCollisionSsndStore */
CCollisionSsndStore* CCollisionSsndStore::NewLC(
	const TTimeIntervalMicroSeconds32 aDelay)
	{
	CCollisionSsndStore* self = new (ELeave) CCollisionSsndStore(aDelay);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/** After using base class method to get the key, add a delay
to cause concurrent threads to collide.
*/
void CCollisionSsndStore::KeyOfSoundTypeL(TBaSystemSoundType& aSoundType,
										  TUint32& aKey) const
	{
	CBaSsndStore::KeyOfSoundTypeL(aSoundType, aKey);
	User::AfterHighRes(iDelay);
	}

/** After using base class method to do SetSound in transaction,
check the return code. Record commitFail condition. If succes,
record the key position where the SoundType is stored.
*/
void CCollisionSsndStore::SetSoundInTransactionL(
		TBaSystemSoundType& aSoundType,
		const TDesC8& aDes8) const
	{
	// call base class method to do the real work
	TRAPD(ret, CBaSsndStore::SetSoundInTransactionL(aSoundType, aDes8));

	// record any collision
	if (KErrLocked == ret || KErrAbort == ret)
		{
		iNumCollisions++;
		}
	else if (KErrNone == ret)
		{
		ret = RecordKeyPosition(aSoundType);
		}
	else if (KErrNoMemory != ret)
		{
		RDebug::Print(_L("tef_ssnd test: unexpected error %d"), ret);
		}

	User::LeaveIfError(ret);
	}

/** Provide similar function of CBaSsndStore::FindKeyL.
FindKeyL ignores duplicate SoundType in repository. It just
returns the first key in the array. Test harness cares because
it should fail the test if duplicate entries are detected.
*/
TInt CCollisionSsndStore::RecordKeyPosition(TBaSystemSoundType& aSoundType) const
	{
	const TUint32 KSoundTypePartialKey = 0x2;
	const TUint32 KSsndKeyMask = 0x3;
	TPckg<TBaSystemSoundType> target(aSoundType);
	RArray<TUint32> foundIds;

	TInt ret(KErrNone);
	TRAPD(err, ret = iRepository->FindEqL(KSoundTypePartialKey,
		KSsndKeyMask, target, foundIds));
	if (err != KErrNone)
		{
		ret = err; // KErrNoMemory
		}

	if (ret == KErrNone)
		{
		iKey = foundIds[0];
		if (foundIds.Count() > 1)
			{
			ret = KErrWrite;
			}

		foundIds.Reset();
		}

	return ret;
	}

//
// Concurrent SetSoundL teststep
//

_LIT(KNumWorkers, "NumWorkers");
_LIT(KWorkerId, "WorkerId");
_LIT(KSameSound, "SameSsndId");

const TTimeIntervalMicroSeconds32 KCollisionDelay = 500000;
const TInt KMaxNumWorkers = 3;
TWorkerData ConcurrentResults[KMaxNumWorkers];

/** CConcurrentStep constructor */
CConcurrentStep::CConcurrentStep()
	{
	SetTestStepName(KConcurrentSetSound);
	}

/** This postamble insures that if a worker thread leaves in
doTestStepL, the iDone flag is still set.
*/
TVerdict CConcurrentStep::doTestStepPostambleL()
	{
	if (iWorkerId && iWorkerId <= KMaxNumWorkers)
		{
		ConcurrentResults[iWorkerId - 1].iDone = ETrue;
		}

	// To silent Leave scan.
	CTestStep::doTestStepPostambleL();

	return TestStepResult();
	}

/** Implementation of pure virtual base class method.
Read from config file to determine if the current thread is 
master or worker. Workers invoke CCollisionSsndStore::SetSoundL.
Master summarises the test results.
*/
TVerdict CConcurrentStep::doTestStepL()
	{
	const TDesC& section = ConfigSection();
	if (!GetIntFromConfig(section, KWorkerId, iWorkerId))
		{
		TInt numWorkers;
		// This is the master thread. It does not call the
		// SetSoundL API. It just sets up the concurrent data,
		// wait for the test to complete, and tally up results.
		TESTL( GetIntFromConfig(section, KNumWorkers, numWorkers) );
		TBool sameSsnd = section.FindF(KSameSound) >= 0;
		DoMasterTaskL(numWorkers, sameSsnd);
		return TestStepResult();
		}

	// Getting here means this is a worker thread.
	TESTL(iWorkerId <= KMaxNumWorkers);

	// pause a bit to ensure master and other workers are
	// all started.
	User::AfterHighRes(200000);

	ConstructSoundL();
	CCollisionSsndStore* ssndCollisionSet = 
		CCollisionSsndStore::NewLC(KCollisionDelay);
	ssndCollisionSet->SetSoundL(iSound);

	TWorkerData& mySlot = ConcurrentResults[iWorkerId - 1];
	mySlot.iTranFailCount = ssndCollisionSet->NumCollisions();
	mySlot.iKey = ssndCollisionSet->Key();
	mySlot.iDone = ETrue;

	CleanupStack::PopAndDestroy(ssndCollisionSet);	
	return TestStepResult();
	}

/** Set up global data and then wait for the concurrent
worker threads. When workers all done, verify that 
1. collisions did occur and
2. collisions did not cause errors in generating the hash keys.
*/
void CConcurrentStep::DoMasterTaskL(TInt aNumWorkers,
								   TBool aSsndIdAreSame)
	{
	if (aNumWorkers > KMaxNumWorkers)
		{
		// If there is valid reason to test more than 3 concurrent
		// threads, CBaSsndStore::SetSoundL needs to bump up the
		// retry count. 3 retries in production code can handle many
		// concurrent threads. But in testing collision is guranteed
		// to happen. The retry count needs to be same as number of
		// workers.
		_LIT(KTooMany, "Not allow to have more than %d concurrent workers.");
		ERR_PRINTF2(KTooMany, KMaxNumWorkers);
		TESTL(EFalse);
		}

	TWorkerData nullData;
	TInt i;
	for (i = 0; i < aNumWorkers; i++)
		{
		ConcurrentResults[i] = nullData;
		}

	// Probably should convert KCollisionDelay.Int() to seconds
	// and multiply by num of workers. But 12 seconds for max of 3
	// workers should be plenty.
	TInt j;
	TInt doneCount = 0;
	TTimeIntervalMicroSeconds32 halfSec(500000);
	const TInt KNumLoops = 24;
	for (i = 0; i < KNumLoops && doneCount < aNumWorkers; i++)
		{
		User::AfterHighRes(halfSec);

		// count number of dones.
		for (j = 0, doneCount = 0; j < aNumWorkers; j++)
			{
			if (ConcurrentResults[j].iDone)
				{
				doneCount++;
				}
			}
		}

	TEST(doneCount == aNumWorkers);

	// If there are 3 workers, expect 2+1 = 3 collisions.
	// But to keep things simple, just use (aNumWorkers - 1)
	TInt minCollisions = aNumWorkers - 1;
	TInt numCommitFails = 0;
	for (j = 0; j < aNumWorkers; j++)
		{
		numCommitFails += ConcurrentResults[j].iTranFailCount;
		}

	TEST(numCommitFails >= minCollisions);

	// Next check the sounds are correctly created in CentRep.
	for (i = 0; i < (aNumWorkers - 1); i++)
		{
		TUint32 key = ConcurrentResults[i].iKey;

		if (aSsndIdAreSame)
			{
			TEST(key == ConcurrentResults[i+1].iKey);
			}
		else
			{
			for (j = i+1; j < aNumWorkers; j++)
				{
				TEST(key != ConcurrentResults[j].iKey);
				}
			}
		}

	}

/** CLockoutTestStep constructor */
CLockoutTestStep::CLockoutTestStep()
	{
	SetTestStepName(KLockoutTest);
	}

/** Implementation of pure virtual base class method.
Backup or Restore operation is running in parallel.
This test waits for the P&S KUidBackupRestoreKey become
active and then call SetSoundL and CBaSystemSoundArray::RestoreL.
Expects KErrServerBusy/KErrAbort/KErrLocked to be returned.
*/
TVerdict CLockoutTestStep::doTestStepL()
	{
using namespace conn;

	const TBool KLogOnError = ETrue;

	RProperty burKey;
	TInt ret = burKey.Attach(KUidSystemCategory, KUidBackupRestoreKey);
	TEST1L(ret == KErrNone, KLogOnError);

	TRequestStatus s;
	burKey.Subscribe(s);
	User::WaitForRequest(s);
	TEST1L(s.Int() == KErrNone, KLogOnError);

	TInt burKeyVal = 0;
	ret = burKey.Get(burKeyVal);
	TEST1L(ret == KErrNone, KLogOnError);

	burKeyVal &= KBURPartTypeMask;
	const TInt KBurInProgress = EBURBackupFull |
								EBURBackupPartial |
								EBURRestoreFull |
								EBURRestorePartial;
	TEST1L(burKeyVal & KBurInProgress, KLogOnError);

	burKey.Close();

	TRAP(ret, addSoundInstanceL(1));
	TEST1(ret == KErrServerBusy || ret == KErrAbort, KLogOnError);

	TRAP(ret, readSoundsBackL());
	if (burKeyVal & EBURRestoreFull || burKeyVal & EBURRestorePartial)
		{
	    TEST1(ret == KErrServerBusy, KLogOnError);
		}
	else
		{
	    TEST1(ret == KErrNone, KLogOnError);
		}

	return TestStepResult();
	}

/** Construct a sound and store it in repository.
@param aSequence used as both minor UID and sound sequence.
@leave any of the system-wide errors.
*/
void CLockoutTestStep::addSoundInstanceL(TInt32 aSequence)
	{
	TUid minorUid = {aSequence};
	TBaSystemSoundType soundId(KLockoutTestSoundCategory, minorUid);
	TBaSystemSoundInfo sound(soundId, aSequence);

	RFs dummy;
	BaSystemSound::SetSoundL(dummy, sound);
	}

/** use CBaSystemSoundArray::RestoreL to read back all
the sounds in KLockoutTestSoundCategory.
@leave any of the system-wide errors.
*/
void CLockoutTestStep::readSoundsBackL()
	{
	CBaSystemSoundArray* reader = CBaSystemSoundArray::NewLC();

	RFs dummy;
	reader->RestoreL(dummy, KLockoutTestSoundCategory);
	CleanupStack::PopAndDestroy(reader);
	}

