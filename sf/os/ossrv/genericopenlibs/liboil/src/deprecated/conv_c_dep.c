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
#include <liboil/liboilfunction.h>
#include <liboil/liboiltest.h>
#include <liboil/liboilrandom.h>

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
 * The <i>conv</i> functions convert value from the source type to
 * the destination type.  Conversion of values outside the destination
 * range is undefined and may vary between implementations.
 *
 * The <i>clipconv</i> functions convert values from the source
 * type to the destination type.  Conversion of values outside the
 * destination range are saturated to the destination range.
 *
 * The <i>scaleconv</i> functions multiply the source values by a
 * constant factor before converting to the destination type.  Conversion
 * of values outside the destination range is undefined and may vary
 * between implementations.
 * 
 * Conversion of values from floating point types to integer types
 * is done using a round-to-nearest policy.  Rounding of half-integers
 * is undefined and may vary between implementations.
 */


static void
conv_test (OilTest *test)
{
  int i;
  int n;
  double min = 0;
  double max = 1;
  int stride = test->params[OIL_ARG_SRC1].stride;
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
            OIL_GET(data, stride * i, float) =
              oil_rand_f32() * (max - min) + min;
            break;
          case 1:
            if (min < 0) {
              OIL_GET(data, stride * i, float) =
                (oil_rand_f32() - 0.5) * 10;
            } else {
              OIL_GET(data, stride * i, float) = oil_rand_f32() * 10;
            }
            break;
        }
      }
      break;
    case OIL_TYPE_f64p:
      for(i=0;i<n;i++){
        OIL_GET(data, stride * i, double) = oil_rand_f64() * (max - min) + min;
      }
      break;
    default:
      break;
  }
}

