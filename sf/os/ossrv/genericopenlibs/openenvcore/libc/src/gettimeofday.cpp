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
// TIME.CPP
// Mapping between EPOC32 time and libc time
// The basic philosophy is to work in time_t units (TInt) which
// will be essentially TTimeIntervalSeconds from the start of Unix time.
// To stay compliant with the C-Standard (with reference to C99 draft), we  
// set the meaning of time_t to be Universal time.
// 
//

#include <e32std.h>
#include <sys/time.h> // struct timeval, timezone

#define UNIX_BASE   TTime(MAKE_TINT64(0x00dcddb3,0x0f2f8000))    // 00:00, Jan 1st 1970

// external interface for the C library

extern "C" {

/*
Intended Usage:	The gettimeofday function obtains the current UTC time, which is 
				expressed as seconds and microseconds since 00:00:00 Coordinated Universal Time (UTC), 
				January 1, 1970, and stores it in a timeval structure.
				Please note that tz_minuteswest includes daytime saving. The struct member tz_dsttime is no 
				longer supported by Symbian OS and therefore set to Zero.

*/
EXPORT_C 
int gettimeofday (struct timeval *tp, struct timezone *tzp)
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

} // extern "C"
