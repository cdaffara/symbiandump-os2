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
 @file te_perfkerneltimer.cpp
 @internalTechnology
 @prototype
*/

#include "te_perfkerneltimer.h"
#include "te_dataparameters.h"
	
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
    //NEED TO CONVERT TO FLOAT!!!!!!? CHECK when device driver implemented
   /*
    float tempMeasurementTime = (float) KMeasurementTime;
    float tempTimeConverter = (float)KMicroToNanoSecondConversion;
    float tempTicks = tempMeasurementTime/tempTimeConverter;
    */
    iTimer.OneShot(NKern::TimerTicks(KMeasurementTime / KMicroToNanoSecondConversion));
    return KErrNone;
	}
	
TUint32 TKernelTimer::FinalTime()
	{
	//cast each to float separately
	//calculate (log each step)
	//cast back to tuint32
	//return
/*	NEED TO CONVERT TO FLOAT!!!!!! CHECK when device driver implemented
	float tempMeasurementTime = (float) KMeasurementTime;
	float tempCount = (float)iCount;
	float tempTimeConverter = (float)KMicroToNanoSecondConversion;
	float tempTimeMicro = tempMeasurementTime/tempCount;
	float tempTimeNano = tempTimeMicro * tempTimeConverter;
	
	return (TUint32) tempTimeNano;

	//KMeasurementTime / iCount* (TUint32) KMicroToNanoSecondConversion;
	
//	return 
*/
	return (KMeasurementTime * (TUint32) KMicroToNanoSecondConversion) / (TUint32)iFinalCount;
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
