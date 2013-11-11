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
// Name        : timer_funcs.cpp
// Part of     : librt-timer specific cpp file
// This is a project specific source file for building the 
// timer related functions as part of librt library.
// 
//

#include <signal.h>
#include <time.h>
#include <errno.h>

#include "lposix.h"
#include "timerhandler.h"

const TInt KErrWouldBlock=-1000;

extern "C" {

/* Create new per-process timer using CLOCK_ID.  */
EXPORT_C int timer_create (clockid_t __clock_id,
                         struct sigevent *__restrict __evp,
                         timer_t *__restrict __timerid)
	{
	if(__timerid == NULL)
		{
		errno = EFAULT;	
		return -1;
		}
		
	if(__clock_id != CLOCK_REALTIME)
		{
		errno = EINVAL;	
		return -1;
		}
		
	int err = getTimerHandler()->CreateTimer(*__timerid, __evp);
	 
	if (err == KErrWouldBlock)
		{
		return MapError(EAGAIN, errno);
		}
	
	return MapError(err, errno);
	}


/* Delete timer TIMERID.  */
EXPORT_C int timer_delete (timer_t __timerid)
	{
	return MapError(getTimerHandler()->RemoveTimer(__timerid),errno);
	}



/* Set timer TIMERID to VALUE, returning old value in OVLAUE.  */
EXPORT_C int timer_settime (timer_t __timerid, int __flags,
                          const struct itimerspec *__restrict __value,
                          struct itimerspec *__restrict __ovalue)
	{
	return MapError(getTimerHandler()->SetTime(__timerid, __flags, __value, __ovalue),errno);
	}

/* Get current value of timer TIMERID and store it in VLAUE.  */
EXPORT_C int timer_gettime (timer_t __timerid, struct itimerspec *__value)
	{
	if(__value == NULL)
		{
		errno = EFAULT;	
		return -1;
		}
		
	return MapError(getTimerHandler()->Time(__timerid, __value),errno);
	}

/* Get expiration overrun for timer TIMERID.  */
EXPORT_C int timer_getoverrun (timer_t __timerid)
	{
		
#if (!defined SYMBIAN_OE_POSIX_SIGNALS || !defined SYMBIAN_OE_LIBRT)
	__timerid = __timerid;//warning fix	
	errno = ENOSYS;
	return -1;
#else
	int overrun = 0;
	int lerr = MapError(getTimerHandler()->OverrunCount(__timerid, overrun),errno); 	
	if(lerr == -1 && errno == EINVAL)
		{
		return lerr;	
		}
	else
		{
		overrun = (overrun <= 1) ? 0 : (overrun-1);
		}
 	
	return overrun;
#endif		
	}

} // extern "C"

//EOF


