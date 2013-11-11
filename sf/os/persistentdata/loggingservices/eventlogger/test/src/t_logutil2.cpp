// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bautils.h>
#include "t_logutil2.h"

//Define "TheTest" variable used in the test cpp files
extern RTest TheTest;

_LIT(KHelperExeName, "t_LogHiCapHelper.exe");

//======================================================================================================

#ifdef LOGGING_ENABLED

void Log::New()
	{
	_LIT(KNewLogText, "===== NEW LOG =====");
	//
	RFileLogger logger;
	TInt ret=logger.Connect();
	if	(ret==KErrNone)
		{
		logger.CreateLog(KLogFolder, KLogFileName, EFileLoggingModeOverwrite);
		logger.Write(KNewLogText);
		}
	logger.Close();
	}

void Log::Write(const TDesC& aText)
	{
	PruneLogFile();

	RFileLogger logger;
	TInt ret=logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime(EFalse,EFalse);
		logger.CreateLog(KLogFolder, KLogFileName,EFileLoggingModeAppend);
		TBuf<KLogEngLogBufferSize> buf;
		TTime now;
		now.HomeTime();
		TDateTime dateTime;
		dateTime = now.DateTime();
		buf.Format(KTimeFormat,dateTime.Hour(),dateTime.Minute(),dateTime.Second(),dateTime.MicroSecond());
		buf.AppendFormat(KTextFormat,&aText);

		logger.Write(buf);
		}

	logger.Close();
	}

void Log::WriteFormat(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list,aFmt);

	PruneLogFile();

	TBuf<2*KLogEngLogBufferSize> buf;
	buf.SetMax();
	buf.FillZ();
	TTime now;
	now.HomeTime();
	TDateTime dateTime;
	dateTime = now.DateTime();
	buf.Format(KTimeFormat,dateTime.Hour(),dateTime.Minute(),dateTime.Second(),dateTime.MicroSecond());
	buf.AppendFormatList(aFmt, list );

	RFileLogger logger;
	TInt ret=logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime(EFalse,EFalse);
		logger.CreateLog(KLogFolder, KLogFileName,EFileLoggingModeAppend);
		logger.Write(buf);
		}

	logger.Close();
	}

void Log::PruneLogFile()
  	{
	const TInt KMaxLogSize = 1024 * 500;
	_LIT(KDriveLetter, "C:\\Logs\\");
	//
	TFileName fileName(KDriveLetter);
	fileName.Append(KLogFolder);
	fileName.Append(KLogFileName);
	//
	RFs fsSession;
	if	(fsSession.Connect() == KErrNone)
		{
		TEntry entry;
		if	(fsSession.Entry(fileName, entry) == KErrNone)
			{
			// Check size and delete if its too big
			if	(entry.iSize >= KMaxLogSize)
				fsSession.Delete(fileName); // ignore error
			}
		}
	fsSession.Close();
	}

#endif

// Globals 
GLDEF_D CTrapCleanup* theCleanup;
GLDEF_D CActiveScheduler *testScheduler;
GLDEF_D RFs theFs;
GLDEF_D TFileName theLogName;
GLDEF_D RFile theLog;
GLDEF_D RLogTestSession theLogServ;

//**********************************
// CTestActive
//**********************************

CTestActive::CTestActive(TInt aPriority)
:	CActive(aPriority)
	{
	CActiveScheduler::Add(this);
	iDelayTime=0;
	}

CTestActive::~CTestActive()
	{
	Cancel();
	}

void CTestActive::DoCancel()
	{
	TRequestStatus* s=&iStatus;
	User::RequestComplete(s, KErrNone);
	}

void CTestActive::StartL()
	{
	iDelayCompletion=EFalse;
	iDelayTime=0;
	iStatus = KRequestPending;
	SetActive();
	}

void CTestActive::StartL(TInt aDelay)
	{
	iDelayCompletion=ETrue;
	iDelayTime=aDelay;
	iStatus = KRequestPending;
	SetActive();
	}

