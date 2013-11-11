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
* Base class functionality for server-side asynchronous requests.
*
*/


/**
 @file
*/


#include <scs/scsserver.h>
#include "scsserverconstants.h"

EXPORT_C CAsyncRequest::CAsyncRequest(CScsSession* aSession, CScsSubsession* aSubsession, const RMessage2& aMessage)
/**
	Record the session, subsession, and function associated with this request.
	These are required to complete or to cancel the request later.
	
	Adds this object to the active scheduler.

	@param	aSession		Session used to launch this request.
	@param	aSubsession		Subsession on which this request is queued.  This
							should be NULL if the request is relative to a session,
							instead of a subsession
	@param	aMessage		Handle to the kernel-side request object, which
							will be completed later.
 */
:	CActive(CActive::EPriorityStandard),
	iSession(aSession),
	iSubsession(aSubsession),
	iMessagePtr2(aMessage)
	{
	// extract implementation function
	ScsImpl::ExtractScsAndImplFunctions(aMessage, NULL, &iFunction);
	
	CActiveScheduler::Add(this);
	}

EXPORT_C void CAsyncRequest::TransferToScsFrameworkL()
/**
	Adds this asynchronous request to the global collection.
	This should be called after the subclass has performed its
	own initialization, but before the active request has been
	set up.
 */
	{
	iSession->iServer.AddAsyncRequestL(this);
	}

EXPORT_C void CAsyncRequest::RunL()
/**
	Implement CActive by completing the server client's request status
	with the AO error code, i.e. iStatus.  This function also marks this
	object for deletion.

	If a subclass overrides this implementation then it must call
	CompleteAndMarkForDeletion itself to complete the client-side
	request.

	@see CompleteAndMarkForDeletion
 */
	{
	TInt r = iStatus.Int();
	CompleteAndMarkForDeletion(r);
	}

EXPORT_C TInt CAsyncRequest::RunError(TInt aError)
/**
	Override CActive by completing the request with the
	supplied error code and marking this object for deletion.
	The default implementation of RunL cannot leave, but the
	subclass may override it with a more complex implementation.
	
	@param	aError			Error code with which aError left.
	@return					KErrNone, meaning do not propagate
							the error to the active scheduler.
 */
	{
	CompleteAndMarkForDeletion(aError);
	return KErrNone;
	}

void CAsyncRequest::CancelCompleteAndMarkForDeletion()
/**
	Cancel this asynchronous request and complete the client
	request with KErrCancel.  On exit, this object has been
	marked for deletion.
 */
	{
	CompleteAndMarkForDeletion(KErrCancel);
	}

EXPORT_C void CAsyncRequest::CompleteAndMarkForDeletion(TInt aError)
/**
	Completes the client-side request associated with this
	asynchronous request and marks this object for deletion.
	
	If the error was because the client passed a bad descriptor then
	panick the client instead of completing the request for consistency
	with synchronous requests.

	@param	aError			Error code with which the client request
							will be completed.
 */
	{
	if (aError == KErrBadDescriptor)
		PanicClient(iMessagePtr2, ScsImpl::EScsClBadDesc);
	else
		iMessagePtr2.Complete(aError);
	MarkForDeletion();
	}

void CAsyncRequest::MarkForDeletion()
/**
	Mark this outstanding request for deletion, so it is
	cleaned up when the cleanup AO runs.
 */
	{
	DoCleanup();
	// The deletion is processed at priority CActive::EPriorityHigh,
	// so should happen ahead of any pending or new requests.
	CAsyncCallBack* acb = iSession->iServer.iAsyncCleanup;
	acb->CallBack();		// no effect if already queued
	iSession = NULL;		// mark for deletion		
	}

EXPORT_C void CAsyncRequest::DoCleanup()
/**
	This virtual function is called whenever the SCS needs to cancel an operation.

	If the asynchronous request implementation ALWAYS has an internal request active
	for the duration of the client request, then this default implementation can be used, which
	simply calls  CActive::Cancel which will call the derived classe's DoCancel (if, and only if
	the object IsActive).

	A more complex case is where the asynchronous requests may enter an internal queue where
	the client request is (obviously still active), but the derived class has not outstanding request.
	In this case the default implementation will cause a HANG, because Cancel will not call DoCancel if
	the object is not active...

	In the generic case, the derived class should provide an overload for the Cleanup method.
	This function should call Cancel to cancel any outstanding operation, then perform whatever other
	cleanup is required to cleanup the request (closing handles, removing from queues etc).
	The framework will then complete the client request.

	 nb. The destructor of the CAsyncRequest may not run until sometime after the client request has been
	 completed.
 */
{
	Cancel();
}

