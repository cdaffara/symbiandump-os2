// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Platform security prohibits the existing logeng tests from doing
// direct operations on data-caged files.  Giving the tests a higher
// capability might mask other errors.  Therefore any file manipulation
// may be sub-contracted to this process as needed.
// 
//
#include <s32file.h>
#include <babackup.h>
#include "t_logutil2.h"
#include "t_logutil3.h"

const TUid KTestEventUid = {0x10005393};
_LIT(KTestEventDesc1, "Event Type Description");
_LIT(KTestDirection1, "Direction");
_LIT(KTestStatus1, "Status");
_LIT(KTestStatus2, "Changed Status");
_LIT(KTestNumber1, "Number");
const TLogContactItemId KTestContact1 = 0x1234;
const TLogContactItemId KTestContact2 = 0x1234567;
_LIT(KTestRemote1, "Remote Test 1");
_LIT(KTestRemote2, "Remote Test 2");
_LIT(KTestRemote3, "Remote Test 3");
_LIT(KTestStatusT3, "Status Test 3");
_LIT(KTestEventDesc, "Test Event");

_LIT(KLogEngPrivatePath, "c:\\private\\101f401d\\");

_LIT(KLogHiCapHelperPanic, "TLHCHlpr");

RTest TheTest(_L("t_loghicaphelper")); //used in t_logutil.cpp only

RFs theFs;
CActiveScheduler *TheTestScheduler = NULL;

//===========================================================================================

//Supported remote operaions
enum THelperOp
	{
	EOpNotDefined,
    EDeleteDatabase1,
    EDeleteDatabase2,
	EIsDatabaseOpen,
	EDatabaseSize,
	ECopyCorruptDbFile,
	ECopyCorruptDamagedDbFile,
	ECopyOldDbFile,
	EAddEvent,
	EAddTestEvents,
	EAddEventType,
	EInvalidSchema,
	EIsMatchingEnabled
	} ;

_LIT(KOldCorruptDatabase,"z:\\test\\corruptLogdbu.dat");
_LIT(KOldCorruptDamagedDatabase,"z:\\test\\corruptDamagedLogdbu.dat");
_LIT(KOldFormatDatabase,"z:\\test\\oldLogdbu.dat");


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

//.................................................................................................

//See TestUtils::TestInvalidSchemaL(). Re-creates the LogEng database and checkes whether a new 
//LogEng connection can be established (by creating a CLogClient object).
static void TestInvalidSchemaL()
	{
	TheTestScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL( TheTestScheduler );
	CActiveScheduler::Install( TheTestScheduler );

   	//Reseting of log engine databse is done via its backup server.  This seems a 
   	//bit odd.  Perhaps write a CLogClient API that does it?
   	//Create backup session wrapper
   	CBaBackupSessionWrapper* backup = CBaBackupSessionWrapper::NewL();
   	CleanupStack::PushL(backup);
   
   	//This eventually calls CLogBackup::ChangeFileLockL(..) which closes the database 
   	//file and notifies all handles to that file that it has closed.
   	backup->CloseFileL(KLogDatabaseName, MBackupObserver::EReleaseLockNoAccess);
 	User::After(1000000);
   	
   	//Since the log engine database file is closed we can replace it.   
   	//Once this file is deleted, the backup server notices this and attempts to reopen 
   	//the database.  Since the file is deleted a default database is created instead.
    RDbNamedDatabase database;
	TInt err = database.Replace(theFs, KLogDatabaseName);
	database.Close();
	LEAVE_IF_ERROR(err);

	// The following will leave if there is a problem
	CLogClient* client = CLogClient::NewL(theFs);
	delete client;
	
	CleanupStack::PopAndDestroy(2); // scheduler + backup
	TheTestScheduler = NULL;
	}

//.................................................................................................

//See TestUtils::AddEventTypeL(). Adds an event type to the LogEng database.
static void AddEventTypeL()
	{
	TheTestScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(TheTestScheduler);
	CActiveScheduler::Install(TheTestScheduler);

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);
	type->SetDescription(KTestEventDesc1);
	type->SetLoggingEnabled(ETrue);

	client->AddEventType(*type, active->iStatus);
	
	active->StartL();
	CActiveScheduler::Start();
	LEAVE_IF_ERROR(active->iStatus.Int());
	
	CleanupStack::PopAndDestroy(4); // scheduler, client, active, type
	TheTestScheduler = NULL;
	}
	
//.................................................................................................

