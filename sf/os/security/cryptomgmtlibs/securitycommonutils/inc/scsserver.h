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
* Server-side classes which are required to implement a session counting server.
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef SCSSERVER_H
#define SCSSERVER_H

#include <e32base.h>

#include <scs/scsclient.h>
#include <scs/scscommon.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "scsserverconstants.h"
#endif

// -------- error handling --------


const TInt KErrScsAsyncAlreadyQueued = -5702;	///< Tried to re-queue an outstanding request.



// forward declarations
class CScsServer;
class CScsSession;
class CScsSubsession;

class CShutdownTimer : public CTimer
/**
	When an instance of this class is constructed it is given a delay period.
	If the object runs to completion, i.e. if it is not cancelled, then it stops
	the active scheduler, which has the effect of terminating the server.
*/
	{
public:
	static CShutdownTimer* NewL(TInt aDelayUs);

	void Restart();

	void ImmediateTimeoutNextRestart();

private:
	CShutdownTimer(TInt aDelayUs);

	// implement CTimer
	virtual void RunL();

private:
	TInt iDelayUs;			///< Timer delay in microseconds.
	TBool iImmediateTimeoutNextRestart;
	};

class CAsyncRequest : public CActive
/**
	An outstanding request on the server side.	A subclass should
	be created for each type of outstanding request.

	This class is implemented as an active object, because it is
	anticipated that some asynchronous requests can be implemented
	simply by completing the client request when a TRequestStatus
	completes.	The implementor can override this behavior by
	re-implementing RunL, e.g. to free some resources before completing
	the request.  If they do so, they must call CompleteAndMarkForDeletion to 
	complete the client request and mark this object for deletion (alternatively
	they can simply call CAsyncRequest::RunL from their RunL).

	This class cannot, however, anticipate the cancellation
	mechanism.	The implementor must implement DoCancel for that.
	DoCancel should not delete the client request or mark this object
	for deletion.

	When the implementor has allocated an instance of this
	class, they must add it to the session by calling CAsyncRequest::TransferToScsFrameworkL.
	The second-phase constructor must not  leave between that and calling
	SetActive.

	These objects are cancelled and destroyed when the
	client-side session / subsession sends a cancel command.

	@see CScsSession::AddAsyncRequestL
*/
	{
public:
	IMPORT_C CAsyncRequest(CScsSession* aSession, CScsSubsession* aSubsession, const RMessage2& aMessage);
	
	void CancelCompleteAndMarkForDeletion();	// explicit cancel request
	void MarkForDeletion();						// session close
	IMPORT_C virtual void DoCleanup();	// Cancel this request
	
	IMPORT_C void TransferToScsFrameworkL();
protected:
	
	IMPORT_C void CompleteAndMarkForDeletion(TInt aError);
	
	// implement CActive
	IMPORT_C virtual void RunL();

	// override CActive
	IMPORT_C virtual TInt RunError(TInt aError);
		
public:
	/** Session on which this request is queued.  NULL if completed. */
	CScsSession* iSession;
	/** Subsession on which this request is queued.	 NULL if session-relative. */
	CScsSubsession* iSubsession;
	/** Outstanding message to complete. */
	const RMessagePtr2 iMessagePtr2;
	/** Identifies outstanding request. */
	TInt iFunction;
	};

class CScsSubsession : public CObject
/**
	If the server implementation supports subsessions, they must
	derive from this class.

	WARNING: Because this class derives from CObject you must not simply delete
	an instance of a class derived from this class (ie. your subsession), instead you MUST
	Close() it. 

	In practise this probably means you need a ConstructL like this:-
	CFred *self = new(ELeave) CFred(....);
	CleanupClosePushL(*self); // Note use of CleanupClosePushL and of *self
	self->ConstructL(...);
	CleanupStack::Pop(self); // Note use of *self instead of self
 */
	{
public:
	/**
		SCS routes subsession messages to this function; the session
		object does not have to decode them	 itself.

		@param	aFunction		Implementation function, i.e. the function
								identifier with the SCS field removed.
		@param	aMessage		Standard server message object.
		@return ETrue means complete client request now.
	 */
	virtual TBool DoServiceL(TInt aFunction, const RMessage2& aMessage) = 0;

protected:
	IMPORT_C CScsSubsession(CScsSession &aSession);
	
public:
	// Note this is setup by the constructor, before the derived class
	// ConstructL is called.
	CScsSession& iSession;			///< Owning session.
	};

