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
// UTrace Performance Tests Kernel-Side Timer.
//



/**
 @file te_upttimerhelpers.h
 @internalTechnology
 @prototype
*/
#ifndef TE_UPTTIMERHELPERS_H_
#define TE_UPTTIMERHELPERS_H_

#include <e32cmn.h>

// We expect SanityFoo to take roughly xx... ns
#ifdef __WINSCW__
static const TInt KMinFooTime = 700; //600; //ns
static const TInt KMaxFooTime = 950; //800; //ns
#else //on hardware
static const TInt KMinFooTime = 3000; //2700; //ns
static const TInt KMaxFooTime = 3750; //3200; //ns
#endif
static const TInt KFooStackSize = 400;


/*
 * Dummy function for Test function - 
 * used to calibrate and debug the timer class.
 */
void SanityFoo();
void SanityFooLong();


#endif /*TE_UPTTIMERHELPERS_H_*/
