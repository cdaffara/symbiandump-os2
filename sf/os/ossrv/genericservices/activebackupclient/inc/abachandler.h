/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Declaration of CActiveBackupCallbackHandler
* 
*
*/



/**
 @file
*/
#ifndef __ACTIVEBACKUPCALLBACKHANDLER_H__
#define __ACTIVEBACKUPCALLBACKHANDLER_H__

#include <e32base.h>
#include <connect/abclient.h>
#include <connect/abclientserver.h>

namespace conn
	{ 
	class MActiveBackupDataClient;
	class RABClientSession;
	
	class CActiveBackupCallbackHandler : public CActive
		/**
		CActiveBackupCallbackHandler is used to handle the asynchronous calls back from the
		active backup server. IPC only allows client initiated message transfer and hence the 
		Active Backup Callback Handler must prime the server with an initial message and use 
		the async response to "send" a message to the client side.
		
		@internal component
		*/
		{
	public:

		/** 
		Empty virtual destructor to avoid memory leaks
		*/
		virtual ~CActiveBackupCallbackHandler();
		
		static CActiveBackupCallbackHandler* NewL(MActiveBackupDataClient* aABDC, 
			RABClientSession& aClientSession);
		
		/**
		RunL will synchronously call the callback implementation passed in to this object
		*/
		void RunL();
		
		/**
		*/
		void DoCancel();
		
		/**
		RunError implementation should send the leave code to the ABServer for propagation back 
		to the SBEngine client. This enables any leaves from the callbacks to be returned to the 
		PC for display/handling by the user
		
		@return any unhandled error code
		*/
		TInt RunError(TInt aError);
		
		void ConstructL();
		
		void StartListeningForServerMessagesL();
	
	private:
		CActiveBackupCallbackHandler(MActiveBackupDataClient* aABDC, RABClientSession& aClientSession);
		void PrimeServerForCallbackL();
		void PrimeServerForCallbackWithResponseL(TInt aResponse);
		void PrimeServerForCallbackWithResponseL(TDesC8& aResponse);
		TPtr8 CreateFixedBufferL();
			
	private:
		/** Callback mixin implementation provided by the active backup client */
		MActiveBackupDataClient* iActiveBackupDataClient;
		
		/** The client session to call methods on */
		RABClientSession& iClientSession;

		/** This modifiable package buf is set by the Server to define which callback should 
		be called on the client */
		TPckgBuf<TABCallbackCommands> iCallbackCommand;
		
		/** Modifiable package buf containing the first of a maximum of two arguments for the callback */
		TPckgBuf<TInt> iCallbackArg1;

		/** Modifiable package buf containing the second of a maximum of two arguments for the callback */
		TPckgBuf<TInt> iCallbackArg2;
		
		/** Buffer for holding transferred data from server */
		HBufC8* iTransferBuffer;
		};

	} // end namespace

#endif // __ACTIVEBACKUPCALLBACKHANDLER_H__

