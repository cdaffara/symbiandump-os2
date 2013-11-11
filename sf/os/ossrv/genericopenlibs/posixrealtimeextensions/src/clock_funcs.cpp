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
* Description:  This is a project specific source file for building the 
*                clock related functions as part of librt library.
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

#define BOUNDARY_CHECK(rqtp) ( (rqtp->tv_nsec != 0 && rqtp->tv_nsec < 1000) || \
							   rqtp->tv_nsec >= 1000000000L )\

extern "C" {

//Returns the resolution (granularity) of a clock
//This value is placed in a (non-NULL) *res
EXPORT_C int clock_getres(clockid_t clock_id, struct timespec* res)
{
	int retval = -1;
	//We expect the user of the library to give us a valid pointer	
	if(res == NULL)
		{
		return 0; //no strict reactions please.
		}
			
	switch (clock_id) 
		{
		case CLOCK_REALTIME:
			//Since Symbian OS is not realtime,we simulate the same using
			//the available wall clock whose resolution is upto microseconds
			res->tv_sec = 0;
			res->tv_nsec = 1000;
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
			//Since Symbian OS is not realtime,we simulate the same using
			//the available wall clock.We use TTime::HomeTime() call to get
			//the wall clock time
			t.HomeTime();
			err = t.SecondsFrom(UNIX_BASE, iSeconds); //TODO check for the negative tests..
			if (!err)
				{
				t-=iSeconds;//extracting seconds info into iSeconds 			
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
	
	if(tp == NULL)
		{
		errno = EFAULT;
		return retval;
		}
			
	//Check for boundary values of seconds and microseconds
	if (BOUNDARY_CHECK(tp))
		{
		errno = EINVAL;
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

	if(clock_id == NULL)
		{
		errno = EFAULT;
		return retval;
		}
			
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


//clock_nanosleep will not be interrupted by the signal emulation. 
//hence EINTR is not valid here. 

EXPORT_C int clock_nanosleep(clockid_t clock_id, int flags,
       const struct timespec *rqtp, struct timespec */*rmtp*/)
{
	int retval = -1;
	
	if(rqtp == NULL)
		{
		errno = EFAULT;
		return retval;
		}
			
	//Check for boundary values of seconds and microseconds
	if (BOUNDARY_CHECK(rqtp))
		{
		errno = EINVAL;
		return retval;
		}
		
	switch(clock_id)
		{
		case CLOCK_REALTIME:
			{
			switch(flags)
				{
				case TIMER_ABSTIME:
					{
					TTime lSetTime(MAKE_TINT64 (0x00dcddb3 ,0x0f2f8000)) ;  // 00:00, Jan 1st 1970				
						
					lSetTime+=(TTimeIntervalSeconds)rqtp->tv_sec;
					lSetTime+=(TTimeIntervalMicroSeconds)(rqtp->tv_nsec/1000);
						
					User::At(lSetTime);
					}
					break;
				
				default:
					{
					unsigned long timeout;
					timeout = (1000000 * rqtp->tv_sec) + (rqtp->tv_nsec /1000);
					User::AfterHighRes(timeout);
					}
					break;
				}
			}
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
} // extern "C"

//EOF



