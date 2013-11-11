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
* Generic client-side code for client-server interaction.
* Attempts to establish a connection to a session counting server,
* starting the process if required.
*
*/


/**
 @file
*/

#include <scs/scsclient.h>

#include <scs/scscommon.h>

using namespace ScsImpl;


EXPORT_C RScsClientBase::RScsClientBase()
/**
	This constructor is protected to ensure this class is not
	instantiated directly.
 */
:	RSessionBase()
	{
	// empty.
	}

EXPORT_C void RScsClientBase::Close()
/**
	This method should be used in preference to RScsSessionBase::Close
	because it sends a message to cancel any outstanding requests on the
	session or its subsessions.
 */
	{
	if(iHandle)
		{
		RSessionBase::SendReceive(EPreCloseSession);
		RSessionBase::Close();
		}
	}

TInt RScsClientBase::StartServerProcess(const TDesC& aExeName, const TUidType& aFullExeUid)
/**
	This function is defined for the convenience of subclasses which need to start
	a server process before they can connect to the server.

	@param	aExeName		Executable which hosts the server.
	@param	aFullExeUid		The server executable's full UID.  This is used to ensure the
							intended executable is started, and not another executable
							with the same name.
	@return					Symbian OS error code.  KErrNone indicates success, and any other
							value indicates failure.
	@pre This function should only be called by Connect(const TVersion&) if the server is
		not already running.
 */
	{
	RProcess pr;
	TInt r = pr.Create(aExeName, /* aCommand */ KNullDesC, aFullExeUid);
	if (r != KErrNone)
		return r;

	TRequestStatus rs;
	pr.Rendezvous(rs);
	if (rs != KRequestPending)
		r = rs.Int();
	else
		{
		pr.Resume();
		User::WaitForRequest(rs);
		if (rs.Int()==KErrAlreadyExists)
			r=KErrAlreadyExists;
		else
			r = (pr.ExitType() == EExitPending) ? rs.Int() : KErrGeneral;
		}

	pr.Close();
	return r;
	}

EXPORT_C TInt RScsClientBase::Connect(
	const TDesC& aSvrName, const TVersion& aReqVer, const TDesC& aExeName, const TUidType& aFullExeUid)
/**
	Attempt to connect to the named server.  If the server is not available then attempt
	to start its hosting process.

	@param	aSvrName		Name of server to connect to.
	@param	aReqVer			Required server version.
	@param	aExeName		Executable which hosts the server.  This function will launch this
							executable if the server is not running.
	@param	aFullExeUid		The server executable's full UID.  This ensures the intended
							executable is started, and not another executable with the same name.
	@return					Symbian OS error code.  KErrNone indicates success,
							and any other value indicates failure.
 */
	{
	TInt retries = 2;		// number of remaining retries

	for (;;)
		{
		TInt r = CreateSession(aSvrName, aReqVer);
		
		// if connected then finished
		if (r == KErrNone)
			return r;

		// if any reason other than server not available then abort
		if (r != KErrNotFound && r != KErrServerTerminated)
			return r;

		if (--retries == 0)
			return r;

		r = StartServerProcess(aExeName, aFullExeUid);
		if (r != KErrNone && r != KErrAlreadyExists)
			return r;
		}	// for (;;)
	}

// -------- server heap checking --------

EXPORT_C TInt RScsClientBase::SetServerHeapFail(TInt aRate)
/**
	Start marking the server heap and set a deterministic
	fail rate.  This should matched with a call to EndServerHeapFail.
	
	This function is empty in release builds.
	
	@param	aRate			Number of allocations after which allocation
							should fail on the server heap.
	@see EndServerHeapFail
	@see __UHEAP_MARK
	@see __UHEAP_SETFAIL
 */
	{
#ifndef _DEBUG
	(void) aRate;
	return KErrNone;
#else
	TIpcArgs ipc(aRate);
	return RSessionBase::SendReceive(ScsImpl::EUHeapSetFail, ipc);
#endif
	}

EXPORT_C TInt RScsClientBase::ResetServerHeapFail()
/**
	Finish marking the server heap and reset the failure rate.
	This should match a previous call to SetServerHeapFail.

	If there is a heap imbalance, then the server will be panicked.

	This function is empty in release builds.
	
	@see SetServerHeapFail
	@see __UHEAP_MARKEND
	@see __UHEAP_RESET
 */
	{
#ifdef _DEBUG
	return RSessionBase::SendReceive(ScsImpl::EUHeapResetFail);	
#else
	return KErrNone;
#endif
	}