#define CONV_DEFINE_REF_CAST(desttype,srctype) \
static void conv_ ## desttype ## _ ## srctype ## _ref ( \
    oil_type_ ## desttype *dest,    \
    int dest_stride,        \
    oil_type_ ## srctype *src,      \
    int src_stride, int n)      \
{                   \
    int i;              \
    for(i=0;i<n;i++){       \
        OIL_GET(dest,i*dest_stride, oil_type_ ## desttype) = \
            OIL_GET(src,i*src_stride, oil_type_ ## srctype);    \
    }               \
}                   \
OIL_DEFINE_CLASS(conv_ ## desttype ## _ ## srctype, \
    "oil_type_" #desttype " *dest, "    \
    "int dstr, "            \
    "oil_type_" #srctype " *src, "  \
    "int sstr, int n");     \
OIL_DEFINE_IMPL_REF(conv_ ## desttype ## _ ## srctype ## _ref,  \
    conv_ ## desttype ## _ ## srctype)

#define CONV_DEFINE_FLOAT_REF(desttype,srctype) \
static void conv_ ## desttype ## _ ## srctype ## _ref ( \
    oil_type_ ## desttype *dest,    \
    int dest_stride,        \
    oil_type_ ## srctype *src,      \
    int src_stride, int n)      \
{                   \
    int i;              \
    for(i=0;i<n;i++){       \
        OIL_GET(dest,i*dest_stride, oil_type_ ## desttype) =        \
            rint(OIL_GET(src,i*src_stride, oil_type_ ## srctype));  \
    }               \
}                   \
OIL_DEFINE_CLASS_FULL(conv_ ## desttype ## _ ## srctype,    \
    "oil_type_" #desttype " *dest, "    \
    "int dstr, "            \
    "oil_type_" #srctype " *src, "  \
    "int sstr, int n", conv_test);  \
OIL_DEFINE_IMPL_REF(conv_ ## desttype ## _ ## srctype ## _ref,  \
    conv_ ## desttype ## _ ## srctype)

CONV_DEFINE_REF_CAST(s8,u8);
CONV_DEFINE_REF_CAST(s8,s16);
CONV_DEFINE_REF_CAST(s8,u16);
CONV_DEFINE_REF_CAST(s8,s32);
CONV_DEFINE_REF_CAST(s8,u32);
CONV_DEFINE_FLOAT_REF(s8,f32);
CONV_DEFINE_FLOAT_REF(s8,f64);

CONV_DEFINE_REF_CAST(u8,s8);
CONV_DEFINE_REF_CAST(u8,s16);
CONV_DEFINE_REF_CAST(u8,u16);
CONV_DEFINE_REF_CAST(u8,s32);
CONV_DEFINE_REF_CAST(u8,u32);
CONV_DEFINE_FLOAT_REF(u8,f32);
CONV_DEFINE_FLOAT_REF(u8,f64);

CONV_DEFINE_REF_CAST(s16,s8);
CONV_DEFINE_REF_CAST(s16,u8);
CONV_DEFINE_REF_CAST(s16,u16);
CONV_DEFINE_REF_CAST(s16,s32);
CONV_DEFINE_REF_CAST(s16,u32);
CONV_DEFINE_FLOAT_REF(s16,f32);
CONV_DEFINE_FLOAT_REF(s16,f64);

CONV_DEFINE_REF_CAST(u16,s8);
CONV_DEFINE_REF_CAST(u16,u8);
CONV_DEFINE_REF_CAST(u16,s16);
CONV_DEFINE_REF_CAST(u16,s32);
CONV_DEFINE_REF_CAST(u16,u32);
CONV_DEFINE_FLOAT_REF(u16,f32);
CONV_DEFINE_FLOAT_REF(u16,f64);

CONV_DEFINE_REF_CAST(s32,s8);
CONV_DEFINE_REF_CAST(s32,s16);
CONV_DEFINE_REF_CAST(s32,u8);
CONV_DEFINE_REF_CAST(s32,u16);
CONV_DEFINE_REF_CAST(s32,u32);
CONV_DEFINE_FLOAT_REF(s32,f32);
CONV_DEFINE_FLOAT_REF(s32,f64);

CONV_DEFINE_REF_CAST(u32,s8);
CONV_DEFINE_REF_CAST(u32,s16);
CONV_DEFINE_REF_CAST(u32,u8);
CONV_DEFINE_REF_CAST(u32,u16);
CONV_DEFINE_REF_CAST(u32,s32);
CONV_DEFINE_FLOAT_REF(u32,f32);
CONV_DEFINE_FLOAT_REF(u32,f64);

CONV_DEFINE_REF_CAST(f32,s8);
CONV_DEFINE_REF_CAST(f32,s16);
CONV_DEFINE_REF_CAST(f32,u8);
CONV_DEFINE_REF_CAST(f32,u16);
CONV_DEFINE_REF_CAST(f32,s32);
CONV_DEFINE_REF_CAST(f32,u32);
CONV_DEFINE_REF_CAST(f32,f64);

CONV_DEFINE_REF_CAST(f64,s8);
CONV_DEFINE_REF_CAST(f64,u8);
CONV_DEFINE_REF_CAST(f64,s16);
CONV_DEFINE_REF_CAST(f64,u16);
CONV_DEFINE_REF_CAST(f64,s32);
CONV_DEFINE_REF_CAST(f64,u32);
CONV_DEFINE_REF_CAST(f64,f32);




#define CLIPCONV_DEFINE_BOTH_REF(desttype,srctype) \
static void clipconv_ ## desttype ## _ ## srctype ## _ref ( \
    oil_type_ ## desttype *dest,    \
    int dest_stride,        \
    oil_type_ ## srctype *src,      \
    int src_stride, int n)      \
{                   \
    int i;              \
    oil_type_ ## srctype x;     \
    for(i=0;i<n;i++){       \
        x = OIL_GET(src,i*src_stride, oil_type_ ## srctype);            \
        if(x<oil_type_min_ ## desttype) x=oil_type_min_ ## desttype;    \
        if(x>oil_type_max_ ## desttype) x=oil_type_max_ ## desttype;    \
        OIL_GET(dest,i*dest_stride, oil_type_ ## desttype) = x;         \
    }               \
}                   \
OIL_DEFINE_CLASS(clipconv_ ## desttype ## _ ## srctype, \
    "oil_type_" #desttype " *dest, "    \
    "int dstr, "            \
    "oil_type_" #srctype " *src, "  \
    "int sstr, int n");     \
OIL_DEFINE_IMPL_REF(clipconv_ ## desttype ## _ ## srctype ## _ref,  \
    clipconv_ ## desttype ## _ ## srctype)

#define CLIPCONV_DEFINE_UPPER_REF(desttype,srctype) \
static void clipconv_ ## desttype ## _ ## srctype ## _ref ( \
    oil_type_ ## desttype *dest,    \
    int dest_stride,        \
    oil_type_ ## srctype *src,      \
    int src_stride, int n)      \
{                   \
    int i;              \
    oil_type_ ## srctype x;     \
    for(i=0;i<n;i++){       \
        x = OIL_GET(src,i*src_stride, oil_type_ ## srctype);            \
        if(x>oil_type_max_ ## desttype) x=oil_type_max_ ## desttype;    \
        OIL_GET(dest,i*dest_stride, oil_type_ ## desttype) = x;         \
    }               \
}                   \
OIL_DEFINE_CLASS(clipconv_ ## desttype ## _ ## srctype, \
    "oil_type_" #desttype " *dest, "    \
    "int dstr, "            \
    "oil_type_" #srctype " *src, "  \
    "int sstr, int n");     \
OIL_DEFINE_IMPL_REF(clipconv_ ## desttype ## _ ## srctype ## _ref,  \
    clipconv_ ## desttype ## _ ## srctype)

#define CLIPCONV_DEFINE_LOWER_REF(desttype,srctype) \
static void clipconv_ ## desttype ## _ ## srctype ## _ref ( \
    oil_type_ ## desttype *dest,    \
    int dest_stride,        \
    oil_type_ ## srctype *src,      \
    int src_stride, int n)      \
{                   \
    int i;              \
    oil_type_ ## srctype x;     \
    for(i=0;i<n;i++){       \
        x = OIL_GET(src,i*src_stride, oil_type_ ## srctype);            \
        if(x<oil_type_min_ ## desttype) x=oil_type_min_ ## desttype;    \
        OIL_GET(dest,i*dest_stride, oil_type_ ## desttype) = x;         \
    }               \
}                   \
OIL_DEFINE_CLASS(clipconv_ ## desttype ## _ ## srctype, \
    "oil_type_" #desttype " *dest, "    \
    "int dstr, "            \
    "oil_type_" #srctype " *src, "  \
    "int sstr, int n");     \
OIL_DEFINE_IMPL_REF(clipconv_ ## desttype ## _ ## srctype ## _ref,  \
    clipconv_ ## desttype ## _ ## srctype)

#define CLIPCONV_DEFINE_FLOAT_REF(desttype,srctype) \
static void clipconv_ ## desttype ## _ ## srctype ## _ref ( \
    oil_type_ ## desttype *dest,    \
    int dest_stride,        \
    oil_type_ ## srctype *src,      \
    int src_stride, int n)      \
{                   \
    int i;              \
    oil_type_ ## srctype x;     \
    for(i=0;i<n;i++){       \
        x = OIL_GET(src,i*src_stride, oil_type_ ## srctype);            \
        if(x<oil_type_min_ ## desttype) x=oil_type_min_ ## desttype;    \
        if(x>oil_type_max_ ## desttype) x=oil_type_max_ ## desttype;    \
        OIL_GET(dest,i*dest_stride, oil_type_ ## desttype) = rint(x);       \
    }               \
}                   \
OIL_DEFINE_CLASS(clipconv_ ## desttype ## _ ## srctype, \
    "oil_type_" #desttype " *dest, "    \
    "int dstr, "            \
    "oil_type_" #srctype " *src, "  \
    "int sstr, int n");     \
OIL_DEFINE_IMPL_REF(clipconv_ ## desttype ## _ ## srctype ## _ref,  \
    clipconv_ ## desttype ## _ ## srctype)

/* clip upper */
CLIPCONV_DEFINE_UPPER_REF(s8,u8);
CLIPCONV_DEFINE_UPPER_REF(s8,u16);
CLIPCONV_DEFINE_UPPER_REF(s8,u32);
CLIPCONV_DEFINE_UPPER_REF(u8,u32);
CLIPCONV_DEFINE_UPPER_REF(u8,u16);
CLIPCONV_DEFINE_UPPER_REF(s16,u16);
CLIPCONV_DEFINE_UPPER_REF(s16,u32);
CLIPCONV_DEFINE_UPPER_REF(s32,u32);
CLIPCONV_DEFINE_UPPER_REF(u16,u32);

/* clip both */
CLIPCONV_DEFINE_BOTH_REF(s8,s16);
CLIPCONV_DEFINE_BOTH_REF(s8,s32);
CLIPCONV_DEFINE_BOTH_REF(u8,s16);
CLIPCONV_DEFINE_BOTH_REF(u8,s32);
CLIPCONV_DEFINE_BOTH_REF(s16,s32);
CLIPCONV_DEFINE_BOTH_REF(u16,s32);

/* clip lower */
CLIPCONV_DEFINE_LOWER_REF(u8,s8);
CLIPCONV_DEFINE_LOWER_REF(u16,s16);
CLIPCONV_DEFINE_LOWER_REF(u32,s32);

/* clip both, float */
CLIPCONV_DEFINE_FLOAT_REF(s8,f32);
CLIPCONV_DEFINE_FLOAT_REF(s8,f64);
CLIPCONV_DEFINE_FLOAT_REF(u8,f32);
CLIPCONV_DEFINE_FLOAT_REF(u8,f64);
CLIPCONV_DEFINE_FLOAT_REF(s16,f32);
CLIPCONV_DEFINE_FLOAT_REF(s16,f64);
CLIPCONV_DEFINE_FLOAT_REF(u16,f32);
CLIPCONV_DEFINE_FLOAT_REF(u16,f64);
CLIPCONV_DEFINE_FLOAT_REF(s32,f32);
CLIPCONV_DEFINE_FLOAT_REF(s32,f64);
CLIPCONV_DEFINE_FLOAT_REF(u32,f32);
CLIPCONV_DEFINE_FLOAT_REF(u32,f64);



#define SCALECONV_DEFINE_REF_RINT(desttype,srctype) \
static void scaleconv_ ## desttype ## _ ## srctype ## _ref ( \
    oil_type_ ## desttype *dest,    \
    oil_type_ ## srctype *src,      \
    int n, double *offset, double *multiplier) \
{                   \
    int i;              \
        double x;                       \
    for(i=0;i<n;i++){       \
        x = *offset + *multiplier * src[i]; \
        if(x<oil_type_min_ ## desttype) x=oil_type_min_ ## desttype;    \
        if(x>oil_type_max_ ## desttype) x=oil_type_max_ ## desttype;    \
        dest[i] = rint(x);  \
    }               \
}                   \
OIL_DEFINE_CLASS(scaleconv_ ## desttype ## _ ## srctype,    \
    "oil_type_" #desttype " *dest, "    \
    "oil_type_" #srctype " *src, "  \
    "int n, double *s2_1, double *s3_1"); \
OIL_DEFINE_IMPL_REF(scaleconv_ ## desttype ## _ ## srctype ## _ref, \
    scaleconv_ ## desttype ## _ ## srctype)

#define SCALECONV_DEFINE_REF_CAST(desttype,srctype) \
static void scaleconv_ ## desttype ## _ ## srctype ## _ref ( \
    oil_type_ ## desttype *dest,    \
    oil_type_ ## srctype *src,      \
    int n, double *offset, double *multiplier) \
{                   \
    int i;              \
    for(i=0;i<n;i++){       \
        dest[i] = *offset + *multiplier * src[i];   \
    }               \
}                   \
OIL_DEFINE_CLASS(scaleconv_ ## desttype ## _ ## srctype,    \
    "oil_type_" #desttype " *dest, "    \
    "oil_type_" #srctype " *src, "  \
    "int n, double *s2_1, double *s3_1"); \
OIL_DEFINE_IMPL_REF(scaleconv_ ## desttype ## _ ## srctype ## _ref, \
    scaleconv_ ## desttype ## _ ## srctype)

SCALECONV_DEFINE_REF_RINT(s8,f32);
SCALECONV_DEFINE_REF_RINT(u8,f32);
SCALECONV_DEFINE_REF_RINT(s16,f32);
SCALECONV_DEFINE_REF_RINT(u16,f32);
SCALECONV_DEFINE_REF_RINT(s32,f32);
SCALECONV_DEFINE_REF_RINT(u32,f32);

SCALECONV_DEFINE_REF_RINT(s8,f64);
SCALECONV_DEFINE_REF_RINT(u8,f64);
SCALECONV_DEFINE_REF_RINT(s16,f64);
SCALECONV_DEFINE_REF_RINT(u16,f64);
SCALECONV_DEFINE_REF_RINT(s32,f64);
SCALECONV_DEFINE_REF_RINT(u32,f64);

SCALECONV_DEFINE_REF_CAST(f32,s8);
SCALECONV_DEFINE_REF_CAST(f32,u8);
SCALECONV_DEFINE_REF_CAST(f32,s16);
SCALECONV_DEFINE_REF_CAST(f32,u16);
SCALECONV_DEFINE_REF_CAST(f32,s32);
SCALECONV_DEFINE_REF_CAST(f32,u32);

SCALECONV_DEFINE_REF_CAST(f64,s8);
SCALECONV_DEFINE_REF_CAST(f64,u8);
SCALECONV_DEFINE_REF_CAST(f64,s16);
SCALECONV_DEFINE_REF_CAST(f64,u16);
SCALECONV_DEFINE_REF_CAST(f64,s32);
SCALECONV_DEFINE_REF_CAST(f64,u32);

/**
 * oil_conv_f32_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f32_s16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f32_s32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f32_s8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f32_u16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f32_u32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f32_u8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f64_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f64_s16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f64_s32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f64_s8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f64_u16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f64_u32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_f64_u8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s16_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s16_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s16_s32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s16_s8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s16_u16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s16_u32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s16_u8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s32_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s32_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s32_s16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s32_s8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s32_u16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s32_u32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s32_u8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s8_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s8_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s8_s16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s8_s32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s8_u16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s8_u32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_s8_u8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u16_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u16_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u16_s16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u16_s32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u16_s8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u16_u32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u16_u8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u32_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u32_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u32_s16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u32_s32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u32_s8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u32_u16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u32_u8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u8_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u8_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u8_s16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u8_s32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u8_s8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u8_u16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_conv_u8_u32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are undefined
 * and implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s16_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s16_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s16_s32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s16_u16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s16_u32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s32_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s32_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s32_u32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s8_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s8_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s8_s16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s8_s32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s8_u16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s8_u32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_s8_u8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u16_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u16_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u16_s16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u16_s32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u16_u32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u32_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u32_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u32_s32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u8_f32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u8_f64:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u8_s16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u8_s32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u8_s8:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u8_u16:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_clipconv_u8_u32:
 * @dest:
 * @dstr:
 * @src:
 * @sstr:
 * @n:
 * 
 * Converts elements in  from the source type
 * to the destination type and places the result in .
 * Values outside the destination range are clipped to
 * the destination range.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_f32_s16:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_f32_s32:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_f32_s8:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_f32_u16:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_f32_u32:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_f32_u8:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_f64_s16:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_f64_s32:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_f64_s8:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_f64_u16:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_f64_u32:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_f64_u8:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_s16_f32:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_s16_f64:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_s32_f32:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_s32_f64:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_s8_f32:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_s8_f64:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_u16_f32:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_u16_f64:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_u32_f32:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_u32_f64:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_u8_f32:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */

/**
 * oil_scaleconv_u8_f64:
 * @dest:
 * @src:
 * @n:
 # @s2_1:
 # @s3_1:
 * 
 * Multiplies elements in  by  and adds  and then
 * converts the result
 * to the destination type and places the result in .
 * Values outside the destination range are undefined and
 * implementation dependent.
 * See the comments at the beginning of this section.
 */
 




#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s8_u8() {
        return &_oil_function_class_conv_s8_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s8_s16() {
        return &_oil_function_class_conv_s8_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s8_u16() {
        return &_oil_function_class_conv_s8_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s8_s32() {
        return &_oil_function_class_conv_s8_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s8_u32() {
        return &_oil_function_class_conv_s8_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s8_f32() {
        return &_oil_function_class_conv_s8_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s8_f64() {
        return &_oil_function_class_conv_s8_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u8_s8() {
        return &_oil_function_class_conv_u8_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u8_s16() {
        return &_oil_function_class_conv_u8_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u8_u16() {
        return &_oil_function_class_conv_u8_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u8_s32() {
        return &_oil_function_class_conv_u8_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u8_u32() {
        return &_oil_function_class_conv_u8_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u8_f32() {
        return &_oil_function_class_conv_u8_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u8_f64() {
        return &_oil_function_class_conv_u8_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s16_s8() {
        return &_oil_function_class_conv_s16_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s16_u8() {
        return &_oil_function_class_conv_s16_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s16_u16() {
        return &_oil_function_class_conv_s16_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s16_s32() {
        return &_oil_function_class_conv_s16_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s16_u32() {
        return &_oil_function_class_conv_s16_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s16_f32() {
        return &_oil_function_class_conv_s16_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s16_f64() {
        return &_oil_function_class_conv_s16_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u16_s8() {
        return &_oil_function_class_conv_u16_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u16_u8() {
        return &_oil_function_class_conv_u16_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u16_s16() {
        return &_oil_function_class_conv_u16_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u16_s32() {
        return &_oil_function_class_conv_u16_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u16_u32() {
        return &_oil_function_class_conv_u16_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u16_f32() {
        return &_oil_function_class_conv_u16_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u16_f64() {
        return &_oil_function_class_conv_u16_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s32_s8() {
        return &_oil_function_class_conv_s32_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s32_s16() {
        return &_oil_function_class_conv_s32_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s32_u8() {
        return &_oil_function_class_conv_s32_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s32_u16() {
        return &_oil_function_class_conv_s32_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s32_u32() {
        return &_oil_function_class_conv_s32_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s32_f32() {
        return &_oil_function_class_conv_s32_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_s32_f64() {
        return &_oil_function_class_conv_s32_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u32_s8() {
        return &_oil_function_class_conv_u32_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u32_s16() {
        return &_oil_function_class_conv_u32_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u32_u8() {
        return &_oil_function_class_conv_u32_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u32_u16() {
        return &_oil_function_class_conv_u32_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u32_s32() {
        return &_oil_function_class_conv_u32_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u32_f32() {
        return &_oil_function_class_conv_u32_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_u32_f64() {
        return &_oil_function_class_conv_u32_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f32_s8() {
        return &_oil_function_class_conv_f32_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f32_s16() {
        return &_oil_function_class_conv_f32_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f32_u8() {
        return &_oil_function_class_conv_f32_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f32_u16() {
        return &_oil_function_class_conv_f32_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f32_s32() {
        return &_oil_function_class_conv_f32_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f32_u32() {
        return &_oil_function_class_conv_f32_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f32_f64() {
        return &_oil_function_class_conv_f32_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f64_s8() {
        return &_oil_function_class_conv_f64_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f64_u8() {
        return &_oil_function_class_conv_f64_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f64_s16() {
        return &_oil_function_class_conv_f64_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f64_u16() {
        return &_oil_function_class_conv_f64_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f64_s32() {
        return &_oil_function_class_conv_f64_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f64_u32() {
        return &_oil_function_class_conv_f64_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_conv_f64_f32() {
        return &_oil_function_class_conv_f64_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s8_u8() {
        return &_oil_function_class_clipconv_s8_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s8_u16() {
        return &_oil_function_class_clipconv_s8_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s8_u32() {
        return &_oil_function_class_clipconv_s8_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u8_u32() {
        return &_oil_function_class_clipconv_u8_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u8_u16() {
        return &_oil_function_class_clipconv_u8_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s16_u16() {
        return &_oil_function_class_clipconv_s16_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s16_u32() {
        return &_oil_function_class_clipconv_s16_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s32_u32() {
        return &_oil_function_class_clipconv_s32_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u16_u32() {
        return &_oil_function_class_clipconv_u16_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s8_s16() {
        return &_oil_function_class_clipconv_s8_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s8_s32() {
        return &_oil_function_class_clipconv_s8_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u8_s16() {
        return &_oil_function_class_clipconv_u8_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u8_s32() {
        return &_oil_function_class_clipconv_u8_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s16_s32() {
        return &_oil_function_class_clipconv_s16_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u16_s32() {
        return &_oil_function_class_clipconv_u16_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u8_s8() {
        return &_oil_function_class_clipconv_u8_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u16_s16() {
        return &_oil_function_class_clipconv_u16_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u32_s32() {
        return &_oil_function_class_clipconv_u32_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s8_f32() {
        return &_oil_function_class_clipconv_s8_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s8_f64() {
        return &_oil_function_class_clipconv_s8_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u8_f32() {
        return &_oil_function_class_clipconv_u8_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u8_f64() {
        return &_oil_function_class_clipconv_u8_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s16_f32() {
        return &_oil_function_class_clipconv_s16_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s16_f64() {
        return &_oil_function_class_clipconv_s16_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u16_f32() {
        return &_oil_function_class_clipconv_u16_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u16_f64() {
        return &_oil_function_class_clipconv_u16_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s32_f32() {
        return &_oil_function_class_clipconv_s32_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_s32_f64() {
        return &_oil_function_class_clipconv_s32_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u32_f32() {
        return &_oil_function_class_clipconv_u32_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_clipconv_u32_f64() {
        return &_oil_function_class_clipconv_u32_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_s8_f32() {
        return &_oil_function_class_scaleconv_s8_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_u8_f32() {
        return &_oil_function_class_scaleconv_u8_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_s16_f32() {
        return &_oil_function_class_scaleconv_s16_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_u16_f32() {
        return &_oil_function_class_scaleconv_u16_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_s32_f32() {
        return &_oil_function_class_scaleconv_s32_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_u32_f32() {
        return &_oil_function_class_scaleconv_u32_f32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_s8_f64() {
        return &_oil_function_class_scaleconv_s8_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_u8_f64() {
        return &_oil_function_class_scaleconv_u8_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_s16_f64() {
        return &_oil_function_class_scaleconv_s16_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_u16_f64() {
        return &_oil_function_class_scaleconv_u16_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_s32_f64() {
        return &_oil_function_class_scaleconv_s32_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_u32_f64() {
        return &_oil_function_class_scaleconv_u32_f64;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_f32_s8() {
        return &_oil_function_class_scaleconv_f32_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_f32_u8() {
        return &_oil_function_class_scaleconv_f32_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_f32_s16() {
        return &_oil_function_class_scaleconv_f32_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_f32_u16() {
        return &_oil_function_class_scaleconv_f32_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_f32_s32() {
        return &_oil_function_class_scaleconv_f32_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_f32_u32() {
        return &_oil_function_class_scaleconv_f32_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_f64_s8() {
        return &_oil_function_class_scaleconv_f64_s8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_f64_u8() {
        return &_oil_function_class_scaleconv_f64_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_f64_s16() {
        return &_oil_function_class_scaleconv_f64_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_f64_u16() {
        return &_oil_function_class_scaleconv_f64_u16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_f64_s32() {
        return &_oil_function_class_scaleconv_f64_s32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_scaleconv_f64_u32() {
        return &_oil_function_class_scaleconv_f64_u32;
}
#endif



#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s8_u8_ref() {
		return &_oil_function_impl_conv_s8_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s8_s16_ref() {
        return &_oil_function_impl_conv_s8_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s8_u16_ref() {
        return &_oil_function_impl_conv_s8_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s8_s32_ref() {
        return &_oil_function_impl_conv_s8_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s8_u32_ref() {
        return &_oil_function_impl_conv_s8_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s8_f32_ref() {
        return &_oil_function_impl_conv_s8_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s8_f64_ref() {
        return &_oil_function_impl_conv_s8_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u8_s8_ref() {
        return &_oil_function_impl_conv_u8_s8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u8_s16_ref() {
        return &_oil_function_impl_conv_u8_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u8_u16_ref() {
        return &_oil_function_impl_conv_u8_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u8_s32_ref() {
        return &_oil_function_impl_conv_u8_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u8_u32_ref() {
        return &_oil_function_impl_conv_u8_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u8_f32_ref() {
        return &_oil_function_impl_conv_u8_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u8_f64_ref() {
        return &_oil_function_impl_conv_u8_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s16_s8_ref() {
        return &_oil_function_impl_conv_s16_s8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s16_u8_ref() {
        return &_oil_function_impl_conv_s16_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s16_u16_ref() {
        return &_oil_function_impl_conv_s16_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s16_s32_ref() {
        return &_oil_function_impl_conv_s16_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s16_u32_ref() {
        return &_oil_function_impl_conv_s16_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s16_f32_ref() {
        return &_oil_function_impl_conv_s16_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s16_f64_ref() {
        return &_oil_function_impl_conv_s16_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u16_s8_ref() {
        return &_oil_function_impl_conv_u16_s8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u16_u8_ref() {
        return &_oil_function_impl_conv_u16_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u16_s16_ref() {
        return &_oil_function_impl_conv_u16_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u16_s32_ref() {
        return &_oil_function_impl_conv_u16_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u16_u32_ref() {
        return &_oil_function_impl_conv_u16_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u16_f32_ref() {
        return &_oil_function_impl_conv_u16_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u16_f64_ref() {
        return &_oil_function_impl_conv_u16_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s32_s8_ref() {
        return &_oil_function_impl_conv_s32_s8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s32_s16_ref() {
        return &_oil_function_impl_conv_s32_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s32_u8_ref() {
        return &_oil_function_impl_conv_s32_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s32_u16_ref() {
        return &_oil_function_impl_conv_s32_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s32_u32_ref() {
        return &_oil_function_impl_conv_s32_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s32_f32_ref() {
        return &_oil_function_impl_conv_s32_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_s32_f64_ref() {
        return &_oil_function_impl_conv_s32_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u32_s8_ref() {
        return &_oil_function_impl_conv_u32_s8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u32_s16_ref() {
        return &_oil_function_impl_conv_u32_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u32_u8_ref() {
        return &_oil_function_impl_conv_u32_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u32_u16_ref() {
        return &_oil_function_impl_conv_u32_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u32_s32_ref() {
        return &_oil_function_impl_conv_u32_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u32_f32_ref() {
        return &_oil_function_impl_conv_u32_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_u32_f64_ref() {
        return &_oil_function_impl_conv_u32_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f32_s8_ref() {
        return &_oil_function_impl_conv_f32_s8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f32_s16_ref() {
        return &_oil_function_impl_conv_f32_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f32_u8_ref() {
        return &_oil_function_impl_conv_f32_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f32_u16_ref() {
        return &_oil_function_impl_conv_f32_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f32_s32_ref() {
        return &_oil_function_impl_conv_f32_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f32_u32_ref() {
        return &_oil_function_impl_conv_f32_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f32_f64_ref() {
        return &_oil_function_impl_conv_f32_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f64_s8_ref() {
        return &_oil_function_impl_conv_f64_s8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f64_u8_ref() {
        return &_oil_function_impl_conv_f64_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f64_s16_ref() {
        return &_oil_function_impl_conv_f64_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f64_u16_ref() {
        return &_oil_function_impl_conv_f64_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f64_s32_ref() {
        return &_oil_function_impl_conv_f64_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f64_u32_ref() {
        return &_oil_function_impl_conv_f64_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_conv_f64_f32_ref() {
        return &_oil_function_impl_conv_f64_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s8_u8_ref() {
        return &_oil_function_impl_clipconv_s8_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s8_u16_ref() {
        return &_oil_function_impl_clipconv_s8_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s8_u32_ref() {
        return &_oil_function_impl_clipconv_s8_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u8_u32_ref() {
        return &_oil_function_impl_clipconv_u8_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u8_u16_ref() {
        return &_oil_function_impl_clipconv_u8_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s16_u16_ref() {
        return &_oil_function_impl_clipconv_s16_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s16_u32_ref() {
        return &_oil_function_impl_clipconv_s16_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s32_u32_ref() {
        return &_oil_function_impl_clipconv_s32_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u16_u32_ref() {
        return &_oil_function_impl_clipconv_u16_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s8_s16_ref() {
        return &_oil_function_impl_clipconv_s8_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s8_s32_ref() {
        return &_oil_function_impl_clipconv_s8_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u8_s16_ref() {
        return &_oil_function_impl_clipconv_u8_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u8_s32_ref() {
        return &_oil_function_impl_clipconv_u8_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s16_s32_ref() {
        return &_oil_function_impl_clipconv_s16_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u16_s32_ref() {
        return &_oil_function_impl_clipconv_u16_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u8_s8_ref() {
        return &_oil_function_impl_clipconv_u8_s8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u16_s16_ref() {
        return &_oil_function_impl_clipconv_u16_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u32_s32_ref() {
        return &_oil_function_impl_clipconv_u32_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s8_f32_ref() {
        return &_oil_function_impl_clipconv_s8_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s8_f64_ref() {
        return &_oil_function_impl_clipconv_s8_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u8_f32_ref() {
        return &_oil_function_impl_clipconv_u8_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u8_f64_ref() {
        return &_oil_function_impl_clipconv_u8_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s16_f32_ref() {
        return &_oil_function_impl_clipconv_s16_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s16_f64_ref() {
        return &_oil_function_impl_clipconv_s16_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u16_f32_ref() {
        return &_oil_function_impl_clipconv_u16_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u16_f64_ref() {
        return &_oil_function_impl_clipconv_u16_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s32_f32_ref() {
        return &_oil_function_impl_clipconv_s32_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_s32_f64_ref() {
        return &_oil_function_impl_clipconv_s32_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u32_f32_ref() {
        return &_oil_function_impl_clipconv_u32_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_clipconv_u32_f64_ref() {
        return &_oil_function_impl_clipconv_u32_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_s8_f32_ref() {
        return &_oil_function_impl_scaleconv_s8_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_u8_f32_ref() {
        return &_oil_function_impl_scaleconv_u8_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_s16_f32_ref() {
        return &_oil_function_impl_scaleconv_s16_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_u16_f32_ref() {
        return &_oil_function_impl_scaleconv_u16_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_s32_f32_ref() {
        return &_oil_function_impl_scaleconv_s32_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_u32_f32_ref() {
        return &_oil_function_impl_scaleconv_u32_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_s8_f64_ref() {
        return &_oil_function_impl_scaleconv_s8_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_u8_f64_ref() {
        return &_oil_function_impl_scaleconv_u8_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_s16_f64_ref() {
        return &_oil_function_impl_scaleconv_s16_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_u16_f64_ref() {
        return &_oil_function_impl_scaleconv_u16_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_s32_f64_ref() {
        return &_oil_function_impl_scaleconv_s32_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_u32_f64_ref() {
        return &_oil_function_impl_scaleconv_u32_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_f32_s8_ref() {
        return &_oil_function_impl_scaleconv_f32_s8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_f32_u8_ref() {
        return &_oil_function_impl_scaleconv_f32_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_f32_s16_ref() {
        return &_oil_function_impl_scaleconv_f32_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_f32_u16_ref() {
        return &_oil_function_impl_scaleconv_f32_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_f32_s32_ref() {
        return &_oil_function_impl_scaleconv_f32_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_f32_u32_ref() {
        return &_oil_function_impl_scaleconv_f32_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_f64_s8_ref() {
        return &_oil_function_impl_scaleconv_f64_s8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_f64_u8_ref() {
        return &_oil_function_impl_scaleconv_f64_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_f64_s16_ref() {
        return &_oil_function_impl_scaleconv_f64_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_f64_u16_ref() {
        return &_oil_function_impl_scaleconv_f64_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_f64_s32_ref() {
        return &_oil_function_impl_scaleconv_f64_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scaleconv_f64_u32_ref() {
        return &_oil_function_impl_scaleconv_f64_u32_ref;
}
#endif



#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s8_u8 ()	{
	oil_function_class_ptr_conv_s8_u8 = __oil_function_class_conv_s8_u8();
	return &oil_function_class_ptr_conv_s8_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s8_s16 ()	{
	oil_function_class_ptr_conv_s8_s16 = __oil_function_class_conv_s8_s16();
	return &oil_function_class_ptr_conv_s8_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s8_u16 ()	{
	oil_function_class_ptr_conv_s8_u16 = __oil_function_class_conv_s8_u16();
	return &oil_function_class_ptr_conv_s8_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s8_s32 ()	{
	oil_function_class_ptr_conv_s8_s32 = __oil_function_class_conv_s8_s32();
	return &oil_function_class_ptr_conv_s8_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s8_u32 ()	{
	oil_function_class_ptr_conv_s8_u32 = __oil_function_class_conv_s8_u32();
	return &oil_function_class_ptr_conv_s8_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s8_f32 ()	{
	oil_function_class_ptr_conv_s8_f32 = __oil_function_class_conv_s8_f32();
	return &oil_function_class_ptr_conv_s8_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s8_f64 ()	{
	oil_function_class_ptr_conv_s8_f64 = __oil_function_class_conv_s8_f64();
	return &oil_function_class_ptr_conv_s8_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u8_s8 ()	{
	oil_function_class_ptr_conv_u8_s8 = __oil_function_class_conv_u8_s8();
	return &oil_function_class_ptr_conv_u8_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u8_s16 ()	{
	oil_function_class_ptr_conv_u8_s16 = __oil_function_class_conv_u8_s16();
	return &oil_function_class_ptr_conv_u8_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u8_u16 ()	{
	oil_function_class_ptr_conv_u8_u16 = __oil_function_class_conv_u8_u16();
	return &oil_function_class_ptr_conv_u8_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u8_s32 ()	{
	oil_function_class_ptr_conv_u8_s32 = __oil_function_class_conv_u8_s32();
	return &oil_function_class_ptr_conv_u8_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u8_u32 ()	{
	oil_function_class_ptr_conv_u8_u32 = __oil_function_class_conv_u8_u32();
	return &oil_function_class_ptr_conv_u8_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u8_f32 ()	{
	oil_function_class_ptr_conv_u8_f32 = __oil_function_class_conv_u8_f32();
	return &oil_function_class_ptr_conv_u8_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u8_f64 ()	{
	oil_function_class_ptr_conv_u8_f64 = __oil_function_class_conv_u8_f64();
	return &oil_function_class_ptr_conv_u8_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s16_s8 ()	{
	oil_function_class_ptr_conv_s16_s8 = __oil_function_class_conv_s16_s8();
	return &oil_function_class_ptr_conv_s16_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s16_u8 ()	{
	oil_function_class_ptr_conv_s16_u8 = __oil_function_class_conv_s16_u8();
	return &oil_function_class_ptr_conv_s16_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s16_u16 ()	{
	oil_function_class_ptr_conv_s16_u16 = __oil_function_class_conv_s16_u16();
	return &oil_function_class_ptr_conv_s16_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s16_s32 ()	{
	oil_function_class_ptr_conv_s16_s32 = __oil_function_class_conv_s16_s32();
	return &oil_function_class_ptr_conv_s16_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s16_u32 ()	{
	oil_function_class_ptr_conv_s16_u32 = __oil_function_class_conv_s16_u32();
	return &oil_function_class_ptr_conv_s16_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s16_f32 ()	{
	oil_function_class_ptr_conv_s16_f32 = __oil_function_class_conv_s16_f32();
	return &oil_function_class_ptr_conv_s16_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s16_f64 ()	{
	oil_function_class_ptr_conv_s16_f64 = __oil_function_class_conv_s16_f64();
	return &oil_function_class_ptr_conv_s16_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u16_s8 ()	{
	oil_function_class_ptr_conv_u16_s8 = __oil_function_class_conv_u16_s8();
	return &oil_function_class_ptr_conv_u16_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u16_u8 ()	{
	oil_function_class_ptr_conv_u16_u8 = __oil_function_class_conv_u16_u8();
	return &oil_function_class_ptr_conv_u16_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u16_s16 ()	{
	oil_function_class_ptr_conv_u16_s16 = __oil_function_class_conv_u16_s16();
	return &oil_function_class_ptr_conv_u16_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u16_s32 ()	{
	oil_function_class_ptr_conv_u16_s32 = __oil_function_class_conv_u16_s32();
	return &oil_function_class_ptr_conv_u16_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u16_u32 ()	{
	oil_function_class_ptr_conv_u16_u32 = __oil_function_class_conv_u16_u32();
	return &oil_function_class_ptr_conv_u16_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u16_f32 ()	{
	oil_function_class_ptr_conv_u16_f32 = __oil_function_class_conv_u16_f32();
	return &oil_function_class_ptr_conv_u16_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u16_f64 ()	{
	oil_function_class_ptr_conv_u16_f64 = __oil_function_class_conv_u16_f64();
	return &oil_function_class_ptr_conv_u16_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s32_s8 ()	{
	oil_function_class_ptr_conv_s32_s8 = __oil_function_class_conv_s32_s8();
	return &oil_function_class_ptr_conv_s32_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s32_s16 ()	{
	oil_function_class_ptr_conv_s32_s16 = __oil_function_class_conv_s32_s16();
	return &oil_function_class_ptr_conv_s32_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s32_u8 ()	{
	oil_function_class_ptr_conv_s32_u8 = __oil_function_class_conv_s32_u8();
	return &oil_function_class_ptr_conv_s32_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s32_u16 ()	{
	oil_function_class_ptr_conv_s32_u16 = __oil_function_class_conv_s32_u16();
	return &oil_function_class_ptr_conv_s32_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s32_u32 ()	{
	oil_function_class_ptr_conv_s32_u32 = __oil_function_class_conv_s32_u32();
	return &oil_function_class_ptr_conv_s32_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s32_f32 ()	{
	oil_function_class_ptr_conv_s32_f32 = __oil_function_class_conv_s32_f32();
	return &oil_function_class_ptr_conv_s32_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_s32_f64 ()	{
	oil_function_class_ptr_conv_s32_f64 = __oil_function_class_conv_s32_f64();
	return &oil_function_class_ptr_conv_s32_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u32_s8 ()	{
	oil_function_class_ptr_conv_u32_s8 = __oil_function_class_conv_u32_s8();
	return &oil_function_class_ptr_conv_u32_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u32_s16 ()	{
	oil_function_class_ptr_conv_u32_s16 = __oil_function_class_conv_u32_s16();
	return &oil_function_class_ptr_conv_u32_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u32_u8 ()	{
	oil_function_class_ptr_conv_u32_u8 = __oil_function_class_conv_u32_u8();
	return &oil_function_class_ptr_conv_u32_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u32_u16 ()	{
	oil_function_class_ptr_conv_u32_u16 = __oil_function_class_conv_u32_u16();
	return &oil_function_class_ptr_conv_u32_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u32_s32 ()	{
	oil_function_class_ptr_conv_u32_s32 = __oil_function_class_conv_u32_s32();
	return &oil_function_class_ptr_conv_u32_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u32_f32 ()	{
	oil_function_class_ptr_conv_u32_f32 = __oil_function_class_conv_u32_f32();
	return &oil_function_class_ptr_conv_u32_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_u32_f64 ()	{
	oil_function_class_ptr_conv_u32_f64 = __oil_function_class_conv_u32_f64();
	return &oil_function_class_ptr_conv_u32_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f32_s8 ()	{
	oil_function_class_ptr_conv_f32_s8 = __oil_function_class_conv_f32_s8();
	return &oil_function_class_ptr_conv_f32_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f32_s16 ()	{
	oil_function_class_ptr_conv_f32_s16 = __oil_function_class_conv_f32_s16();
	return &oil_function_class_ptr_conv_f32_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f32_u8 ()	{
	oil_function_class_ptr_conv_f32_u8 = __oil_function_class_conv_f32_u8();
	return &oil_function_class_ptr_conv_f32_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f32_u16 ()	{
	oil_function_class_ptr_conv_f32_u16 = __oil_function_class_conv_f32_u16();
	return &oil_function_class_ptr_conv_f32_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f32_s32 ()	{
	oil_function_class_ptr_conv_f32_s32 = __oil_function_class_conv_f32_s32();
	return &oil_function_class_ptr_conv_f32_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f32_u32 ()	{
	oil_function_class_ptr_conv_f32_u32 = __oil_function_class_conv_f32_u32();
	return &oil_function_class_ptr_conv_f32_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f32_f64 ()	{
	oil_function_class_ptr_conv_f32_f64 = __oil_function_class_conv_f32_f64();
	return &oil_function_class_ptr_conv_f32_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f64_s8 ()	{
	oil_function_class_ptr_conv_f64_s8 = __oil_function_class_conv_f64_s8();
	return &oil_function_class_ptr_conv_f64_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f64_u8 ()	{
	oil_function_class_ptr_conv_f64_u8 = __oil_function_class_conv_f64_u8();
	return &oil_function_class_ptr_conv_f64_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f64_s16 ()	{
	oil_function_class_ptr_conv_f64_s16 = __oil_function_class_conv_f64_s16();
	return &oil_function_class_ptr_conv_f64_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f64_u16 ()	{
	oil_function_class_ptr_conv_f64_u16 = __oil_function_class_conv_f64_u16();
	return &oil_function_class_ptr_conv_f64_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f64_s32 ()	{
	oil_function_class_ptr_conv_f64_s32 = __oil_function_class_conv_f64_s32();
	return &oil_function_class_ptr_conv_f64_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f64_u32 ()	{
	oil_function_class_ptr_conv_f64_u32 = __oil_function_class_conv_f64_u32();
	return &oil_function_class_ptr_conv_f64_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_conv_f64_f32 ()	{
	oil_function_class_ptr_conv_f64_f32 = __oil_function_class_conv_f64_f32();
	return &oil_function_class_ptr_conv_f64_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s8_u8 ()	{
	oil_function_class_ptr_clipconv_s8_u8 = __oil_function_class_clipconv_s8_u8();
	return &oil_function_class_ptr_clipconv_s8_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s8_u16 ()	{
	oil_function_class_ptr_clipconv_s8_u16 = __oil_function_class_clipconv_s8_u16();
	return &oil_function_class_ptr_clipconv_s8_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s8_u32 ()	{
	oil_function_class_ptr_clipconv_s8_u32 = __oil_function_class_clipconv_s8_u32();
	return &oil_function_class_ptr_clipconv_s8_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u8_u32 ()	{
	oil_function_class_ptr_clipconv_u8_u32 = __oil_function_class_clipconv_u8_u32();
	return &oil_function_class_ptr_clipconv_u8_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u8_u16 ()	{
	oil_function_class_ptr_clipconv_u8_u16 = __oil_function_class_clipconv_u8_u16();
	return &oil_function_class_ptr_clipconv_u8_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s16_u16 ()	{
	oil_function_class_ptr_clipconv_s16_u16 = __oil_function_class_clipconv_s16_u16();
	return &oil_function_class_ptr_clipconv_s16_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s16_u32 ()	{
	oil_function_class_ptr_clipconv_s16_u32 = __oil_function_class_clipconv_s16_u32();
	return &oil_function_class_ptr_clipconv_s16_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s32_u32 ()	{
	oil_function_class_ptr_clipconv_s32_u32 = __oil_function_class_clipconv_s32_u32();
	return &oil_function_class_ptr_clipconv_s32_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u16_u32 ()	{
	oil_function_class_ptr_clipconv_u16_u32 = __oil_function_class_clipconv_u16_u32();
	return &oil_function_class_ptr_clipconv_u16_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s8_s16 ()	{
	oil_function_class_ptr_clipconv_s8_s16 = __oil_function_class_clipconv_s8_s16();
	return &oil_function_class_ptr_clipconv_s8_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s8_s32 ()	{
	oil_function_class_ptr_clipconv_s8_s32 = __oil_function_class_clipconv_s8_s32();
	return &oil_function_class_ptr_clipconv_s8_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u8_s16 ()	{
	oil_function_class_ptr_clipconv_u8_s16 = __oil_function_class_clipconv_u8_s16();
	return &oil_function_class_ptr_clipconv_u8_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u8_s32 ()	{
	oil_function_class_ptr_clipconv_u8_s32 = __oil_function_class_clipconv_u8_s32();
	return &oil_function_class_ptr_clipconv_u8_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s16_s32 ()	{
	oil_function_class_ptr_clipconv_s16_s32 = __oil_function_class_clipconv_s16_s32();
	return &oil_function_class_ptr_clipconv_s16_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u16_s32 ()	{
	oil_function_class_ptr_clipconv_u16_s32 = __oil_function_class_clipconv_u16_s32();
	return &oil_function_class_ptr_clipconv_u16_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u8_s8 ()	{
	oil_function_class_ptr_clipconv_u8_s8 = __oil_function_class_clipconv_u8_s8();
	return &oil_function_class_ptr_clipconv_u8_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u16_s16 ()	{
	oil_function_class_ptr_clipconv_u16_s16 = __oil_function_class_clipconv_u16_s16();
	return &oil_function_class_ptr_clipconv_u16_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u32_s32 ()	{
	oil_function_class_ptr_clipconv_u32_s32 = __oil_function_class_clipconv_u32_s32();
	return &oil_function_class_ptr_clipconv_u32_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s8_f32 ()	{
	oil_function_class_ptr_clipconv_s8_f32 = __oil_function_class_clipconv_s8_f32();
	return &oil_function_class_ptr_clipconv_s8_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s8_f64 ()	{
	oil_function_class_ptr_clipconv_s8_f64 = __oil_function_class_clipconv_s8_f64();
	return &oil_function_class_ptr_clipconv_s8_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u8_f32 ()	{
	oil_function_class_ptr_clipconv_u8_f32 = __oil_function_class_clipconv_u8_f32();
	return &oil_function_class_ptr_clipconv_u8_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u8_f64 ()	{
	oil_function_class_ptr_clipconv_u8_f64 = __oil_function_class_clipconv_u8_f64();
	return &oil_function_class_ptr_clipconv_u8_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s16_f32 ()	{
	oil_function_class_ptr_clipconv_s16_f32 = __oil_function_class_clipconv_s16_f32();
	return &oil_function_class_ptr_clipconv_s16_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s16_f64 ()	{
	oil_function_class_ptr_clipconv_s16_f64 = __oil_function_class_clipconv_s16_f64();
	return &oil_function_class_ptr_clipconv_s16_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u16_f32 ()	{
	oil_function_class_ptr_clipconv_u16_f32 = __oil_function_class_clipconv_u16_f32();
	return &oil_function_class_ptr_clipconv_u16_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u16_f64 ()	{
	oil_function_class_ptr_clipconv_u16_f64 = __oil_function_class_clipconv_u16_f64();
	return &oil_function_class_ptr_clipconv_u16_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s32_f32 ()	{
	oil_function_class_ptr_clipconv_s32_f32 = __oil_function_class_clipconv_s32_f32();
	return &oil_function_class_ptr_clipconv_s32_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_s32_f64 ()	{
	oil_function_class_ptr_clipconv_s32_f64 = __oil_function_class_clipconv_s32_f64();
	return &oil_function_class_ptr_clipconv_s32_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u32_f32 ()	{
	oil_function_class_ptr_clipconv_u32_f32 = __oil_function_class_clipconv_u32_f32();
	return &oil_function_class_ptr_clipconv_u32_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_clipconv_u32_f64 ()	{
	oil_function_class_ptr_clipconv_u32_f64 = __oil_function_class_clipconv_u32_f64();
	return &oil_function_class_ptr_clipconv_u32_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_s8_f32 ()	{
	oil_function_class_ptr_scaleconv_s8_f32 = __oil_function_class_scaleconv_s8_f32();
	return &oil_function_class_ptr_scaleconv_s8_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_u8_f32 ()	{
	oil_function_class_ptr_scaleconv_u8_f32 = __oil_function_class_scaleconv_u8_f32();
	return &oil_function_class_ptr_scaleconv_u8_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_s16_f32 ()	{
	oil_function_class_ptr_scaleconv_s16_f32 = __oil_function_class_scaleconv_s16_f32();
	return &oil_function_class_ptr_scaleconv_s16_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_u16_f32 ()	{
	oil_function_class_ptr_scaleconv_u16_f32 = __oil_function_class_scaleconv_u16_f32();
	return &oil_function_class_ptr_scaleconv_u16_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_s32_f32 ()	{
	oil_function_class_ptr_scaleconv_s32_f32 = __oil_function_class_scaleconv_s32_f32();
	return &oil_function_class_ptr_scaleconv_s32_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_u32_f32 ()	{
	oil_function_class_ptr_scaleconv_u32_f32 = __oil_function_class_scaleconv_u32_f32();
	return &oil_function_class_ptr_scaleconv_u32_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_s8_f64 ()	{
	oil_function_class_ptr_scaleconv_s8_f64 = __oil_function_class_scaleconv_s8_f64();
	return &oil_function_class_ptr_scaleconv_s8_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_u8_f64 ()	{
	oil_function_class_ptr_scaleconv_u8_f64 = __oil_function_class_scaleconv_u8_f64();
	return &oil_function_class_ptr_scaleconv_u8_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_s16_f64 ()	{
	oil_function_class_ptr_scaleconv_s16_f64 = __oil_function_class_scaleconv_s16_f64();
	return &oil_function_class_ptr_scaleconv_s16_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_u16_f64 ()	{
	oil_function_class_ptr_scaleconv_u16_f64 = __oil_function_class_scaleconv_u16_f64();
	return &oil_function_class_ptr_scaleconv_u16_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_s32_f64 ()	{
	oil_function_class_ptr_scaleconv_s32_f64 = __oil_function_class_scaleconv_s32_f64();
	return &oil_function_class_ptr_scaleconv_s32_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_u32_f64 ()	{
	oil_function_class_ptr_scaleconv_u32_f64 = __oil_function_class_scaleconv_u32_f64();
	return &oil_function_class_ptr_scaleconv_u32_f64->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_f32_s8 ()	{
	oil_function_class_ptr_scaleconv_f32_s8 = __oil_function_class_scaleconv_f32_s8();
	return &oil_function_class_ptr_scaleconv_f32_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_f32_u8 ()	{
	oil_function_class_ptr_scaleconv_f32_u8 = __oil_function_class_scaleconv_f32_u8();
	return &oil_function_class_ptr_scaleconv_f32_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_f32_s16 ()	{
	oil_function_class_ptr_scaleconv_f32_s16 = __oil_function_class_scaleconv_f32_s16();
	return &oil_function_class_ptr_scaleconv_f32_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_f32_u16 ()	{
	oil_function_class_ptr_scaleconv_f32_u16 = __oil_function_class_scaleconv_f32_u16();
	return &oil_function_class_ptr_scaleconv_f32_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_f32_s32 ()	{
	oil_function_class_ptr_scaleconv_f32_s32 = __oil_function_class_scaleconv_f32_s32();
	return &oil_function_class_ptr_scaleconv_f32_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_f32_u32 ()	{
	oil_function_class_ptr_scaleconv_f32_u32 = __oil_function_class_scaleconv_f32_u32();
	return &oil_function_class_ptr_scaleconv_f32_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_f64_s8 ()	{
	oil_function_class_ptr_scaleconv_f64_s8 = __oil_function_class_scaleconv_f64_s8();
	return &oil_function_class_ptr_scaleconv_f64_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_f64_u8 ()	{
	oil_function_class_ptr_scaleconv_f64_u8 = __oil_function_class_scaleconv_f64_u8();
	return &oil_function_class_ptr_scaleconv_f64_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_f64_s16 ()	{
	oil_function_class_ptr_scaleconv_f64_s16 = __oil_function_class_scaleconv_f64_s16();
	return &oil_function_class_ptr_scaleconv_f64_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_f64_u16 ()	{
	oil_function_class_ptr_scaleconv_f64_u16 = __oil_function_class_scaleconv_f64_u16();
	return &oil_function_class_ptr_scaleconv_f64_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_f64_s32 ()	{
	oil_function_class_ptr_scaleconv_f64_s32 = __oil_function_class_scaleconv_f64_s32();
	return &oil_function_class_ptr_scaleconv_f64_s32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_scaleconv_f64_u32 ()	{
	oil_function_class_ptr_scaleconv_f64_u32 = __oil_function_class_scaleconv_f64_u32();
	return &oil_function_class_ptr_scaleconv_f64_u32->func;
	}
#endif

