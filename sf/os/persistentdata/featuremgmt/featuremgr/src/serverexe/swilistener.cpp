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

#include "swilistener.h"
#include <sacls.h>
#include "featmgrfeatureregistry.h"
#include "featmgrdebug.h"

/*
* CSWIListener definition
*/

CSWIListener::CSWIListener(CFeatMgrFeatureRegistry* aReg) : CActive(EPriorityIdle), iRegistry(aReg)
	{
	}

CSWIListener* CSWIListener::NewL(CFeatMgrFeatureRegistry* aReg)
	{
	CSWIListener* self = new(ELeave) CSWIListener(aReg);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

CSWIListener::~CSWIListener()
	{
	Cancel();
	iProperty.Close();
	}

void CSWIListener::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(KUidSystemCategory, KSAUidSoftwareInstallKeyValue));
	CActiveScheduler::Add(this);
	// Initial subscription and processing the initial value of the property
	iProperty.Subscribe(iStatus);
	SetActive();
	}

void CSWIListener::RunL()
	{
	if( iStatus.Int() == KErrNone )
		{
		// Resubscribe before processing new values to prevent missing updates
		iProperty.Subscribe(iStatus);
		SetActive();
		
		TInt value = -1;
		if( iProperty.Get(value) != KErrNotFound )
			{
			// If installation or uninstallation successful
			if( ((value&ESASwisInstall) && (value&ESASwisStatusSuccess)) ||
				((value&ESASwisUninstall) && (value&ESASwisStatusSuccess)) )
				{
				// This is the case when SWIEnd is called and there is no OOM
				if( !iRegistry->SWICacheStatusOOM() && !iRegistry->SWICacheStarted() )
					{
					INFO_LOG( "CSWIListener::RunL - Committing after successful install");
					iRegistry->CommitSWIFeatChanges();
					}
				else
					{
					INFO_LOG( "CSWIListener::RunL - Resetting since SWIEnd not called");
					iRegistry->SWIReset();
					}
				}
			// If installation or uninstallation aborted
			else if( ((value&ESASwisInstall) && (value&ESASwisStatusAborted)) ||
					 ((value&ESASwisUninstall) && (value&ESASwisStatusAborted)) )
				{
				INFO_LOG( "CSWIListener::RunL - Aborting");
				iRegistry->SWIAborted();
				}
			}
		}
	}

void CSWIListener::DoCancel()
	{
	iProperty.Cancel();
	}


/*
* CSWITimer definition
*/

CSWITimer* CSWITimer::NewL(const TTimeIntervalMicroSeconds32 aInterval, MSWICallBack* aCallBack)
	{
	CSWITimer* self = new(ELeave) CSWITimer(aCallBack);
	CleanupStack::PushL(self);
	self->ConstructL(aInterval);
	CleanupStack::Pop(1);
	return self;
	}

CSWITimer::CSWITimer(MSWICallBack* aCallBack) : CTimer(EPriorityStandard), iCallBack(aCallBack)
	{
	}

CSWITimer::~CSWITimer()
	{
	Cancel();
	}

void CSWITimer::ConstructL(const TTimeIntervalMicroSeconds32 aInterval)
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
    After(aInterval);
	}

void CSWITimer::RunL()
	{
	iCallBack->SWITimedOut();
	}

void CSWITimer::DoCancel()
	{
	CTimer::DoCancel();
	}