//See TestUtils::AddViewTestEventsL(). Adds events to the LogEng database.
static void AddTestEventsL()
	{
	TheTestScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(TheTestScheduler);
	CActiveScheduler::Install(TheTestScheduler);
	
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

 	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	TLogString direction;
	client->GetString(direction, R_LOG_DIR_IN);

	// Create a test event type
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);
	type->SetUid(KTestEventUid);
	type->SetDescription(KTestEventDesc);
	type->SetLoggingEnabled(ETrue);

	// Register the event type
	active->StartL();
	client->AddEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	LEAVE_IF_ERROR(active->iStatus.Int());
	
	// Now add some events...
	//
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	//
	event->SetEventType(KLogCallEventTypeUid);
	event->SetContact(KTestContact1);
	event->SetDirection(direction);
	event->SetDurationType(KLogDurationValid);
	event->SetNumber(KTestNumber1);
	event->SetRemoteParty(KTestRemote1);
	event->SetStatus(KTestStatus1);

	// Add event
	active->StartL();
	client->AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	LEAVE_IF_ERROR(active->iStatus.Int());
	//
	
	event->SetEventType(KTestEventUid);	// low cap visible
	event->SetContact(KTestContact1);
	event->SetDirection(direction);
	event->SetDurationType(KLogDurationNone);
	event->SetNumber(KTestNumber1);
	event->SetRemoteParty(KTestRemote1);
	event->SetStatus(KTestStatus1);

	// Add event
	active->StartL();
	client->AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	LEAVE_IF_ERROR(active->iStatus.Int());
	//
		
	event->SetEventType(KLogCallEventTypeUid);
	event->SetContact(KTestContact2);
	event->SetDirection(direction);
	event->SetDurationType(KLogDurationValid);
	event->SetNumber(KTestNumber1);
	event->SetRemoteParty(KTestRemote2);
	event->SetStatus(KTestStatus2);

	// Add event and 4 duplicates
	for(TInt i=0; i<5; i++)
		{
		active->StartL();
		client->AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		LEAVE_IF_ERROR(active->iStatus.Int());
		}
	
	event->SetEventType(KTestEventUid);	// low cap visible
	event->SetContact(KTestContact2);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KLogDurationData);
	event->SetNumber(KTestNumber1);
	event->SetRemoteParty(KTestRemote3);
	event->SetStatus(KTestStatusT3);

	// Add event
	active->StartL();
	client->AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	LEAVE_IF_ERROR(active->iStatus.Int());
	
	CleanupStack::PopAndDestroy(5);	// event, client, type, active, scheduler
	TheTestScheduler = NULL;
	}
	
//.................................................................................................

//See TestUtils::AddEventL(). Adds an event to the LogEng database.
//The event type is set to be: KLogCallEventTypeUid.
//Return: the Id of the added event
static TInt AddEventL()
	{
	TheTestScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(TheTestScheduler);
	CActiveScheduler::Install(TheTestScheduler);

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	
	event->SetEventType(KLogCallEventTypeUid);

	active->StartL();
	client->AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	LEAVE_IF_ERROR(active->iStatus.Int());
    TLogId eventId = event->Id();
	
	CleanupStack::PopAndDestroy(4); // scheduler, client, active, event
	TheTestScheduler = NULL;
	return eventId;
	}

//.................................................................................................

#ifdef _DEBUG
static void StopLogServerL()
	{
	static RLogTestSession logServSession;
	//this function doesn't have any effect on UREL builds as LogEng server doesn't 
	//support the transient mode in UREL builds	
	//Used for LogEng server side heap failure testing.
	TInt error = KErrNone;
	  
	if(!logServSession.Handle())
		{
		error = logServSession.Connect();
		}
	  
	// Is the server running?
	if(error == KErrNotFound)
		{
		return;
		}
	LEAVE_IF_ERROR(error);
	  
	// Make the server transient
	TInt p0 = 1;
	TIpcArgs  ipcArgs(p0);
	LEAVE_IF_ERROR(logServSession.Send(ELogMakeTransient, ipcArgs));
	  
	logServSession.Close();
	  
	User::After(6 * 1000000); // Enough time for the server to exit
	}
#else//_DEBUG
static void StopLogServerL()
	{
	RDebug::Print(_L("StopLogServerL(): the LogEng server cannot be stopped in release mode. ELogMakeTransient is a debug message.\n"));
	}
#endif//_DEBUG

//.................................................................................................

