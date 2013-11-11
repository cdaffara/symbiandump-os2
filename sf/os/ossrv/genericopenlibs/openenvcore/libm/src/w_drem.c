/*--------------------------------------------------------------------
 *© Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 *--------------------------------------------------------------------
*/
/*
 * drem() wrapper for remainder().
 *
 * Written by J.T. Conklin, <jtc@wimsey.com>
 * Placed into the Public Domain, 1994.
 */

#include <math.h>

#ifdef __SYMBIAN32__
EXPORT_C double drem(double x, double y)
#else
double drem(x, y)
double x, y;
#endif //__SYMBIAN32__
{
	return remainder(x, y);
}
