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

#ifndef t_logutil2_h
#define t_logutil2_h

#include <e32test.h>
#include <s32file.h>
#include <logcli.h>
#include <logengdurations.h>
#include <logengevents.h>
#include <logwraplimits.h>
#include <logfilterandeventconstants.hrh>
#include <e32base.h>
#include <f32file.h>
#include <logviewchangeobserver.h>
#include <logclientchangeobserver.h>
#include "LogChangeDefinition.h"
#include "LogServShared.h"
#include "LogCliServShared.h"
#include <babackup.h>
#include "t_logservsession.h"
#include "logcntdef.h"
#include "t_logutil.h"

#ifdef LOGGING_ENABLED

#include <f32file.h>
#include <flogger.h>



_LIT(KLogFileName, "LogEng.txt");
_LIT(KLogFolder,   "LogEng");
_LIT(KTimeFormat, "%02d.%02d:%02d:%06d    ");
_LIT(KTextFormat, "%S");
const TInt KLogEngLogBufferSize = 256;

class Log : public RFileLogger
	{
public:
	static void New();
	static void Write(const TDesC& aText);
	static void WriteFormat(TRefByValue<const TDesC> aFmt, ...);
private:
	static void PruneLogFile();
  };

#define LOGNEW							Log::New()
#define LOGTEXT(AAA)					{ _LIT(KString, AAA); Log::Write(KString); }
#define LOGTEXT2(AAA, BBB)				{ _LIT(KString, AAA); Log::WriteFormat(TRefByValue<const TDesC>(KString()), BBB); }
#define LOGTEXT3(AAA, BBB, CCC)			{ _LIT(KString, AAA); Log::WriteFormat(TRefByValue<const TDesC>(KString()), BBB, CCC); }
#define LOGTEXT4(AAA, BBB, CCC, DDD)	{ _LIT(KString, AAA); Log::WriteFormat(TRefByValue<const TDesC>(KString()), BBB, CCC, DDD); }

#else//LOGGING_ENABLED

#define LOGNEW
#define LOGTEXT(AAA)
#define LOGTEXT2(AAA, BBB)
#define LOGTEXT3(AAA, BBB, CCC)
#define LOGTEXT4(AAA, BBB, CCC, DDD)

#endif//LOGGING_ENABLED

// Globals 
GLREF_D CTrapCleanup* theCleanup;
GLREF_D CActiveScheduler *testScheduler;
GLREF_D RFs theFs;
GLREF_D TFileName theLogName;
GLREF_D RFile theLog;
GLREF_D RLogTestSession theLogServ;

#define __FILE_FAILNEXT(err, iteration) theFs.SetErrorCondition(err, iteration)
#define __FILE_RESET theFs.SetErrorCondition(KErrNone, 0)

#if defined(_UNICODE)
	_LIT(KLogDatabaseName,"c:\\private\\101f401d\\Logdbu.dat");
#else
	_LIT(KLogDatabaseName,"c:\\private\\101f401d\\Logdb.dat");
#endif


//**********************************
// CTestActive
//**********************************

class CTestActive : public CActive
	{
public:
	CTestActive(TInt aPriority = EPriorityIdle-100);
	~CTestActive();
	void StartL();
	void StartL(TInt);
protected:
	void DoCancel();
	void RunL();
private:
	TBool iDelayCompletion;
	TRequestStatus iStoredStatus;
	TInt iDelayTime;
	};

void doTestsL();


//**********************************
// CTestTimer
//**********************************

class CTestTimer : public CTimer
	{
public:
	static CTestTimer* NewL();
private:
	CTestTimer();
	void RunL();	
	};

//**********************************
// TestUtils
//**********************************

class TestUtils
	{
public:
	static void Initialize(const TDesC& aName);
	static void DeleteDatabaseL(TBool aCloseBeforeDelete = ETrue);
	static HBufC* CreateBufLC(TInt aLength);
	static HBufC8* CreateBuf8LC(TInt aLength);
	static TBool FiltersEqual(const CLogFilter& aFilter1, const CLogFilter& aFilter2);
	static TBool EventsEqual(const CLogEvent& aEvent1, const CLogEvent& aEvent2);
	static TBool TypesEqual(const CLogEventType& aType1, const CLogEventType& aType2);
	static TBool WaitForKeyL(TTimeIntervalMicroSeconds32 aDelay, TKeyCode& aKeyCode);
	static TBool FileExists(const TDesC& aFile);
	static void SetLogServHeapFailureL(RHeap::TAllocFail aType, TInt aRate);
	static TBool IsDatabaseOpenL();
	static TInt DatabaseSizeL();
	static TInt AddEventL();
	static void AddViewTestEventsL();
	static void AddEventTypeL();
	static void CopyCorruptDbL();
	static void CopyCorruptDamagedDbL();
	static void CopyOldDbL();
	static void TestInvalidSchemaL();
	static TBool MatchingEnabledL();

private:
	// starts t_HiCapHelper.exe which has sufficient capability
    static TInt ExecuteRemoteL(const TDesC& aCommandLineArg);
	};