void CTestActive::RunL() 
	{
	if(iDelayCompletion && iDelayTime)
		{
		// Wait for events in other threads to have a go....
		User::After(iDelayTime);
		iDelayTime=0;
		iStoredStatus=iStatus;
		SetActive();
		TRequestStatus* s=&iStatus;
		User::RequestComplete(s, KErrNone);
		}
	else
		{
		if(iDelayCompletion)
			iStatus=iStoredStatus;

		LOGTEXT("CTestActive::RunL() - Stopping the scheduler");
		CActiveScheduler::Stop();
		}
	}

//**********************************
// CTestTimer
//**********************************

CTestTimer::CTestTimer()
: CTimer(EPriorityLow)
	{}

void CTestTimer::RunL()
	{
	LOGTEXT("CTestTimer::RunL() - Stopping the scheduler");
	CActiveScheduler::Stop();
	}

CTestTimer* CTestTimer::NewL()
	{
	CTestTimer* self = new(ELeave) CTestTimer();
	CleanupStack::PushL(self);
	self->ConstructL(); // CTimer
	CActiveScheduler::Add(self);
	CleanupStack::Pop();
	return self;
	}

//**********************************
// TestUtils
//**********************************

void TestUtils::Initialize(const TDesC& aName)
	{
	TheTest.Title();
	TheTest.Printf(_L("%S\r\n"), &aName);
    User::RenameThread(aName);
	}

TBool TestUtils::FileExists(const TDesC& aFile)
	{
	TEntry entry;
	return theFs.Entry(aFile, entry) == KErrNone;
	}

//Loads t_loghihelper process and passes for execution to t_loghihelper "aCommandLineArg" command line.
//t_loghihelper will run, execute the command and die, returning the result of the command execution.
//TestUtils::ExecuteRemoteL() will leave if error and return the result of the remote cmd execution to the caller.
TInt TestUtils::ExecuteRemoteL(const TDesC& aCommandLineArg)
	{
	RProcess process;
	LEAVE_IF_ERROR(process.Create(KHelperExeName, aCommandLineArg));
	
	TRequestStatus status;
	process.Logon(status);
	process.Resume();

	User::WaitForRequest(status);
	TInt exitReason = process.ExitReason();
	
	process.Close();
	LEAVE_IF_ERROR(exitReason);

	return exitReason;
	}
	
//Runs t_loghihelper. t_loghihelper will execute the "delete LogEng database" command.
//The "delete LogEng database" is a complex operation. The request is sent via the backup server
//which will send a request to the LogEng server to release the LogEng database file locks and close the file.
//After that the database will be deleted. 
//In the same call the LogEng server will restarted and the LogEng server will re-create the database during the 
//server startup.
//
//If "aCloseBeforeDelete" flag is false, then the database wil be only deleted.
//The default value of "aCloseBeforeDelete" is true: the database will be closed, deleted and re-created.
//But some of the LogEng tests create a CBaBackupSessionWrapper object and call CloseFileL() with the logeng
//database name as a parameter. In this case, if another process, as t_loghicaphelper for example, attempts
//to call CloseFileL() with the same file name as a parameter, then the caller will get KErrServerBusy error.
//See how CBaBackupSessionWrapper::CloseFileL() is implemented on the server side.
void TestUtils::DeleteDatabaseL(TBool aCloseBeforeDelete)
	{
    _LIT(KCmdLine1, "-delete_db1");
    _LIT(KCmdLine2, "-delete_db2");
    (void)ExecuteRemoteL(aCloseBeforeDelete ? KCmdLine1 : KCmdLine2);
	}
	
//Runs t_loghihelper. t_loghihelper will check and return whether the LogEng database is open or not.  
TBool TestUtils::IsDatabaseOpenL()
	{
	_LIT(KCmdLine, "-db_is_open");
	TInt result = ExecuteRemoteL(KCmdLine);
	return result != 0;
	}
	
