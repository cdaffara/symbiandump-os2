/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

  
#ifndef STLPORT_PREFIX_H
# define STLPORT_PREFIX_H

# define __BUILDING_STLPORT 1

#if defined (__SYMBIAN32__)
//#pragma message("subpressing local implementation.")
//#   define _STLP_REAL_LOCALE_IMPLEMENTED
#   undef _STLP_REAL_LOCALE_IMPLEMENTED
#else
# if defined (_WIN32) || defined (WIN32)
#  ifdef __cplusplus
#   define WIN32_LEAN_AND_MEAN
#   define NOSERVICE
#  endif
#  if !(defined (__CYGWIN__) || defined(_WIN32_WCE))
#   define _STLP_REAL_LOCALE_IMPLEMENTED
#  endif
# endif
#endif

#   undef _STLP_NO_FORCE_INSTANTIATE

/* Please add extra compilation switches for particular compilers here */

# include <stl/_config.h>

# if defined (_STLP_USE_TEMPLATE_EXPORT) && defined (_STLP_USE_DECLSPEC) && ! defined (_STLP_EXPOSE_GLOBALS_IMPLEMENTATION)
#  define _STLP_EXPOSE_GLOBALS_IMPLEMENTATION
# endif

# ifdef __cplusplus

# include <ctime>
# if defined (_STLP_USE_NAMESPACES) && ! defined (_STLP_VENDOR_GLOBAL_CSTD)
using _STLP_VENDOR_CSTD::time_t;
# endif

# if defined (_STLP_FUNCTION_TMPL_PARTIAL_ORDER) || defined (__BORLANDC__)
#ifdef __SYMBIAN32__
#  define _STLP_OPERATOR_SPEC  EXPORT_C
#else
#  define _STLP_OPERATOR_SPEC _STLP_DECLSPEC 
#endif
# else
#  define _STLP_OPERATOR_SPEC _STLP_TEMPLATE_NULL _STLP_DECLSPEC
# endif

# endif /* __cplusplus */

#endif /* PREFIX */

