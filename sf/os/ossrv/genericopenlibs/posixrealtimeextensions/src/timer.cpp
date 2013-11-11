// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : timer.cpp
// Part of     : librt-timer specific cpp file
// This is a project specific source file for building the 
// timer related functions as part of librt library.
// 
//

#include <e32math.h> 

#include "sysif.h"
#include "timer.h"
#include "timerhandler.h"
#include "timermessage.h"

#define BOUNDARY_CHECK(rqtp) ((rqtp.tv_nsec != 0 && rqtp.tv_nsec < 1000) || \
							   rqtp.tv_nsec >= 1000000000L )\

//ctor
static TInt64 seed = 0xdeadbeef;
CRtTimer::CRtTimer(struct sigevent *aSig)
	{
	iTimer.Set(this);
	iTimerId = Math::Rand (seed);
	
	if(aSig == NULL)	
		{
		iSigEvent.sigev_notify = SIGEV_SIGNAL;
		iSigEvent.sigev_signo = SIGALRM;
		}
	else
		{
		iSigEvent = *aSig;				
		}
		
	iIsArmed = EFalse;
	}

//dtor
CRtTimer::~CRtTimer()
	{
	//should be called in context of the timer server thread.
	if(iIsArmed)
		{
		iTimer.Cancel();	
		}	
	}

//NewL method
CRtTimer* CRtTimer::New(struct sigevent *aSig)
	{
	return new CRtTimer(aSig);
	}

//sets the timer for the given value
TInt CRtTimer::SetTime(TInt aFlag, const struct itimerspec *aIpTime,
			 struct itimerspec *aOpTime)
	{
	//Check for boundary values of seconds and microseconds
	if (aIpTime == NULL || ((BOUNDARY_CHECK(aIpTime->it_value) || BOUNDARY_CHECK(aIpTime->it_interval)) &&
	   (aIpTime->it_value.tv_sec != 0 || aIpTime->it_value.tv_nsec != 0)) )
		{
		return KErrArgument;
		}
		
	if(aIpTime->it_value.tv_sec == 0 && aIpTime->it_value.tv_nsec == 0)
		{
		if(!iIsArmed)	
			return KErrNone;
		else
			iIsTimerReset = ETrue;
		}

	//load the time to expiration in the output timer value.	
	if(aOpTime)
		{
		Time(aOpTime);	
		}

	//start setting the timer value...
	clock_gettime(CLOCK_REALTIME, &iStartTime.it_value);
	
	iStartTime.it_interval = aIpTime->it_interval;		
	iEndTime = aIpTime->it_value;
	
	if((aFlag & TIMER_ABSTIME) == 0) // relative timer
		{
		iEndTime.tv_sec+=iStartTime.it_value.tv_sec;
		iEndTime.tv_nsec+=iStartTime.it_value.tv_nsec;
		}
	getTimerHandler()->session.OnDemandConnect(getTimerHandler()->iServer);
	TInt lRet = getTimerHandler()->session.SetTime(iTimerId);
	return lRet;
	}

//gets the time to expiry.
TInt CRtTimer::Time(struct itimerspec *aTime) const
	{
	TInt err = KErrNone;
	if(NULL == aTime)
		{
		err = KErrArgument;	
		}
	else if (!iIsArmed)
		{
		memset(aTime, 0, sizeof(struct itimerspec));
		}
	else
		{
		struct timespec clktime;
		clock_gettime(CLOCK_REALTIME, &clktime);
		
		aTime->it_value.tv_sec = iEndTime.tv_sec - clktime.tv_sec;
		aTime->it_value.tv_nsec = iEndTime.tv_nsec - clktime.tv_nsec;	
		aTime->it_interval = iStartTime.it_interval;				
		}	
		
	return err;	
	}

//gets the overruns for this timer
TInt CRtTimer::OverrunCount(TInt& aOverrunCount) const
	{
#if (!defined SYMBIAN_OE_POSIX_SIGNALS || !defined SYMBIAN_OE_LIBRT)
	aOverrunCount = aOverrunCount;//warning fix
	return KErrNotSupported;	

#else
	aOverrunCount = Backend()->Overrun(iTimerId);
	if(aOverrunCount >= DELAYTIMER_MAX)
		aOverrunCount = DELAYTIMER_MAX;
	
	return KErrNone;
#endif
	}
//EOF

