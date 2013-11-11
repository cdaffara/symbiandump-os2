/*--------------------------------------------------------------------
 *© Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 *--------------------------------------------------------------------
*/
#ifndef __SYMBIAN32__
#include <math.h>
#endif
#include "math_private.h"


#ifndef __SYMBIAN32__
int signgam = 0;
#else
#ifndef __WINSCW__
int _signgam = 0;
#endif//__WINSCW__
#endif //__SYMBIAN32__