//Runs t_loghihelper. t_loghihelper will add an event type to the LogEng database.  
void TestUtils::AddEventTypeL()
	{
	_LIT(KCmdLine, "-add_event_type");
	(void)ExecuteRemoteL(KCmdLine);
	}
	
//Runs t_loghihelper. t_loghihelper will add an event to the LogEng database.  
TInt TestUtils::AddEventL()
	{
	_LIT(KCmdLine, "-add_event");
	return ExecuteRemoteL(KCmdLine);		
	}
	
//Runs t_loghihelper. t_loghihelper will add events to the LogEng database.  
void TestUtils::AddViewTestEventsL()
	{
	_LIT(KCmdLine, "-add_view_test_events");
	(void)ExecuteRemoteL(KCmdLine);
	}

//Runs t_loghihelper. t_loghihelper will return the size of the LogEng database.  
TInt TestUtils::DatabaseSizeL()
	{
	_LIT(KCmdLine, "-db_size");
	return ExecuteRemoteL(KCmdLine);		
	}	
	
//Runs t_loghihelper. t_loghihelper will replace the LogEng database with a corrupted database (for testing purposes).
//The LogEng server will be stopped before that. The function can be used only in debug mode.
#ifdef _DEBUG
void TestUtils::CopyCorruptDbL()
	{

	_LIT(KCmdLine, "-copy_corrupt");
	(void)ExecuteRemoteL(KCmdLine);
	}
	
//Runs t_loghihelper. t_loghihelper will replace the LogEng database with a corrupted database (for testing purposes).
//The LogEng server will be stopped before that. The function can be used only in debug mode.
void  TestUtils::CopyCorruptDamagedDbL()
	{

	_LIT(KCmdLine, "-copy_corrupt_damaged");
	(void)ExecuteRemoteL(KCmdLine);
	}
	
//Runs t_loghihelper. t_loghihelper will replace the LogEng database with an old format database 
//(no SimId column, phone number length is different). The LogEng server will be stopped before that.
//The function can be used only in debug mode.
void TestUtils::CopyOldDbL()
	{
	_LIT(KCmdLine, "-copy_old");
	(void)ExecuteRemoteL(KCmdLine);
	}
#else //_DEBUG
void TestUtils::CopyCorruptDbL()
	{
	TheTest.Printf(_L("TestUtils::CopyCorruptDbL() has a meaningfull implementation in debug builds only.\n"));
	}

void TestUtils::CopyCorruptDamagedDbL()
	{
	TheTest.Printf(_L("TestUtils::CopyCorruptDamagedDbL() has a meaningfull implementation in debug builds only.\n"));
	}

void TestUtils::CopyOldDbL()
	{
	TheTest.Printf(_L("TestUtils::CopyOldDbL() has a meaningfull implementation in debug builds only.\n"));
	}

#endif//_DEBUG

//Runs t_loghihelper. t_loghihelper will re-create the LogEng database and check whether LogEng client can connect to the server.   
void TestUtils::TestInvalidSchemaL()
	{
	_LIT(KCmdLine, "-invalid_schema");
	(void)ExecuteRemoteL(KCmdLine);
	}

//Runs t_loghihelper. t_loghihelper checks whether the phone number mathcing is enabled.   
TBool TestUtils::MatchingEnabledL()
	{
	_LIT(KCmdLine, "-is_matching_enabled");
	return ExecuteRemoteL(KCmdLine) != 0;
	}

//Creates HBufC object and puts it on the cleanup stack.
//The buffer will be filled with (' ' + pos) characters, where pos is the character position in the buffer. 
HBufC* TestUtils::CreateBufLC(TInt aLength)
	{
	HBufC* buf = HBufC::NewLC(aLength);
	TPtr ptr = buf->Des();
	for(TInt pos=0;pos<aLength;++pos)
		{
		ptr.Append(TChar(' ' + pos));
		}
	return buf;
	}