class CScsSession : public CSession2
/**
	When this session object is destroyed (because the client-server session
	has closed,) this notifies the server object which decrements the session
	count.	Therefore, the server can be shut down after an inactivity
	period during which there are no open sessions.

	This object also frees any remaining subsession objects when it is closed.
*/
	{
protected:
	IMPORT_C void ConstructL();
	IMPORT_C virtual ~CScsSession();
	
	// implement CSession2
	IMPORT_C virtual void ServiceL(const RMessage2& aMessage);
	// override CSession2
	IMPORT_C virtual void ServiceError(const RMessage2& aMessage, TInt aError);
	IMPORT_C void CloseAllSubsessionsL();
	
	// asynchronous requests
	//IMPORT_C void CompleteAsyncRequest(CAsyncRequest* aAsyncRequest, TInt aReason);
		
private:
	/**
		This function is called from ServiceL after it has removed the
		SCS-specific field and handled any other messages.	I.e., it is
		called for functions which really require the session as opposed
		to subsession commands or debugging commands such as heap failure.

		@param	aFunction		Implementation function, i.e. the function
								identifier with the SCS field removed.
		@param	aMessage		Standard server message object.
		@return ETrue if client message should be completed now.
	 */
	virtual TBool DoServiceL(TInt aFunction, const RMessage2& aMessage) = 0;

	void PreCloseSession();

	IMPORT_C virtual CScsSubsession* DoCreateSubsessionL(TInt aFunction, const RMessage2& aMessage);
	
	// subessions
	CScsSubsession* GetSubsessionL(const RMessage2& aMessage);
	void DeleteSubsessionContainers();
	
	void CreateSubsessionL(TInt aFunction, const RMessage2& aMessage);
	void CloseSubsessionL(const RMessage2& aMessage);
	TBool CallSubsessionFunctionL(TInt aFunction, const RMessage2& aMessage);
	void CancelAsyncSubsessionRequestL(TInt aFunction, const RMessage2& aMessage);
	
	// asynchronous requests (support)
	CAsyncRequest* FindSessionAsyncRequest(TInt aFunction);
	void CancelAsyncSessionRequestL(TInt aFunction);

private:
	/**
		Number of open subsessions before a new one is created.	 This is used
		to manage cleanup if the subsession cannot be created.
	 */
	TInt iPreCreateSubsessionCount;

	CObjectCon* iSsObjects;				///< Currently open subsessions.
	CObjectIx* iSsHandles;				///< Maps handles to open subsessions.
	
public:
	IMPORT_C CScsSession(CScsServer &aServer);
	// This duplicates the iServer/Server() code in the base class,
	// BUT this variable IS setup before derived class ConstructL is
	// called and is NOT const. This trades off 4 bytes of memory
	// against making the code cleaner and more consistent....
	CScsServer& iServer;			///< Owning server.
	};

/**
	Pass to CScsServer::CancelOutstandingRequests to mean all requests
	associated with a session or its subsessions.
 */
CScsSubsession*const KWildSubsession = reinterpret_cast<CScsSubsession*>(~0);

