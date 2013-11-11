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
* Implements CScsServer functionality.  See class and functions definitions
* for more detail.
*
*/


/**
 @file
*/

#include <scs/scsserver.h>
#include "scsserverconstants.h"

static const TInt defaultPolicyRangeCount = 1;
static const TInt defaultPolicyRanges[defaultPolicyRangeCount] = 
				{
				0 // All requests
				};

static const TUint8 defaultPolicyElementsIndex[defaultPolicyRangeCount] = 
				{
				CPolicyServer::EAlwaysPass
				};

static const CPolicyServer::TPolicyElement defaultPolicyPolicyElements[1] = {};


static const CPolicyServer::TPolicy defaultPolicy =
				{
				CPolicyServer::EAlwaysPass, // Allow all connects
				defaultPolicyRangeCount,
				defaultPolicyRanges,
				defaultPolicyElementsIndex,
				defaultPolicyPolicyElements,
				};

EXPORT_C CScsServer::CScsServer(const TVersion& aVersion, CActive::TPriority aPriority)
/**
	Record this server's version so it can be compared against the requested
	version in NewSessionL.

	@param	aVersion		This server's version.	This is compared against each
							client's requested version when the client tries to connect.
	@param	aPriority		This server's active object priority.
 */
	:	CPolicyServer(aPriority, defaultPolicy, ESharableSessions)
	,iVersion(aVersion)
//	,iSessionCount(0)
	{
	// empty.
	}

EXPORT_C CScsServer::CScsServer(const TVersion& aVersion, const CPolicyServer::TPolicy& aPolicy, CActive::TPriority aPriority)
/**
	Record this server's version so it can be compared against the requested
	version in NewSessionL.

	@param	aVersion		This server's version.	This is compared against each
							client's requested version when the client tries to connect.
	@param	aPolicy			Can be used to configure security for server connect and session functions
	@param	aPriority		This server's active object priority.

	Note that aPolicy must be configured to allow all functions
	covered by the KScsFunctionMask mask.
 */
	:	CPolicyServer(aPriority, aPolicy, ESharableSessions)
	,iVersion(aVersion)
//	,iSessionCount(0)
	{
	// empty.
	}

EXPORT_C void CScsServer::ConstructL(TInt aShutdownPeriodUs)
/**
	Second-phase constructor allocates the shutdown timer for this server object.

	If aShutdownPeriodUs is not 0, this function starts the shutdown
	timer because the server starts up with no current sessions.

	If aShutdownPeriodUs is 0, the timer is not started and the server
	will not auto-exit.

	nb. It must be called, even if you do not want a shutdown timer.

	This function does not start the server, i.e. it does not call StartL.	The
	calling function must do this after this function returns.

	@param	aShutdownPeriodUs Shutdown period in microseconds.
 */
	{	
	if(aShutdownPeriodUs > 0)
		{
		iShutdownTimer = CShutdownTimer::NewL(aShutdownPeriodUs);
		iShutdownTimer->Restart();
		}
	
	iContainerIndex = CObjectConIx::NewL();
	
	TCallBack cb(RemoveCompletedRequests, this);
	// EPriorityHigh to encourage the active scheduler to action the
	// delete ASAP, in particular ahead of pending or new requests.
	iAsyncCleanup = new(ELeave) CAsyncCallBack(cb, CActive::EPriorityHigh);
	}


EXPORT_C void CScsServer::DisableShutdownTimer()
/**
	The server will no longer shutdown after the last client session closes.
	Client calls to RScsClientBase::ShutdownServer will fail with KErrNotSupported.
*/
	{
	if(iShutdownTimer)
		{
		iShutdownTimer->Cancel();
		}
	delete iShutdownTimer;
	iShutdownTimer = 0;
	}

EXPORT_C void CScsServer::EnableShutdownTimerL(TInt aShutdownPeriodUs)
/**
	Enable shutdown timer support in the server.
	If there are currently no client sessions the timer will be immediately started, otherwise
	it will be started when the last client session closes.
	If the timer expires, before another client creates a session, the server will shutdown.
	The RScsClientBase::ShutdownServer api will now be supported, if called the server timeout
	will effectively be reduced to 0.
*/
	{
	if(aShutdownPeriodUs <= 0)
		{
		return;
		}
	DisableShutdownTimer();
	iShutdownTimer = CShutdownTimer::NewL(aShutdownPeriodUs);
	if(iSessionCount == 0)
		{
		iShutdownTimer->Restart();
		}
	}


