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
 @file te_uptkerneltimer.h
 @internalTechnology
 @prototype
*/

#ifndef TE_UPTKERNELTIMER_H_
#define TE_UPTKERNELTIMER_H_
	
#include <kern_priv.h>
#include <kernel.h>
#include <platform.h>

class TKernelTimer
{
public:
	TKernelTimer();
	~TKernelTimer();
	TInt 	StartBackgroundTimer();
	TUint32 FinalTime();
	TBool	IsTimerExpired(){return iTimerExpired;};
	void	IncreaseCount(){iCount++;};
	TUint32	FinalCount(){return iFinalCount;};
	/**
	 * This is used by the NTimer when the timer expires.
	 */
     static void FinishTiming(TKernelTimer* aSelf);
private:
	void 	WaitForNextTick();
private:
	//This is all about the timer and timing a trace call... 
	//should refactor into its own class, but it's so small so far it doesnt matter too much 
	//This is used to set up a timer such that we can measure how long a trace call takes
    NTimer 			iTimer;
    //For the timer to work we need a volatile member that we can change the status of when the timer has expired
    volatile TBool	iTimerExpired;
    //We count how many times we could make a call to trace before the timer expires
 	volatile TUint32 	iCount;
 	//Because the iCount can be increased before we knew that the timer had expired we make sure
 	//that we keep the last iCount as it was when the timer expired. So iFinalCount has the
 	//exact count and iCount may have the corect count or may have been increased while the timer expired.
 	TUint32			iFinalCount;
};



#endif /*TE_UPTKERNELTIMER_H_*/


