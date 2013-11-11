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
// scanningdisabledonstant.cpp
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <e32def.h>

// The constants in this file is for testing the macros of
// ecompatchconstants.hby. Specifically, ensure each patchable
// constant can indeed be patched to a specific value.
//
// NB: on hw the constant is patched to 0x14 (C: and E:).
extern const TUint32 KDiscoveryDisabledDriveList = 0x00800004;


// Note that the following patched values do not make sense.
// They are just random numbers different from the defaults.

// NB: on hw this is patched to 1182.
extern const TUint32 KCustomResolverCacheSize = 4;

// NB: on hw this is patch to 4001182
extern const TUint32 KCustomResolverCacheTimeout = 4000000;