static void DoDeleteDatabaseL(const TDesC& aDbPath, TBool aCloseBeforeDelete)
    {
    CBaBackupSessionWrapper* backup = NULL;
    if(aCloseBeforeDelete)
        {
        //Reseting of log engine databse is done via its backup server.
        //Create backup session wrapper
        backup = CBaBackupSessionWrapper::NewL();
        CleanupStack::PushL(backup);

        //This eventually calls CLogBackup::ChangeFileLockL(..) which closes the database 
        //file and notifies all handles to that file that it has closed.
        backup->CloseFileL(aDbPath, MBackupObserver::EReleaseLockNoAccess);
        User::After(1000000);
        }

    //Since the log engine database file is closed we can delete it.   
    //Once this file is deleted, the backup server notices this and attempts to reopen 
    //the database.  Since the file is deleted a default database is created instead.
    
    TInt error = theFs.Delete(aDbPath);

    if(!(error == KErrNone || error == KErrNotFound || error == KErrPathNotFound) )
        {
        RDebug::Print(_L("DoDeleteDatabaseL(), Error %d deleting database \"%S\"\n"),error, &aDbPath);
        }
    else
        {
        RDebug::Print(_L("DoDeleteDatabaseL(), Database \"%S\" deleted ok\n"), &aDbPath);
        }

    if(aCloseBeforeDelete)
        {
        //The next line restarts the logeng server and re-creates logdbu.dat file.
        CleanupStack::PopAndDestroy(backup);
            
        TInt count = 10;
        while(count--)
            {
            User::After(100000);
            TEntry entry;
            if(theFs.Entry(aDbPath, entry) == KErrNone)
                {
                break;
                }
            }
        }
    }

//See TestUtils::DeleteDatabaseL().
//If "aCloseBeforeDelete" is true then re-create the LogEng database.
static void DeleteDatabaseL(TBool aCloseBeforeDelete)
    {
    RDebug::Print(_L("DeleteDatabaseL(), Deleting database \"%S\"\r\n"), &KLogDatabaseName);

    TRAPD(err, DoDeleteDatabaseL(KLogDatabaseName, aCloseBeforeDelete));
    if(err == KErrNotFound || err == KErrPathNotFound)
        {
        err = KErrNone;
        }
    LEAVE_IF_ERROR(err);
    }

//.................................................................................................

//See TestUtils::CopyCorruptDbL().
//See TestUtils::CopyOldDbL(). 
//See TestUtils::CopyCorruptDamagedDbL()
//
//The LogEng database will be replaced with a the database which name is passed as a parameter (for testing purposes).
//The LogEng server will be stopped.
//This call works only in debug mode.
static void CopyDatabaseL(const TDesC& aNewDatabase)
	{
	StopLogServerL();
	
	CFileMan* fileMan=CFileMan::NewL(theFs);
	CleanupStack::PushL(fileMan);
	
	DeleteDatabaseL(ETrue); // it won't be replaced as the server has stopped

  	TInt err = fileMan->Copy(aNewDatabase, KLogDatabaseName);
	if(err != KErrNone)
		{
		// Note this only works on textshell ROMs, techview ROMs fail here with KErrInUse (-14)
		RDebug::Print(_L("CopyDatabaseL(), File copy \"%S\" to \"%S\", err=%d\n"), &aNewDatabase, &KLogDatabaseName, err);
		LEAVE(err);
		}
	// files copied are sometimes read-only, so make read-write	
	err = theFs.SetAtt(KLogDatabaseName, 0, KEntryAttReadOnly);
	if(err != KErrNone)
		{
		RDebug::Print(_L("CopyDatabaseL(), Set \"%S\" file attributes err=%d\n"), &KLogDatabaseName, err);
		LEAVE(err);
		}

	CleanupStack::PopAndDestroy(); // fileMan
	}

//.................................................................................................

//See TestUtils::DatabaseSizeL().
//Returns the LogEng database size.
static TInt DatabaseSizeL()
	{
	TEntry file;
	LEAVE_IF_ERROR(theFs.Entry(KLogDatabaseName, file));
	return file.iSize;
	}

//.................................................................................................

static void Initialize(const TDesC& aName)
	{
    User::RenameThread(aName);
	}

//.................................................................................................

//See TestUtils::IsDatabaseOpenL().
//Returns whether the LogEng database is open or not.
static TBool DatabaseIsOpenL()
	{
	TBool answer;
	LEAVE_IF_ERROR(theFs.IsFileOpen(KLogDatabaseName, answer));
	return answer;
	}

//.................................................................................................

//See TestUtils::MatchingEnabledL().
//The function opens the LogEng repository (KUidLogengRepository) and gets the value of 
//KContactMatchCountRepKey resource.
//If the value is 0 - "contacts matching" part of the test will be skipped.
static TBool MatchingEnabledL()
	{
	TInt contactMatchCount = 0;
	TLogContactNameFormat contactNameFormat = ELogWesternFormat; 
	LogGetContactmatchCountAndNameFormatL(contactMatchCount, contactNameFormat);
    RDebug::Print(_L("** contact match count = %d, contact name format = %d\r\n"), contactMatchCount, (TInt)contactNameFormat);
	return contactMatchCount > 0;
	}

