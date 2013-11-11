// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of CActiveBackupClient class.
// 
//

/**
 @file
 @released
*/

#include "abclient.h"
#include "abclientsession.h"
#include "abachandler.h"

namespace conn
	{

	CActiveBackupClient::CActiveBackupClient() : iClientSession(NULL), iABCallbackHandler(NULL)
	/** 
	Class constructor.
	*/
		{
		}


	EXPORT_C CActiveBackupClient* CActiveBackupClient::NewL()
	/**
    This method creates a CActiveBackupClient, connects to the Secure Backup Server
	and does not wish to be called back so does not supply an implementation of
	MActiveBackupDataClient.

    If this is called when the Secure Backup Server is not active then it will leave
    with KErrNotSupported.
    
	@return Pointer to a created CActiveBackupClient object
	*/
		{
		CActiveBackupClient* self = new (ELeave) CActiveBackupClient();
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		return self;
		}

	EXPORT_C CActiveBackupClient* CActiveBackupClient::NewL(MActiveBackupDataClient* aClient)
	/**
    This method creates a CActiveBackupClient, connects to the Secure Backup Server
    and supplies a pointer to a MActiveBackupDataClient implementation.

    If this is called when the Secure Backup Server is not active then it will leave
    with KErrNotSupported.

    @param aClient pointer to an object that implements the MActiveBackupDataClient
				   mixin.  If this is NULL then the data owner does not take part in
				   active backup or restore.

	@panic KErrNotFound Debug only - If an ActiveScheduler is not installed
	@leave Release only - If an ActiveScheduler is not installed
	@return Pointer to a created CActiveBackupClient object
	*/
		{
		CActiveBackupClient* self = new (ELeave) CActiveBackupClient();
		CleanupStack::PushL(self);
		self->ConstructL(aClient);
		CleanupStack::Pop(self);
		return self;
		}

	void CActiveBackupClient::ConstructL()
	/**
	Construct this instance of CActiveBackupClient
	*/
		{
		iClientSession = new (ELeave) RABClientSession;
		
		User::LeaveIfError(iClientSession->Connect());
		}

	void CActiveBackupClient::ConstructL(MActiveBackupDataClient* aClient)
	/**
	Construct this instance of CActiveBackupClient

	@param aClient  Pointer to a concrete instance of MActiveBackupDataClient
	*/
		{
		ConstructL();
		
		iABCallbackHandler = CActiveBackupCallbackHandler::NewL(aClient, *iClientSession);
		iABCallbackHandler->StartListeningForServerMessagesL();
		}


	EXPORT_C CActiveBackupClient::~CActiveBackupClient()
	/**
	Standard destructor.
	*/
		{
		delete iABCallbackHandler;
		if (iClientSession)
			{
			iClientSession->Close();
			}
		delete iClientSession;
		iClientSession = NULL;
		}

    EXPORT_C void CActiveBackupClient::BURModeInfoL(TDriveList& aDriveList, TBURPartType& aBackupType, TBackupIncType& aIncBackupType)
    /**
    This method returns the type(s) of backup / restore operation currently active

    @param aDriveList list of drives involved in backup and restore
    @param aBackupType enumerated type indicating whether a backup or restore
    is in progress and whether full or partial.
    @param aIncBackupType enumerated type indicating whetherr a backup is base
    or incremental.
    */
		{
		iClientSession->BURModeInfoL(aDriveList, aBackupType, aIncBackupType);
		}


    EXPORT_C TBool CActiveBackupClient::DoesPartialBURAffectMeL()
    /**
    This method can be called when a partial backup or restore is active and will indicate
    whether the calling process is expected to take part.  If a full backup or restore is 
    active then this method will return ETrue for all data owners.  If no backup or restore
    is active then this method will return EFalse for all data owners.

    @return ETrue if the calling data owner is involved in the current backup or restore
    operation.
    */
		{
		return iClientSession->DoesPartialBURAffectMeL();
		}


    EXPORT_C void CActiveBackupClient::ConfirmReadyForBURL(TInt aErrorCode)
    /**
    This method is called to indicate to the Secure Backup Server that the data owner is ready
    to participate in backup or restore.  The data owner must call this method to indicate
    readiness or the Secure Backup Server will not request or supply backup data.

    N.B. The Secure Backup Server will supply snapshot data (if relevant) before a data 
    owner indicates readiness as it assumes that the data owner requires snapshot data in
    order to prepare for a backp or restore.

    @param aErrorCode this should be set to KErrNone when the client is ready for
    backup or restore. If it is set to any other value then it indicates that the client
    cannot continue with the backup or restore and the error code will be supplied to
    the remote backup client.
    */
		{
		iClientSession->ConfirmReadyForBURL(aErrorCode);
		}

	} // end of conn namespace