//Returns whether the two filters are equal or not.
TBool TestUtils::FiltersEqual(const CLogFilter& aFilter1, const CLogFilter& aFilter2)
	{
	return aFilter1.EventType() == aFilter2.EventType() &&
		   aFilter1.RemoteParty() == aFilter2.RemoteParty() &&
		   aFilter1.Direction() == aFilter2.Direction() &&
		   aFilter1.DurationType() == aFilter2.DurationType() &&
		   aFilter1.Status() == aFilter2.Status() &&
		   aFilter1.Contact() == aFilter2.Contact() &&
		   aFilter1.Number() == aFilter2.Number() 
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		   &&
		   aFilter1.SimId() == aFilter2.SimId()
#endif		   
		   ;
	}

//Creates HBufC8 object and puts it on the cleanup stack.
//The buffer will be filled with (' ' + pos % (0xff - 32)) characters, where pos is the character position in the buffer. 
HBufC8* TestUtils::CreateBuf8LC(TInt aLength)
	{
	HBufC8* buf = HBufC8::NewLC(aLength);
	TPtr8 ptr = buf->Des();
	for(TInt pos=0;pos<aLength;++pos)
		{
		ptr.Append(TChar(' ' + pos % (0xff - 32)));
		}
	return buf;
	}

//Returns whether the two events are equal or not.
TBool TestUtils::EventsEqual(const CLogEvent& aEvent1, const CLogEvent& aEvent2)
	{
	return 	aEvent1.Id() == aEvent2.Id() &&
			aEvent1.EventType() == aEvent2.EventType() &&
			aEvent1.RemoteParty() == aEvent2.RemoteParty() &&
			aEvent1.Direction() == aEvent2.Direction() &&
			aEvent1.Time() == aEvent2.Time() &&
			aEvent1.DurationType() == aEvent2.DurationType() &&
			aEvent1.Duration() == aEvent2.Duration() &&
			aEvent1.Status() == aEvent2.Status() &&
			aEvent1.Subject() == aEvent2.Subject() &&
			aEvent1.Number() == aEvent2.Number() &&
			aEvent1.Contact() == aEvent2.Contact() &&
			aEvent1.Link() == aEvent2.Link() &&
			aEvent1.Description() == aEvent2.Description() &&
			aEvent1.Data() == aEvent2.Data() 
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
			&& 
			aEvent1.SimId() == aEvent2.SimId()
#endif			
			; 
	}

//Returns whether the two event types are equal or not.
TBool TestUtils::TypesEqual(const CLogEventType& aType1, const CLogEventType& aType2)
	{
	return	aType1.Uid() == aType2.Uid() &&
			aType1.Description() == aType2.Description() &&
			aType1.LoggingEnabled() == aType2.LoggingEnabled();
	}

//Waits for a key to be pressed.
TBool TestUtils::WaitForKeyL(TTimeIntervalMicroSeconds32 aDelay, TKeyCode& aKeyCode)
	{
	TEST(TheTest.Console() != NULL);

	// Create timer
	CTestTimer* timer = CTestTimer::NewL();
	CleanupStack::PushL(timer);
	timer->After(aDelay);

	CTestActive* wait = new(ELeave)CTestActive;
	CleanupStack::PushL(wait);
	wait->StartL();

	// Wait for key press
	TheTest.Console()->Read(wait->iStatus);
	CActiveScheduler::Start();

	// If timer still active a key was pressed
	TBool keyPressed = timer->IsActive();

	if (keyPressed)
		{
		// Get the key pressed
		aKeyCode = TheTest.Console()->KeyCode();

		// Cancel timer
		timer->Cancel();
		}
	else
		{
		// Cancel wait for character
		TheTest.Console()->ReadCancel();
		User::WaitForRequest(wait->iStatus);
		}

	CleanupStack::PopAndDestroy(2); // wait, timer
	return keyPressed;
	}

