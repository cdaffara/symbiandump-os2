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
* Functionality to panic the client when it sends invalid input,
* or the server when it detects an invalid state.
*
*/



#include "sraserver.h"


void PanicClient(const RMessagePtr2& aMessage, TSraClientPanic aReason)
/**
	Panic the client with the supplied reason.

	@param	aMessage		Client message.
	@param	aReason			Why the client will be panicked.
	@see KSraClientPanicCat
 */
	{
	aMessage.Panic(KSraClientPanicCat, aReason);
	}

#ifdef _DEBUG
void PanicServer(TSraServerPanic aReason)
/**
	Panic the current thread in the server process with category
	KSraServerPanicCat and the supplied reason.

	This function is only defined for debug builds.

	@param	aReason			Panic reason.
	@see KSraServerPanicCat
 */
	{
	User::Panic(KSraServerPanicCat, aReason);
	}
#endif	// #ifdef _DEBUG
