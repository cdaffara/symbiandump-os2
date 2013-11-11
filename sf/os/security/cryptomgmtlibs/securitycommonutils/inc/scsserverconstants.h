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
* Description:  scsserverconstants.h
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef SCSSERVERCONSTANTS_H
#define SCSSERVERCONSTANTS_H

// -------- debugging --------

/**
	The server is panicked with this category when it detects an invalid state.
	
	@see TServerPanic
	/**
 */
_LIT(KServerPanicCat, "SCS-Server");


enum TServerPanic
/**
	The server is panicked with these reasons when it detects an invalid state.
	
	@see TScsServerPanicCat
 */
	{
	ESvrRemSessions = 0,			///< Server object destroyed with outstanding sessions.
	ESvrRemCleanup = 1,				///< Server object destroyed with outstanding cleanup.
	ESvrRemRequests = 2				///< Server object destroyed with outstanding requests.
	};


void PanicServer(TServerPanic aPanic);

void PanicClient(const RMessagePtr2& aMessage, ScsImpl::TScsClientPanic aReason);

#endif // SCSSERVERCONSTANTS_H
