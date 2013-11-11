/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Base class for implementations of ctframework interfaces
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CFSCLIENT_H__
#define __CFSCLIENT_H__

#include "fstokencliserv.h"
#include <e32base.h>

class MCTToken;
class RFileStoreClientSession;

/**
 * Base class for implementations of ctframework interfaces.  Derived classes
 * inherit from this and also implement the appropriate interface.
 *
 * It provides asynchronous functionality for sending requests to the filetokens
 * server and processing the responses.
 *
 * Some methods are const so they can be called by the cert apps client, whose
 * interface contains several const methods.  These methods need to create
 * internal buffers, and this leads to iRequestDataBuf and iRequestPtr being
 * mutable.
 */
NONSHARABLE_CLASS(CFSClient) : public CActive
	{
public:
	virtual ~CFSClient();
	
protected:
	CFSClient(TInt aUID, MCTToken& aToken, RFileStoreClientSession& aClient);
	TInt AllocRequestBuffer(TInt aReqdSize) const;
	void FreeRequestBuffer() const;
	void SendSyncRequestAndHandleOverflowL(TFSTokenMessages aMessage,
										   TInt aInitialBufSize,
										   const TIpcArgs& aArgs) const;

protected:
	// From CActive
	virtual void DoCancel();	
	virtual TInt RunError(TInt aError);
	
protected:
	/**
	 * Maintains state for async requests to filetokens server, and completes
	 * caller when required.
	 */
	class TAsyncRequest
	{
	public:
		void operator()(TFSTokenMessages aRequest, TRequestStatus* aStatus);
		void Complete(TInt aCompletionResult);			
		void Cancel();
		inline TFSTokenMessages OutstandingRequest() { return iRequest; }
	public:
		~TAsyncRequest();
	private:
		TFSTokenMessages iRequest;
		TRequestStatus* iClientStatus;
	};
	
	TAsyncRequest iCurrentRequest;		///< The outstanding server request
	
private:
	mutable HBufC8* iRequestDataBuf;  	///< Buffer for messages passed to the server
	
protected:
	MCTToken& iToken;					///< The token we belong to
	TInt iInterfaceUID;					///< UID of the cryptoken interface we implement
	RFileStoreClientSession& iClient;	///< Client session object for sending messages to the server
	mutable TPtr8 iRequestPtr;			///< Buffer pointer for derived classes to use
	};

#endif	//	__CFSCLIENT_H__