EXPORT_C CScsServer::~CScsServer()
/**
	Frees resources used at this level.	 Specifically, frees the
	container index, which is used to generate subsession containers,
	the shutdown timer, and the async request cleanup object.
 */
	{
	__ASSERT_DEBUG(iSessionCount == 0, PanicServer(ESvrRemSessions));
	__ASSERT_DEBUG(iAsyncCleanup == 0 || !iAsyncCleanup->IsActive(), PanicServer(ESvrRemCleanup));
	__ASSERT_DEBUG(iAsyncRequests.Count() == 0, PanicServer(ESvrRemRequests));

	delete iContainerIndex;
	delete iShutdownTimer;
	
	delete iAsyncCleanup;
	iAsyncRequests.Reset();
	}

// -------- sessions --------

EXPORT_C CSession2* CScsServer::NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const
/**
	Implement CServer2 by allocating a new session object.	This function
	delegates the actual allocation to the subclass.  Before creating the
	session object, it compares the requested version with its own version.
	After creating the session object, it increments the session count, which
	reset the shutdown timer if it is the only	session.

	@param	aVersion		Version of server which client requires.
	@param	aMessage		Connect message.
	@return					New initialized instance of CSession2 subclass.
 */
	{
	TBool versionOk = User::QueryVersionSupported(/* aCurrent */ iVersion, /* aRequested */ aVersion);
	if (! versionOk)
		User::Leave(KErrNotSupported);
	
	CScsSession* s = const_cast<CScsServer*>(this)->DoNewSessionL(aMessage);
	
	return s;
	}

EXPORT_C void CScsServer::DoPreHeapMarkOrCheckL()
	{
	}

EXPORT_C void CScsServer::DoPostHeapMarkOrCheckL()
	{
	}

void CScsServer::IncrementSessionCount()
/**
	Record the fact that another session has been created.	If this new
	session is the only session then cancel the shutdown timer.
 */
	{
	++iSessionCount;
	if (iSessionCount == 1)
		{
		if(iShutdownTimer)
			{
			iShutdownTimer->Cancel();
			}
		}
	}

void CScsServer::DecrementSessionCount()
/**
	Record the fact that a session has been deleted.  If this was the
	only remaining session then start the shutdown timer.

	Note the shutdown timer may be deferred if a request cleanup is
	pending.
 */
	{
	--iSessionCount;
	if (iShutdownTimer && (iSessionCount == 0) && (! iAsyncCleanup->IsActive()))
		{
		iShutdownTimer->Restart();
		}
	}

EXPORT_C void CScsServer::ShutdownWhenIdleL()
	/**
	   Shutdown immediately when server is next idle.  If the server
	   is not transient (ie. has no shutdown timer), this call is
	   prohibited and will leave with KErrNotSupported.
	 */
	{
	if(! iShutdownTimer)
		{
		// We do not allow shutdown of non-transient servers.
		User::Leave(KErrNotSupported);
		}
	
	// Next time we are idle, shutdown immediately.
	iShutdownTimer->ImmediateTimeoutNextRestart();
	}



// -------- asynchronous requests --------

CAsyncRequest* CScsServer::FindAsyncRequest(
	CScsSession* aSession, CScsSubsession* aSubsession, TInt aFunction)
/**
	Find the outstanding request which matches the supplied criteria.

	@param	aSession		Session which hosts the request.
	@param	aSubsession		Subsession which hosts the request, NULL if
							the request is relative to a session.
	@param	aFunction		The function identifier, without any SCS code.
	@return					The matching asynchronous request, NULL if not
							found.
 */
	{
	TInt reqCount = iAsyncRequests.Count();
	for (TInt i = reqCount - 1; i >= 0; --i)
		{
		CAsyncRequest* req = iAsyncRequests[i];
		
		if (req->iSession != aSession)
			continue;
		
		if (req->iSubsession != aSubsession)
			continue;
		
		if (req->iFunction != aFunction)
			continue;
		
		return req;
		}
	
	return 0;				// request not found
	}

void CScsServer::AddAsyncRequestL(CAsyncRequest* aAsyncRequest)
/**
	Add the supplied request to the server's collection.

	@param	aAsyncRequest	Request to add.	 If this function succeeds
							then ownership has been transferred to the
							collection.
 */
	{
	// ensure this session does not already have an outstanding request
	// for the same function
	CAsyncRequest* existReq = FindAsyncRequest(
		aAsyncRequest->iSession, aAsyncRequest->iSubsession,
		aAsyncRequest->iFunction);
	
	if (existReq != 0)
		User::Leave(KErrScsAsyncAlreadyQueued);

	iAsyncRequests.AppendL(aAsyncRequest);
	}

