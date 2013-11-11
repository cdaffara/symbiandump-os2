// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "tef_schsvr_bursuitesimstepbase.h"
#include "tef_schsvr_bursuitedefs.h"

CTefSchsvrBURSuiteSimStepBase::CTefSchsvrBURSuiteSimStepBase()
	{
	}

CTefSchsvrBURSuiteSimStepBase::~CTefSchsvrBURSuiteSimStepBase()
	{
	delete iBackupClient;	
	delete iSched;
	}

//
// building block test Helper functions
//

void CTefSchsvrBURSuiteSimStepBase::Simulate_StartBackupL()
	{
	// Use the babackup API to "start" a backup.  Ultimate effect
	// is to cause CTaskScheduler::BURBeginning() to be called.
	TBackupOperationAttributes attribs;
	
	attribs.iFileFlag=MBackupObserver::EReleaseLockReadOnly;
	attribs.iOperation=MBackupOperationObserver::EStart;
	iBackupClient->NotifyBackupOperationL(attribs);

	// Set BUR property to trigger task execution
	TInt ret = RProperty::Set(KTestUid , conn::KUidBackupRestoreKey, conn::EBackupBase|conn::EBURBackupPartial);
	if (ret != KErrNone && ret != KErrNotFound)
		{
		User::Leave(ret);
		}
	
	// allow observers to be notified
	User::After(100000);
	}


void CTefSchsvrBURSuiteSimStepBase::Simulate_EndBackupL()
	{
	// Use the babackup API to "end" a backup.  Ultimate effect
	// is to cause CTaskScheduler::BURCompleteL() to be called.
	TBackupOperationAttributes attribs;
	
	attribs.iFileFlag=MBackupObserver::ETakeLock;
	attribs.iOperation=MBackupOperationObserver::EEnd;
	iBackupClient->NotifyBackupOperationL(attribs);

	// allow observers to be notified
	User::After(100000);
	}


void CTefSchsvrBURSuiteSimStepBase::Simulate_StartRestoreL()
	{
	// Use the babackup API to "start" a restore.  Ultimate effect
	// is to cause CTaskScheduler::BURBeginning() to be called.
	TBackupOperationAttributes attribs;
	
	attribs.iFileFlag=MBackupObserver::EReleaseLockNoAccess;
	attribs.iOperation=MBackupOperationObserver::EStart;
	iBackupClient->NotifyBackupOperationL(attribs);

	// Set BUR property to trigger task execution
	TInt ret = RProperty::Set(KTestUid , conn::KUidBackupRestoreKey, conn::EBackupBase|conn::EBURRestorePartial);
	if (ret != KErrNone && ret != KErrNotFound)
		{
		User::Leave(ret);
		}

	// allow observers to be notified
	User::After(100000);
	}


void CTefSchsvrBURSuiteSimStepBase::Simulate_EndRestoreL(TBool aSuccess)
	{
	TBackupOperationAttributes attribs;

	// Use the babackup API to "end" a restore.  Ultimate effect
	// is to cause CTaskScheduler::BURCompleteL() to be called.
	if (aSuccess)
		{
		attribs.iFileFlag=MBackupObserver::ETakeLock;
		attribs.iOperation=MBackupOperationObserver::EEnd;
		iBackupClient->NotifyBackupOperationL(attribs);
		}
	else
		{
		attribs.iFileFlag=MBackupObserver::ETakeLock;
		attribs.iOperation=MBackupOperationObserver::EAbort;
		iBackupClient->NotifyBackupOperationL(attribs);
		}
	// allow observers to be notified
	User::After(100000);
	}

TVerdict CTefSchsvrBURSuiteSimStepBase::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);
	
	iSched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iSched);

	iBackupClient = CBaBackupSessionWrapper::NewL();

	SetTestStepResult(EPass);		
	return TestStepResult();
	}
