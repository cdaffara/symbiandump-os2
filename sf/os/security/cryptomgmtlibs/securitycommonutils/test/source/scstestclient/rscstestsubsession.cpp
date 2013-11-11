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
* by implementing a handle to a subsession.
*
*/


/**
 @file
*/

#include "scstestclient.h"
#include "scstestcommon.h"


EXPORT_C RScsTestSubsession::RScsTestSubsession()
/**
	This constructor provides a single definition from
	which to call the base class constructor.
 */
:	RScsClientSubsessionBase()
	{
	// empty.
	}

EXPORT_C TInt RScsTestSubsession::Create(RScsTestSession& aSession, TInt aValue)
/**
	Create a new subsession over the supplied session.

	@param	aSession		Session which will host this subsession.
	@param	aValue			Value which will be stored by the subsession.
	@return					Symbian OS error code where KErrNone indicates
							success and any other value indicates failure.
 */
	{
	TIpcArgs args(aValue);
	return CreateSubsession(aSession, ScsTestImpl::ESessSubsessFromInt, args);
	}

EXPORT_C TInt RScsTestSubsession::SendFunction(TInt aFunction)
/**
	Send an arbitrary function identifier to the subsession.
	Used to test the subsession implementation handles unrecognized
	functions correctly.

	@param	aFunction		Arbitrary function.  Note that, unlike the
							RScsTestSession::SendFunction, this cannot
							include an SCS code.
	@see RScsTestSession::SendFunction
 */
	{
	return CallSubsessionFunction(aFunction);
	}

EXPORT_C TInt RScsTestSubsession::Quadruple(TInt& aResult)
/**
	Send a synchronous request to the subsession.

	@param	aResult			On return this should be set to four times
							the value of the integer which was supplied to
							Create when the subsession was created.
	@return					Symbian OS error code where KErrNone indicates
							success and any other value indicates failure.
	@see Create
 */
	{
	TPckg<TInt> resultPckg(aResult);
	TIpcArgs args(&resultPckg);
	return CallSubsessionFunction(ScsTestImpl::ESubsessQuadruple, args);
	}

EXPORT_C void RScsTestSubsession::Treble(TDes8& aValue, TRequestStatus& aStatus)
/**
	Launch an aysnchronous request on the subsession.

	To reuse server-side code, this does not use the integer
	value stored in the subsession object, but transforms
	the supplied value in the same way as the session implementation.

	@param	aValue			Describes an integer, e.g. TPckgBuf<TInt>.
							The descriptor must be supplied by the caller
							instead of being constructed in this function
							because it must persist until the request has
							been completed.
	@param	aStatus			The server will complete this status object
							when it has handled the function.	
 */
	{
	TIpcArgs args(&aValue);
	CallSubsessionFunction(ScsTestImpl::ESubsessTreble, args, aStatus);
	}

EXPORT_C void RScsTestSubsession::CancelTreble()
/**
	Cancel an outstanding synchronous request launched with Treble.
	
	@see Treble
 */
	{
	return CancelSubsessionFunction(ScsTestImpl::ESubsessTreble);
	}

