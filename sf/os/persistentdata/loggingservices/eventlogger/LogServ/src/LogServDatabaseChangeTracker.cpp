// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "LogServDatabaseChangeTracker.h"
#include "LogChangeDefinition.h"
#include "LogServDatabaseTransactionInterface.h"
#include "logservpanic.h"
#include "LogServBackupInterface.h"

// Constants
const TInt KLogArrayGranularityObservers = 3;
const TInt KLogArrayGranularityGlobalChanges = 2;

#define UNUSED_VAR(a) a = a

/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServDatabaseChangeTracker (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServDatabaseChangeTracker::CLogServDatabaseChangeTracker(MLogServDatabaseTransactionInterface& aDatabase, MLogServBackupInterface& aBackupInterface, TInt aPriority)
:	CActive(aPriority), iDatabase(aDatabase), iBackupInterface(aBackupInterface), iObservers(KLogArrayGranularityObservers), iGlobalChanges(KLogArrayGranularityGlobalChanges)
	{
	CActiveScheduler::Add(this);
	}

CLogServDatabaseChangeTracker::~CLogServDatabaseChangeTracker()
	{
	Cancel();
	//
	iBackupInterface.BIObserverRemove(*this);
	iNotifier.Close();
	iObservers.Close();
	iGlobalChanges.Close();
	//
	delete iIdler;
	delete iChanges;
	}

void CLogServDatabaseChangeTracker::ConstructL()
	{
	iBackupInterface.BIObserverAddL(*this, MLogServBackupInterface::EObjectChangeTracker);
	iChanges = CLogChangeDefinition::NewL();
	//
	const TInt error = iNotifier.Open(iDatabase.DTIDatabase());
	User::LeaveIfError(error);
	//
	Request();
	//
	iIdler = CIdle::NewL(CActive::EPriorityIdle);
	}

CLogServDatabaseChangeTracker* CLogServDatabaseChangeTracker::NewL(MLogServDatabaseTransactionInterface& aDatabase, MLogServBackupInterface& aBackupInterface, TInt aPriority)
	{
	CLogServDatabaseChangeTracker* self = new(ELeave) CLogServDatabaseChangeTracker(aDatabase, aBackupInterface, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServDatabaseChangeTracker::DCISubmitChangedEventContextL(TLogDatabaseChangeType aType, TLogId aEventId)
	{
	__ASSERT_DEBUG(aType != ELogChangeTypeUndefined, Panic(ELogUnrecognizedChangeType2));
	iChanges->AddL(aEventId, aType, KErrGeneral);
	}

void CLogServDatabaseChangeTracker::DCISubmitGlobalChangeContextL(TUid aChangeType, TInt aContextParam1, TInt aContextParam2, TInt aContextParam3)
	{
	const TLogServDatabaseChangeDefinition item(aChangeType, aContextParam1, aContextParam2, aContextParam3);
	User::LeaveIfError(iGlobalChanges.Append(item));
	//
	iIdler->Cancel();
	iIdler->Start(TCallBack(IdleNotifyGlobalChangeEvents, this));
	}

void CLogServDatabaseChangeTracker::DCIRequestChangeNotificationsL(MLogServDatabaseChangeObserver& aObserver)
	{
	const TInt error = iObservers.InsertInAddressOrder(&aObserver);
	User::LeaveIfError(error);
	}

void CLogServDatabaseChangeTracker::DCIRequestChangeNotificationsCancel(MLogServDatabaseChangeObserver& aObserver)
	{
	const TInt error = iObservers.FindInAddressOrder(&aObserver);
	if	(error >= KErrNone)
		iObservers.Remove(error);
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServDatabaseChangeTracker::BOHandleEventL(TLogServBackupEvent aEvent)
	{
	switch(aEvent)
		{
	case EBackupStarting:
		{
		Cancel();
		iNotifier.Close();
		break;
		}
	case EBackupEnded:
		{
		const TInt error = iNotifier.Open(iDatabase.DTIDatabase());
		User::LeaveIfError(error);
		Request();
		break;
		}
	default:
		__ASSERT_DEBUG(EFalse, User::Invariant());
		break;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServDatabaseChangeTracker::RunL()
	{
	// We are only interested if a commit has occurred
	const TInt completionCode = iStatus.Int();
	if	(completionCode == RDbNotifier::ECommit)
		{
		LOGTEXT("CLogServDatabaseChangeTracker::RunL() - database commit");

		// Notify all observers
		NotifyObserversL();

		// Reset changes
		iChanges->Reset();
		}
	Request();
	}

void CLogServDatabaseChangeTracker::DoCancel()
	{
	iNotifier.Cancel();
	}

TInt CLogServDatabaseChangeTracker::RunError(TInt /*aError*/)
	{
	// Ignore errors such as leaves from notifying observers
	Request();
	return KErrNone;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServDatabaseChangeTracker::Request()
	{
	iNotifier.NotifyChange(iStatus);
	SetActive();
	}

void CLogServDatabaseChangeTracker::NotifyObserversL()
	{
	LOGTEXT("CLogServDatabaseChangeTracker::NotifyObserversL()");
	//
	const TInt count = iObservers.Count();
	for(TInt i=0; i<count; i++)
		{
		MLogServDatabaseChangeObserver* observer = iObservers[i];
		observer->DCOHandleChangeEventsL(*iChanges);
		}

	// Reduce memory usage
	iChanges->Reset();
	//
	LOGTEXT("CLogServDatabaseChangeTracker::NotifyObserversL() - end");
	}

void CLogServDatabaseChangeTracker::NotifyGlobalChangeEventsL()
	{
	LOGTEXT("CLogServDatabaseChangeTracker::NotifyGlobalChangeEventsL()");
	//
	const TInt count = iGlobalChanges.Count();
	for(TInt i=0; i<count; i++)
		{
		const TLogServDatabaseChangeDefinition& change = iGlobalChanges[i];
		//
		const TInt observerCount = iObservers.Count();
		for(TInt j=0; j<observerCount; j++)
			{
			MLogServDatabaseChangeObserver* observer = iObservers[j];
			observer->DCOHandleGlobalChangeEventL(change);
			}
		}

	// Reduce memory usage
	iGlobalChanges.Reset();
	iGlobalChanges.GranularCompress();
	//
	LOGTEXT("CLogServDatabaseChangeTracker::NotifyGlobalChangeEventsL() - end");
	}

TBool CLogServDatabaseChangeTracker::IdleNotifyGlobalChangeEvents(TAny* aSelf)
	{
	CLogServDatabaseChangeTracker* self = reinterpret_cast<CLogServDatabaseChangeTracker*>(aSelf);
	TRAPD(err, self->NotifyGlobalChangeEventsL());
    UNUSED_VAR(err);
	return EFalse;
	}
