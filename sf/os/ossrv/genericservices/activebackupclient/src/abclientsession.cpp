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
// Implementation of RABClientSession class.
// 
//

/**
 @file
*/

#include "abclientsession.h"
#include "abclientserver.h"
#include "panic.h"

namespace conn
	{
	HBufC8* RABClientSession::GetDataFromServerLC(const TInt aDataLength, 
		TABCallbackCommands aCallbackRequestingData)
	/**
	Following a callback call to the Active Backup Callback Handler, this method requests the data 
	from the server. aDataLength will have been supplied by the original callback call from the server 
	and all that remains is to make a synchronous call to get it copied over
	
	@param aDataLength The length of the data that will be received
	@param aCallbackRequestingData The callback enum identifying the callback requesting this data
	@return Pointer to a heap allocated descriptor containing data from the server
	*/
		{
		HBufC8* pReturnedData = HBufC8::NewLC(aDataLength);
		
		TPtr8 returnedData(pReturnedData->Des());
		
		// Request that the server returns the promised data to the client for the specified callback 
		User::LeaveIfError(SendReceive(EABMsgGetDataSync, 
			TIpcArgs(static_cast<TInt>(aCallbackRequestingData), &returnedData)));
		
		return pReturnedData;
		}

	HBufC8* RABClientSession::GetDataFromServerLC(const TInt aDataLength, 
		TABCallbackCommands aCallbackRequestingData, TDriveNumber& aDriveNum)
	/**
	Following a callback call to the Active Backup Callback Handler, this method requests the data 
	from the server. aDataLength will have been supplied by the original callback call from the server 
	and all that remains is to make a synchronous call to get it copied over. This should only be called 
	from a ReceiveSnapshot callback
	
	@param aDataLength The length of the data that will be received
	@param aCallbackRequestingData The callback enum identifying the callback requesting this data
	@param aDriveNum The drive number

	@return Pointer to a heap allocated descriptor containing data from the server
	*/
		{
		TPckg<TDriveNumber> drvPkg(aDriveNum);
		HBufC8* pReturnedData = HBufC8::NewLC(aDataLength);
		
		TPtr8 returnedData(pReturnedData->Des());
		
		// Request that the server returns the promised data to the client for the specified callback 
		User::LeaveIfError(SendReceive(EABMsgGetDataSync, 
			TIpcArgs(static_cast<TInt>(aCallbackRequestingData), &returnedData)));
		
		// Request that the server returns the drive number to the client for the specified callback 
		TInt retVal = SendReceive(EABMsgGetDriveNumForSuppliedSnapshot);
		
		User::LeaveIfError(retVal);
		
		aDriveNum = static_cast<TDriveNumber>(retVal);
		
		return pReturnedData;
		}

	void RABClientSession::SendDataLengthToServerL(TDesC8& aData, TBool aFinished, 
		TABCallbackCommands aCallbackSendingData)
	/**
	Send the length and finished flag back to the server. Limitations of IPC mean that we can only send 
	fixed size data to the server, hence the length and finished flag are sent first so that the server 
	may allocate the appropriate amount of space prior to the response containing the data
	
	@param aData 		Descriptor containing the data to send back to the server. Only the length of this 
						is sent in this message
	@param aFinished 	Flag indicating that this is the last transfer from the client to server. If EFalse, 
						the server must call the callback again
	@param aCallbackSendingData This 
	*/
		{
		// Make the synchronous call back to the server telling it to allocate a big enough buffer
		// to handle the data transfer that will follow this 
		User::LeaveIfError(SendReceive(EABMsgSendDataLength, TIpcArgs(
										static_cast<TInt>(aData.Size()),
										static_cast<TInt>(aFinished),
										static_cast<TInt>(aCallbackSendingData))));
		}

	void RABClientSession::PrimeServerForCallbackL(TPckgBuf<TABCallbackCommands>& aCallback, 
		TPckgBuf<TInt>& aArg1, TPckgBuf<TInt>& aArg2, TRequestStatus& aStatus)
	/** 
	Send an async message to the server so that it can call us back when it's ready to make callback's
	
	@param aCallback This modifiable package buf is set by the server to indicate which callback to call
	@param aArg1 This is the first argument for the callback, set by the server
	@param aArg2 This is the second argument for the callback, set by the server
	@param aStatus The status 
	*/
		{
		SendReceive(EABMsgPrimeForCallback, TIpcArgs(&aCallback, &aArg1, &aArg2), aStatus);
		}
		
	void RABClientSession::PrimeServerForCallbackWithResponseL(TPckgBuf<TABCallbackCommands>& aCallback, 
		TPckgBuf<TInt>& aArg1, TPckgBuf<TInt>& aArg2, TInt aResult, TRequestStatus& aStatus)
	/** 
	Send an async message to the server so that it can call us back when it's ready to make callback's.
	This call also returns a result from the previous callback for the server to respond to/return to the PC etc.
	
	@param aCallback This modifiable package buf is set by the server to indicate which callback to call
	@param aArg1 This is the first argument for the callback, set by the server
	@param aArg2 This is the second argument for the callback, set by the server
	@param aResult The return value of the previous callback to pass back to the server
	@param aStatus The status 
	*/
		{
		SendReceive(EABMsgPrimeForCallbackAndResponse, TIpcArgs(&aCallback, &aArg1, &aArg2, aResult), aStatus);
		}

	void RABClientSession::PrimeServerForCallbackWithResponseL(TPckgBuf<TABCallbackCommands>& aCallback, 
		TPckgBuf<TInt>& aArg1, TPckgBuf<TInt>& aArg2, TDesC8& aResult, TRequestStatus& aStatus)
	/** 
	Send an async message to the server so that it can call us back when it's ready to make callback's.
	This call also returns a result from the previous callback for the server to respond to/return to the PC etc.
	
	@param aCallback This modifiable package buf is set by the server to indicate which callback to call
	@param aArg1 This is the first argument for the callback, set by the server
	@param aArg2 This is the second argument for the callback, set by the server
	@param aResult Data to send back to the server
	@param aStatus The status 
	
	@pre A call to SendDataLengthToServerL must have preceeded this call so that the server may prepare to receive data
	*/
		{
		SendReceive(EABMsgPrimeForCallbackAndResponseDes, TIpcArgs(&aCallback, &aArg1, &aArg2, &aResult), aStatus);
		}

	void RABClientSession::PropagateLeave(TInt aLeaveCode)		
	/**
	Send a synchronous IPC message to the server indicating that a leave has ocurred whilst executing a callback
	
	@param aLeaveCode The code to leave back to the server with
	@param aStatus The status 
	*/
		{
		SendReceive(EABMsgPropagateLeave, TIpcArgs(aLeaveCode));
		}
		
    void RABClientSession::BURModeInfoL(TDriveList& aDriveList, TBURPartType& aBackupType, TBackupIncType& aIncBackupType)
    /**
    This method returns the type(s) of backup / restore operation currently active

    @param aDriveList list of drives involved in backup and restore
    @param aBackupType enumerated type indicating whether a backup or restore
    is in progress and whether full or partial.
    @param aIncBackupType enumerated type indicating whetherr a backup is base
    or incremental.
    */
		{
		TPckg<TBURPartType> partTypePkg(aBackupType);
		TPckg<TBackupIncType> incTypePkg(aIncBackupType);
		
		User::LeaveIfError(SendReceive(EABMsgBURModeInfo, TIpcArgs(&aDriveList, &partTypePkg, &incTypePkg)));
		}


    TBool RABClientSession::DoesPartialBURAffectMeL()
    /**
    This method can be called when a partial backup or restore is active and will indicate
    whether the calling process is expected to take part.  If a full backup or restore is 
    active then this method will return ETrue for all data owners.  If no backup or restore
    is active then this method will return EFalse for all data owners.

    @return ETrue if the calling data owner is involved in the current backup or restore
    operation.
    */
		{
		TPckgBuf<TBool> partialAffectsMe;
		
		User::LeaveIfError(SendReceive(EABMsgDoesPartialAffectMe, TIpcArgs(&partialAffectsMe)));
		
		return partialAffectsMe();
		}


    void RABClientSession::ConfirmReadyForBURL(TInt aErrorCode)
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
		User::LeaveIfError(SendReceive(EABMsgConfirmReadyForBUR, TIpcArgs(aErrorCode)));
		}
		
	void RABClientSession::CancelServerCallbackL()
	/**
	Inform the server that it can no longer call callbacks on the client
	*/
		{
		User::LeaveIfError(SendReceive(EABMsgClosingDownCallback));
		}
		
	RABClientSession::RABClientSession()
	/** Class constructor. */
		{
		}

	void RABClientSession::Close()
	/** Closes the Secure Backup Engine handle. */
		{
		RSessionBase::Close();
		}

	TInt RABClientSession::Connect()
	/** Connects the handle to the Secure Backup Engine.

	@return KErrNone if successful, KErrCouldNotConnect otherwise
	*/
		{
		TInt nRetry = KABRetryCount;
		TInt nRet = KErrNotFound;

		while(nRetry > 0 && nRet != KErrNone)
			{
		    const TSecurityPolicy policy(static_cast<TSecureId>(KSBServerUID3));
			nRet = CreateSession(KABServerName, Version(), KABASyncMessageSlots, EIpcSession_Unsharable,&policy);
			if(nRet == KErrNotFound || nRet == KErrServerTerminated)
				{
				StartServer();
				}
			nRetry--;
			}

		return nRet;
		}

	TVersion RABClientSession::Version() const
	/** Returns the version of this API

	@return The version of this API
	*/
		{
	    return TVersion (KABMajorVersionNumber,
							KABMinorVersionNumber,
							KABBuildVersionNumber);
	  	}

	//
	// Server startup code
	TInt RABClientSession::StartServer()
	/** Start the server as a thread on WINS or a process on ARM.
	
	Called by Connect when the kernel is unable to create a session
	with the AB server (if the process hosting it is not running).

	@return Standard Symbian OS code from RProcess/RThread create.
	*/
		{
		//
		// Servers UID
		const TUidType serverUid(KNullUid, KNullUid, KSBServerUID3);
		
	
		RProcess server;
    	TInt nRet=server.Create(KSBImageName,KNullDesC,serverUid);
    	if (nRet != KErrNone)
    	    {
    		return nRet;
    		}
    		
    	TRequestStatus stat;
    	server.Rendezvous(stat);
    	if (stat != KRequestPending)
    	{
    		server.Kill(0);
    	}
    	else
    	{
    		server.Resume();
    	}
    	User::WaitForRequest(stat);
    	return (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
		
		}



	} // conn namespace
