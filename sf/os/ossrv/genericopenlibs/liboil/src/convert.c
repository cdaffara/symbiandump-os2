/*
 * LIBOIL - Library of Optimized Inner Loops
 * Copyright (c) 2001,2003,2004 David A. Schleef <ds@schleef.org>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
//Portions Copyright (c)  2008-2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <liboilfunction.h>
#include <liboiltest.h>
#include <liboilrandom.h>

#include <math.h>
#include <string.h>
#include <stdlib.h>


/**
 * SECTION:liboilfuncs-conv
 * @title: Type Conversion
 * @short_description: Type conversion
 *
 * The functions in this section perform type conversion.
 *
 * The <i>convert</i> functions convert value from the source type to
 * the destination type.  Conversion of values outside the
 * destination range are saturated to the destination range.
 *
 * The <i>scaleconv</i> functions multiply the source values by a
 * constant factor before converting to the destination type.  Conversion
 * of values outside the destination range are clamped to the
 * destination range.
 * 
 * Conversion of values from floating point types to integer types
 * is done using a round-to-nearest policy.  Rounding of half-integers
 * is undefined and may vary between implementations.
 */


static void
convert_float_test (OilTest *test)
{
  int i;
  int n;
  double min = 0;
  double max = 1;
  void *data = oil_test_get_source_data (test, OIL_ARG_SRC1);

  n = test->params[OIL_ARG_SRC1].post_n;

  switch(test->params[OIL_ARG_DEST1].type) {
    case OIL_TYPE_s8p:
      min = oil_type_min_s8;
      max = oil_type_max_s8;
      break;
    case OIL_TYPE_u8p:
      min = oil_type_min_u8;
      max = oil_type_max_u8;
      break;
    case OIL_TYPE_s16p:
      min = oil_type_min_s16;
      max = oil_type_max_s16;
      break;
    case OIL_TYPE_u16p:
      min = oil_type_min_u16;
      max = oil_type_max_u16;
      break;
    case OIL_TYPE_s32p:
      min = oil_type_min_s32;
      max = oil_type_max_s32;
      break;
    case OIL_TYPE_u32p:
      min = oil_type_min_u32;
      max = oil_type_max_u32;
      break;
    default:
      break;
  }

  switch (test->params[OIL_ARG_SRC1].type) {
    case OIL_TYPE_f32p:
      for(i=0;i<n;i++){
        int x;
        x = oil_rand_u8() & 0x1;
        switch (x) {
          case 0:
            ((float *)data)[i] = oil_rand_f32() * (max - min) + min;
            break;
          case 1:
            if (min < 0) {
              ((float *)data)[i] = (oil_rand_f32() - 0.5) * 10;
            } else {
              ((float *)data)[i] = oil_rand_f32() * 10;
            }
            break;
        }
      }
      break;
    case OIL_TYPE_f64p:
      for(i=0;i<n;i++){
        ((double *)data)[i] = oil_rand_f64() * (max - min) + min;
      }
      break;
    default:
      break;
  }
}


#define CONVERT_DEFINE_NONE_REF(desttype,srctype) \
static void convert_ ## desttype ## _ ## srctype ## _ref ( \
  oil_type_ ## desttype *dest, \
  oil_type_ ## srctype *src, \
  int n) \
{ \
  int i; \
  oil_type_ ## srctype x; \
  for(i=0;i<n;i++){ \
    x = src[i]; \
    dest[i] = x; \
  } \
} \
OIL_DEFINE_CLASS(convert_ ## desttype ## _ ## srctype, \
  "oil_type_" #desttype " *dest, " \
  "oil_type_" #srctype " *src, " \
  "int n"); \
