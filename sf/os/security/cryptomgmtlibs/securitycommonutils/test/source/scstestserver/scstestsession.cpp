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
* Implements CScsTestSession.  See class and function definitions
* for more information.
*
*/


/**
 @file
*/

#include "scstestserver.h"


CScsTestSession* CScsTestSession::NewL(CScsTestServer &aServer)
/**
	Factory function allocates new instance of CScsTestSession.

	@param		aServer		Reference to our parent server
	@return					New, initialized instance of CScsTestSession
							which is owned by the caller.
 */
	{
	CScsTestSession* s = new(ELeave) CScsTestSession(aServer);
	CleanupStack::PushL(s);
	s->ConstructL();				// CScsSession implementation
	CleanupStack::Pop(s);
	return s;
	}

CScsTestSession::CScsTestSession(CScsTestServer &aServer)
/**
	This private constructor prevents direct instantiation.
 */
:	CScsSession(aServer)
	{
	// empty.
	}

CScsTestSession::~CScsTestSession()
/**
	Because this object does not (yet) own any resources, this
	destructor is only defined to ensure a single definition is
	generated.

	The base class destructor destroys any remaining subsessions
	or outstanding requests.
 */
	{
	// empty.
	}

TBool CScsTestSession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
	Implement CScsTestSession by handling the supplied message.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side handle to message.
	@return ETrue means complete client request now.

	@see CScsSession::ServiceError.
 */
	{
	ScsTestImpl::TSessionFunction f = static_cast<ScsTestImpl::TSessionFunction>(aFunction);
	
	switch (f)
		{
	case ScsTestImpl::ESessNukeServer:
		CActiveScheduler::Stop();
		return EFalse; // Server will crash due to outstanding reqs

	case ScsTestImpl::ESessDouble:
		{
		TPckgBuf<TInt> value;
		aMessage.Read(0, value);
		value() *= 2;
		aMessage.WriteL(0, value);
		}
		break;
	
	case ScsTestImpl::ESessTreble:
		{
		CTrebleRequest::NewL(this, /* aSubsession */ NULL, aMessage);
		return EFalse; // Do NOT complete client request yet
		}
	
	default:
		User::Leave(KErrNotSupported);
		}
	return ETrue; // Complete client request now
	}

CScsSubsession* CScsTestSession::DoCreateSubsessionL(TInt aFunction, const RMessage2& aMessage)
/**
	Override CScsSession by allocating an instance of CScsTestSubsession
	to handle ScsTestImpl::ESessSubsessFromInt.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side handle to message.
	@see ScsTestImpl::ESessSubsessFromInt
 */
	{
	switch (aFunction)
		{
	case ScsTestImpl::ESessSubsessFromInt:
		return CScsTestSubsession::NewL(*this, aMessage);
	
	default:
		User::Leave(KErrNotSupported);
		return 0;			// avoid compiler warning
		}
	}