class CScsServer : public CPolicyServer
/**
	The main server object allocates sessions.	It also uses
	a shutdown timer to stop the server when there have been no
	open sessions for a set period.

	The implementor must define a subclass which returns session
	objects of the appropriate type.
 */
	{
public:
	IMPORT_C virtual ~CScsServer();

	void IncrementSessionCount();
	void DecrementSessionCount();
	inline TInt SessionCount() const;

	IMPORT_C void ShutdownWhenIdleL();

	// asynchronous requests
	void AddAsyncRequestL(CAsyncRequest* aAsyncRequest);
	void CancelAsyncRequest(
		CScsSession* aSession, CScsSubsession* aSubsession, TInt aFunction);
	
	void CancelOutstandingRequests(CScsSession* aSession, TBool aCompleteClientRequests);
	void CancelOutstandingRequests(CScsSession* aSession, CScsSubsession* aSubsession, TBool aCompleteClientRequests);

	/**
		This function is called just before the SCS framework marks the heap for OOM 
		testing and just before checking the heap.

		Typically this function should compact any arrays and free objects which change size and can
		not be compacted back to the same level.
	*/
	IMPORT_C virtual void DoPreHeapMarkOrCheckL();

	/**
		This function is called just after the heap has either been marked or checked (which is just 
		after DoPreHeapMarkOrCheck).

		Typically this function should re-create any objects which had to be freed by DoPreHeapMarkOrCheck.
	*/
	IMPORT_C virtual void DoPostHeapMarkOrCheckL();

	enum TFunctionRanges
	/**
	   Function ranges to be used when configuring the server security via
	   a CPolicyServer::TPolicy object.
	   
	   Session/sSubsession function codes will be ORed into the
	   EBaseSession/EBaseSubSession ranges.
	   
	   Values from EBaseMustAllow, upwards, are used internally and must be allowed.
	   
	   If there are multiple subsession types (with different function
	   code values), then codes must be be different for each subsession
	   (this restriction only applies if using the CPolicyServer::TPolicy
	   mechanism to configure server security).
	   
	   WARNNIG: These ranges MUST match the values in scscommon.h TScsFunction.
	*/
	{
		EBaseSession	= 0x01000000,
		EBaseSubSession = 0x02000000,
		EBaseMustAllow	= 0x03000000 //< Must allow from EBaseMustAllow upwards
	};
	
	static TInt StripScsFunctionMask(TInt aFunctionId) { return aFunctionId & ~ScsImpl::KScsFunctionMask;}

protected:
	IMPORT_C CScsServer(const TVersion& aVersion, CActive::TPriority aPriority = CActive::EPriorityStandard);
	IMPORT_C CScsServer(const TVersion& aVersion, const CPolicyServer::TPolicy& aPolicy, CActive::TPriority aPriority = CActive::EPriorityStandard);
	IMPORT_C void ConstructL(TInt aShutdownPeriodUs);

	IMPORT_C void DisableShutdownTimer();
	IMPORT_C void EnableShutdownTimerL(TInt aShutdownPeriodUs);


	// implement CServer2
	IMPORT_C virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

	/**
		NewSessionL checks that this server supports the version which is
		requested by the client.  If that is so, then it calls this function
		to allocate a new session object.

		@param	aMessage		Connection message, as passed to NewSessionL.
		@return					New session object.	 This is owned by the kernel,
								which will delete it when the session is closed.
	 */
	virtual CScsSession* DoNewSessionL(const RMessage2& aMessage) = 0;

private:
	// asynchronous requests
	CAsyncRequest* FindAsyncRequest(
		CScsSession* aSession, CScsSubsession* aSubsession, TInt aFunction);
	
	static TInt RemoveCompletedRequests(TAny* aPtr);
	void RemoveCompletedRequests();

private:
	/**
		This server's version.	It is compared against client's requested version
		when the client attempts to make a connection.
	 */
	const TVersion iVersion;

	/** Number of open sessions.  Used to start and cancel the shutdown timer. */
	TInt iSessionCount;

	/**
		Shutdown timer, started when there are no open sessions, cancelled when
		the first session is created.
	 */
	CShutdownTimer* iShutdownTimer;

	
public:
	/**
		Generates instances of CObjectCon, for each session to host its subsessions.
		Public because must be accessible to session objects.
	 */
	CObjectConIx* iContainerIndex;
	
	/** Currently outstanding requests. */
	RPointerArray<CAsyncRequest> iAsyncRequests;
	
	/** Runs to remove completed requests from iAsyncRequests. */
	CAsyncCallBack* iAsyncCleanup;
	};

inline TInt CScsServer::SessionCount() const
	{
	return iSessionCount;
	}



// -------- startup --------

/**
	The server executable must implement a factory function with this
	signature and pass it to StartScsServer to allocate and start the server.
 */	
typedef CScsServer* (*TScsServerFactory)();

IMPORT_C TInt StartScsServer(TScsServerFactory aServerFactoryLC);


#endif	// #ifndef SCSSERVER_H