// -------- passing arguments to a server-side session --------

EXPORT_C TInt RScsClientBase::ShutdownServer()
/**
DEBUG USE ONLY - Tells the server to shutdown down ASAP, and block
until it has done so. This also closes the current session.

If the server is not configured to use a inactivity shutdown timer,
this will fail with KErrNotSupported.

nb. You may still need to call the Close function of a derived class
to ensure it gets to cleanup...

	@return					Symbian OS error code where KErrNone indicates
							success and any other value indicates failure.
 */
	{
	// Find servers PID
	TPckgBuf<TProcessId> idBuf;
	TIpcArgs args(&idBuf);
	TInt r = RSessionBase::SendReceive(ScsImpl::EGetServerPid, args);
	if(r != KErrNone) return r;

	// Open a handle for the server thread
	RProcess server;
	r = server.Open(idBuf(), EOwnerThread);
	if(r != KErrNone) return r;
	
	// Logon to the server process to spot when it exits
	TRequestStatus rs;
	server.Logon(rs);

	// Ask the server to exit ASAP
	r = RSessionBase::SendReceive(ScsImpl::EShutdownAsap);
	if(r != KErrNone)
		{
		(void) server.LogonCancel(rs);
		server.Close();
		return r;
		}

	// Close our session
	Close();

	// Wait for the server to finish shutting down
	User::WaitForRequest(rs); // nb. we do not care what code it shutdown with.

	// Close our server process handle
	server.Close();

	return KErrNone;
	}

// -------- passing arguments to a server-side session --------

EXPORT_C TInt RScsClientBase::CallSessionFunction(TInt aFunction) const
/**
	Send a command to the corresponding server-side session.  The
	subclass uses this function instead of directly calling
	RSubSessionBase::SendReceive because it adds the SCS code
	which marks this as an ordinary session call.
	
	@param	aFunction		Function identifier.  Bits 31:24 must be zero,
							because they are reserved for SCS commands.
	@return					Error code with which the server completed the request.
 */
	{
	__ASSERT_DEBUG(! ScsFieldUsed(aFunction), ClientSidePanic(EScsClNoArgsSessUsedScs));
	
	TInt f = ECallSessionFunc | aFunction;
	return RSessionBase::SendReceive(f);
	}

EXPORT_C TInt RScsClientBase::CallSessionFunction(TInt aFunction, const TIpcArgs& aArgs) const
/**
	Send a command to the corresponding server-side session.  The
	subclass uses this function instead of directly calling
	RSubSessionBase::SendReceive because it adds the SCS code which
	marks this as an ordinary session call.

	@param	aFunction		Session function identifier.  Bits 31:24 must be zero,
							because they are reserved for SCS commands.
	@param	aArgs			Standard IPC arguments.
	@return					Error code with which the server completed the request.
 */
	{
	__ASSERT_DEBUG(! ScsFieldUsed(aFunction), ClientSidePanic(EScsClArgsSessUsedScs));
	
	TInt f = ECallSessionFunc | aFunction;
	return RSessionBase::SendReceive(f, aArgs);
	}

EXPORT_C void RScsClientBase::CallSessionFunction(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus) const
/**
	Send the supplied function identifier and arguments to the server-side
	session.  The subclass uses this function instead of directly calling
	RSubSessionBase::SendReceive because it adds the SCS code which marks
	this as an ordinary session call.

	@param	aFunction		Session function identifier.  Bits 31:24 must be zero,
							because they are reserved for SCS commands.
	@param	aArgs			Standard IPC arguments.
	@param	aStatus			This will be completed by the server when it has
							finished handling the function.
 */
	{
	__ASSERT_DEBUG(! ScsFieldUsed(aFunction), ClientSidePanic(EScsClArgsSessAsyncUsedScs));
	
	TInt f = ECallSessionFunc | aFunction;
	RSessionBase::SendReceive(f, aArgs, aStatus);
	}

EXPORT_C void RScsClientBase::CancelSessionFunction(TInt aFunction) const
/**
	Cancel an outstanding session request.  This has no effect if the
	request is not outstanding.
	
	@param	aFunction		Implementation function.  This must be the
							same value that was supplied to CallSessionFunction.
 */
	{
	__ASSERT_DEBUG(! ScsFieldUsed(aFunction), ClientSidePanic(EScsClCancelSessUsedScs));
	
	TInt f = ECancelSessionFunc | aFunction;
	RSessionBase::SendReceive(f);
	}

