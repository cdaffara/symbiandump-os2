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
* Defines the functionality which client-side subsession handles use
* to create and destroy subsessions, and to send messages to them.
*
*/


/**
 @file
*/


#include <scs/scsclient.h>
#include <scs/scscommon.h>

using namespace ScsImpl;


EXPORT_C RScsClientSubsessionBase::RScsClientSubsessionBase()
/**
	This protected constructor prevents the class from being
	instantiated directly.
 */
:	RSubSessionBase()
	{
	// empty.
	}

EXPORT_C void RScsClientSubsessionBase::Close()
/**
	Closes the subsession, releasing resources on the server side.
 */
	{
	CloseSubSession(ECloseSubsession);
	}

EXPORT_C TInt RScsClientSubsessionBase::CreateSubsession(const RScsClientBase& aSession, TInt aFunction, const TIpcArgs& aArgs)
/**
	Create a subsession over the supplied session.

	@param	aSession		Session which will host the subsession.
	@param	aFunction		Command identifier.	 Bits 31:24 must be zero, because
							they are reserved for SCS commands.
	@param	aArgs			Standard IPC arguments.	 The fourth argument cannot be
							used because that is reserved for the subsession handle.
	@return					Error code with which the server completed the request.
 */
	{
	return RSubSessionBase::CreateSubSession(
		aSession,
		ECreateSubsession | aFunction,
		aArgs);
	}

EXPORT_C TInt RScsClientSubsessionBase::CallSubsessionFunction(TInt aFunction) const
/**
	Send a command to the corresponding server-side subsession.
	The subclass uses this function instead of directly calling
	RSubSessionBase::SendReceive because it adds the SCS code which
	marks this as a subsession call.  Therefore, it can be routed
	to the subsession object on the server side without any custom
	decoding.
	
	@param	aFunction		Subsession command identifier.	Bits 31:24 must be zero,
							because they are reserved for SCS commands.
	@return					Error code with which the server completed the request.
 */
	{
	__ASSERT_DEBUG(! ScsFieldUsed(aFunction), ClientSidePanic(EScsClNoArgsSubsessUsedScs));
	
	TInt f = ECallSubsessionFunc | aFunction;
	return RSubSessionBase::SendReceive(f);
	}

EXPORT_C TInt RScsClientSubsessionBase::CallSubsessionFunction(TInt aFunction, const TIpcArgs& aArgs) const
/**
	Send a command to the corresponding server-side subsession.
	The subclass uses this function instead of directly calling
	RSubSessionBase::SendReceive because it adds the SCS code which
	marks this as a subsession call.  Therefore, it can be routed
	to the subsession object on the server side without any custom
	decoding.
	
	@param	aFunction		Subsession command identifier.	Bits 31:24 must be zero,
							because they are reserved for SCS commands.
	@param	aArgs			Standard IPC arguments.	 The fourth argument cannot be
							used because that is reserved for the subsession handle.
	@return					Error code with which the server completed the request.
 */
	{
	__ASSERT_DEBUG(! ScsFieldUsed(aFunction), ClientSidePanic(EScsClArgsSubsessUsedScs));
	
	TInt f = ECallSubsessionFunc | aFunction;
	return RSubSessionBase::SendReceive(f, aArgs);	
	}

EXPORT_C void RScsClientSubsessionBase::CallSubsessionFunction(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus) const
/**
	Send a command to the corresponding server-side subsession.
	
	@param	aFunction		Subsession command identifier.	Bits 31:24 must be zero,
							because they are reserved for SCS commands.
	@param	aArgs			Standard IPC arguments.	 The fourth argument cannot be
							used because that is reserved for the subsession handle.
	@param	aStatus			The server completes this object when it has finished
							handling the session.
 */
	{
	__ASSERT_DEBUG(! ScsFieldUsed(aFunction), ClientSidePanic(EScsClArgsSubsessAsyncUsedScs));
	
	TInt f = ECallSubsessionFunc | aFunction;
	RSubSessionBase::SendReceive(f, aArgs, aStatus);		
	}

EXPORT_C void RScsClientSubsessionBase::CancelSubsessionFunction(TInt aFunction) const
/**
	Cancel an outstanding subsession request.  This has no effect if the
	request is not outstanding.
	
	@param	aFunction		Implementation function.  This must be the
							same value that was supplied to CallSubsessionFunction.
 */
	{
	__ASSERT_DEBUG(! ScsFieldUsed(aFunction), ClientSidePanic(EScsClCancelSubsessUsedScs));
	
	TInt f = ECancelSubsessionFunc | aFunction;
	RSubSessionBase::SendReceive(f);
	}