void CScsServer::CancelAsyncRequest(
	CScsSession* aSession, CScsSubsession* aSubsession, TInt aFunction)
/**
	Cancels a specific (sub)session request in response to a client
	command.  Also completes the client request.
	
	CancelOutstandingRequest should be called when a (sub)session is closed.

	@param	aSession		Session which hosts the request.
	@param	aSubsession		Subsession which hosts the request, NULL if
							the request is relative to a session.
	@param	aFunction		The function identifier, without any SCS code.
 */
	{
	CAsyncRequest* req = FindAsyncRequest(aSession, aSubsession, aFunction);
	
	// not an error if the request is not queued; could have been
	// completed before the cancel function was processed.
	if (req != 0)
		req->CancelCompleteAndMarkForDeletion();
	}

void CScsServer::CancelOutstandingRequests(CScsSession* aSession, TBool aCompleteClientRequests)
/**
	Cancels and deletes all outstanding asynchronous requests associated
	with the supplied session or any of its subsessions.  Does not complete
	the associated client requests.
	
	This function should be called when a session is closed.
	CancelAsyncRequest should be called when a specific request is cancelled.
	
	@param	aSession		Session which is being closed.
	@param	aCompleteClientRequests Whether to complete the client-side requests
							with KErrCancel.
 */
	{
	CancelOutstandingRequests(aSession, KWildSubsession, aCompleteClientRequests);
	}

void CScsServer::CancelOutstandingRequests(CScsSession* aSession, CScsSubsession* aSubsession, TBool aCompleteClientRequests)
/**
	Cancels and deletes all outstanding asynchronous requests associated
	with the supplied session and subsession.  This should be called when
	a session or subsession is closed.
	
	As an exception, the user-side request is completed when a subsession is
	closed, else the request would not be completed	until the session itself
	was destroyed.
	
	CancelAsyncRequest should be called when a specific request is cancelled.
	
	@param	aSession		Session which is being closed.
	@param	aSubsession		Subsession which is being closed.  If this is
							KWildSubsession then a session is being closed so all of
							its subsession requests should be destroyed as well.
	@param	aCompleteClientRequests Whether to complete the client-side requests
							with KErrCancel.
 */
	{
	TBool wildSubsession = (aSubsession == KWildSubsession);
	
	TInt reqCount = iAsyncRequests.Count();
	for (TInt i = reqCount - 1; i >= 0; --i)
		{
		CAsyncRequest* req = iAsyncRequests[i];

		TBool sessionMatch = (req->iSession == aSession);
		if (! sessionMatch)
			continue;
		
		TBool subsessionMatch = wildSubsession || (req->iSubsession == aSubsession);
		if (! subsessionMatch)
			continue;

		if (aCompleteClientRequests)
			req->CancelCompleteAndMarkForDeletion();
		else
			{
			req->DoCleanup();
			req->MarkForDeletion();
			}
		}
	}

TInt CScsServer::RemoveCompletedRequests(TAny* aPtr)
/**
	This static function is called when iAsyncCleanup
	runs.  It interprets its argument as a pointer to
	an instance of CScsServer and removes any asynchronous
	requests which have been completed.
	
	@param	aPtr			Required callback argument.	 Interpreted
							as a pointer to an instance of CScsServer.
	@return					KErrNone.  Required to satisfy the TCallBack
							function signature.
	@see RemoveCompletedRequests()
 */
	{
	CScsServer* svr = static_cast<CScsServer*>(aPtr);
	svr->RemoveCompletedRequests();
	return KErrNone;
	}

void CScsServer::RemoveCompletedRequests()
/**
	Delete any asynchronous requests which are marked for deletion.
 */
	{
	// The requests have already been completed; they just need to be removed.
	TInt reqCount = iAsyncRequests.Count();
	for (TInt i = reqCount - 1; i >= 0; --i)
		{
		CAsyncRequest* req = iAsyncRequests[i];
		if (req->iSession != 0)		// still outstanding if iSession != 0
			continue;
		
		delete req;
		iAsyncRequests.Remove(i);
		}
	
	// if no more outstanding requests then reset array so heap balances
	if (iAsyncRequests.Count() == 0)
		iAsyncRequests.Compress();
	
	// if the shutdown timer was deferred because of an impending
	// cleanup then launch it now.
	if (iShutdownTimer && (iSessionCount == 0))
		{
		iShutdownTimer->Restart();
		}
	}
// End of file

