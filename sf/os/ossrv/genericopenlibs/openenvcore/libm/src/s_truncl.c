/*--------------------------------------------------------------------
 *� Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 *--------------------------------------------------------------------
*/
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 *
 * From: @(#)s_floor.c 5.1 93/09/24
 */
#ifndef __SYMBIAN32__
#ifndef lint
static char rcsid[] = "$FreeBSD: src/lib/msun/src/s_truncl.c,v 1.4 2005/04/28 19:45:55 stefanf Exp $";
#endif
#endif //__SYMBIAN32__

/*
 * truncl(x)
 * Return x rounded toward 0 to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to truncl(x).
 */

#include <float.h>
#include <math.h>
#include <stdint.h>

#include "fpmath.h"

#ifdef LDBL_IMPLICIT_NBIT
#define	MANH_SIZE	(LDBL_MANH_SIZE + 1)
#else
#define	MANH_SIZE	LDBL_MANH_SIZE
#endif

static const long double huge = 1.0e300;

EXPORT_C long double
truncl(long double x)
{
	union IEEEl2bits u ;
	#ifdef __SYMBIAN32__
	int e;
	u.e = x ;
	e = u.bits.exp - LDBL_MAX_EXP + 1;
	#else
		union IEEEl2bits u = { .e = x };
	#endif //__SYMBIAN32__

	if (e < MANH_SIZE - 1) {
		if (e < 0) {			/* raise inexact if x != 0 */
			if (huge + x > 0.0)
				u.e = 0.0;
		} else {
			uint64_t m = ((1llu << MANH_SIZE) - 1) >> (e + 1);
			if (((u.bits.manh & m) | u.bits.manl) == 0)
				return (x);	/* x is integral */
			if (huge + x > 0.0) {	/* raise inexact flag */
				u.bits.manh &= ~m;
				u.bits.manl = 0;
			}
		}
	} else if (e < LDBL_MANT_DIG - 1) {
		uint64_t m = (uint64_t)-1 >> (64 - LDBL_MANT_DIG + e + 1);
		if ((u.bits.manl & m) == 0)
			return (x);	/* x is integral */
		if (huge + x > 0.0)		/* raise inexact flag */
			u.bits.manl &= ~m;
	}
	return (u.e);
}