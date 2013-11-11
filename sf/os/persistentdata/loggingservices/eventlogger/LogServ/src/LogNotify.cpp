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

#include "LogNotify.h"
#include "logservpanic.h"
#include "LogServBackupInterface.h"
#include "LogServDatabaseChangeInterface.h"


CLogNotify::CLogNotify(MLogServBackupInterface& aBackupInterface, MLogServDatabaseChangeInterface& aChangeInterface, TInt aPriority)
:	CTimer(aPriority), iBackupInterface(aBackupInterface), iChangeInterface(aChangeInterface), iCompleteClientMessage(ETrue)
	{
	CActiveScheduler::Add(this);
	}

CLogNotify::~CLogNotify()
	{
	LOGTEXT("CLogNotify::~CLogNotify()");

	// This will only complete the client-side request if its not already
	// been completed
	Cancel();
	//
	iChangeInterface.DCIRequestChangeNotificationsCancel(*this);
	iBackupInterface.BIObserverRemove(*this);

	LOGTEXT("CLogNotify::~CLogNotify() - end");
	}

void CLogNotify::ConstructL()
	{
	LOGTEXT("CLogNotify::ConstructL()");

	CTimer::ConstructL();

	// Register for change events
	iChangeInterface.DCIRequestChangeNotificationsL(*this);

	// Register for backup events
	iBackupInterface.BIObserverAddL(*this, MLogServBackupInterface::EObjectClientChangeNotifications);

	LOGTEXT("CLogNotify::ConstructL() - end");
	}

CLogNotify* CLogNotify::NewL(MLogServBackupInterface& aBackupInterface, MLogServDatabaseChangeInterface& aChangeInterface, TInt aPriority)
	{
	CLogNotify* self = new(ELeave)CLogNotify(aBackupInterface, aChangeInterface, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogNotify::DCOHandleChangeEventsL(const CLogChangeDefinition& /*aChanges*/)
//
//	Handle changes within the log database
//
	{
	LOGTEXT2("CLogNotify::DCOHandleChangeEventsL() - client thread is: %S", &iClientThreadName);

	// Increment number of changes but only notify the client side request
	// if the delay timer isn't running.
	++iCommit;
	//
	if	(!IsActive())
		CheckForChanges();

	LOGTEXT("CLogNotify::DCOHandleChangeEventsL() - end");
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogNotify::BOHandleEventL(TLogServBackupEvent aEvent)
	{
	switch(aEvent)
		{
	case EBackupStarting:
		LOGTEXT("CLogNotify::BOHandleEventL(EBackupStarting)");

		// A rather inelegant fix but gets rid of one whole class
		iCompleteClientMessage = EFalse;
		Cancel();
		iCompleteClientMessage = ETrue;
		break;

	case EBackupEnded:
		LOGTEXT("CLogNotify::BOHandleEventL(EBackupEnded)");
		// Simulate a change
		++iCommit;
		if	(!IsActive())
			CheckForChanges();
		break;
		}

	LOGTEXT("CLogNotify::BOHandleEventL() - end");
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogNotify::Notify(TTimeIntervalMicroSeconds32 aDelay, const RMessage2& aMessage)

	{
#ifdef LOGGING_ENABLED
	RThread t;
	aMessage.Client(t);
	iClientThreadName = t.Name();
	t.Close();
	LOGTEXT3("CLogNotify::Notify(%d, %S)", aDelay.Int(), &iClientThreadName);
#endif

	__ASSERT_DEBUG(!IsActive(), Panic(ELogAlreadyActive18));
	Cancel();

	// Reset our change counter
	if (iLastCommit == 0)
		iLastCommit = iCommit;

	// Save a message pointer to client request status
	iMessagePtr = aMessage;

	// Wait until the delay has passed before notifying clients of changes
	After(aDelay);

	LOGTEXT("CLogNotify::Notify() - end");
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogNotify::Cancel()
	{
	LOGTEXT2("CLogNotify::Cancel() - client thread is: %S", &iClientThreadName);

	CActive::Cancel();

	// Reset
	iLastCommit = 0;

	// Complete the client-side observer
	if	(iCompleteClientMessage)
		CompleteClientRequest(KErrCancel);

	LOGTEXT("CLogNotify::Cancel() - end");
	}

void CLogNotify::RunL()
//
//	RunL is called if the timer has expired
//
	{
	LOGTEXT3("CLogNotify::RunL(%d) - client thread is: %S", iStatus.Int(), &iClientThreadName);

	CheckForChanges();

	LOGTEXT("CLogNotify::RunL() - end");
	}

void CLogNotify::DoCancel()
	{
	LOGTEXT2("CLogNotify::DoCancel() - client thread is: %S", &iClientThreadName);

	// This is what really cancel's us
	CTimer::DoCancel();

	LOGTEXT("CLogNotify::DoCancel() - end");
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogNotify::CheckForChanges()
	{
	LOGTEXT4("CLogNotify::CheckForChanges() - iLastCommit = %d, iCommit = %d, client thread name: %S", iLastCommit, iCommit, &iClientThreadName);

	if	(iLastCommit != iCommit)
		{
		LOGTEXT("CLogNotify::CheckForChanges() - changes detected, attempting to notify client");
		iLastCommit = iCommit;
		CompleteClientRequest(RDbNotifier::ECommit);
		}

	LOGTEXT("CLogNotify::CheckForChanges() - end");
	}

void CLogNotify::CompleteClientRequest(TInt aCompletionCode)
	{
	LOGTEXT3("CLogNotify::CompleteClientRequest() - aCompletionCode = %d, thread: %S", aCompletionCode, &iClientThreadName);

	if	(!iMessagePtr.IsNull())
		{
		LOGTEXT2("CLogNotify::CompleteClientRequest() - completing outstanding client request status for thread: %S", &iClientThreadName);
		iMessagePtr.Complete(aCompletionCode);
		}
	else
		{
		LOGTEXT2("CLogNotify::CompleteClientRequest() - thread %S doesn't have an outstanding notification request", &iClientThreadName);
		}

	LOGTEXT("CLogNotify::CompleteClientRequest() - end");
	}
