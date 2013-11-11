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
 @file te_uptkerneltimer.cpp
 @internalTechnology
 @prototype
*/

#include "te_kerneltimer.h"
#include "te_traceglobals.h"

	
TKernelTimer::TKernelTimer()
: iTimer((NTimerFn)FinishTiming,this)
	{}

TKernelTimer::~TKernelTimer()
	{
	iTimer.Cancel();
	}

TInt TKernelTimer::StartBackgroundTimer()
	{	
    // wait for next tick...
	 WaitForNextTick();

	 // do benchmark...
    iCount = 0;
    iFinalCount = 0;    
    iTimerExpired = EFalse;
    //TimerTicks does it in milliseconds, not microseconds!!!
    iTimer.OneShot(NKern::TimerTicks(KTimerDuration / KMicroToNanoSecondConversion));
    return KErrNone;
	}
	
TUint32 TKernelTimer::FinalTime()
	{
	return (KTimerDuration * (TUint32) KMicroToNanoSecondConversion) / (TUint32)iFinalCount;
	}

void TKernelTimer::FinishTiming(TKernelTimer* aSelf)
     {
     aSelf->iTimerExpired = ETrue;
     aSelf->iFinalCount = aSelf->iCount;
     }

void TKernelTimer::WaitForNextTick()
	{
	TInt tick = NKern::TickCount();
	while( NKern::TickCount() == tick ){}
	}