OIL_DEFINE_IMPL_REF(convert_ ## desttype ## _ ## srctype ## _ref, \
  convert_ ## desttype ## _ ## srctype)

#define CONVERT_DEFINE_BOTH_REF(desttype,srctype) \
static void convert_ ## desttype ## _ ## srctype ## _ref ( \
  oil_type_ ## desttype *dest, \
  oil_type_ ## srctype *src, \
  int n) \
{ \
  int i; \
  oil_type_ ## srctype x; \
  for(i=0;i<n;i++){ \
    x = src[i]; \
    if(x<oil_type_min_ ## desttype) x=oil_type_min_ ## desttype; \
    if(x>oil_type_max_ ## desttype) x=oil_type_max_ ## desttype; \
    dest[i] = x; \
  } \
} \
OIL_DEFINE_CLASS(convert_ ## desttype ## _ ## srctype, \
  "oil_type_" #desttype " *dest, " \
  "oil_type_" #srctype " *src, " \
  "int n"); \
OIL_DEFINE_IMPL_REF(convert_ ## desttype ## _ ## srctype ## _ref, \
  convert_ ## desttype ## _ ## srctype)

#define CONVERT_DEFINE_UPPER_REF(desttype,srctype) \
static void convert_ ## desttype ## _ ## srctype ## _ref ( \
  oil_type_ ## desttype *dest, \
  oil_type_ ## srctype *src, \
  int n) \
{ \
  int i; \
  oil_type_ ## srctype x; \
  for(i=0;i<n;i++){ \
    x = src[i]; \
    if(x>oil_type_max_ ## desttype) x=oil_type_max_ ## desttype; \
    dest[i] = x; \
  } \
} \
OIL_DEFINE_CLASS(convert_ ## desttype ## _ ## srctype, \
  "oil_type_" #desttype " *dest, " \
  "oil_type_" #srctype " *src, " \
  "int n"); \
OIL_DEFINE_IMPL_REF(convert_ ## desttype ## _ ## srctype ## _ref, \
  convert_ ## desttype ## _ ## srctype)

#define CONVERT_DEFINE_LOWER_REF(desttype,srctype) \
static void convert_ ## desttype ## _ ## srctype ## _ref ( \
  oil_type_ ## desttype *dest, \
  oil_type_ ## srctype *src, \
  int n) \
{ \
  int i; \
  oil_type_ ## srctype x; \
  for(i=0;i<n;i++){ \
    x = src[i]; \
    if(x<oil_type_min_ ## desttype) x=oil_type_min_ ## desttype; \
    dest[i] = x; \
  } \
} \
OIL_DEFINE_CLASS(convert_ ## desttype ## _ ## srctype, \
  "oil_type_" #desttype " *dest, " \
  "oil_type_" #srctype " *src, " \
  "int n"); \
OIL_DEFINE_IMPL_REF(convert_ ## desttype ## _ ## srctype ## _ref, \
  convert_ ## desttype ## _ ## srctype)

#define CONVERT_DEFINE_FLOAT_REF(desttype,srctype) \
static void convert_ ## desttype ## _ ## srctype ## _ref ( \
  oil_type_ ## desttype *dest, \
  oil_type_ ## srctype *src, \
  int n) \
{ \
  int i; \
  oil_type_ ## srctype x; \
  for(i=0;i<n;i++){ \
    x = src[i]; \
    if(x<oil_type_min_ ## desttype) x=oil_type_min_ ## desttype; \
    if(x>oil_type_max_ ## desttype) x=oil_type_max_ ## desttype; \
    dest[i] = x; \
  } \
} \
OIL_DEFINE_CLASS_FULL(convert_ ## desttype ## _ ## srctype, \
  "oil_type_" #desttype " *dest, " \
  "oil_type_" #srctype " *src, " \
  "int n", convert_float_test); \
OIL_DEFINE_IMPL_REF(convert_ ## desttype ## _ ## srctype ## _ref, \
  convert_ ## desttype ## _ ## srctype)

/* no clip */
CONVERT_DEFINE_NONE_REF(s16,s8);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s16_s8_ref() {
        return &_oil_function_impl_convert_s16_s8_ref;
}
#endif

CONVERT_DEFINE_NONE_REF(s16,u8);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s16_u8_ref() {
        return &_oil_function_impl_convert_s16_u8_ref;
}
#endif

CONVERT_DEFINE_NONE_REF(s32,s8);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s32_s8_ref() {
        return &_oil_function_impl_convert_s32_s8_ref;
}
#endif

CONVERT_DEFINE_NONE_REF(s32,u8);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s32_u8_ref() {
        return &_oil_function_impl_convert_s32_u8_ref;
}
#endif

CONVERT_DEFINE_NONE_REF(s32,s16);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s32_s16_ref() {
        return &_oil_function_impl_convert_s32_s16_ref;
}
#endif

CONVERT_DEFINE_NONE_REF(s32,u16);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s32_u16_ref() {
        return &_oil_function_impl_convert_s32_u16_ref;
}
#endif

CONVERT_DEFINE_NONE_REF(u16,u8);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u16_u8_ref() {
        return &_oil_function_impl_convert_u16_u8_ref;
}
#endif

CONVERT_DEFINE_NONE_REF(u32,u8);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u32_u8_ref() {
        return &_oil_function_impl_convert_u32_u8_ref;
}
#endif

CONVERT_DEFINE_NONE_REF(u32,u16);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u32_u16_ref() {
        return &_oil_function_impl_convert_u32_u16_ref;
}
#endif

/* clip upper */
CONVERT_DEFINE_UPPER_REF(s8,u8);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s8_u8_ref() {
        return &_oil_function_impl_convert_s8_u8_ref;
}
#endif

CONVERT_DEFINE_UPPER_REF(s8,u16);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s8_u16_ref() {
        return &_oil_function_impl_convert_s8_u16_ref;
}
#endif

CONVERT_DEFINE_UPPER_REF(s8,u32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s8_u32_ref() {
        return &_oil_function_impl_convert_s8_u32_ref;
}
#endif

CONVERT_DEFINE_UPPER_REF(u8,u32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u8_u32_ref() {
        return &_oil_function_impl_convert_u8_u32_ref;
}
#endif

CONVERT_DEFINE_UPPER_REF(u8,u16);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u8_u16_ref() {
        return &_oil_function_impl_convert_u8_u16_ref;
}
#endif

CONVERT_DEFINE_UPPER_REF(s16,u16);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s16_u16_ref() {
        return &_oil_function_impl_convert_s16_u16_ref;
}
#endif

CONVERT_DEFINE_UPPER_REF(s16,u32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s16_u32_ref() {
        return &_oil_function_impl_convert_s16_u32_ref;
}
#endif

CONVERT_DEFINE_UPPER_REF(s32,u32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s32_u32_ref() {
        return &_oil_function_impl_convert_s32_u32_ref;
}
#endif

CONVERT_DEFINE_UPPER_REF(u16,u32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u16_u32_ref() {
        return &_oil_function_impl_convert_u16_u32_ref;
}
#endif

/* clip both */
CONVERT_DEFINE_BOTH_REF(s8,s16);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s8_s16_ref() {
        return &_oil_function_impl_convert_s8_s16_ref;
}
#endif

CONVERT_DEFINE_BOTH_REF(s8,s32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s8_s32_ref() {
        return &_oil_function_impl_convert_s8_s32_ref;
}
#endif

CONVERT_DEFINE_BOTH_REF(u8,s16);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u8_s16_ref() {
        return &_oil_function_impl_convert_u8_s16_ref;
}
#endif

CONVERT_DEFINE_BOTH_REF(u8,s32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u8_s32_ref() {
        return &_oil_function_impl_convert_u8_s32_ref;
}
#endif

CONVERT_DEFINE_BOTH_REF(s16,s32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s16_s32_ref() {
        return &_oil_function_impl_convert_s16_s32_ref;
}
#endif

CONVERT_DEFINE_BOTH_REF(u16,s32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u16_s32_ref() {
        return &_oil_function_impl_convert_u16_s32_ref;
}
#endif

/* clip lower */
CONVERT_DEFINE_LOWER_REF(u8,s8);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u8_s8_ref() {
        return &_oil_function_impl_convert_u8_s8_ref;
}
#endif

CONVERT_DEFINE_LOWER_REF(u16,s16);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u16_s16_ref() {
        return &_oil_function_impl_convert_u16_s16_ref;
}
#endif

CONVERT_DEFINE_LOWER_REF(u32,s32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u32_s32_ref() {
        return &_oil_function_impl_convert_u32_s32_ref;
}
#endif

/* clip both, float */
CONVERT_DEFINE_FLOAT_REF(s8,f32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s8_f32_ref() {
        return &_oil_function_impl_convert_s8_f32_ref;
}
#endif

CONVERT_DEFINE_FLOAT_REF(s8,f64);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s8_f64_ref() {
        return &_oil_function_impl_convert_s8_f64_ref;
}
#endif

CONVERT_DEFINE_FLOAT_REF(u8,f32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u8_f32_ref() {
        return &_oil_function_impl_convert_u8_f32_ref;
}
#endif

CONVERT_DEFINE_FLOAT_REF(u8,f64);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u8_f64_ref() {
        return &_oil_function_impl_convert_u8_f64_ref;
}
#endif

CONVERT_DEFINE_FLOAT_REF(s16,f32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s16_f32_ref() {
        return &_oil_function_impl_convert_s16_f32_ref;
}
#endif

CONVERT_DEFINE_FLOAT_REF(s16,f64);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s16_f64_ref() {
        return &_oil_function_impl_convert_s16_f64_ref;
}
#endif

CONVERT_DEFINE_FLOAT_REF(u16,f32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u16_f32_ref() {
        return &_oil_function_impl_convert_u16_f32_ref;
}
#endif

CONVERT_DEFINE_FLOAT_REF(u16,f64);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u16_f64_ref() {
        return &_oil_function_impl_convert_u16_f64_ref;
}
#endif

CONVERT_DEFINE_FLOAT_REF(s32,f64);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_s32_f64_ref() {
        return &_oil_function_impl_convert_s32_f64_ref;
}
#endif

CONVERT_DEFINE_FLOAT_REF(u32,f64);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_convert_u32_f64_ref() {
        return &_oil_function_impl_convert_u32_f64_ref;
}
#endif



/**
 * oil_convert_f32_f64:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f32_s16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f32_s32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f32_s8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f32_u16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f32_u32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f32_u8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f64_f32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f64_s16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f64_s32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f64_s8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f64_u16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f64_u32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_f64_u8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s16_f32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s16_f64:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s16_s32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s16_s8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s16_u16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s16_u32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s16_u8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s32_f32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s32_f64:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s32_s16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s32_s8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s32_u16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s32_u32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s32_u8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s8_f32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s8_f64:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s8_s16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s8_s32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s8_u16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s8_u32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_s8_u8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u16_f32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u16_f64:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u16_s16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u16_s32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u16_s8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u16_u32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u16_u8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u32_f32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u32_f64:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u32_s16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u32_s32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u32_s8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u32_u16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u32_u8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u8_f32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u8_f64:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u8_s16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u8_s32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u8_s8:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u8_u16:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_convert_u8_u32:
 * @dest:
 * @src:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */



#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s16_s8() {
        return &_oil_function_class_convert_s16_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s16_u8() {
        return &_oil_function_class_convert_s16_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s32_s8() {
        return &_oil_function_class_convert_s32_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s32_u8() {
        return &_oil_function_class_convert_s32_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s32_s16() {
        return &_oil_function_class_convert_s32_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s32_u16() {
        return &_oil_function_class_convert_s32_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u16_u8() {
        return &_oil_function_class_convert_u16_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u32_u8() {
        return &_oil_function_class_convert_u32_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u32_u16() {
        return &_oil_function_class_convert_u32_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s8_u8() {
        return &_oil_function_class_convert_s8_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s8_u16() {
        return &_oil_function_class_convert_s8_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s8_u32() {
        return &_oil_function_class_convert_s8_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u8_u32() {
        return &_oil_function_class_convert_u8_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u8_u16() {
        return &_oil_function_class_convert_u8_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s16_u16() {
        return &_oil_function_class_convert_s16_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s16_u32() {
        return &_oil_function_class_convert_s16_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s32_u32() {
        return &_oil_function_class_convert_s32_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u16_u32() {
        return &_oil_function_class_convert_u16_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s8_s16() {
        return &_oil_function_class_convert_s8_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s8_s32() {
        return &_oil_function_class_convert_s8_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u8_s16() {
        return &_oil_function_class_convert_u8_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u8_s32() {
        return &_oil_function_class_convert_u8_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s16_s32() {
        return &_oil_function_class_convert_s16_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u16_s32() {
        return &_oil_function_class_convert_u16_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u8_s8() {
        return &_oil_function_class_convert_u8_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u16_s16() {
        return &_oil_function_class_convert_u16_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u32_s32() {
        return &_oil_function_class_convert_u32_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s8_f32() {
        return &_oil_function_class_convert_s8_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s8_f64() {
        return &_oil_function_class_convert_s8_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u8_f32() {
        return &_oil_function_class_convert_u8_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u8_f64() {
        return &_oil_function_class_convert_u8_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s16_f32() {
        return &_oil_function_class_convert_s16_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s16_f64() {
        return &_oil_function_class_convert_s16_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u16_f32() {
        return &_oil_function_class_convert_u16_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u16_f64() {
        return &_oil_function_class_convert_u16_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_s32_f64() {
        return &_oil_function_class_convert_s32_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_convert_u32_f64() {
        return &_oil_function_class_convert_u32_f64;
}
#endif



#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s16_s8 ()	{
	oil_function_class_ptr_convert_s16_s8 = __oil_function_class_convert_s16_s8();
	return &oil_function_class_ptr_convert_s16_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s16_u8 ()	{
	oil_function_class_ptr_convert_s16_u8 = __oil_function_class_convert_s16_u8();
	return &oil_function_class_ptr_convert_s16_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s32_s8 ()	{
	oil_function_class_ptr_convert_s32_s8 = __oil_function_class_convert_s32_s8();
	return &oil_function_class_ptr_convert_s32_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s32_u8 ()	{
	oil_function_class_ptr_convert_s32_u8 = __oil_function_class_convert_s32_u8();
	return &oil_function_class_ptr_convert_s32_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s32_s16 ()	{
	oil_function_class_ptr_convert_s32_s16 = __oil_function_class_convert_s32_s16();
	return &oil_function_class_ptr_convert_s32_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s32_u16 ()	{
	oil_function_class_ptr_convert_s32_u16 = __oil_function_class_convert_s32_u16();
	return &oil_function_class_ptr_convert_s32_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u16_u8 ()	{
	oil_function_class_ptr_convert_u16_u8 = __oil_function_class_convert_u16_u8();
	return &oil_function_class_ptr_convert_u16_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u32_u8 ()	{
	oil_function_class_ptr_convert_u32_u8 = __oil_function_class_convert_u32_u8();
	return &oil_function_class_ptr_convert_u32_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u32_u16 ()	{
	oil_function_class_ptr_convert_u32_u16 = __oil_function_class_convert_u32_u16();
	return &oil_function_class_ptr_convert_u32_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s8_u8 ()	{
	oil_function_class_ptr_convert_s8_u8 = __oil_function_class_convert_s8_u8();
	return &oil_function_class_ptr_convert_s8_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s8_u16 ()	{
	oil_function_class_ptr_convert_s8_u16 = __oil_function_class_convert_s8_u16();
	return &oil_function_class_ptr_convert_s8_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s8_u32 ()	{
	oil_function_class_ptr_convert_s8_u32 = __oil_function_class_convert_s8_u32();
	return &oil_function_class_ptr_convert_s8_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u8_u32 ()	{
	oil_function_class_ptr_convert_u8_u32 = __oil_function_class_convert_u8_u32();
	return &oil_function_class_ptr_convert_u8_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u8_u16 ()	{
	oil_function_class_ptr_convert_u8_u16 = __oil_function_class_convert_u8_u16();
	return &oil_function_class_ptr_convert_u8_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s16_u16 ()	{
	oil_function_class_ptr_convert_s16_u16 = __oil_function_class_convert_s16_u16();
	return &oil_function_class_ptr_convert_s16_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s16_u32 ()	{
	oil_function_class_ptr_convert_s16_u32 = __oil_function_class_convert_s16_u32();
	return &oil_function_class_ptr_convert_s16_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s32_u32 ()	{
	oil_function_class_ptr_convert_s32_u32 = __oil_function_class_convert_s32_u32();
	return &oil_function_class_ptr_convert_s32_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u16_u32 ()	{
	oil_function_class_ptr_convert_u16_u32 = __oil_function_class_convert_u16_u32();
	return &oil_function_class_ptr_convert_u16_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s8_s16 ()	{
	oil_function_class_ptr_convert_s8_s16 = __oil_function_class_convert_s8_s16();
	return &oil_function_class_ptr_convert_s8_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s8_s32 ()	{
	oil_function_class_ptr_convert_s8_s32 = __oil_function_class_convert_s8_s32();
	return &oil_function_class_ptr_convert_s8_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u8_s16 ()	{
	oil_function_class_ptr_convert_u8_s16 = __oil_function_class_convert_u8_s16();
	return &oil_function_class_ptr_convert_u8_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u8_s32 ()	{
	oil_function_class_ptr_convert_u8_s32 = __oil_function_class_convert_u8_s32();
	return &oil_function_class_ptr_convert_u8_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s16_s32 ()	{
	oil_function_class_ptr_convert_s16_s32 = __oil_function_class_convert_s16_s32();
	return &oil_function_class_ptr_convert_s16_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u16_s32 ()	{
	oil_function_class_ptr_convert_u16_s32 = __oil_function_class_convert_u16_s32();
	return &oil_function_class_ptr_convert_u16_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u8_s8 ()	{
	oil_function_class_ptr_convert_u8_s8 = __oil_function_class_convert_u8_s8();
	return &oil_function_class_ptr_convert_u8_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u16_s16 ()	{
	oil_function_class_ptr_convert_u16_s16 = __oil_function_class_convert_u16_s16();
	return &oil_function_class_ptr_convert_u16_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u32_s32 ()	{
	oil_function_class_ptr_convert_u32_s32 = __oil_function_class_convert_u32_s32();
	return &oil_function_class_ptr_convert_u32_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s8_f32 ()	{
	oil_function_class_ptr_convert_s8_f32 = __oil_function_class_convert_s8_f32();
	return &oil_function_class_ptr_convert_s8_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s8_f64 ()	{
	oil_function_class_ptr_convert_s8_f64 = __oil_function_class_convert_s8_f64();
	return &oil_function_class_ptr_convert_s8_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u8_f32 ()	{
	oil_function_class_ptr_convert_u8_f32 = __oil_function_class_convert_u8_f32();
	return &oil_function_class_ptr_convert_u8_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u8_f64 ()	{
	oil_function_class_ptr_convert_u8_f64 = __oil_function_class_convert_u8_f64();
	return &oil_function_class_ptr_convert_u8_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s16_f32 ()	{
	oil_function_class_ptr_convert_s16_f32 = __oil_function_class_convert_s16_f32();
	return &oil_function_class_ptr_convert_s16_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s16_f64 ()	{
	oil_function_class_ptr_convert_s16_f64 = __oil_function_class_convert_s16_f64();
	return &oil_function_class_ptr_convert_s16_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u16_f32 ()	{
	oil_function_class_ptr_convert_u16_f32 = __oil_function_class_convert_u16_f32();
	return &oil_function_class_ptr_convert_u16_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u16_f64 ()	{
	oil_function_class_ptr_convert_u16_f64 = __oil_function_class_convert_u16_f64();
	return &oil_function_class_ptr_convert_u16_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_s32_f64 ()	{
	oil_function_class_ptr_convert_s32_f64 = __oil_function_class_convert_s32_f64();
	return &oil_function_class_ptr_convert_s32_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_convert_u32_f64 ()	{
	oil_function_class_ptr_convert_u32_f64 = __oil_function_class_convert_u32_f64();
	return &oil_function_class_ptr_convert_u32_f64->func;
	}
#endif