//.................................................................................................

static TInt DoTaskL(THelperOp aOperation)
    {
    TInt rc = KErrNone;
	switch(aOperation)
	    {
        case EDeleteDatabase1:
            DeleteDatabaseL(ETrue);
            break;
        case EDeleteDatabase2:
            DeleteDatabaseL(EFalse);
            break;
        case EIsDatabaseOpen:
            rc = DatabaseIsOpenL();
            break;
        case EDatabaseSize:
            rc = DatabaseSizeL();
            break;
        case ECopyCorruptDbFile:
            CopyDatabaseL(KOldCorruptDatabase);
            break;
        case ECopyCorruptDamagedDbFile:
            CopyDatabaseL(KOldCorruptDamagedDatabase);
            break;
        case ECopyOldDbFile:
            CopyDatabaseL(KOldFormatDatabase);
            break;
        case EAddEvent:
            rc = AddEventL();
            break;
        case EAddTestEvents:
            AddTestEventsL();
            break;
        case EAddEventType:
            AddEventTypeL();
            break;
        case EInvalidSchema:
            TestInvalidSchemaL();
            break;
        case EIsMatchingEnabled:
            rc = MatchingEnabledL();
            break;
        default:
            rc = KErrNone;	// go away quietly
            break;
	    }
	LEAVE_IF_ERROR(rc);
	return rc;
    }

//.................................................................................................

static THelperOp TaskType(const TDesC& aCmdLine)
    {
    THelperOp task = EOpNotDefined;
    
    if(aCmdLine.CompareF(_L("-delete_db1")) == 0)
        {
        task = EDeleteDatabase1;
        }
    else if(aCmdLine.CompareF(_L("-delete_db2")) == 0)
        {
        task = EDeleteDatabase2;
        }
    else if(aCmdLine.CompareF(_L("-db_is_open")) == 0)
        {
        task = EIsDatabaseOpen;
        }
    else if(aCmdLine.CompareF(_L("-db_size")) == 0)
        {
        task = EDatabaseSize;
        }
    else if(aCmdLine.CompareF(_L("-copy_corrupt")) == 0)
        {
        task = ECopyCorruptDbFile;
        }
    else if(aCmdLine.CompareF(_L("-copy_corrupt_damaged")) == 0)
        {
        task = ECopyCorruptDamagedDbFile;
        }
    else if(aCmdLine.CompareF(_L("-copy_old")) == 0)
        {
        task = ECopyOldDbFile;
        }
    else if(aCmdLine.CompareF(_L("-add_event")) == 0)
        {
        task = EAddEvent;
        }
    else if(aCmdLine.CompareF(_L("-add_view_test_events")) == 0)
        {
        task = EAddTestEvents;
        }
    else if(aCmdLine.CompareF(_L("-add_event_type")) == 0)   
        {
        task = EAddEventType;
        }
    else if(aCmdLine.CompareF(_L("-invalid_schema")) == 0)
        {
        task = EInvalidSchema;
        }
    else if(aCmdLine.CompareF(_L("-is_matching_enabled")) == 0)
        {
        task = EIsMatchingEnabled;
        }
    else
        {
        RDebug::Print(_L("** t_logHiCapHelper, ** Bad command line argument: %S\r\n"), &aCmdLine);
        User::Panic(KLogHiCapHelperPanic, KErrArgument);
        }
    return task;
    }

static TInt EnvCreate()
    {
    TInt err = theFs.Connect();
    if(err == KErrNone)
        {
        err = theFs.MkDirAll(KLogEngPrivatePath);
        if(err == KErrAlreadyExists)
            {
            err = KErrNone; 
            }
        }
    if(err != KErrNone)
        {
        RDebug::Print(_L("** t_logHiCapHelper, error %d creating test environment\r\n"), err);
        }
    return err;
    }

TInt E32Main()
	{	
	__UHEAP_MARK;
	
	Initialize(_L("t_loghicaphelper"));

	CTrapCleanup* tc = CTrapCleanup::New();
	if(!tc)
	   {
        User::Panic(KLogHiCapHelperPanic, KErrNoMemory);
	   }
	
	TBuf<64> cmdLine;
	User::CommandLine(cmdLine);
	THelperOp task = TaskType(cmdLine);

    TInt rc = 0;
	TInt err = EnvCreate();
	if(err == KErrNone)
	    {
	    TRAP(err, rc = DoTaskL(task));    
	    if(err < 0)
	        {
	        rc = err;
	        RDebug::Print(_L("** t_logHiCapHelper, DoTaskL(), Task %d, Error %d\n"), task, rc);
	        }
	    }

    theFs.Close();
	delete tc;
	
	__UHEAP_MARKEND;

	return rc;
	}
	
