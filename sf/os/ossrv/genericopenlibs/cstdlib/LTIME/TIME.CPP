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
#include "LTIME.H"
#include <sys/reent.h>	    // for _ASCTIME_SIZE
#include <sys/time.h>	    // for gettimeofday
#include <time.h>
#include <string.h>	    // for strcpy

#define UNIX_BASE   TTime(MAKE_TINT64(0x00dcddb3,0x0f2f8000))    // 00:00, Jan 1st 1970

// Utility routines for converting between representations

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

GLDEF_C time_t as_time_t(const TTime& t)
    {
    TTimeIntervalSeconds res;
    TInt err = t.SecondsFrom(UNIX_BASE, res);
    if (err)
	return -1;
    else
	return res.Int();
    }

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

/**
Intended Usage:	Utility routine for converting from UTC to localtime.
@return   		Localtime in seconds
@param 			aUniversalTime Universaltime in seconds
*/
inline time_t toLocal (const time_t aUniversalTime)
    {
    TTimeIntervalSeconds offset = User::UTCOffset();
    return aUniversalTime + offset.Int();
    }

/**
Intended Usage:	Utility routine for converting from localtime to UTC.
				However, having decided that time_t is always Universal time, toGMT is empty.
@return			Localtime Universaltime in seconds
@param 			aLocalTime Universaltime in seconds (meaning of time_t is UTC)
*/
inline time_t toGMT (const time_t aLocalTime)
    {
    return aLocalTime;
    }

// external interface for the C library

extern "C" {
/**
Intended Usage:	Get current UTC time.
				Get the number of seconds elapsed since 00:00 hours, 
				Jan 1, 1970 UTC from the system clock.
@return   		Elapsed time in seconds, as described.
@param 			p Location where to store the retrieved value. 
				If this is NULL the value is not stored. 
				But it is still returned by the function.
				time_t is generally defined by default to long.
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
/**
Intended Usage:	The gettimeofday function obtains the current UTC time, which is 
				expressed as seconds and microseconds since 00:00:00 Coordinated Universal Time (UTC), 
				January 1, 1970, and stores it in a timeval structure.
				Please note that tz_minuteswest includes daytime saving. The struct member tz_dsttime is no 
				longer supported by Symbian OS and therefore set to Zero.
@publishedAll
@released
@return
@param 			tp	Struct with two members of type long (tv_sec, tv_usec)
@param 			tzp Struct with two members of type integer (tz_minuteswest, tz_dsttime)
*/
EXPORT_C int gettimeofday (struct timeval *tp, struct timezone *tzp)
    {
    if (tp)
	{
	TTime t;
	t.UniversalTime();
	
	TTimeIntervalSeconds sec;
	TInt err = t.SecondsFrom(UNIX_BASE, sec);
	if (err)
	    return -1;
	else
	    tp->tv_sec = sec.Int();
	t -= sec;
	TTimeIntervalMicroSeconds usec = t.MicroSecondsFrom(UNIX_BASE);
	TInt64 hackyfix = usec.Int64();	// because GetTInt() isn't declared const
	tp->tv_usec = I64INT(hackyfix);
	}
    if (tzp)
	{
	    tzp->tz_minuteswest = (User::UTCOffset().Int())/60;
	    tzp->tz_dsttime = 0;
	}
    return 0;
    }
/**
Return number of clock ticks since process start.
Returns the number of clock ticks elapsed.
A macro constant called CLK_TCK defines the relation betwen
clock tick and second (clock ticks per second).
@return   The number of clock ticks elapsed since start.
clock_t type is defined by default as long int by most compilers.
*/
EXPORT_C clock_t clock ()
    {
    return -1;
    }
/**
A reentrant version of gmtime(). 
*/
EXPORT_C struct tm* gmtime_r (const time_t* p, struct tm* res)
    {
    return &as_struct_tm( toGMT(*p), *res);
    }
/**
A reentrant version of localtime().
*/
EXPORT_C struct tm* localtime_r (const time_t* p, struct tm* res)
    {
    return &as_struct_tm( toLocal(*p), *res);
    }
/**
Convert tm structure to time_t value.
Checks the members of the tm structure passed as parameter ptm 
adjusting the values if the ones provided are not in the possible range 
or they are not complete or mistaken and then translates that structure 
to a time_t value (seconds elapsed since Jan 1, 1970) that is returned.
The original values of tm_wday and tm_yday members of ptm are ignored 
and filled with the correspondent ones to the calculated date. 
The range of tm_mday is not checked until tm_mon and tm_year are determined.
@return A time_t value corresponding to the date and time passed in ptm parameter.
On error, a -1 value is returned.
@param p Pointer to a tm structure, that contains data to be computed.
*/
EXPORT_C time_t mktime (struct tm *p)
    {
    TTime t;
    as_ttime(*p, t, ETrue);
    time_t res = as_time_t(t);

    as_struct_tm(res, *p);    // Must also update the struct tm passed to us...
    return res;
    }

/**
A reentrant version of asctime().
*/
EXPORT_C char* asctime_r (const struct tm *p, char* result)
    {
    TTime t;
    as_ttime(*p, t);

    TPtr8 rDes((TUint8*)result, _ASCTIME_SIZE);

    return (char *)as_string(t, rDes).PtrZ();
    }

/**
A reentrant version of ctime().
*/
EXPORT_C char* ctime_r (const time_t* p, char* result)
    {
    TTime t;
    as_ttime(toLocal(*p), t);

    TPtr8 rDes((TUint8*)result, _ASCTIME_SIZE);

    return (char *)as_string(t, rDes).PtrZ();
    }

} // extern "C"
