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

#ifndef __SSNDTESTSTEP_H__
#define __SSNDTESTSTEP_H__

#include <test/testexecutestepbase.h>
#include <bassnd.h>

/** CResetDbStep class resets the system sound
repository to factory default condition which is
an empty sound table.
*/
_LIT(KResetDb, "ResetDb");
class CResetDbStep : public CTestStep
	{
public:
	CResetDbStep();
	virtual TVerdict doTestStepL(void);
	};

/** This base class is common to teststeps which
need to read sound data from the TEF ini file to
construct the sound for testing.
*/
class CSsndBaseStep : public CTestStep
	{
protected:
	void ConstructSoundL(void);

protected:
	/** sound created with input from TEF ini file  */
	TBaSystemSoundInfo iSound;
	};

/**
CGetSoundStep class reads a sound instance from CentRep
and compares with expected values.
*/
_LIT(KGetSound, "GetSound");
class CGetSoundStep : public CSsndBaseStep
	{
public:
	CGetSoundStep();
	virtual TVerdict doTestStepL(void);

private:
	/** sound data read from CentRep */
	TBaSystemSoundInfo iReadFromRep;
	};

/** CSetSoundStep class sets a sound in repository.
*/
_LIT(KSetSound, "SetSound");
class CSetSoundStep : public CSsndBaseStep
	{
public:
	CSetSoundStep();
	virtual TVerdict doTestStepL(void);
	};

/**
CCorruptSoundStep class tests GetSound able to detect
corruption of the soundtype, sound cat. and path length fields.
*/
_LIT(KCorruptSound, "CorruptSound");
class CCorruptSoundStep : public CSsndBaseStep
	{
public:
	CCorruptSoundStep();
	virtual TVerdict doTestStepL(void);

private:
	/** sound data read from CentRep */
	TBaSystemSoundInfo iReadFromRep;
	};

/** CSystemSoundFileTestStep class exercise the SystemSoundFile API.
*/
_LIT(KSystemSoundFileTest, "SystemSoundFileTest");
class CSystemSoundFileTestStep : public CTestStep
	{
public:
	CSystemSoundFileTestStep();
	virtual TVerdict doTestStepL(void);
	};

/** CGapTestStep class tests BaSystemSound can detect rogue 
applications created entries which will interfere with SetSoundL.
*/
_LIT(KGapTest, "GapTest");
class CGapTestStep : public CTestStep
	{
public:
	CGapTestStep();
	virtual TVerdict doTestStepL(void);
	};

/** CSoundFileNoPathTestStep class tests BaSystemSound can detect rogue 
applications created entries which will interfere with SetSoundL.
*/
_LIT(KSoundFileNoPathTest, "SoundFileNoPathTest");
class CSoundFileNoPathTestStep : public CTestStep
	{
public:
	CSoundFileNoPathTestStep();
	virtual TVerdict doTestStepL(void);
	};

/** CLockoutTestStep class exercise the SetSoundL
and CBaSystemSoundArray::RestoreL APIs while backup/restore
is in progress to verify server lockout is handled correctly.
*/
_LIT(KLockoutTest, "LockoutTest");
class CLockoutTestStep : public CTestStep
	{
public:
	CLockoutTestStep();
	virtual TVerdict doTestStepL(void);
private:
	void addSoundInstanceL(TInt32 aSequence);
	void readSoundsBackL();
	};

/** TWorkerData struct is provided for concurrent SetSound threads
to record their results.
*/
struct TWorkerData
	{
	inline TWorkerData() : iTranFailCount(0), iKey(0), iDone(EFalse) {}

	/** how many times the set sound transaction commit fail */
	TInt	iTranFailCount;

	/** the CentRep key used by (assigned to) the sound */
	TUint32 iKey;

	/** indicates to the master thread that the worker has
	 completed the set sound task */
	TBool	iDone;
	};

/** CConcurrentStep class tests multiple threads calling
SetSoundL simultaneously. One of the threads running in paraellel
is assigned the master role. It does not call SetSoundL. It just
waits for the other threads to complete their calls and tallies 
the results.
*/
_LIT(KConcurrentSetSound, "ConcurrentSetSound");
class CConcurrentStep : public CSsndBaseStep
	{
public:
	CConcurrentStep();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
private:
	void DoMasterTaskL(TInt aNumWorkers, TBool aSsndIdAreSame);

private:
	/** Id of the worker threads. Also used as index to store test
	 results in the global array of TWorkerData.
	 @see TWorkerData
	 */
	TInt 	iWorkerId;
	};

#endif
