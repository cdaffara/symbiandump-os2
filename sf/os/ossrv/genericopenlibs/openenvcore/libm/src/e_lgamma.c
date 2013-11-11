/*--------------------------------------------------------------------
 *© Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 *--------------------------------------------------------------------
*/
/* @(#)e_lgamma.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */
#ifndef __SYMBIAN32__
#ifndef lint
static char rcsid[] = "$FreeBSD: src/lib/msun/src/e_lgamma.c,v 1.8 2005/02/04 18:26:06 das Exp $";
#endif
#endif //__SYMBIAN32__


/* __ieee754_lgamma(x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call __ieee754_lgamma_r
 */

#include <math.h>
#include "math_private.h"

#ifndef __SYMBIAN32__
extern int signgam;
#endif //__SYMBIAN32__

EXPORT_C double
__ieee754_lgamma(double x)
{
	
	return __ieee754_lgamma_r(x, &signgam);
	
}
