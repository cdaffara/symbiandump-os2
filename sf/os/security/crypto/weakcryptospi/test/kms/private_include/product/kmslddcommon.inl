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
* Inline functions provide values which are shared between KMS LDD client
* API and the driver itself.
*
*/




/**
 @file
 @internalComponent
*/


#ifndef KMSLDDCOMMON_INL
#define KMSLDDCOMMON_INL

#include "kmslddcommon.h"

inline TVersion KmsLddImpl::Version()
/**
	This function is used by both user- and kernel-side to
	identify which version of the component has been built.

	When the user-side channel is opened, the kernel-side driver
	must have at least this version number.

	@return					Current driver version.  Currently, only 1.0.0 is supported.
 */
	{
	return TVersion(KMajorVersionNumber, KMinorVersionNumber, KBuildVersionNumber);
	}

#endif	// #ifdef KMSLDDCOMMON_INL

