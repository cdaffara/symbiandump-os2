/*--------------------------------------------------------------------
 *� Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 *--------------------------------------------------------------------
*/
/*-
 * Copyright (c) 2005 David Schultz <das@FreeBSD.ORG>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef __SYMBIAN32__
__FBSDID("$FreeBSD: src/lib/msun/src/s_exp2f.c,v 1.1 2005/04/05 02:57:15 das Exp $");
#endif //__SYMBIAN32__
 
#include <math.h>
#include "math_private.h"

#define	TBLBITS	4
#define	TBLSIZE	(1 << TBLBITS)

#ifndef __SYMBIAN32__
static const float
    huge    = 0x1p100f,
    twom100 = 0x1p-100f,
    redux   = 0x1.8p23f / TBLSIZE,
    P1	    = 0x1.62e430p-1f,
    P2	    = 0x1.ebfbe0p-3f,
    P3	    = 0x1.c6b348p-5f,
    P4	    = 0x1.3b2c9cp-7f;

static const double exp2ft[TBLSIZE] = {
	0x1.6a09e667f3bcdp-1,
	0x1.7a11473eb0187p-1,
	0x1.8ace5422aa0dbp-1,
	0x1.9c49182a3f090p-1,
	0x1.ae89f995ad3adp-1,
	0x1.c199bdd85529cp-1,
	0x1.d5818dcfba487p-1,
	0x1.ea4afa2a490dap-1,
	0x1.0000000000000p+0,
	0x1.0b5586cf9890fp+0,
	0x1.172b83c7d517bp+0,
	0x1.2387a6e756238p+0,
	0x1.306fe0a31b715p+0,
	0x1.3dea64c123422p+0,
	0x1.4bfdad5362a27p+0,
	0x1.5ab07dd485429p+0,
};

#else
static const float
    huge    = 1.26765e+30f,
    twom100 	= 7.88861e-31f,
    redux   	= 12582912/ TBLSIZE,
    P1	    	= 0.6931472f,
    P2	    	= 0.2402265f,
    P3	    	= 0.05550541f,
    P4	    	= 0.009618355;

static const double exp2ft[TBLSIZE] = {
	0.70710677,
	0.7384131,
	0.7711054,
	0.80524516,
	0.8408964,
	0.8781261,
	0.91700405,
	0.9576033,
	1.000000,
	1.0442737,
	1.0905077,
	1.1387886,
	1.1892071,
	1.2418578,
	1.2968396,
	1.3542556,
};

#endif //__SYMBIAN32__
	
/*
 * exp2f(x): compute the base 2 exponential of x
 *
 * Accuracy: Peak error < 0.501 ulp; location of peak: -0.030110927.
 *
 * Method: (equally-spaced tables)
 *
 *   Reduce x:
 *     x = 2**k + y, for integer k and |y| <= 1/2.
 *     Thus we have exp2f(x) = 2**k * exp2(y).
 *
 *   Reduce y:
 *     y = i/TBLSIZE + z for integer i near y * TBLSIZE.
 *     Thus we have exp2(y) = exp2(i/TBLSIZE) * exp2(z),
 *     with |z| <= 2**-(TBLSIZE+1).
 *
 *   We compute exp2(i/TBLSIZE) via table lookup and exp2(z) via a
 *   degree-4 minimax polynomial with maximum error under 1.4 * 2**-33.
 *   Using double precision in the final calculation avoids roundoff error.
 *
 *   This method is due to Tang, but I do not use his suggested parameters:
 *
 *	Tang, P.  Table-driven Implementation of the Exponential Function
 *	in IEEE Floating-Point Arithmetic.  TOMS 15(2), 144-157 (1989).
 */
EXPORT_C float exp2f(float x)
{
	double tv;
	float r, z;
	volatile float t;	/* prevent gcc from using too much precision */
	uint32_t hx, hr, ix, i0;
	int32_t k;

	/* Filter out exceptional cases. */
	GET_FLOAT_WORD(hx,x);
	ix = hx & 0x7fffffff;		/* high word of |x| */
	if(ix >= 0x43000000) {			/* |x| >= 128 */
		if(ix >= 0x7f800000) {
			if ((ix & 0x7fffff) != 0 || (hx & 0x80000000) == 0)
				return (x); 	/* x is NaN or +Inf */
			else 
				return (0.0);	/* x is -Inf */
		}
		#ifdef __SYMBIAN32__
		if(x >= 128.0f)
		#else
		if(x >= 0x1.0p7f)
		#endif //__SYMBIAN32__
			return (huge * huge);	/* overflow */
		#ifdef __SYMBIAN32__
		if(x <= -150.0f)
		#else
		if(x <= -0x1.2cp7f)
		#endif //__SYMBIAN32__
			return (twom100 * twom100); /* underflow */
	} else if (ix <= 0x33000000) {		/* |x| <= 0x1p-25 */
		return (1.0f + x);
	}

	/* Reduce x, computing z, i0, and k. */
	t = x + redux;
	GET_FLOAT_WORD(i0, t);
	i0 += TBLSIZE / 2;
	k = (i0 >> TBLBITS) << 23;
	i0 &= TBLSIZE - 1;
	t -= redux;
	z = x - t;

	/* Compute r = exp2(y) = exp2ft[i0] * p(z). */
	tv = exp2ft[i0];
	r = tv + tv * (z * (P1 + z * (P2 + z * (P3 + z * P4))));

	/* Scale by 2**(k>>23). */
	if(k >= -125 << 23) {
		if (k != 0) {
			GET_FLOAT_WORD(hr, r);
			SET_FLOAT_WORD(r, hr + k);
		}
		return (r);
	} else {
		GET_FLOAT_WORD(hr, r);
		SET_FLOAT_WORD(r, hr + (k + (100 << 23)));
		return (r * twom100);
	}
}