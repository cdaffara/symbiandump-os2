/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Key Management Server session class code.
*
*/


/**
 @file
*/

#include "kmsclient.h"

#include "kmsserver.h"


CKeyMgmtSession* CKeyMgmtSession::NewL()
/**
	Factory function allocates new instance of CKeyMgmtSession.  Used
	when the KMS client opens a session with the server.
	
	@return					New, initialized instance of CKeyMgmtSession.
							The returned object is owned by the caller.
 */
	{
	CKeyMgmtSession* self = new(ELeave) CKeyMgmtSession();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CKeyMgmtSession::ConstructL()
/**
	Initialize this session by opening a channel to the KMS device driver.
	This session is maintained until the session is destroyed.
 */
	{
	TInt r = iChannel.Open();
	User::LeaveIfError(r);
	}

CKeyMgmtSession::~CKeyMgmtSession()
/**
	Free resources owned by this session.  Specifically, closes the
	KMS LDD channel.
 */
	{
	iChannel.Close();
	
	// if the last session was removed then stop the server
	CKeyMgmtServer* srv = (CKeyMgmtServer*) Server();
	srv->DecrementSessionCount();
	}

void CKeyMgmtSession::ServiceL(const RMessage2& aMessage)
/**
	Implement CSession2 by handling the supplied function request.

	@param	aMessage		Information about request from client.
							Provides access to function identifier
							and arguments which were passed.
 */
	{
	RThread thd;
	aMessage.ClientL(thd);
	CleanupClosePushL(thd);
	
	RProcess pr;
	User::LeaveIfError(thd.Process(pr));
	CleanupClosePushL(pr);
	
	KeyMgmtSvrImpl::TServerFunction f =
		static_cast<KeyMgmtSvrImpl::TServerFunction>(aMessage.Function());
	
	switch (f)
		{
	case KeyMgmtSvrImpl::EGenerateKey:
		GenerateKeyL(aMessage, pr);
		break;

	case KeyMgmtSvrImpl::EStoreKey:
		StoreKeyL(aMessage, pr);
		break;
	
	case KeyMgmtSvrImpl::EDeleteKey:
		DeleteKeyL(aMessage, pr);
		break;
	
	case KeyMgmtSvrImpl::EAddUsage:
		AddUsageL(aMessage, pr);
		break;
	
	case KeyMgmtSvrImpl::EDeleteUsage:
		DeleteUsageL(aMessage, pr);
		break;
	
	case KeyMgmtSvrImpl::ESetHeapFail:
		__UHEAP_MARK;
		__UHEAP_SETFAIL(RAllocator::EDeterministic, aMessage.Int0());
		break;
	
	case KeyMgmtSvrImpl::EEndHeapFail:
		__UHEAP_MARKEND;
		__UHEAP_RESET;
		break;
	
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	
	CleanupStack::PopAndDestroy(2, &thd);
	
	// if an error occured then the handler function will have left
	aMessage.Complete(KErrNone);
	}

void CKeyMgmtSession::ServiceError(const RMessage2& aMessage, TInt aError)
/**
	Override CSession2 by handling any leave which occured during the ServiceL.

	If the leave is because of a bad descriptor, then panick the client with
	EBadClientDescriptor.  Otherwise, complete the request with that error code.

	@param	aMessage		Message which caused leave to occur.
	@param	aError			Leave code.  This is a Symbian OS error code.
	@see CSession2::ServiceError
 */
	{
	if (aError == KErrBadDescriptor)
		aMessage.Panic(KKmsClientPanicCat, EBadClientDescriptor);
	else
		aMessage.Complete(aError);
	}

void CKeyMgmtSession::GenerateKeyL(const RMessage2& aMessage, RProcess aClientProc)
/**
	Generate a new key in the hardware store.

	@param	aMessage		Provides access to data sent by client.
	@param	aClientProc		The client process, which is used to manage key ownership.
 */
	{
	TInt len = aMessage.Int0();
	TKeyHandle kh;
	TRequestStatus rs;
	
	iChannel.GenerateKey(len, aClientProc, kh, rs);
	User::WaitForRequest(rs);
	User::LeaveIfError(rs.Int());

	WriteHandleToClientL(aMessage, aClientProc, kh);
	}

void CKeyMgmtSession::StoreKeyL(const RMessage2& aMessage, RProcess aClientProc)
/**
	Store the client's descriptor as a key in the hardware store.

	@param	aMessage		Provides access to data sent by client.
	@param	aClientProc		The client process, which is used to manage key ownership.
 */
	{
	TInt dataLen = aMessage.GetDesLengthL(0);
	
	// create server-side copy of the data
	HBufC8* srvData = HBufC8::NewLC(dataLen);
	TPtr8 srvPtr = srvData->Des();
	aMessage.ReadL(0, srvPtr);
	
	TKeyHandle kh;
	TRequestStatus rs;
	iChannel.StoreKey(srvPtr, aClientProc, kh, rs);
	User::WaitForRequest(rs);
	User::LeaveIfError(rs.Int());

	WriteHandleToClientL(aMessage, aClientProc, kh);
	
	CleanupStack::PopAndDestroy(srvData);
	}

void CKeyMgmtSession::WriteHandleToClientL(const RMessage2& aMessage, RProcess aClientProc, TKeyHandle aHandle)
/**
	Helper function for GenerateKeyL and StoreKeyL attempts to write the supplied
	key handle back to the client which owns this session object.
	
	If the handle cannot be written to the client, this function attempts to delete
	the supplied key, but cannot perform further cleanup if that fails.

	@param	aMessage		Provides access to data sent by client.  Specifically, argument 1
							refers to a descriptor which packages the client's TKeyHandle object.
	@param	aClientProc		The client process, which is used to manage key ownership.  This is
							used to delete the newly-created key if cannot write to client handle.
	@param	aHandle			Handle value to write back to client.
 */
	{
	TPckgC<TKeyHandle> khPckg(aHandle);
	
	TInt r = aMessage.Write(1, khPckg);
	if (r != KErrNone)
		{
		TRequestStatus rs;
		iChannel.DeleteKey(aClientProc, aHandle, rs);
		User::WaitForRequest(rs);
		User::Leave(r);
		}
	}

void CKeyMgmtSession::DeleteKeyL(const RMessage2& aMessage, RProcess aClientProc)
/**
	Delete a key from the hardware store.

	@param	aMessage		Provides access to data sent by client.  Specifically, argument 0
							is the key handle as an integer.
	@param	aClientProc		The client process, which is used to manage key ownership.  This is
							used to delete the newly-created key if cannot write to client handle.
 */
	{
	TKeyHandle kh = { aMessage.Int0() };
	
	TRequestStatus rs;
	iChannel.DeleteKey(aClientProc, kh, rs);
	User::WaitForRequest(rs);
	User::LeaveIfError(rs.Int());
	}

void CKeyMgmtSession::AddUsageL(const RMessage2& aMessage, RProcess aClientProc)
/**
	Add a usage entry to the hardware store.

	@param	aMessage		Provides access to data sent by client.  Specifically, argument 0
							is the key handle as an integer.  Argument 1 is the operation as an
							integer, and argument 3 refers to the descriptor which packages the
							client's security policy object.
	@param	aClientProc		The client process, which is used to manage key ownership.  This is
							used to delete the newly-created key if cannot write to client handle.
 */
	{
	TKeyHandle kh = { aMessage.Int0() };
	TInt op = aMessage.Int1();
	
	TSecurityPolicyBuf secPolBuf;
	TInt r = aMessage.Read(2, secPolBuf);
	User::LeaveIfError(r);
	
	TRequestStatus rs;
	iChannel.AddUsage(aClientProc, kh, op, secPolBuf(), rs);
	User::WaitForRequest(rs);
	User::LeaveIfError(rs.Int());
	}

void CKeyMgmtSession::DeleteUsageL(const RMessage2& aMessage, RProcess aClientProc)
/**
	Delete a usage entry from the hardware store.

	@param	aMessage		Provides access to data sent by client.  Specifically, argument 0
							is the key handle as an integer.  Argument 1 is the operation as an
							integer.
	@param	aClientProc		The client process which claims to own the key.
 */
	{
	TKeyHandle kh = { aMessage.Int0() };
	TInt op = aMessage.Int1();
	
	TRequestStatus rs;
	iChannel.DeleteUsage(aClientProc, kh, op, rs);
	User::WaitForRequest(rs);
	User::LeaveIfError(rs.Int());
	}