//Used for LogEng server side heap failure testing.
#ifdef _DEBUG
void TestUtils::SetLogServHeapFailureL(RHeap::TAllocFail aType, TInt aRate)
	{
	//this function doesn't have any effect on UREL builds 
 	//get rid of warnings in release builds
 	aType = aType;
 	aRate = aRate;
	if (!theLogServ.Handle())
	    LEAVE_IF_ERROR(theLogServ.Connect());

	TIpcArgs  ipcArgs(aType,aRate) ;
	LEAVE_IF_ERROR(theLogServ.Send(ELogSetHeapFail, ipcArgs));
	}
#else
void TestUtils::SetLogServHeapFailureL(RHeap::TAllocFail, TInt)
	{
	}
#endif//_DEBUG

//**********************************
// CLogViewChangeObserver
//**********************************

CLogViewChangeObserver* CLogViewChangeObserver::NewLC()
	{
	CLogViewChangeObserver* self = new(ELeave) CLogViewChangeObserver();
	CleanupStack::PushL(self);
	return self;
	}

CLogViewChangeObserver::~CLogViewChangeObserver()
	{
	Cancel();
	delete iChanges;
	}

CLogViewChangeObserver::CLogViewChangeObserver()
:	CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}


CLogChangeDefinition* CLogViewChangeObserver::WaitForChangesLC(TStopType aType, TInt aCount)
	{
	__ASSERT_ALWAYS(!iSchedulerStarted, User::Invariant());
	Reset();
	//
	iExpectedChangeCount = aCount;
	iType = aType;
	if	(aType != EStopOnChanges)
		SetActive();
	//
	iSchedulerStarted = ETrue;
	CActiveScheduler::Start();
	iSchedulerStarted = EFalse;
	//
	CLogChangeDefinition* ret = iChanges;
	TEST(iChanges != NULL);
	iChanges = NULL;
	CleanupStack::PushL(ret);
	return ret;
	}

CLogChangeDefinition* CLogViewChangeObserver::WaitForChangesLC(TCallBack aCallBack, TStopType aType, TInt aCount)
	{
	iHaveCallBack = ETrue;
	iCallBack = aCallBack;
	return WaitForChangesLC(aType, aCount);
	}

void CLogViewChangeObserver::HandleLogViewChangeEventAddedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
	AddChangeL(ELogChangeTypeEventAdded, aId, aViewIndex);
	if	(aChangeIndex == aTotalChangeCount-1)
		CheckForSchedulerStop();
	}

void CLogViewChangeObserver::HandleLogViewChangeEventChangedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
	AddChangeL(ELogChangeTypeEventChanged, aId, aViewIndex);
	if	(aChangeIndex == aTotalChangeCount-1)
		CheckForSchedulerStop();
	}

void CLogViewChangeObserver::HandleLogViewChangeEventDeletedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
	AddChangeL(ELogChangeTypeEventDeleted, aId, aViewIndex);
	if	(aChangeIndex == aTotalChangeCount-1)
		CheckForSchedulerStop();
	}

void CLogViewChangeObserver::RunL()
	{
	__ASSERT_ALWAYS(iType == EStopOnRunL || iType == EStopOnBoth, User::Invariant());
	iHaveFinishedOperation = ETrue;
	CheckForSchedulerStop();
	}

void CLogViewChangeObserver::DoCancel()
	{
	TRequestStatus* s=&iStatus;
	User::RequestComplete(s, KErrCancel);
	}

void CLogViewChangeObserver::Reset()
	{
	iExpectedChangeCount = 0;
	iHaveFinishedOperation = EFalse;
	iHaveObtainedChanges = EFalse;
	iSchedulerStarted = EFalse;
	iType = EStopOnChanges;
	delete iChanges;
	iChanges = NULL;
	}

