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
* CScsSession implementation.	 See class and function definitions
* for more detail.
*
*/


/**
 @file
*/


#include <scs/scsserver.h>
#include "scsserverconstants.h"

EXPORT_C void CScsSession::ConstructL()
/**
	The subsession [handle] container could be constructed
	here, but that is deferred until the subsession is allocated to
	avoid the memory overhead for sessions which may not require
	subsessions.
 */
	{
	iServer.IncrementSessionCount();
	}

EXPORT_C CScsSession::CScsSession(CScsServer &aServer)
	: iServer(aServer)
/**
   Setup the iServer member variable so it can be used during construction
 */
	{
	}


EXPORT_C CScsSession::~CScsSession()
/**
	Deletes any subsessions.

	Deletes any outstanding requests.

	Decrements the server's session count so the server can be shut down
	if there are no open sessions.
 */
	{
	iServer.CancelOutstandingRequests(this, /* aCompleteClientRequests */ EFalse);
	
	DeleteSubsessionContainers();	// closes any remaining subsessions
	
	// decrement the session count, so the server can be shut down if
	// there are no more open sessions.
	iServer.DecrementSessionCount();
	}
	
EXPORT_C void CScsSession::CloseAllSubsessionsL()
	{
	DeleteSubsessionContainers();
	}

EXPORT_C void CScsSession::ServiceL(const RMessage2& aMessage)
/**
	Implement CSession2 by handling any SCS-specific messages, and
	otherwise delegating to the subclass' implementation.

	@param	aMessage		Standard server-side message object.
 */
	{
	ScsImpl::TScsFunction scsFunc;
	TInt implFunc;
	ScsImpl::ExtractScsAndImplFunctions(aMessage, &scsFunc, &implFunc);
	TBool completeMessage = ETrue;
	
	switch (scsFunc)
		{
	// sessions
	case ScsImpl::ECallSessionFunc:
		completeMessage = DoServiceL(implFunc, aMessage);
		break;
	
	case ScsImpl::EPreCloseSession:
		PreCloseSession();
		break;
	
	case ScsImpl::ECancelSessionFunc:
		CancelAsyncSessionRequestL(implFunc);
		break;

	case ScsImpl::EGetServerPid:
		{
		TPckgBuf<TProcessId> idBuf;
		RProcess proc;
		idBuf() = proc.Id();
		aMessage.WriteL(0, idBuf);
		break;
		}

	case ScsImpl::EShutdownAsap:
		iServer.ShutdownWhenIdleL();
		break;

	// subsessions
	case ScsImpl::ECreateSubsession:
		// if there are no open subsessions before this attempt then clean up
		// the containers if it fails.	This is to ensure the server heap balances
		// in the event of failure.
		iPreCreateSubsessionCount = iSsHandles ? iSsHandles->ActiveCount() : 0;
		CreateSubsessionL(implFunc, aMessage);
		break;
	
	case ScsImpl::ECloseSubsession:
		CloseSubsessionL(aMessage);
		break;
	
	case ScsImpl::ECallSubsessionFunc:
		completeMessage = CallSubsessionFunctionL(implFunc, aMessage);
		break;
	
	case ScsImpl::ECancelSubsessionFunc:
		CancelAsyncSubsessionRequestL(implFunc, aMessage);
		break;
	
	// server heap testing
	case ScsImpl::EUHeapSetFail:
#ifdef _DEBUG
		iServer.DoPreHeapMarkOrCheckL();
		__UHEAP_MARK;
		__UHEAP_SETFAIL(RAllocator::EDeterministic, aMessage.Int0());
		iServer.DoPostHeapMarkOrCheckL();
#endif
		break;
	
	case ScsImpl::EUHeapResetFail:
#ifdef _DEBUG
		iServer.DoPreHeapMarkOrCheckL();
		__UHEAP_RESET;
		__UHEAP_MARKEND;
		iServer.DoPostHeapMarkOrCheckL();
#endif
		break;
	
	// unrecognized SCS code, so fail with KErrNotSupported
	default:
		User::Leave(KErrNotSupported);
		}
	
	// None of the delegate functions have left so complete with KErrNone.
	if(completeMessage)
		{
		aMessage.Complete(KErrNone);
		}
	}

EXPORT_C void CScsSession::ServiceError(const RMessage2& aMessage, TInt aError)
/**
	Override CSession2 by handling any leave which occurred during the ServiceL.

	Panick the client if the leave is because of a bad descriptor or subsession
	handle.	 Otherwise, complete the request with the error code.

	@param	aMessage		Message which caused leave to occur.
	@param	aError			Leave code.	 This is a Symbian OS error code.
	@see CSession2::ServiceError
 */
	{
	// if failed to create first subsession then free containers so heap
	// balances after message has been completed.
	
	ScsImpl::TScsFunction scsFunc;
	ScsImpl::ExtractScsAndImplFunctions(aMessage, &scsFunc, NULL);
	
	if (scsFunc == ScsImpl::ECreateSubsession && iPreCreateSubsessionCount == 0)
		DeleteSubsessionContainers();
	
	switch (aError)
		{
	case KErrBadDescriptor:
		PanicClient(aMessage, ScsImpl::EScsClBadDesc);
		break;
	
	case KErrBadHandle:
		PanicClient(aMessage, ScsImpl::EScsClBadHandle);
		break;
	
	case KErrScsAsyncAlreadyQueued:
		PanicClient(aMessage, ScsImpl::EScsClAsyncAlreadyQueued);
		break;

	default:
		aMessage.Complete(aError);
		break;
		}
	}

