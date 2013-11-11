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
* Functionality to panick the client when it sends invalid input,
* or the server when it detects an invalid state.
*
*/


/**
 @file
*/


#include <scs/scsserver.h>
#include "scsserverconstants.h"

void PanicClient(const RMessagePtr2& aMessage, ScsImpl::TScsClientPanic aReason)
/**
	Panic the client which sent the supplied message with
	category KScsClientPanicCat and the supplied reason.

	@param	aMessage		Client message.
	@param	aReason			Why the client will be panicked.
	@see KScsClientPanicCat
 */
	{
	aMessage.Panic(ScsImpl::KScsClientPanicCat, aReason);
	}

#ifdef _DEBUG
void PanicServer(TServerPanic aReason)
/**
	Panic the current thread in the server process with category
	KServerPanicCat and the supplied reason.

	This function is only defined for debug builds.

	@param	aReason			Panic reason.
	@see KServerPanicCat
 */
	{
	User::Panic(KServerPanicCat, aReason);
	}
#endif	// #ifdef _DEBUG