#define __LOGSERV_UHEAP_FAILNEXT(X) TestUtils::SetLogServHeapFailureL(RHeap::EFailNext, X)
#define __LOGSERV_UHEAP_RESET TestUtils::SetLogServHeapFailureL(RHeap::ENone, 0)

//**********************************
// CLogViewChangeObserver
//**********************************

class CLogViewChangeObserver : public CActive, public MLogViewChangeObserver
	{
public:
	enum TStopType
		{
		EStopOnChanges = 0,
		EStopOnRunL,
		EStopOnBoth,
		EStopOnCount,
		EDontStopScheduler
		};
public:
	static CLogViewChangeObserver* NewLC();
	~CLogViewChangeObserver();

protected:
	CLogViewChangeObserver();

public:
	inline void StartCollectingChanges() 
		{ 
		Reset(); 
		iType = EDontStopScheduler; 
		}
	CLogChangeDefinition* WaitForChangesLC(TStopType aType = EStopOnChanges, TInt aCount = 0);
	CLogChangeDefinition* WaitForChangesLC(TCallBack aCallBack, TStopType aType, TInt aCount = 0);
	inline TBool HaveChanges() const 
		{ 
		return iChanges != NULL; 
		}
	inline const CLogChangeDefinition& Changes() const 
		{ 
		__ASSERT_ALWAYS(iChanges, User::Invariant()); 
		return *iChanges; 
		}
	inline void SetActive() 
		{ 
		CActive::SetActive(); 
		iStatus = KRequestPending; 
		}
	inline void ResetChanges() 
		{ 
		__ASSERT_ALWAYS(iChanges, User::Invariant()); 
		iChanges->Reset(); 
		}

public:
	void HandleLogViewChangeEventAddedL(TLogId aId,   TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
	void HandleLogViewChangeEventChangedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
	void HandleLogViewChangeEventDeletedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);

private:
	void RunL();
	void DoCancel();

private:
	void Reset();	 

protected:
	void CheckForSchedulerStop();
	void AddChangeL(TLogDatabaseChangeType aType, TLogId aId, TInt aViewIndex);
 	
private:
	TInt iExpectedChangeCount;
	TBool iHaveFinishedOperation;
	TBool iHaveObtainedChanges;
	TBool iSchedulerStarted;
	TStopType iType;
	CLogChangeDefinition* iChanges;
	TBool iHaveCallBack;
	TCallBack iCallBack;
	};

//**********************************
// CLogViewChangeObserverErrorTest
//**********************************

class CLogViewChangeObserverErrorTest : public  CLogViewChangeObserver 
	{
public:
	static CLogViewChangeObserverErrorTest* NewLC();

private:
	CLogViewChangeObserverErrorTest();

public:
	void HandleLogViewChangeEventAddedL(TLogId aId,   TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
	};


//**********************************
// CLogSchedulerTimer
//**********************************

class CLogSchedulerTimer : public CTimer
	{
public:
	static CLogSchedulerTimer* NewLC();
	~CLogSchedulerTimer();

private:
	CLogSchedulerTimer();
	void ConstructL();

public:
	void Wait(TTimeIntervalMicroSeconds32 aTime);

private:
	void RunL();
	};

//**********************************
// CLogChangeNotifier
//**********************************

class CLogChangeNotifier : public CActive
	{
public:
	static CLogChangeNotifier* NewL();
	~CLogChangeNotifier();
	//
private:
	CLogChangeNotifier();
	void ConstructL();
	//
	void RunL();
	void DoCancel();
	//
private:
	CLogClient* iClient;
	TTime iStart;
	};

//**********************************
// Global
//**********************************

void SetupSchedulerL();
void CloseScheduler();
void doMainL();
TInt E32Main();
void DeleteDataFile(const TDesC& aFullName);

#endif//t_logutil2_h
