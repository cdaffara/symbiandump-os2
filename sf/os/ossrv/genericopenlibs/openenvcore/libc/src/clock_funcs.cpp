/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This is a project specific include file for building the 
*                clock related functions as part of libc library.
*
*/


#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <e32std.h>
#include <sys/_timeval.h>
#include <sys/types.h>
#include "lposix.h"

#define UNIX_BASE	TTime(MAKE_TINT64(0x00dcddb3,0x0f2f8000))    // 00:00, Jan 1st 1970

extern "C" {

//Returns the resolution (granularity) of a clock
//This value is placed in a (non-NULL) *res
EXPORT_C int clock_getres(clockid_t clock_id, struct timespec* res)
{
	int retval = -1;
	//We expect the user of the library to give us a valid pointer	
	if(res == NULL)
	{
		errno = EFAULT;
		return retval;
	}
			
	switch (clock_id) 
	{
		case CLOCK_REALTIME:
			//Since Symbian OS is not realtime,we simulate the same using
			//the available wall clock whose resolution is upto microseconds
			res->tv_sec = 0;
			res->tv_nsec = 1000000;
			retval = 0;
			break;
		
		default:
			//For all other clocks that cannot be supported or invalid clockids,
			//we set errno to not-supported
			retval = -1;
			errno = EINVAL;
			break;
	}
	return retval;
}

//Allow the calling process to retrieve the value used by a clock which 
//is specified by clock_id
EXPORT_C int clock_gettime (clockid_t clock_id, struct timespec *tp)
{
	int retval = -1;
	TTime t;
	TTimeIntervalSeconds iSeconds;
	TInt err;
	//We expect the user of the library to give us a valid pointer	
	if(tp == NULL)
	{
		errno = EFAULT;
		return retval;
	}

	switch(clock_id)
	{
		case CLOCK_REALTIME:
			//We use TTime::UniversalTime() call to get the Universal time
			t.UniversalTime();
			err = t.SecondsFrom(UNIX_BASE, iSeconds);
			t-=iSeconds;//extracting seconds info into iSeconds
			if (!err)
			{
				tp->tv_sec = iSeconds.Int();
				tp->tv_nsec = t.Int64();
				retval = 0;
			}
			break;
		
		default:
			//For all other clocks that cannot be supported or invalid clockids,
			//we set errno to invalid
			retval = -1;
			errno = EINVAL;
			break;
	}
	return retval;
}


//The clock_settime allow the calling process to set the value used by a 
//clock which is specified by  clock_id
EXPORT_C int clock_settime (clockid_t clock_id, const struct timespec *tp)
{
	int retval = -1;
	TTime t(MAKE_TINT64 (0x00dcddb3 ,0x0f2f8000)) ;  // 00:00, Jan 1st 1970
	TInt err;
	TInt64 microtime;
	//We must get a filled structure from the user of the library
	if(tp == NULL)
	{
		errno = EFAULT;
		return retval;
	}
	//Check for boundary values of seconds and microseconds
	if (tp->tv_nsec < 0 || tp->tv_nsec >= 1000000000L) 
	{
		errno = ERANGE;
		return retval;
	}

	switch(clock_id)
	{
		case CLOCK_REALTIME:
			//We support only the wall-clock,hence use the 
			//User::SetHomeTime call to set the time
			t+=(TTimeIntervalSeconds)tp->tv_sec;
			microtime = (tp->tv_nsec)/1000;
			t+=(TTimeIntervalMicroSeconds)microtime;        
			err = User::SetUTCTime(t);
			if(err)
			{
				MapError(err,errno);
				break;
			}
			else
				retval = 0;
			break;
			
		default:
			//For all other clocks that cannot be supported or invalid clockids,
			//we set errno to invalid
			retval = -1;
			errno = EINVAL;
			break;
	}
	return retval;
}


//Returns the clock ID of the CPU-time clock of the process specified by pid
EXPORT_C int clock_getcpuclockid (pid_t pid, clockid_t *clock_id)
{
	int retval = -1;
	/* We don't allow any process ID but our own.  */
	if (pid == 0)
	{
		//The only available clock is the realtime wall clock
		//Hence we set the clockid to that
		*clock_id = CLOCK_REALTIME;
		retval = 0;
	}
	else
		errno = ESRCH;
	
	return retval;
}

//Makes small adjustments to the system time,
//advancing it by the time specified by the timeval delta
EXPORT_C int adjtime(const struct timeval *delta, struct timeval *olddelta)
{
	//We can make only positive adjutments to the available clock
	//The synchronization to the global time server is provided to us
	//by the User::SetHomeTime api itself
	int retval = -1;
	TTime t;
	TInt err;
	TInt64 microtime;
	long secs = 0;
	suseconds_t usec = 0;
	
	//We expect the user of the library to give us a valid pointer		
	if(delta == NULL)
	{
		errno = EFAULT;
		return retval;
	}
		
	if ((delta->tv_usec) > 1000000) 
	{
		errno = ERANGE;
		return retval;
	}
	//Check for negative of seconds and make it positive
	if((delta->tv_sec) < 0)
		secs = -1*(delta->tv_sec);
	else
		secs = delta->tv_sec;

	//Check for negative of microseconds and make it positive
	if((delta->tv_usec) < 0)
		usec = -1*(delta->tv_usec);
	else
		usec = delta->tv_usec;

	t.HomeTime();
	t+=(TTimeIntervalSeconds)secs;
	microtime = usec;
	t+=(TTimeIntervalMicroSeconds)microtime;        
	err = User::SetHomeTime(t);
	if (!err)
	{
		retval = 0;
	  	//olddelta is always zero in case of success
		olddelta->tv_sec = 0;
		olddelta->tv_usec = 0;
	}
	else
	{
		MapError(err,errno);
		retval = -1;
	}
	return retval;
}

} // extern "C"
