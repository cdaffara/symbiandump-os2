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
* Client-side API tests an implementation of the session count server
* by implementing a handle to a session.
*
*/


/**
 @file
*/


#include "scstestclient.h"
#include "scstestcommon.h"


EXPORT_C RScsTestSession::RScsTestSession()
/**
	This constructor defines a single point from which
	the base class constructor is called.
 */
:	RScsClientBase()
	{
	// empty.
	}

EXPORT_C TInt RScsTestSession::Connect()
/**
	Connect to the SCS test server using the default version.

	@return					Symbian OS error code where KErrNone indicates
							success and any other value indicates failure.
 */
	{
	const TVersion serverVersion = ScsTestImpl::Version();
	return Connect(serverVersion);
	}

EXPORT_C TInt RScsTestSession::Connect(const TVersion& aVersion)
/**
	Connect to the SCS test server with the supplied version.  This
	function is defined to test the version checking functionality in
	CScsServer.  A client API would normally just ask for a server
	version which at least matches its own.

	@param	aVersion		Server version to ask for.
	@return					Symbian OS error code where KErrNone indicates
							success and any other value indicates failure.
 */
	{
	const TUidType serverFullUid = ScsTestImpl::ServerImageFullUid();
	
	TInt r = RScsClientBase::Connect(
		ScsTestImpl::KServerName, aVersion,
		ScsTestImpl::KServerImageName, serverFullUid);
	
	return r;
	}

EXPORT_C TInt RScsTestSession::SendCustomFunction(TInt aFunction)
/**
	Sends the supplied function identifier to the server with
	no IPC arguments.  This function should be used to test the
	server can handle an invalid SCS or session function.
	
	@param	aFunction		Function identifier to send to server-side
							session.  This is sent as-is, and can use the
							SCS field.
 */
	{
	return SendReceive(aFunction);
	}

EXPORT_C TInt RScsTestSession::NukeServer()
/**
	Calls an synchronous function which causes the server to panic
*/
	{
	return CallSessionFunction(ScsTestImpl::ESessNukeServer);
	}

EXPORT_C TInt RScsTestSession::Double(TInt& aValue)
/**
	Calls a synchronous session function to ensure the function
	call is routed to the session subclass on the server side.
	
	@param	aValue			An arbitrary integer.  On completion
							this should be double its entry value.
	@return					Symbian OS error code where KErrNone indicates
							success and any other value indicates failure.
 */
	{
	TPckg<TInt> valPckg(aValue);
	TIpcArgs ipc(&valPckg);
	return CallSessionFunction(ScsTestImpl::ESessDouble, ipc);
	}

EXPORT_C void RScsTestSession::Treble(TDes8& aValue, TRequestStatus& aStatus)
/**
	Calls an asynchronous function to test the SCS can successfully manage
	asynchronous requests.
	
	@param	aValue			Describes an integer, e.g. TPckgBuf<TInt>.
							The descriptor must be supplied by the caller
							instead of being constructed in this function
							because it must persist until the request has
							been completed.
	@param	aStatus			The server will complete this status object
							when it has handled the function.
	@see CancelTreble
 */
	{
	TIpcArgs ipc(&aValue);
	CallSessionFunction(ScsTestImpl::ESessTreble, ipc, aStatus);
	}

EXPORT_C void RScsTestSession::CancelTreble()
/**
	Cancel an outstanding request set up with Treble.  This function has
	no effect if there is no outstanding request.

	@see Treble
 */
	{
	CancelSessionFunction(ScsTestImpl::ESessTreble);
	}