// -------- session close --------

void CScsSession::PreCloseSession()
/**
	This function is invoked from RScsClientBase::Close
	just before the session is closed, to cancel any
	outstanding requests.
 */
	{
	iServer.CancelOutstandingRequests(this, /* aCompleteClientRequests */ ETrue);
	}

// -------- asynchronous requests --------

void CScsSession::CancelAsyncSessionRequestL(TInt aFunction)
/**
	This function is called when handling an ECancelSessionFunction message.
	If the outstanding function cannot be found, this function does nothing.

	@param aFunction		Implementation function without SCS code.
 */
	{
	iServer.CancelAsyncRequest(this, /* aSubsession */ 0, aFunction);
	}

// -------- subsessions --------

CScsSubsession* CScsSession::GetSubsessionL(const RMessage2& aMessage)
/**
	Extract subsession handle from the supplied message and return
	a pointer to the corresponding subsession object.
	
	@param	aMessage		Standard server-side message object.  The fourth
							argument is the subsession handle.
	@return					Pointer to corresponding subsession object.
	@leave					KErrBadHandle if the handle does not identify a
							current subsession.
 */
	{
	TInt handle = aMessage.Int3();
	CObject* obj = iSsHandles->AtL(handle);	// leaves with KErrBadHandle if not found
	return static_cast<CScsSubsession*>(obj);
	}

void CScsSession::DeleteSubsessionContainers()
/**
	Free the handle and object containers which this session uses
	to manage subsessions.	It is safe to call this function if the
	containers were not set up successfully.
 */
	{
	delete iSsHandles;
	iSsHandles = 0;

	if (iSsObjects != 0)
		{
		iServer.iContainerIndex->Remove(iSsObjects);
		iSsObjects = 0;
		}
	}

void CScsSession::CreateSubsessionL(TInt aFunction, const RMessage2& aMessage)
/**
	Attempt to allocate a subsession object for this session.  The actual
	subsession allocation is delegated to the subclass's implementation
	of DoCreateSubsessionL.	 If the subclass has not reimplemented this
	function, then default implementation leaves with KErrNotSupported.
	
	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side message object.
 */
	{
	// if this is the first subsession then create the object container and index
	if (iSsObjects == 0)
		{
		iSsObjects = iServer.iContainerIndex->CreateL();
		iSsHandles = CObjectIx::NewL();
		}
	
	CScsSubsession* ss = DoCreateSubsessionL(aFunction, aMessage);
	CleanupClosePushL(*ss);

	iSsObjects->AddL(ss);
	TInt handle = iSsHandles->AddL(ss);
	CleanupStack::Pop(ss);
	
	TPckg<TInt> handlePckg(handle);
	TInt r = aMessage.Write(3, handlePckg);
	User::LeaveIfError(r);
	}

#ifdef _BullseyeCoverage
static const char * const bull1="BullseyeCoverage save off";
#endif
EXPORT_C CScsSubsession* CScsSession::DoCreateSubsessionL(TInt aFunction, const RMessage2& aMessage)
/**
	This default implementation leaves with KErrNotSupported.  The subclass
	does not have to supply its own implementation unless it actually wants
	to support subsessions.
	
	@param	aFunction		Function identifier without SCS code.  The subclass
							implementation of this function would use this to decide
							what kind of subsession object to create.
	@param	aMessage		Client message.	 Not used.
	@leave					KErrNotSupported.
 */
	{
	(void) aFunction;
	(void) aMessage;
	
	User::Leave(KErrNotSupported);	
	/*lint -unreachable*/ // Avoid compiler warning and keep lint happy
	return 0;	
	}
#ifdef _BullseyeCoverage
static const char * const bull2="BullseyeCoverage restore";
#endif

void CScsSession::CloseSubsessionL(const RMessage2& aMessage)
/**
	Delete the subsession identified in the supplied message.

	If the subsession cannot be found this function leaves with KErrBadHandle,
	and the client is panicked in ServiceError.

	@param	aMessage		Standard server-side message handle.  The fourth
							integer contains the subsession handle.
 */
	{
	TInt handle = aMessage.Int3();
	CObject* obj = iSsHandles->AtL(handle);	// leaves with KErrBadHandle if not found
	CScsSubsession* ss = static_cast<CScsSubsession*>(obj);
	
	iServer.CancelOutstandingRequests(this, ss, /* aCancelClientRequests */ ETrue);

	iSsHandles->Remove(handle);
	
	// close the container objects to ensure the heap balances when the last subsession is closed
	if (iSsHandles->ActiveCount() == 0)
		DeleteSubsessionContainers();
	}

TBool CScsSession::CallSubsessionFunctionL(TInt aFunction, const RMessage2& aMessage)
/**
	Pass the supplied function identifier and message to the subsession
	which is identified in the message.

	If the subsession cannot be found this function leaves with KErrBadHandle,
	and the client is panicked in ServiceError.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Client message.
	@return ETrue means complete client request now.

 */
	{
	CScsSubsession* ss = GetSubsessionL(aMessage);
	return ss->DoServiceL(aFunction, aMessage);
	}

void CScsSession::CancelAsyncSubsessionRequestL(TInt aFunction, const RMessage2& aMessage)
/**
	Cancel an outstanding asynchronous request for the subsession
	identified in the supplied message.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side message handle.  The fourth
							integer contains the subsession handle.
 */
	{
	CScsSubsession* ss = GetSubsessionL(aMessage);
	iServer.CancelAsyncRequest(this, ss, aFunction);
	}

