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
// All patchable constants need to be defined in a separate file.
// Care should be taken not to define the constant data in the same source file in which
// they are referred, lest the compiler inline's it. If inlined, the data has no storage
// location and cannot be patched.
// Hence, the definition of data must be put in a separate source file and they must be 
// referred in other source files by using the 'extern' qualifier.
// 
//

/**
 @file
 @internalComponent
*/

#include <e32def.h>

//
//This constant lets licensees specify a list of drives which they
//do not want ECOM to scan, e.g. because the drives can never have
//valid plug-ins. 
//This is a 32-bit integer with each bit representing a drive to disable:
//0x1 disables drive A, 0x2 disables drive B etc. There are only 25 drive
//letters supported (A:->Y:) therefore the last 7 bits are ignored.
//Note that Read-only internal drives cannot be disabled.
#ifdef __EXE__
	extern const TUint32 KDiscoveryDisabledDriveList = 0;
#endif


// This constant specifies the maximum number of resolver DLLs ECOM can cache.
// When the cache queue is full, the next custom resolver DLL requested by
// client will bump off the oldest DLL in the cache.
// The default (unpatched) value is 4.
#ifdef __EXE__
	extern const TUint32 KCustomResolverCacheSize = 4;
#endif


// This constant specifies the timeout in microseconds that a resolver DLL is
// kept in cache. When the time expires, the resolver DLL is released from
// memory. The default (unpatched) value is 4000000.
#ifdef __EXE__
	extern const TUint32 KCustomResolverCacheTimeout = 4000000;
#endif
