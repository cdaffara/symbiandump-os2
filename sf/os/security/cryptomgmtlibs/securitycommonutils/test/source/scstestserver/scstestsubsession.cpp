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
* Implements CScsTestSubsession.  See class and function definitons
* for more information.
*
*/


/**
 @file
*/

#include "scstestserver.h"


CScsTestSubsession* CScsTestSubsession::NewL(CScsTestSession &aSession, const RMessage2& aMessage)
/**
	Factory function allocates new, initialized instance of CScsTestSubsession.

	@param	aSession		Reference to our parent session
	@param	aMessage		Create subsession message sent from the client.
							This contains the integer on which the subsession
							is curried.
	@return					New, initialized instance of CScsTestSubsession
							which is owned by the caller.
 */
	{
	CScsTestSubsession* self = new(ELeave) CScsTestSubsession(aSession);

	// record value on which this subsession is curried
	self->iValue = aMessage.Int0();
	return self;
	}

CScsTestSubsession::CScsTestSubsession(CScsTestSession &aSession)
/**
	This private constructor prevents direct instantiation and provides
	a single point of definition.
 */
:	CScsSubsession(aSession)
	{
	// empty.
	}

CScsTestSubsession::~CScsTestSubsession()
/**
	This destructor is only defined to breakpoint when the
	subsession is destroyed.
 */
	{
	// empty.
	}

TBool CScsTestSubsession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
	Implement CSscSubsession by handling messages sent to this subsession.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side handle to message.
	@return ETrue means complete client request now.
 */
	{
	ScsTestImpl::TSubsessionFunction f =
		static_cast<ScsTestImpl::TSubsessionFunction>(aFunction);

	TBool completeMessage = ETrue;
	switch (f)
		{
	case ScsTestImpl::ESubsessQuadruple:
		{
		TPckgBuf<TInt> times4 = iValue * 4;
		aMessage.WriteL(0, times4);
		}
		break;

	case ScsTestImpl::ESubsessTreble:
		{
		CScsTestSession* owningSession = static_cast<CScsTestSession*>(&iSession);
		CTrebleRequest::NewL(owningSession, this, aMessage);
		completeMessage = EFalse; // do not complete yet		
		}
		break;
	
	default:
		User::Leave(KErrNotSupported);
		}
	return completeMessage;
	}

