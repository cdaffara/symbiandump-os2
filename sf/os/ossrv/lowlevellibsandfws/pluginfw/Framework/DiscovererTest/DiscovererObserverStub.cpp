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


#include "DiscovererObserverStub.h"

// ______________________________________________________________________________
//
CObserverStub* CObserverStub::NewL(CUnitTest& aOwner, CDiscoverer_UnitTestContext& aContext)
	{
	CObserverStub* self= NewLC(aOwner, aContext);  // calls c'tor and ConstructL
	CleanupStack::Pop();				// removes self
	return self;
	}

CObserverStub* CObserverStub::NewLC(CUnitTest& aOwner, CDiscoverer_UnitTestContext& aContext)
	{
	CObserverStub* self=new(ELeave) CObserverStub(aOwner, aContext);  // calls c'tor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL();	// Complete the 'construction'.
	return self;
	}

CObserverStub::~CObserverStub()
	{
	// Do nothing
	}

CObserverStub::CObserverStub(CUnitTest& aOwner, CDiscoverer_UnitTestContext& aContext)
: CBase(), iOwner(aOwner), iContext(aContext)
	{
	// Deliberately do nothing here : See ConstructL() for initialisation completion.
	}

void CObserverStub::ConstructL()
	{
	// Do nothing
	}

// Support of the CDiscovererObserver interface
void CObserverStub::DiscoveriesBegin()
	{
	// Log out some information
	_LIT(KDiscoveriesBegin,"CObserverStub::DiscoveriesBegin called");
	iContext.DataLogger().LogInformation(KDiscoveriesBegin);
	}

void CObserverStub::RegisterDiscoveryL(const TEntry& /* aDirEntry */)
	{
	// Check the iteration and leave if its the first one
	// after asking for a repeat
	CTransition& transition = iOwner.GetCurrentTransition();
	const TTransitionInfo& info = transition.TransitionInfo();
	if(info.iIteration == KFirstTransitionIteration)
		{
		_LIT(KRegisterDiscoverRepeat,"CObserverStub::RegisterDiscoveryL repeat requested");
		iContext.DataLogger().LogInformation(KRegisterDiscoverRepeat);
		transition.RepeatOnce();
		User::Leave(KTestBedRepeatTest);
		}
	else
		{
		_LIT(KRegisterDiscoverSuccess,"CObserverStub::RegisterDiscoveryL completed");
		iContext.DataLogger().LogInformation(KRegisterDiscoverSuccess);
		}
	}

void CObserverStub::DiscoveriesComplete(TBool aSuccessful)
	{
	// Log out some info and cleanup the context
	_LIT(KDiscoveriesComplete,"CObserverStub::DiscoveriesComplete called");
	iContext.DataLogger().LogInformation(KDiscoveriesComplete);
	TInt error = KErrNone;
	if(!aSuccessful)
		error = KErrCancel;
	if(iContext.iNotificationStatus)
		{
		User::RequestComplete(iContext.iNotificationStatus, error);
		iContext.iNotificationStatus = NULL;
		}
	}

void CObserverStub::DriveRemovedL(TDriveUnit aDrive)
	{
	_LIT(KDriveRemovedL,"CObserverStub::DriveRemovedL called for drive %d ");
	iContext.DataLogger().LogInformationWithParameters(KDriveRemovedL,aDrive);
	}

void CObserverStub::DriveReinstatedL(TDriveUnit aDrive)
	{
	// Check the iteration and leave if its the first one
	// after asking for a repeat
	CTransition& transition = iOwner.GetCurrentTransition();
	const TTransitionInfo& info = transition.TransitionInfo();
	if(info.iIteration == KFirstTransitionIteration)
		{
		_LIT(KRegisterDiscoverRepeat,"CObserverStub::DriveReinstatedL for drive %d repeat requested");
		iContext.DataLogger().LogInformationWithParameters(KRegisterDiscoverRepeat, aDrive);
		transition.RepeatOnce();
		User::Leave(KTestBedRepeatTest);
		}
	else
		{
		_LIT(KRegisterDiscoverSuccess,"CObserverStub::DriveReinstatedL for drive %d completed");
		iContext.DataLogger().LogInformationWithParameters(KRegisterDiscoverSuccess, aDrive);
		}
	}

TBool CObserverStub::NotifiedWithErrorCode(TInt aError)
	{
	// Log out some info and cleanup the context
	_LIT(KNotifiedWithErrorCode,"CObserverStub::NotifiedWithErrorCode called");
	iContext.DataLogger().LogInformation(KNotifiedWithErrorCode);
	if(iContext.iNotificationStatus)
		{
		User::RequestComplete(iContext.iNotificationStatus, aError);
		iContext.iNotificationStatus = NULL;
		}
	return EFalse;
	}
