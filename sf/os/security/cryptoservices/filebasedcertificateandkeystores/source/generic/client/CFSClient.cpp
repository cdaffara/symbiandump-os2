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
*
*/


#include "CFSClient.h"
#include "clientutils.h"
#include "clientsession.h"

CFSClient::CFSClient(TInt aUID,
					 MCTToken& aToken, 
					 RFileStoreClientSession& aClient)
	:	CActive(EPriorityNormal),
		iToken(aToken), 	
		iInterfaceUID(aUID),	
		iClient(aClient),
		iRequestPtr(NULL, 0, 0)
	{
	}

CFSClient::~CFSClient()
	{
    Cancel(); 
    delete iRequestDataBuf;
	}

CFSClient::TAsyncRequest::~TAsyncRequest()
	{
	__ASSERT_DEBUG(EIdle==iRequest, FSTokenPanic(ERequestOutstanding));
	}

void CFSClient::TAsyncRequest::operator()(TFSTokenMessages aRequest, TRequestStatus* aStatus)
	{
	__ASSERT_DEBUG(EIdle==iRequest, FSTokenPanic(ERequestOutstanding));

	iRequest = aRequest; 
	iClientStatus = aStatus;
	*aStatus = KRequestPending;
	}

void CFSClient::TAsyncRequest::Complete(TInt aCompletionResult)
	{
	__ASSERT_DEBUG(EIdle!=iRequest, FSTokenPanic(ENoRequestOutstanding));
	User::RequestComplete(iClientStatus, aCompletionResult);
	iRequest = EIdle;
	}

void CFSClient::TAsyncRequest::Cancel()
	{
	User::RequestComplete(iClientStatus, KErrCancel);
	iRequest = EIdle;
	}

void CFSClient::FreeRequestBuffer() const
	{
    delete iRequestDataBuf; 
	iRequestDataBuf = NULL;
	iRequestPtr.Set(NULL, 0, 0);
	}

TInt CFSClient::AllocRequestBuffer(TInt aReqdSize) const
	{
    ASSERT(aReqdSize > 0);
	TInt result = KErrNoMemory;
	
	FreeRequestBuffer();
	iRequestDataBuf = HBufC8::NewMax(aReqdSize);
	if (iRequestDataBuf)
		{
	    iRequestPtr.Set(iRequestDataBuf->Des());
		iRequestPtr.FillZ();
		result = KErrNone;
		}

	return result;
	}

/**
 * Execute a synchronous request that returns a buffer of indetermintate length.
 *
 * If the initial buffer is too short, the server leaves with KErrOverflow and
 * passes us the required length - the reuest is then re-sent.
 */
void CFSClient::SendSyncRequestAndHandleOverflowL(TFSTokenMessages aMessage,
												  TInt aInitialBufSize,
												  const TIpcArgs& aArgs) const
	{
	User::LeaveIfError(AllocRequestBuffer(aInitialBufSize));
	
	TInt err = iClient.SendRequest(aMessage, aArgs);
	if (err == KErrOverflow)
		{
		TInt sizeReqd = 0;
		TPckg<TInt> theSize(sizeReqd);
		theSize.Copy(iRequestPtr);
		User::LeaveIfError(AllocRequestBuffer(sizeReqd));			
		err = iClient.SendRequest(aMessage, aArgs);
		}

	User::LeaveIfError(err);
	}

//	*********************************************************************************
//	CActive implementation
//	*********************************************************************************

void CFSClient::DoCancel()
	{
	iCurrentRequest.Cancel();
	}

TInt CFSClient::RunError(TInt aError)
	{
	iCurrentRequest.Complete(aError);
	return KErrNone; // Handled
	}
