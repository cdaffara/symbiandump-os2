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

#include <logview.h>
#include "t_logutil2.h"

RTest TheTest(_L("t_logcontact"));

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1018
@SYMTestCaseDesc	    Tests for backup operations
@SYMTestPriority 	    High
@SYMTestActions  	    Check by adding events and closing the backup file.
                        Check for ErrNone flag. 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void TestBackupL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1018 "));
	CTestActive* active = new (ELeave) CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

   	CBaBackupSessionWrapper* backup = CBaBackupSessionWrapper::NewL();
   	CleanupStack::PushL(backup);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TDriveUnit driveUnit(EDriveC);
	TDriveName name = driveUnit.Name();

	TParse parse;
	LEAVE_IF_ERROR(parse.Set(KLogDatabaseName, &name, NULL));

	//Close, then delete LogEng database file. Restart backup operations monitoring
	//on LogEng database.
    backup->CloseFileL(parse.FullName(), MBackupObserver::EReleaseLockNoAccess);
 	User::After(1000000);
	TestUtils::DeleteDatabaseL(EFalse);
	backup->RestartFile(parse.FullName());
 	User::After(1000000);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//Close LogEng database file
	backup->CloseFileL(parse.FullName(), MBackupObserver::EReleaseLockNoAccess);
	//Delete LogEng database file
 	User::After(1000000);
	TestUtils::DeleteDatabaseL(EFalse);
	//Restart backup operations monitoring on LogEng database.
	backup->RestartFile(parse.FullName());
	User::After(1000000);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	//Close LogEng database file.
	backup->CloseFileL(parse.FullName(), MBackupObserver::EReleaseLockNoAccess);
	//Delete LogEng database file.
 	User::After(1000000);
	TestUtils::DeleteDatabaseL(EFalse);
	//Restart backup operations monitoring on LogEng database.
	backup->RestartFile(parse.FullName());
	User::After(1000000);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//Close LogEng database
	backup->CloseFileL(parse.FullName(), MBackupObserver::EReleaseLockNoAccess);
 	User::After(1000000);
 	//Delete LogEng database
	TestUtils::DeleteDatabaseL(EFalse);
	//Restart backup operations monitoring on LogEng database.
	backup->RestartFile(parse.FullName());

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//Close LogEng database
	backup->CloseFileL(parse.FullName(), MBackupObserver::EReleaseLockNoAccess);
	User::After(1000000);
 	//Delete LogEng database
	TestUtils::DeleteDatabaseL(EFalse);
	//Restart backup operations monitoring on LogEng database.
	backup->RestartFile(parse.FullName());
	User::After(1000000);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	backup->CloseAll(MBackupObserver::EReleaseLockNoAccess, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
 	User::After(1000000);

	TestUtils::DeleteDatabaseL(EFalse);
	backup->RestartAll();
 	User::After(1000000);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(3); // backup, event, active
	}
	

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logcontact"));

	TheTest.Start(_L("Prepare the test environment"));
	
	// Create log client
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	// Send a message to the hicap helper to delete logeng DB 
	TestUtils::DeleteDatabaseL();
			
	TheTest.Next(_L("Backup"));
	TestBackupL(*client);
	TheTest.Next(_L("Backup completed ok"));
	theLog.Write(_L8("Test 2 OK\n"));
	
	CleanupStack::PopAndDestroy(client);
	}