void CLogViewChangeObserver::CheckForSchedulerStop()
	{
	if(iSchedulerStarted)
		{
		if	(iHaveCallBack)
			{
			iCallBack.CallBack();
			iCallBack.iFunction = NULL;
			iCallBack.iPtr = NULL;
			iHaveCallBack = EFalse;
			}
		//
		TBool stopScheduler = EFalse;
		switch(iType)
			{
		case EStopOnChanges:
			stopScheduler = iHaveObtainedChanges;
			break;
		case EStopOnRunL:
			stopScheduler = iHaveFinishedOperation;
			break;
		case EStopOnBoth:
			stopScheduler = (iHaveObtainedChanges && iHaveFinishedOperation);
			break;
		case EStopOnCount:
			if	(iChanges)
				{
				TEST(iChanges->Count() <= iExpectedChangeCount);
				stopScheduler = (iChanges->Count() == iExpectedChangeCount);
				}
		case EDontStopScheduler:
			break;
			}

		if	(stopScheduler)
			{
			LOGTEXT("CLogViewChangeObserver::CheckForSchedulerStop() - Stopping the scheduler");
			CActiveScheduler::Stop();
			}
		}
	}

void CLogViewChangeObserver::AddChangeL(TLogDatabaseChangeType aType, TLogId aId, TInt aViewIndex)
	{
	CLogChangeDefinition* changes;

	if	(iChanges)
		changes = iChanges;
	else
		{
		changes = CLogChangeDefinition::NewL();
		CleanupStack::PushL(changes);
		}
	//
	changes->AddL(aId, aType, aViewIndex);
	//
	if	(!iChanges)
		{
		delete iChanges;
		iChanges = changes;
		CleanupStack::Pop(changes);
		}
	//
	iHaveObtainedChanges = ETrue;
	}

//**********************************
// CLogViewChangeObserverErrorTest
//**********************************
CLogViewChangeObserverErrorTest* CLogViewChangeObserverErrorTest::NewLC()
	{
	CLogViewChangeObserverErrorTest* self = new(ELeave) CLogViewChangeObserverErrorTest();
	CleanupStack::PushL(self);
	return self;
	}	

CLogViewChangeObserverErrorTest::CLogViewChangeObserverErrorTest()
	{}
	 
void CLogViewChangeObserverErrorTest::HandleLogViewChangeEventAddedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
  	// DEF108741L - the error condition tested here is that a leave is dealt with 
  	// gracefully without any panics.
 
 	// Add a new event to the log
	AddChangeL(ELogChangeTypeEventAdded, aId, aViewIndex);
	if	(aChangeIndex == aTotalChangeCount-1)
		CheckForSchedulerStop();
	
	// In the test case for DEF108741L this method will be effectively
	// invoked 3 times. This code forces a leave on the middle event to 
	// ensure that the leave is dealt with and the rest of the test 
	// completes successfully.
	if (aId == 1)
		{	
		LEAVE(KErrGeneral);
		} 
	}
 
//**********************************
// CLogSchedulerTimer
//**********************************

