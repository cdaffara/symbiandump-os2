/* FDLIBM.H
 * 
 * Portions Copyright (c) 1993-1999 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 */


/* @(#)fdlibm.h 5.1 93/09/24 */
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

#ifdef __cplusplus
extern "C" {
#endif

/**
SYMBIAN adaptations 
@internalComponent
*/
#define huge	fhuge
#define tiny	ftiny
/**
@internalComponent
*/
typedef unsigned long	__uint32_t;
typedef long			__int32_t;

#ifdef __VC32__
/* warning C4056: overflow in floating-point constant arithmetic 
 * Caused by negative floating point constants, it seems!
 * For example, static double foo = -1.0;
 */
#pragma warning( disable: 4056 )
#endif
#ifdef __ARMCC__
/* Warning:  #222-D: floating-point operation result is out of range
 * The compiler detects constant math that overflows, we want overflow though!
 */
#pragma diag_suppress 222
#endif

#include <math.h>
/**
@internalComponent
*/
#define	HUGE	((float)3.40282346638528860e+38)

/** 
set X_TLOSS = pi*2**52, which is possibly defined in <values.h>
(one may replace the following line by "#include <values.h>")
@internalComponent
*/
#define X_TLOSS		1.41484755040568800000e+16 

/**
ieee style elementary functions - ESTLIB is exporting
these functions directly, so we simply map the names across
@internalComponent
*/
#define __ieee754_cosh	cosh			
#define __ieee754_sinh	sinh			
#define __ieee754_tanh	tanh	
#define __ieee754_exp	exp			

/* The original code used statements like
	n0 = ((*(int*)&one)>>29)^1;		* index of high word *
	ix0 = *(n0+(int*)&x);			* high word of x *
	ix1 = *((1-n0)+(int*)&x);		* low word of x *
   to dig two 32 bit words out of the 64 bit IEEE floating point
   value.  That is non-ANSI, and, moreover, the gcc instruction
   scheduler gets it wrong.  We instead use the following macros.
   Unlike the original code, we determine the endianness at compile
   time, not at run time; I don't see much benefit to selecting
   endianness at run time.  */

#ifndef __IEEE_BIG_ENDIAN
#ifndef __IEEE_LITTLE_ENDIAN
 #error Must define endianness
#endif
#endif

/* A union which permits us to convert between a double and two 32 bit
   ints.  */

#ifdef __IEEE_BIG_ENDIAN
/**
@internalComponent
*/
typedef union 
{
  double value;
  struct 
  {
    unsigned long msw;
    unsigned long lsw;
  } parts;
} ieee_double_shape_type;

#else

#ifdef __IEEE_LITTLE_ENDIAN
/**
@internalComponent
*/
typedef union 
{
  double value;
  struct 
  {
    unsigned long lsw;
    unsigned long msw;
  } parts;
} ieee_double_shape_type;

#endif
#endif

/**
Get two 32 bit ints from a double.  
@internalComponent
*/
#define EXTRACT_WORDS(ix0,ix1,d)				\
{								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix0) = ew_u.parts.msw;					\
  (ix1) = ew_u.parts.lsw;					\
}

/** 
Get the more significant 32 bit int from a double.  
@internalComponent
*/
#define GET_HIGH_WORD(i,d)					\
{								\
  ieee_double_shape_type gh_u;					\
  gh_u.value = (d);						\
  (i) = gh_u.parts.msw;						\
}

/**
Get the less significant 32 bit int from a double.  
@internalComponent
*/
#define GET_LOW_WORD(i,d)					\
{								\
  ieee_double_shape_type gl_u;					\
  gl_u.value = (d);						\
  (i) = gl_u.parts.lsw;						\
}

/**
Set a double from two 32 bit ints.  
@internalComponent
*/
#define INSERT_WORDS(d,ix0,ix1)					\
{								\
  ieee_double_shape_type iw_u;					\
  iw_u.parts.msw = (ix0);					\
  iw_u.parts.lsw = (ix1);					\
  (d) = iw_u.value;						\
}

/** 
Set the more significant 32 bits of a double from an int.  
@internalComponent
*/
#define SET_HIGH_WORD(d,v)					\
{								\
  ieee_double_shape_type sh_u;					\
  sh_u.value = (d);						\
  sh_u.parts.msw = (v);						\
  (d) = sh_u.value;						\
}

/** 
Set the less significant 32 bits of a double from an int.  
@internalComponent
*/
#define SET_LOW_WORD(d,v)					\
{								\
  ieee_double_shape_type sl_u;					\
  sl_u.value = (d);						\
  sl_u.parts.lsw = (v);						\
  (d) = sl_u.value;						\
}

/** 
A union which permits us to convert between a float and a 32 bit
int.  
@internalComponent
*/
typedef union
{
  float value;
  unsigned long word;
} ieee_float_shape_type;

/**
Get a 32 bit int from a float.  
@internalComponent
*/
#define GET_FLOAT_WORD(i,d)					\
{								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
}

/** 
Set a float from a 32 bit int.  
@internalComponent
*/
#define SET_FLOAT_WORD(d,i)					\
{								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
}

/**
Test a 32-bit value for being 0.
(x==0)? 0: (some value with top bit set to 1)
@internalComponent
*/

#define CHECK_ZERO(x)	((x)|-(__int32_t)(x))

#ifdef __cplusplus
}
#endif
