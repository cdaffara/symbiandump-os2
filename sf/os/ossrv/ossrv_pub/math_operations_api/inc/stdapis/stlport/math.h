/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
 *
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

# if !defined (_STLP_OUTERMOST_HEADER_ID)
#  define _STLP_OUTERMOST_HEADER_ID 0x244
#  include <stl/_prolog.h>
# elif (_STLP_OUTERMOST_HEADER_ID == 0x244) && ! defined (_STLP_DONT_POP_0x244)
#  define _STLP_DONT_POP_0x244
# endif

#ifndef _STLP_MATH_H
#define _STLP_MATH_H

# if ! defined (exception) && (! defined (__KCC) || (__KCC_VERSION < 4000)) && \
   ! (defined(__IBMCPP__) && (500 <= __IBMCPP__))
#  define __EXCEPTION_WAS_REDEFINED 1
#  define exception __math_exception
# endif

#ifdef __SYMBIAN32__
#  include <math.h>
#else
#  include _STLP_NATIVE_C_HEADER(math.h)
#endif


# if defined (__EXCEPTION_WAS_REDEFINED)
#  undef exception
#  undef __EXCEPTION_WAS_REDEFINED
# endif

#endif //_STLP_MATH_H

# if (_STLP_OUTERMOST_HEADER_ID == 0x244)
#  if ! defined (_STLP_DONT_POP_0x244)
#   include <stl/_epilog.h>
#   undef  _STLP_OUTERMOST_HEADER_ID
#   endif
#   undef  _STLP_DONT_POP_0x244
# endif


// Local Variables:
// mode:C++
// End:
