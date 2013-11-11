/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)time.h	8.3 (Berkeley) 1/21/94
 * © Portions Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 */

/*
 * $FreeBSD: src/include/time.h,v 1.32 2005/04/02 12:33:27 das Exp $
 */

#ifndef _TIME_H_
#define	_TIME_H_

#include <sys/cdefs.h>
#include <sys/_null.h>
#include <sys/_types.h>
#ifdef __SYMBIAN32__
#include <sys/types.h>
#endif

#if __POSIX_VISIBLE > 0 && __POSIX_VISIBLE < 200112 || __BSD_VISIBLE
/*
 * Frequency of the clock ticks reported by times().  Deprecated - use
 * sysconf(_SC_CLK_TCK) instead.  (Removed in 1003.1-2001.)
 */
#ifdef __SYMBIAN32__
#define	CLK_TCK		128
#endif //__SYMBIAN32__
#endif

/* Frequency of the clock ticks reported by clock().  */
#ifndef __SYMBIAN32__
#define	CLOCKS_PER_SEC	128
#endif //__SYMBIAN32__

#ifndef _CLOCK_T_DECLARED
typedef	__clock_t	clock_t;
#define	_CLOCK_T_DECLARED
#endif

#ifndef _TIME_T_DECLARED
typedef	__time_t	time_t;
#define	_TIME_T_DECLARED
#endif

#ifndef _SIZE_T_DECLARED
typedef	__size_t	size_t;
#define	_SIZE_T_DECLARED
#endif

#if __POSIX_VISIBLE >= 199309
/*
 * New in POSIX 1003.1b-1993.
 */
#ifndef _CLOCKID_T_DECLARED
typedef	__clockid_t	clockid_t;
#define	_CLOCKID_T_DECLARED
#endif

#ifndef _TIMER_T_DECLARED
typedef	__timer_t	timer_t;
#define	_TIMER_T_DECLARED
#endif

#include <sys/timespec.h>

/* POSIX.1b structure for timer start values and intervals.  */
struct itimerspec
  {
    struct timespec it_interval;
    struct timespec it_value;
  };

/* We can use a simple forward declaration.  */
struct sigevent; 
#endif /* __POSIX_VISIBLE >= 199309 */

/* These macros are also in sys/time.h. */
#if !defined(CLOCK_REALTIME) && __POSIX_VISIBLE >= 200112
#define CLOCK_REALTIME	0
#ifdef __BSD_VISIBLE
#define CLOCK_VIRTUAL	1
#define CLOCK_PROF	2
#endif
#define CLOCK_MONOTONIC	4
#endif /* !defined(CLOCK_REALTIME) && __POSIX_VISIBLE >= 200112 */

#if !defined(TIMER_ABSTIME) && __POSIX_VISIBLE >= 200112
#if __BSD_VISIBLE
#define TIMER_RELTIME	0x0	/* relative timer */
#endif
#define TIMER_ABSTIME	0x1	/* absolute timer */
#endif /* !defined(TIMER_ABSTIME) && __POSIX_VISIBLE >= 200112 */

struct tm {
	int	tm_sec;		/* seconds after the minute [0-60] */
	int	tm_min;		/* minutes after the hour [0-59] */
	int	tm_hour;	/* hours since midnight [0-23] */
	int	tm_mday;	/* day of the month [1-31] */
	int	tm_mon;		/* months since January [0-11] */
	int	tm_year;	/* years since 1900 */
	int	tm_wday;	/* days since Sunday [0-6] */
	int	tm_yday;	/* days since January 1 [0-365] */
	int	tm_isdst;	/* Daylight Savings Time flag */
	long	tm_gmtoff;	/* offset from UTC in seconds */
	char	*tm_zone;	/* timezone abbreviation */
};

#if __POSIX_VISIBLE
extern char *tzname[];
#endif

__BEGIN_DECLS
#ifdef __SYMBIAN32__
#ifdef __WINSCW__
#ifndef __SOFTFP
#define __SOFTFP
#endif /* __SOFTFP */
#endif//__WINSCW__
IMPORT_C char*** __tzname(void);
#define	tzname		(*__tzname())
#endif //__SYMBIAN32__

IMPORT_C
char *asctime(const struct tm *);
IMPORT_C
clock_t clock(void);
IMPORT_C
char *ctime(const time_t *);
IMPORT_C
double difftime(time_t, time_t) __SOFTFP;
/* XXX missing: getdate() */
IMPORT_C
struct tm *gmtime(const time_t *);
IMPORT_C
struct tm *localtime(const time_t *);
IMPORT_C
time_t mktime(struct tm *);
IMPORT_C 
size_t strftime(char * __restrict, size_t, const char * __restrict,
    const struct tm * __restrict);
IMPORT_C
time_t time(time_t *);

#if __POSIX_VISIBLE
IMPORT_C
void tzset(void);
#endif

#if __POSIX_VISIBLE >= 199309
IMPORT_C
int clock_getres(clockid_t, struct timespec *);
IMPORT_C
int clock_gettime(clockid_t, struct timespec *);
IMPORT_C
int clock_settime(clockid_t, const struct timespec *);
/* XXX missing: clock_nanosleep() */
IMPORT_C
int nanosleep(const struct timespec *, struct timespec *);

//////////////////////////LIBRT: START/////////////////////////////////

/* Create new per-process timer using CLOCK_ID.  */
IMPORT_C int timer_create (clockid_t __clock_id,
                         struct sigevent *__restrict __evp,
                         timer_t *__restrict __timerid);

/* Delete timer TIMERID.  */
IMPORT_C int timer_delete (timer_t __timerid);

/* Set timer TIMERID to VALUE, returning old value in OVLAUE.  */
IMPORT_C int timer_settime (timer_t __timerid, int __flags,
                          const struct itimerspec *__restrict __value,
                          struct itimerspec *__restrict __ovalue);

/* Get current value of timer TIMERID and store it in VLAUE.  */
IMPORT_C int timer_gettime (timer_t __timerid, struct itimerspec *__value);

/* Get expiration overrun for timer TIMERID.  */
IMPORT_C int timer_getoverrun (timer_t __timerid);

IMPORT_C int clock_nanosleep(clockid_t clock_id, int flags,
       const struct timespec *rqtp, struct timespec *rmtp);
//////////////////////////LIBRT: END/////////////////////////////////

#endif /* __POSIX_VISIBLE >= 199309 */

#ifdef __SYMBIAN32__
IMPORT_C
int clock_getcpuclockid(pid_t , clockid_t *);
#endif //__SYMBIAN32__


#if __POSIX_VISIBLE >= 199506
IMPORT_C
char *asctime_r(const struct tm *, char *);
IMPORT_C
char *ctime_r(const time_t *, char *);
IMPORT_C
struct tm *gmtime_r(const time_t *, struct tm *);
IMPORT_C
struct tm *localtime_r(const time_t *, struct tm *);
#endif

#if __XSI_VISIBLE
IMPORT_C 
char *strptime(const char * __restrict, const char * __restrict,
    struct tm * __restrict);
#endif

#if __BSD_VISIBLE
char *timezone(int, int);	/* XXX XSI conflict */
#ifndef __SYMBIAN32__
void tzsetwall(void);
#endif //__SYMBIAN32__
time_t timelocal(struct tm * const);
IMPORT_C time_t timegm(struct tm * const);
#endif /* __BSD_VISIBLE */
__END_DECLS

#endif /* !_TIME_H_ */
