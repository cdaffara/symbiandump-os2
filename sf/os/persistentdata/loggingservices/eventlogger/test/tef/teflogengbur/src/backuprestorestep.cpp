// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of test execute steps for log engine backup and restore
// 
//

/**
 @file
 @internalComponent
*/
#ifndef __BACKUPRESTORESTEP_H__
#define __BACKUPRESTORESTEP_H__

#include "backuprestorestep.h"
#include "LogCliServShared.h"
#include "LogServShared.h"

const TUid KTestEventUid1 = {0x10005393};
_LIT(KTestEventDesc1, "Event Type Description");
const TUid KTestEventUid2 = {0x10005394};
_LIT(KTestEventDesc2, "Second Event Type Description");
_LIT(KLogDatabaseName,"c:\\private\\101f401d\\Logdbu.dat");
#ifndef _DEBUG
_LIT(KLogEngineServerName,"LogServ*");
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////  CBackupRestoreStepBase  //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Override of base class virtual
Initializes members needed for test
*/
TVerdict CBackupRestoreStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;
	
	iScheduler = new(ELeave)CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	
	User::LeaveIfError(iFs.Connect());
	
	iClient = CLogClient::NewL(iFs);

	iActive = new(ELeave) CTestActive();

	return EPass;
	}

CBackupRestoreStepBase::~CBackupRestoreStepBase()
	{
	delete iActive;
	iActive = NULL;
	
	delete iClient;
	iClient = NULL;
	
	iFs.Close();
	
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	iScheduler = NULL;
	
	__UHEAP_MARKEND;
	}

void CBackupRestoreStepBase::GetDataFromConfigL(TUid& aEventUid, TPtrC& aEventDesctiption)
	{
	TInt dataSet = 0;
	const TDesC& section = ConfigSection();
	TInt err = GetIntFromConfig(section, _L("DataSet"), dataSet);
	if(err < 0 || dataSet < 1 || dataSet > 2)
		{
		ERR_PRINTF1(_L("Failed to read mandatory setting (DataSet) from testexecute config file."));
		User::Leave(KErrNotFound);
		}
	if (dataSet == 1)
		{
		aEventUid = KTestEventUid1;
		aEventDesctiption.Set(KTestEventDesc1);
		}
	else if (dataSet == 2)
		{
		aEventUid = KTestEventUid2;
		aEventDesctiption.Set(KTestEventDesc2);
		}
	}
	

/**
Adds test entry to the log engine
*/
void CBackupRestoreStepBase::TestAddEventTypeL()
	{
	INFO_PRINTF1(_L("Add test entry to log engine\n"));

	TUid eventUid;
	TPtrC eventDescription;
	GetDataFromConfigL(eventUid, eventDescription);
	
	CLogEventType *type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(eventUid);
	type->SetDescription(eventDescription);
	type->SetLoggingEnabled(ETrue);

	iActive->Start();
	iClient->AddEventType(*type, iActive->iStatus);
	CActiveScheduler::Start();
	TESTL((iActive->iStatus == KErrNone)||(iActive->iStatus == KErrAlreadyExists));

	CleanupStack::PopAndDestroy(type);
	}

