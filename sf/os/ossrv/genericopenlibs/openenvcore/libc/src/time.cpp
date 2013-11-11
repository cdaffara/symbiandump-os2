// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mapping between EPOC32 time and libc time
// The basic philosophy is to work in time_t units (TInt) which
// will be essentially TTimeIntervalSeconds from the start of Unix time.
// To stay compliant with the C-Standard (with reference to C99 draft), we  
// set the meaning of time_t to be Universal time.
// 
//

#include <e32std.h>
#include <ltime.h>
#include "reent.h"	    // for _ASCTIME_SIZE
#include <sys/time.h>	    // for gettimeofday
#include <time.h>
#include <string.h>	    // for strcpy

#define UNIX_BASE   TTime(MAKE_TINT64(0x00dcddb3,0x0f2f8000))    // 00:00, Jan 1st 1970

// Utility routines for converting between representations

#ifdef __SYMBIAN_COMPILE_UNUSED__	
static struct tm& as_struct_tm (const time_t& t, struct tm& res)
    {
    TTime us = UNIX_BASE + TTimeIntervalSeconds(t);
    TDateTime dt = us.DateTime();

    res.tm_sec  = dt.Second();
    res.tm_min  = dt.Minute();
    res.tm_hour = dt.Hour();
    res.tm_mday = dt.Day() + 1;
    res.tm_mon  = dt.Month();
    res.tm_year = dt.Year() - 1900;

    // EPOC32 counts the year day as Jan 1st == day 1
    res.tm_yday = us.DayNoInYear() - 1;

    // EPOC32 counts the weekdays from 0==Monday to 6==Sunday
    res.tm_wday = us.DayNoInWeek() + 1;
    if (res.tm_wday==7)
	    res.tm_wday=0;	// Sunday==0 in a struct tm

    // newlib just sets this field to -1
    // tm_isdst doesn't really make sense here since we don't 
    // know the locale for which to interpret this time.

    res.tm_isdst = -1;

    return res;
    }

static void as_ttime (const struct tm& p, TTime& res, TBool normalise=EFalse)
    {
    TDateTime dt;
    TInt err = dt.Set(p.tm_year+1900, (enum TMonth)p.tm_mon, p.tm_mday-1, 
	p.tm_hour, p.tm_min, p.tm_sec, 0);
    if (err == KErrNone)
	{
	res = dt;
	return;
	}
    if (!normalise)
	{
	res = TInt64(-1);
	return;
	}
    // Try to normalise things (for mktime)
    dt.Set(p.tm_year+1900, EJanuary, 0, 0, 0, 0, 0);
    res = dt;
    res += TTimeIntervalMonths (p.tm_mon);
    res += TTimeIntervalDays   (p.tm_mday-1);
    res += TTimeIntervalHours  (p.tm_hour);
    res += TTimeIntervalMinutes(p.tm_min);
    res += TTimeIntervalSeconds(p.tm_sec);
    }
    
inline void as_ttime (const time_t& p, TTime& res)
    {
    res = UNIX_BASE + TTimeIntervalSeconds(p);
    }
    
#endif //__SYMBIAN_COMPILE_UNUSED__


GLDEF_C time_t as_time_t(const TTime& t)
    {
    TTimeIntervalSeconds res;
    TInt err = t.SecondsFrom(UNIX_BASE, res);
    if (err)
	return -1;
    else
	return res.Int();
    }
    
#ifdef __SYMBIAN_COMPILE_UNUSED__
// Utility routine for formatting a TTime into a descriptor using the
// UNIX ctime format. NB. EPOC32 abbreviations can be up to KMaxDayNameAbb
// and KMaxMonthNameAbb characters (both == 4). The %F is needed to
// force the meanings of %D, %Y etc.

static TDes8& as_string (TTime& t, TDes8& res)
    {
    // UNICODE problem - t.Format operates on TDes => TDes16

#if !defined(_UNICODE)
    TRAPD(err, t.FormatL(res, _L("%F%*E %*N %D %H:%T:%S %Y")));
#else
    TBuf<_ASCTIME_SIZE> unires;
    TRAPD(err, t.FormatL(unires, _L("%F%*E %*N %D %H:%T:%S %Y")));
    if (!err)
	res.Copy(unires);
#endif
    if (err)
	res = _L8("Error\n");
    else
	res.Append('\n');
    return res;
    }
#endif

#ifdef __SYMBIAN_COMPILE_UNUSED__
/*
Intended Usage:	Utility routine for converting from UTC to localtime.
*/
inline time_t toLocal (const time_t aUniversalTime)
    {
#ifndef __SERIES60_MRT_1_0
    TTimeIntervalSeconds offset = User::UTCOffset();
    return aUniversalTime + offset.Int();
#else
    TLocale locale;
    return aUniversalTime + locale.UniversalTimeOffset().Int();
#endif //__SERIES60_MRT_1_0
    }

/*
Intended Usage:	Utility routine for converting from localtime to UTC.
				However, having decided that time_t is always Universal time, toGMT is empty.
*/
inline time_t toGMT (const time_t aLocalTime)
    {
    return aLocalTime;
    }
#endif //__SYMBIAN_COMPILE_UNUSED__

// external interface for the C library

extern "C" {
/*
Intended Usage:	Get current UTC time.
				Get the number of seconds elapsed since 00:00 hours, 
				Jan 1, 1970 UTC from the system clock.
*/
EXPORT_C time_t time (time_t* p)
    {
    TTime t;
    t.UniversalTime();
	
    time_t res = as_time_t(t);
    if (p)
		*p = res;
    return res;
    }


/*
Return number of clock ticks since process start.
Returns the number of clock ticks elapsed.
A macro constant called CLK_TCK defines the relation betwen
clock tick and second (clock ticks per second).
*/

EXPORT_C clock_t clock ()
    {
	int retval=-1;
	RThread proc;
	TTimeIntervalMicroSeconds iMicSecsFromEpoc;
	TInt err=proc.GetCpuTime(iMicSecsFromEpoc);
	
	if(err)
		{
		return retval;
		}
	return I64INT(iMicSecsFromEpoc.Int64());
    }
}// extern "C"


