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
* Client-side debug-mode panic alerts the SCS implementor if they
* attempt to construct an invalid function identifier.
*
*/


/**
 @file
*/

#include <scs/scsclient.h>

#include <scs/scscommon.h>
#include "scsserverconstants.h"

#ifdef _DEBUG
#ifdef _BullseyeCoverage
static const char *const bull1="BullseyeCoverage save off";
#endif
void ScsImpl::ClientSidePanic(ScsImpl::TScsClientPanic aReason)
/**
	Panic the current thread with category KScsClientPanicCat and the
	supplied panic reason.

	This function is designed to catch logic errors in the SCS client.
	It is not defined for release builds.
	
	@param	aReason			Reason with which the current thread will be panicked.
	@see KScsClientPanicCat
 */
	{
	User::Panic(ScsImpl::KScsClientPanicCat, aReason);
	}
#ifdef _BullseyeCoverage
static const char * const bull2="BullseyeCoverage restore";
#endif
#endif	// #ifdef _DEBUG

