// -*- C++ -*-
/***************************************************************************
 *
 * _cstddef.h - C++ Standard library interface to the ANSI C header stddef.h
 *
 * $Id: _cstddef.h 225375 2005-07-26 19:04:10Z sebor $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#ifndef _RWSTD_CSTDDEF_H_INCLUDED
#define _RWSTD_CSTDDEF_H_INCLUDED


#include <rw/_defs.h>

#ifndef __SYMBIAN32__
_RWSTD_NAMESPACE (std) { 


#ifndef NULL
#  define NULL 0
#endif   // NULL


extern "C" {

#ifdef _RWSTD_PTRDIFF_T
typedef _RWSTD_PTRDIFF_T ptrdiff_t;
#else
typedef long ptrdiff_t;
#endif   // _RWSTD_PTRDIFF_T

#ifdef _RWSTD_SIZE_T
typedef _RWSTD_SIZE_T size_t;
#else
typedef unsigned long size_t;
#endif   // _RWSTD_SIZE_T


#define offsetof(class, member)  ((size_t)&(((class*)0)->member))


}   // extern "C"


}   // namespace std

#else

#include <sys/types.h>
#include <cfloat>
typedef size_t _RWSTD_SIZE_T;
typedef size_t _RWSTD_SSIZE_T;
#define _RWSTD_INTERNAL static
#define _RWSTD_SIZE_MAX (1<< 32)
#define _RWSTD_INT_MAX (1 << 31)
//#define _RWSTD_ASSERT assert
#define _RWSTD_INT_MIN -(1<<31)
#define _RWSTD_LDBL_EPSILON 2.2204460492503131e-016
#define _RWSTD_NAMESPACE(x) namespace x
#define _RWSTD_EXPORT   __declspec (dllexport)
#define _RWSTD_INT32_T int
#define _RWSTD_INT8_T char
#define _RWSTD_INT16_T short
#undef _RWSTD_LONG_LONG
#define _RWSTD_LDBL_SIZE sizeof(long double)
#define _RWSTD_LDBL_MAX_10_EXP 32
#define _RWSTD_LDBL_DIG 32
#define _RWSTD_FLT_SIZE 8
#define _RWSTD_DBL_SIZE 8
#define _RWSTD_LONG_SIZE 4
#endif

#endif   // _RWSTD_CSTDDEF_H_INCLUDED
