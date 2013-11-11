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
// Defines the object that waits for notification of 
// Backup/Restore events from the EPOC OS
// 
//

#ifndef __BACKUPNOTIFIER_H__
#define __BACKUPNOTIFIER_H__

#include <e32base.h>
#include <babackup.h>
#include "BackupNotifierObserver.h"

 /**
	@internalComponent
	@since 7.0
	The notification object which waits for a signal that backup has begun/ended.
	When its RunL method is called, it notifies its owning CRegistrar class
	object to suspend/resume persistant storage of the Interface Implementation
	registration details.
 */

class CBackupNotifier : public CBase, private MBackupOperationObserver
	{
public:
	/**
		@fn				NewL(MBackupNotifierObserver& aObserver)
		Intended Usage	: Standardised safe construction which leaves nothing 
						on the cleanup stack.
		Error Condition	: Cannot fully construct because of memory limitations.	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aRegistrar A reference to its owning class instance.
		@return			CBackupNotifier* A pointer to the new class
		@pre 			None
		@post			CBackupNotifier is fully constructed, and initialised.
	 */
	
	static CBackupNotifier* NewL(MBackupNotifierObserver& aObserver);

	/**
		@fn				~CBackupNotifier()
		Intended Usage	: Standard default d'tor	
		Error Condition	: None	
		@since			7.0
		@pre 			CBackupNotifier is fully constructed.
		@post			CBackupNotifier is totally destroyed
	 */
	
	virtual ~CBackupNotifier();

private:
	/**
		@fn				CBackupNotifier(MBackupNotifierObserver& aObserver)
		Intended Usage	: Default c'tor	
		Error Condition	: None	
		@since			7.0
		@param			aRegistrar A reference to its owning class instance.
		@pre 			None
		@post			CBackupNotifier is fully constructed
	 */
	
	CBackupNotifier(MBackupNotifierObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Initialisation phase of two phase construction.
		Error Condition	: None
		@leave  		None
		@since			7.0
		@pre 			CBackupNotifier is fully constructed.
		@post			CBackupNotifier is fully initialised.
	 */
	
	void ConstructL();

	/**
		@fn				HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes)
		Intended Usage	: To allow a response to be made by an interested party fo a change in state of the 
						backup operation.
		@leave  		: @see CBaBackupSessionWrapper::NewL
		@leave  		: @see CBaBackupSessionWrapper::RegisterBackupOperationObserverL
		@since			7.0
		@param			aBackupOperationAttributes The current state of the backup operation.
		@return			void 
		@pre 			CBackupNotifier is fully constructed.
		@post			The aBackupOperationAttributes value will have been used
						to drive the suspend and restore behaviours of the ECOM Server.
	 */
	
	void HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes);

	/**
		@fn				RegisterForNotificationsL()
		Intended Usage	: To allow registration for notifications from the backup operation.
		Error Condition	: None
		@since			7.0
		@pre 			CBackupNotifier is fully constructed.
		@post			CBackupNotifier is registered for notification messages.
	 */
	
	void RegisterForNotificationsL();

private:
	/** A reference to its observer instance */
	
	MBackupNotifierObserver& iObserver;
	/** The owned notification control object */
	
	CBaBackupSessionWrapper* iBackupSession;
	// Forward declare the retry controller as a friend
	class CRetryTimer;
	friend class CRetryTimer;
	/** The friendly timer for connection retries */
	
	CRetryTimer* iRetryTimer;

	/** The flag indicating that registration occurred sucessfully */
	
	TBool iIsRegistered;
	/** Friendship declaration for testing purposes */
	friend class TBackupNotifier_StateAccessor;
	};

#endif	// __BACKUPNOTIFIER_H__