CLogSchedulerTimer* CLogSchedulerTimer::NewLC()
	{
	CLogSchedulerTimer* self = new(ELeave) CLogSchedulerTimer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CLogSchedulerTimer::~CLogSchedulerTimer()
	{
	Cancel();
	}

CLogSchedulerTimer::CLogSchedulerTimer()
:	CTimer(0)
	{
	CActiveScheduler::Add(this);
	}

void CLogSchedulerTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

void CLogSchedulerTimer::Wait(TTimeIntervalMicroSeconds32 aTime)
	{
	After(aTime);
	CActiveScheduler::Start();
	}

void CLogSchedulerTimer::RunL()
	{
	LOGTEXT("CLogSchedulerTimer::RunL() - Stopping the scheduler");
	CActiveScheduler::Stop();
	}




//**********************************
// CLogChangeNotifier
//**********************************

CLogChangeNotifier* CLogChangeNotifier::NewL()
	{
	CLogChangeNotifier* self = new(ELeave)CLogChangeNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CLogChangeNotifier::~CLogChangeNotifier()
	{
	Cancel();
	delete iClient;
	}

CLogChangeNotifier::CLogChangeNotifier()
: CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

void CLogChangeNotifier::ConstructL()
	{
	iClient = CLogClient::NewL(theFs);

	iStart.UniversalTime();
	iClient->NotifyChange(10000000, iStatus);
	SetActive();
	}

void CLogChangeNotifier::RunL()
	{
	TTime now;
	now.UniversalTime();
	TTimeIntervalSeconds seconds;
	now.SecondsFrom(iStart, seconds);

	TBuf<256> buf;
 	const TInt error = iStatus.Int();
 	if (error == KErrServerTerminated)
 		{
 		buf.Format(_L("KErrServerTerminated"));
		User::InfoPrint(buf);
		return;
 		}
 		
	buf.Format(_L("%d seconds"), seconds.Int());
	User::InfoPrint(buf);
	
	iStart.UniversalTime();
	iClient->NotifyChange(10000000, iStatus);
	SetActive();
	}

void CLogChangeNotifier::DoCancel()
	{
	iClient->NotifyChangeCancel();	
	}

//**********************************
// Global
//**********************************

void SetupSchedulerL()
	{
	testScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL( testScheduler );
	CActiveScheduler::Install( testScheduler );
	}

void CloseScheduler()
	{
    CleanupStack::PopAndDestroy(); // Scheduler
    testScheduler = NULL;
	}

static void CreateLogL()
    {
    LEAVE_IF_ERROR(theFs.Connect());

    theLogName.Copy(RProcess().FileName());
    TInt start = theLogName.LocateReverse('\\');
    TInt end = theLogName.LocateReverse('.');
    theLogName = theLogName.Mid(start + 1, end - start - 1);

    // create the log filename
    theLogName.Insert(0, _L("C:\\"));
#if defined(__WINS__)
    theLogName.Append(_L(".WINS."));
#else
    theLogName.Append(_L(".MARM."));
#endif
#if defined(_UNICODE)
    theLogName.Append(_L("UNICODE."));
#else
    theLogName.Append(_L("ASCII."));
#endif
#if defined(_DEBUG)
    theLogName.Append(_L("DEB."));
#else
    theLogName.Append(_L("REL."));
#endif
    theLogName.Append(_L("LOG"));

    // create the logfile
    LEAVE_IF_ERROR(theLog.Replace(theFs, theLogName, EFileWrite|EFileShareExclusive));
    TBuf8<256> text;
    text.Copy(theLogName);
    theLog.Write(text);
    theLog.Write(_L8("\nTest results\n"));
    }

static void CloseLog()
    {
    theLog.Write(_L8("Tests completed\n"));
    TheTest.Printf(_L("Results saved in %S\n"), &theLogName);
    theLog.Close();
    theFs.Close();
    }

void DeleteDataFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			TheTest.Printf(_L("Deleting \"%S\" file.\n"), &aFullName);
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone) 
				{
				TheTest.Printf(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone) 
				{
				TheTest.Printf(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		TheTest.Printf(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	}

static void Cleanup(void*)
	{
	TRAP_IGNORE(TestUtils::DeleteDatabaseL());
	::DeleteDataFile(theLogName);
	}

static void DoMainL()
	{
	::SetupSchedulerL();
	TCleanupItem cleanup(&Cleanup, NULL);
	CleanupStack::PushL(cleanup);
	CreateLogL();
	::doTestsL();
	CloseLog();
    CleanupStack::PopAndDestroy();//cleanup
	::CloseScheduler();
	}

TInt E32Main()
	{	
	__UHEAP_MARK;

	theCleanup = CTrapCleanup::New();
    if(!theCleanup)
       {
       _LIT(KLogHiCapHelperPanic, "LogTestPanic");
        User::Panic(KLogHiCapHelperPanic, KErrNoMemory);
       }

	TRAPD(err, ::DoMainL());	
	TEST2(err, KErrNone);

	delete theCleanup;	

	TheTest.Console()->SetPos(0, 13);

	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;

	return KErrNone;
	}


