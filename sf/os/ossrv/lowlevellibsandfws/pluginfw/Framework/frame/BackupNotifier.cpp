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
// Implements the object that waits for notification of 
// Backup/Restore events from the SymbianOS
// 
//

#include "BackupNotifier.h"
#include "TestUtilities.h"	// For __FILE__LINE__

const TInt KRetryDelayPeriod = 15000000;	// 15 second delay before retry

/**
	@since 7.0	
	The timer Active object for providing backup server connection retries. 
	Note that task execution is dependant upon the task priority
	The default priority is idle time execution only.
	It is first scheduled by the CBackupNotifier::ConstructL() call.
 */
class CBackupNotifier::CRetryTimer : public CTimer
	{
public:
	/**
		@fn				NewL(CBackupNotifier& aBackupNotifier)
		Intended Usage	: Standardised safe construction which leaves nothing
						  on the cleanup stack.
		Error Condition	: Not enough memory available.	
		@leave			KErrNoMemory
		@since			7.0
		@param			aBackupNotifier A reference to the owning CBackupNotifier
		@return			CRetryTimer* a pointer to the new class
		@pre 			None
		@post			CRetryTimer is fully constructed, and initialised.
	 */	
	static CRetryTimer* NewL(CBackupNotifier& aBackupNotifier);

	/**
		@fn				virtual ~CRetryTimer()
		Intended Usage	: Standard default d'tor	
		Error Condition	: None	
		@since			7.0
		@pre 			CRetryTimer is fully constructed.
		@post			CRetryTimer is totally destroyed
	 */
	virtual ~CRetryTimer();

private:
	/**
		@fn				CRetryTimer(CBackupNotifier& aBackupNotifier)
		Intended Usage	: Standardised default c'tor made explicit to avoid unintentional
						conversion construction by the compiler.	
		Error Condition	: None	
		@since			7.0
		@param			aBackupNotifier A reference to its owning class instance.
		@pre 			None
		@post			CRetryTimer is fully constructed
	 */
	explicit CRetryTimer(CBackupNotifier& aBackupNotifier);

	/**
		@fn				void ConstructL()
		Intended Usage	: Standardised 2nd, (Initialisation) phase of two phase construction.
		Error Condition	: Leaves with error code : usually KErrNoMemory.
		@leave			KErrNoMemory.
		@since			7.0
		@return			void 
		@pre 			CRetryTimer is fully constructed.
		@post			CRetryTimer is fully initialised.
	 */
	void ConstructL();

	/**
		@fn				DoCancel()
		Intended Usage	: The cancel action called by CActive::Cancel(). 
		Error Condition	: None	
		@since			7.0
		@pre 			CRetryTimer is fully constructed.
		@post			CRetryTimer is no longer active on the current scheduler.
	 */
	void DoCancel();

	/**
		@fn				RunL()
		Intended Usage	: When the object activates, this method calls
						the CBackupNotifier to attempt a reconnection to the backup service notifier. 
		@leave  		KErrNoMemory
		@since			7.0
		@pre 			CRetryTimer is fully constructed.
		@post			The notification service has been sccessfully connected.
	 */
	void RunL();

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Called when the backup notification connection
						failed by leaving.
		@since			7.0
		@param			aError The error code that the RunL left with.
		@return			TInt KErrNone. 
		@pre 			CRetryTimer is fully constructed.
		@post			CRetryTimer is re-activated.
	 */
	TInt RunError(TInt aError);

private:
	/** A reference to its owning class instance */
	CBackupNotifier& iNotifier;	
	};

// __________________________________________________________________________
//
/*
	The timer Active object for providing backup service connection retry attempts.
*/
CBackupNotifier::CRetryTimer* CBackupNotifier::CRetryTimer::NewL(CBackupNotifier& aBackupNotifier)
	{
	CRetryTimer* self = new(ELeave) CRetryTimer(aBackupNotifier);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBackupNotifier::CRetryTimer::CRetryTimer(CBackupNotifier& aBackupNotifier)
	: CTimer(CActive::EPriorityIdle), iNotifier(aBackupNotifier)
	{
	// Safe because it cannot fail
	CActiveScheduler::Add(this);
	}

void CBackupNotifier::CRetryTimer::ConstructL()
	{	
	CTimer::ConstructL();
	}

CBackupNotifier::CRetryTimer::~CRetryTimer()
	{
	Cancel();
	}

void CBackupNotifier::CRetryTimer::DoCancel()
	{
	// Call the base class to ensure the timer is cancelled
	CTimer::DoCancel();
	}

void CBackupNotifier::CRetryTimer::RunL()
//	When the object activates, this is method is called
//  and delegates to the CBackupNotifier to re-attempt
//  the service connection.
//
	{
	iNotifier.RegisterForNotificationsL();
	}

TInt CBackupNotifier::CRetryTimer::RunError(TInt /*aError*/)
// When the notification connection fails, the RunL of the retry timer
// leaves. We trap this here and allow the BackupNotifier to clean up
// and reset for a retry.
	{
	After(KRetryDelayPeriod);
	return KErrNone;	// Do not pass error back, as retry will happen
	}


//___________________________________________________________________________
//
CBackupNotifier* CBackupNotifier::NewL(MBackupNotifierObserver& aObserver)
	{
	CBackupNotifier* self = new(ELeave) CBackupNotifier(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBackupNotifier::CBackupNotifier(MBackupNotifierObserver& aObserver)
: CBase(), iObserver(aObserver)
	{
	// Do nothing
	}

CBackupNotifier::~CBackupNotifier()
	{
	if(iBackupSession)
		{
		if(iIsRegistered)
			iBackupSession->DeRegisterBackupOperationObserver(*this);
		delete iBackupSession;
		}
	delete iRetryTimer;
	}

void CBackupNotifier::HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes)
	{
	switch(aBackupOperationAttributes.iOperation)
		{
		case EStart:
			iObserver.Suspend();
			break;
		case EEnd:
			iObserver.Resume();
			break;
		default:
			break;
		}
	}

void CBackupNotifier::ConstructL()
	{
	iRetryTimer = CRetryTimer::NewL(*this);
	iRetryTimer->After(KRetryDelayPeriod);
	return;
	}

void CBackupNotifier::RegisterForNotificationsL()
	{
	if(!iBackupSession)
		iBackupSession = CBaBackupSessionWrapper::NewL();

	iBackupSession->RegisterBackupOperationObserverL(*this);
	iIsRegistered = ETrue;
	}
