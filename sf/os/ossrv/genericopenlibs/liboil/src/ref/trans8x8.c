/*
 * LIBOIL - Library of Optimized Inner Loops
 * Copyright (c) 2003,2004 David A. Schleef <ds@schleef.org>
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
#include <math.h>

#define TRANS8x8_DEFINE_REF(type)		\
static void trans8x8_ ## type ## _ref (		\
    oil_type_ ## type *dest, int dstr,		\
    oil_type_ ## type *src, int sstr)		\
{						\
  int i,j;					\
  for(i=0;i<8;i++){				\
    for(j=0;j<8;j++){				\
      OIL_GET(dest,dstr*i+j*sizeof(oil_type_ ## type),oil_type_ ## type) = \
        OIL_GET(src,sstr*j+i*sizeof(oil_type_ ## type),oil_type_ ## type); \
    }						\
  }						\
}						\
OIL_DEFINE_CLASS (trans8x8_ ## type, "oil_type_" #type " *d_8x8, int ds,	" \
    "oil_type_" #type " *s_8x8, int ss"); \
OIL_DEFINE_IMPL_REF (trans8x8_ ## type ## _ref, trans8x8_ ## type)

TRANS8x8_DEFINE_REF(u8);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_trans8x8_u8_ref() {
		return &_oil_function_impl_trans8x8_u8_ref;
}
#endif

TRANS8x8_DEFINE_REF(u16);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_trans8x8_u16_ref() {
		return &_oil_function_impl_trans8x8_u16_ref;
}
#endif

TRANS8x8_DEFINE_REF(u32);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_trans8x8_u32_ref() {
		return &_oil_function_impl_trans8x8_u32_ref;
}
#endif

TRANS8x8_DEFINE_REF(f64);
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_trans8x8_f64_ref() {
		return &_oil_function_impl_trans8x8_f64_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_trans8x8_u8() {
        return &_oil_function_class_trans8x8_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_trans8x8_u16() {
        return &_oil_function_class_trans8x8_u16;
}
#endif


#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_trans8x8_u32() {
        return &_oil_function_class_trans8x8_u32;
}
#endif


#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_trans8x8_f64() {
        return &_oil_function_class_trans8x8_f64;
}
#endif


/**
 * oil_trans8x8_u8:
 * @d_8x8:
 * @ds:
 * @s_8x8:
 * @ss:
 *
 * Performs a matrix transpose the 8x8 block represented by @s_8x8
 * and places the
 * result in @d_8x8.  Note that this function can also be used for
 * any type of this size.
 */

/**
 * oil_trans8x8_u16:
 * @d_8x8:
 * @ds:
 * @s_8x8:
 * @ss:
 *
 * Performs a matrix transpose the 8x8 block represented by @s_8x8
 * and places the
 * result in @d_8x8.  Note that this function can also be used for
 * any type of this size.
 */

/**
 * oil_trans8x8_u32:
 * @d_8x8:
 * @ds:
 * @s_8x8:
 * @ss:
 *
 * Performs a matrix transpose the 8x8 block represented by @s_8x8
 * and places the
 * result in @d_8x8.  Note that this function can also be used for
 * any type of this size.
 */

/**
 * oil_trans8x8_f64:
 * @d_8x8:
 * @ds:
 * @s_8x8:
 * @ss:
 *
 * Performs a matrix transpose the 8x8 block represented by @s_8x8
 * and places the
 * result in @d_8x8.
 */



#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_trans8x8_u8 ()	{
	oil_function_class_ptr_trans8x8_u8 = __oil_function_class_trans8x8_u8();
	return &oil_function_class_ptr_trans8x8_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_trans8x8_u16 ()	{
	oil_function_class_ptr_trans8x8_u16 = __oil_function_class_trans8x8_u16();
	return &oil_function_class_ptr_trans8x8_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_trans8x8_u32 ()	{
	oil_function_class_ptr_trans8x8_u32 = __oil_function_class_trans8x8_u32();
	return &oil_function_class_ptr_trans8x8_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_trans8x8_f64 ()	{
	oil_function_class_ptr_trans8x8_f64 = __oil_function_class_trans8x8_f64();
	return &oil_function_class_ptr_trans8x8_f64->func;
	}
#endif

