/*--------------------------------------------------------------------
 *� Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 *--------------------------------------------------------------------
*/
/* e_lgammaf.c -- float version of e_lgamma.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
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
 */
#ifndef __SYMBIAN32__
#ifndef lint
static char rcsid[] = "$FreeBSD: src/lib/msun/src/e_lgammaf.c,v 1.7 2002/05/28 18:15:04 alfred Exp $";
#endif
#endif //__SYMBIAN32__



/* __ieee754_lgammaf(x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call __ieee754_lgammaf_r
 */

#include <math.h>
#include "math_private.h"

#ifndef __SYMBIAN32__
extern int signgam;
#endif //__SYMBIAN32__

EXPORT_C float
__ieee754_lgammaf(float x)
{
	
	
	return __ieee754_lgammaf_r(x, &signgam);
	
}