/**
Checks whether the test entry added by TestAddEventTypeL exists or not
*/
void CBackupRestoreStepBase::TestGetEventTypeL(TInt aExpectedError)
	{
	INFO_PRINTF1(_L("Make sure test entry is retrieved successfully from log engine\n"));
	
	TUid eventUid;
	TPtrC eventDescription;
	GetDataFromConfigL(eventUid, eventDescription);
	
	CLogEventType *type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(eventUid);

	iActive->Start();
	iClient->GetEventType(*type, iActive->iStatus);
	CActiveScheduler::Start();
	TEST_CHECKL(iActive->iStatus.Int(), aExpectedError, _L("Error retrieving log event"));

	if(aExpectedError == KErrNone)
		{
		TEST_CHECKL(type->Uid().iUid, eventUid.iUid, _L("UID retrieved from log event does not match"));
		TEST_CHECKL(type->Description().Compare(eventDescription), 0, _L("Description retrieved from log event does not match"));
		TEST_CHECKL(type->LoggingEnabled(), ETrue, _L("Logging enabled value retreived from log event does not match"));
		}
	CleanupStack::PopAndDestroy(type);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////  CBackupRestoreInitializeStep  ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CBackupRestoreInitializeStep::~CBackupRestoreInitializeStep()
	{
	}

CBackupRestoreInitializeStep::CBackupRestoreInitializeStep()
	{
	SetTestStepName(KBackupRestoreInitializeStep);
	}

/**
Override of base class pure virtual
Adds an entry to the log engine then tests that it was added successfully
*/
TVerdict CBackupRestoreInitializeStep::doTestStepL()
	{
	TestAddEventTypeL();
	
	TestGetEventTypeL(KErrNone);

	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////  CBackupRestoreVerifyStep  ////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CBackupRestoreVerifyStep::~CBackupRestoreVerifyStep()
	{
	}

CBackupRestoreVerifyStep::CBackupRestoreVerifyStep()
	{
	SetTestStepName(KBackupRestoreVerifyStep);
	}

/**
Override of base class pure virtual
Tests that entry added in CBackupRestoreInitializeStep exists
*/
TVerdict CBackupRestoreVerifyStep::doTestStepL()
	{
	TestGetEventTypeL(KErrNone);

	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////  CBackupRestoreVerifyStep2  ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CBackupRestoreVerifyStep2::~CBackupRestoreVerifyStep2()
	{
	}

CBackupRestoreVerifyStep2::CBackupRestoreVerifyStep2()
	{
	SetTestStepName(KBackupRestoreVerifyStep2);
	}

/**
Override of base class pure virtual.
Tests that the entry added after the backup but before the restore operation - that entry
does not exist anymore.
*/
TVerdict CBackupRestoreVerifyStep2::doTestStepL()
	{
	TestGetEventTypeL(KErrNotFound);

	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////  CStopLogServerStep  //////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CStopLogServerStep::~CStopLogServerStep()
	{
	}

CStopLogServerStep::CStopLogServerStep()
	{
	SetTestStepName(KStopLogServerStep);
	}

/**
Override of base class pure virtual
Causes the the log engine server to exit
*/
TVerdict CStopLogServerStep::doTestStepL()
	{
	TInt err;
	//making the server transient only works on UDEB builds
#ifdef _DEBUG
	INFO_PRINTF1(_L("Stopping the log engine server\n"));
	TInt error = iLogServ.Connect();
	// Is the server running?
	if (error != KErrNotFound && error != KErrNone)
		{
		INFO_PRINTF1(_L("Error connecting to log engine server\n"));
		User::Leave(error);
		}

	// Make the server transient
	TInt p0=1;
	TIpcArgs  ipcArgs(p0);
	User::LeaveIfError(iLogServ.Send(ELogMakeTransient,ipcArgs));

	iLogServ.Close();
	User::After(6 * 1000000); // Enough time for the server to exit
		
#else
	//"ELogMakeTransient" IPC message is compiled in _DEUG mode only.
	//The logengine server could hold the database, then deleting its file will return an error
	//We force the killing of Logengine server here.
	// Find process and Kill
	// Note: this needs CAPABILITY PowerMgmt
	INFO_PRINTF1(_L("Killing the log engine server...\n"));
	TFindProcess findProcess(KLogEngineServerName);
	TFullName result;
	err = findProcess.Next(result);
    if (err == KErrNone)
    	{
    	RProcess server;
    	User::LeaveIfError( server.Open(findProcess, EOwnerProcess) );
    	server.Kill(0);		
    	INFO_PRINTF1(_L("Logengine server killed...\n"));
    	User::After(6000000);
    	}
    else
    	{
    	INFO_PRINTF1(_L("Unable to find the process linked to Logengine server.\n"));
    	}
    
#endif//_DEBUG
    
    RFs fs;
    CleanupClosePushL(fs);
    err = fs.Connect();
	TEST_CHECKL(err, KErrNone, _L("RFs::Connect() failed"));
	INFO_PRINTF1(_L("Deleting log engine database...\n"));
	err = fs.Delete(KLogDatabaseName);
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("Unable to delete log engine database, err=%d.\n"), err);
		}
	INFO_PRINTF1(_L("Log engine database deleted...\n"));
	CleanupStack::PopAndDestroy(&fs);
	    
	return EPass;
	}

#endif //__BACKUPRESTORESTEP_H__

