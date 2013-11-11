// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalComponent
*/

#ifndef __ECOMPATCHDATACONSTANTV2_H__
#define __ECOMPATCHDATACONSTANTV2_H__

#include <e32def.h>


#ifdef __EXE__
/** This constant lets licensees specify a list of drives which they do not want
ECOM to scan, e.g. because the drives can never have valid plug-ins.
@SYMPatchable
@publishedPartner
@released
*/
	extern const TUint32 KDiscoveryDisabledDriveList;
#endif


#ifdef __EXE__
/** This constant lets licensees specify the maximum number of resolver DLLs
ECOM can cache.
@SYMPatchable
@publishedPartner
@released
*/
	extern const TUint32 KCustomResolverCacheSize;
#endif


#ifdef __EXE__
/** This constant lets licensees specify the timeout (in microseconds) 
after which cached resolver DLLs will be released.
@SYMPatchable
@publishedPartner
@released
*/
	extern const TUint32 KCustomResolverCacheTimeout;
#endif

#endif
