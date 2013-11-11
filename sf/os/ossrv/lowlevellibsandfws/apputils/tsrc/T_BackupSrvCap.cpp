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
//

#include <e32debug.h>
#include <e32test.h>
#include <e32std.h>
#include <babackup.h>

LOCAL_D RTest				TheTest (_L("T_BACKUPSRVCAP"));

/**
@SYMTestCaseID          SYSLIB-BAFL-UT-4049
@SYMTestCaseDesc        BAFL Backup System Potential Security Vulnerability
@SYMTestPriority        High
@SYMTestActions         Call CloseAll, CloseFileL, RestartAll, RestartFile and NotifyBackupOperationL without correct capablity.
@SYMTestExpectedResults Capablity error is expected. Test must not fail*/
void Defect_DEF103793L()
	{
	TInt err;

	CBaBackupSessionWrapper* backupSession  = CBaBackupSessionWrapper::NewL();

	TBackupOperationAttributes attribs;
	attribs.iFileFlag=MBackupObserver::EReleaseLockNoAccess;
	attribs.iOperation=MBackupOperationObserver::EStart;

	TRAP(err, backupSession->NotifyBackupOperationL(attribs));
	TheTest(err == KErrNone);

	attribs.iOperation=MBackupOperationObserver::EEnd;

	TRAP(err, backupSession->NotifyBackupOperationL(attribs));
	TheTest(err == KErrNone);

	TRequestStatus status;
	backupSession->CloseAll(MBackupObserver::EReleaseLockNoAccess, status);
	User::WaitForRequest (status);
	TheTest(status == KErrPermissionDenied);

	_LIT(KFileName1, "FileName1");
	TFileName file;
	file.Copy(KFileName1);

	backupSession->RestartFile(file);

	backupSession->RestartAll();

	TRAP(err, backupSession->CloseFileL(file, MBackupObserver::EReleaseLockNoAccess));
	TheTest(err == KErrPermissionDenied);

	delete backupSession;
	}

/**
Invoke the tests
*/
LOCAL_C void DoTestsL()
    {
	Defect_DEF103793L();
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* theTrapCleanup=CTrapCleanup::New();

	TheTest.Printf(_L("\n"));
	TheTest.Title();
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-UT-4049 Backupsrc capablity Tests "));

	TRAPD(err, DoTestsL());
	TheTest(err == KErrNone);

	TheTest.End();
	TheTest.Close();

	delete theTrapCleanup;

	__UHEAP_MARKEND;
	return (KErrNone);
	}
