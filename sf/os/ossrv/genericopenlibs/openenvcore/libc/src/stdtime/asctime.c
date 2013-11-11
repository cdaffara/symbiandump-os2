/*
** This file is in the public domain, so clarified as of
** 1996-06-05 by Arthur David Olson (arthur_david_olson@nih.gov).
** © Portions copyright (c) 2005-2006 Nokia Corporation.  All rights reserved.
*/

#include <sys/cdefs.h>
#ifndef lint
#ifndef NOID
static char	elsieid[] __unused = "@(#)asctime.c	7.9";
#endif /* !defined NOID */
#endif /* !defined lint */
__FBSDID("$FreeBSD: src/lib/libc/stdtime/asctime.c,v 1.12 2004/06/14 10:31:52 stefanf Exp $");

/*LINTLIBRARY*/

#include "namespace.h"
#include "private.h"
#include "un-namespace.h"
#include "tzfile.h"

#ifdef __SYMBIAN32__
#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

#ifdef EMULATOR

GET_STATIC_ARRAY_FROM_TLS(result, char)
#define result (GET_WSD_VAR_NAME(result, s)())
#endif //EMULATOR
#endif //__SYMBIAN32__
/*
** A la ISO/IEC 9945-1, ANSI/IEEE Std 1003.1, Second Edition, 1996-07-12.
*/

EXPORT_C
char *
asctime_r(timeptr, buf)
const struct tm *	timeptr;
char *			buf;
{
	static const char	wday_name[][3] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	static const char	mon_name[][3] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	const char *	wn;
	const char *	mn;
	
	#ifdef __SYMBIAN32__
		if(timeptr==NULL)
			return NULL;
	#endif
	
	if (timeptr->tm_wday < 0 || timeptr->tm_wday >= DAYSPERWEEK)
		wn = "???";
	else	wn = wday_name[timeptr->tm_wday];
	if (timeptr->tm_mon < 0 || timeptr->tm_mon >= MONSPERYEAR)
		mn = "???";
	else	mn = mon_name[timeptr->tm_mon];
	/*
	** The X3J11-suggested format is
	**	"%.3s %.3s%3d %02.2d:%02.2d:%02.2d %d\n"
	** Since the .2 in 02.2d is ignored, we drop it.
	*/
	(void) sprintf(buf, "%.3s %.3s%3d %02d:%02d:%02d %d\n",
		wn, mn,
		timeptr->tm_mday, timeptr->tm_hour,
		timeptr->tm_min, timeptr->tm_sec,
		TM_YEAR_BASE + timeptr->tm_year);
	return buf;
}

/*
** A la X3J11, with core dump avoidance.
*/

EXPORT_C
char *
asctime(timeptr)
const struct tm *	timeptr;
{
	/*
	** Big enough for something such as
	** ??? ???-2147483648 -2147483648:-2147483648:-2147483648 -2147483648\n
	** (two three-character abbreviations, five strings denoting integers,
	** three explicit spaces, two explicit colons, a newline,
	** and a trailing ASCII nul).
	*/
#ifdef __SYMBIAN32__
#ifndef EMULATOR
	static char		result[3 * 2 + 5 * INT_STRLEN_MAXIMUM(int) +
					3 + 2 + 1 + 1];
#endif //EMULATOR
#else
	static char		result[3 * 2 + 5 * INT_STRLEN_MAXIMUM(int) +
					3 + 2 + 1 + 1];
#endif // __SYMBIAN32__

	return asctime_r(timeptr, result);
}
EXPORT_C
wchar_t *
wasctime(timeptr)
const struct tm *	timeptr;
{
	static wchar_t		wresult[3 * 2 + 5 * INT_STRLEN_MAXIMUM(int) +
					3 + 2 + 1 + 1];
	/*
	** Big enough for something such as
	** ??? ???-2147483648 -2147483648:-2147483648:-2147483648 -2147483648\n
	** (two three-character abbreviations, five strings denoting integers,
	** three explicit spaces, two explicit colons, a newline,
	** and a trailing ASCII nul).
	*/
#ifdef __SYMBIAN32__
#ifndef EMULATOR
	static char	result[3 * 2 + 5 * INT_STRLEN_MAXIMUM(int) +
					3 + 2 + 1 + 1];				
	#endif //EMULATOR
#else
	static char	result[3 * 2 + 5 * INT_STRLEN_MAXIMUM(int) +
					3 + 2 + 1 + 1];				
#endif // __SYMBIAN32__
	if (timeptr==NULL)
		 return NULL;
asctime_r(timeptr, result);
mbstowcs(wresult, result, sizeof(wresult)/sizeof(wchar_t));
return wresult;
}
