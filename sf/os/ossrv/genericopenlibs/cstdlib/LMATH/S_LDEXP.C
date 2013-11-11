/* S_LDEXP.C
 * 
 * Portions Copyright (c) 1993-2005 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 */


/* @(#)s_ldexp.c 5.1 93/09/24 */
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

/*
FUNCTION
       <<ldexp>>, <<ldexpf>>---load exponent

INDEX
	ldexp
INDEX
	ldexpf

ANSI_SYNOPSIS
       #include <math.h>
       double ldexp(double <[val]>, int <[exp]>);
       float ldexpf(float <[val]>, int <[exp]>);

TRAD_SYNOPSIS
       #include <math.h>

       double ldexp(<[val]>, <[exp]>)
              double <[val]>;
              int <[exp]>;

       float ldexpf(<[val]>, <[exp]>)
              float <[val]>;
              int <[exp]>;


DESCRIPTION
<<ldexp>> calculates the value 
@ifinfo
<[val]> times 2 to the power <[exp]>.
@end ifinfo
@tex
$val\times 2^{exp}$.
@end tex
<<ldexpf>> is identical, save that it takes and returns <<float>>
rather than <<double>> values.

RETURNS
<<ldexp>> returns the calculated value.

Underflow and overflow both set <<errno>> to <<ERANGE>>.
On underflow, <<ldexp>> and <<ldexpf>> return 0.0.
On overflow, <<ldexp>> returns plus or minus <<HUGE_VAL>>.

PORTABILITY
<<ldexp>> is ANSI, <<ldexpf>> is an extension.
              
*/   

#include "FDLIBM.H"
#include <errno.h>

/**
Get floating-point value from mantissa and exponent.
Calculates the floating point value corresponding to 
the given mantissa and exponent
@return   Floating-point value equal to value * 2exp.
@param x Floating point value representing mantissa 
@param exp Integer exponent
*/	
EXPORT_C double ldexp(double value, int exp) __SOFTFP
{
	if(!finite(value)||value==0.0) return value;
	value = scalbn(value,exp);
	if(!finite(value)||value==0.0) errno = ERANGE;
	return value;
}
