/*
 * LIBOIL - Library of Optimized Inner Loops
 * Copyright (c) 2005 David A. Schleef <ds@schleef.org>
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
#include "liboil/simdpack/simdpack.h"

#ifdef __SYMBIAN32__
#ifdef __WINSCW__
#pragma warn_emptydecl off
#endif//__WINSCW__
#ifdef __ARMCC__
#pragma diag_remark 186
#endif//__ARMCC__
#endif//__SYMBIAN32__

#define oil_type_min_f32 -1.0
#define oil_type_max_f32 1.0
#define oil_type_min_f64 -1.0
#define oil_type_max_f64 1.0

#define VECTORADD_S_DEFINE_IMPL(type,bigger)		\
static void vectoradd_s_ ## type ## _ref (	\
    oil_type_ ## type *dest, int dstr,		\
    oil_type_ ## type *src1, int sstr1,		\
    oil_type_ ## type *src2, int sstr2,		\
    int n) \
{						\
  int i;					\
  for(i=0;i<n;i++) {				\
    OIL_GET(dest,i*dstr, oil_type_ ## type) = CLAMP((oil_type_ ## bigger)OIL_GET(src1,i*sstr1, oil_type_ ## type) +	\
        (oil_type_ ## bigger)OIL_GET(src2,i*sstr2, oil_type_ ## type),oil_type_min_ ## type, oil_type_max_ ## type);		\
  }						\
}						\
OIL_DEFINE_CLASS (vectoradd_s_ ## type,         \
    "oil_type_" #type " *d, int dstr, "		\
    "oil_type_" #type " *s1, int sstr1, "		\
    "oil_type_" #type " *s2, int sstr2, "		\
    "int n");	\
OIL_DEFINE_IMPL_REF (vectoradd_s_ ## type ## _ref, vectoradd_s_ ## type);


/**
 * oil_vectoradd_s_s8:
 * @d:
 * @dstr:
 * @s1:
 * @sstr1:
 * @s2:
 * @sstr2:
 * @n:
 *
 * Adds each element of @s1 to @s2 and clamps the result to the range
 * of the type and places the result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
VECTORADD_S_DEFINE_IMPL (s8,s16);

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_vectoradd_s_s8() {
        return &_oil_function_class_vectoradd_s_s8;
}
#endif
/**
 * oil_vectoradd_s_u8:
 * @d:
 * @dstr:
 * @s1:
 * @sstr1:
 * @s2:
 * @sstr2:
 * @n:
 *
 * Adds each element of @s1 to @s2 and clamps the result to the range
 * of the type and places the result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
VECTORADD_S_DEFINE_IMPL (u8,u16);

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_vectoradd_s_u8() {
        return &_oil_function_class_vectoradd_s_u8;
}
#endif

/**
 * oil_vectoradd_s_s16:
 * @d:
 * @dstr:
 * @s1:
 * @sstr1:
 * @s2:
 * @sstr2:
 * @n:
 *
 * Adds each element of @s1 to @s2 and clamps the result to the range
 * of the type and places the result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
VECTORADD_S_DEFINE_IMPL (s16,s32);

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_vectoradd_s_s16() {
        return &_oil_function_class_vectoradd_s_s16;
}
#endif
/**
 * oil_vectoradd_s_u16:
 * @d:
 * @dstr:
 * @s1:
 * @sstr1:
 * @s2:
 * @sstr2:
 * @n:
 *
 * Adds each element of @s1 to @s2 and clamps the result to the range
 * of the type and places the result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
VECTORADD_S_DEFINE_IMPL (u16,u32);

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_vectoradd_s_u16() {
        return &_oil_function_class_vectoradd_s_u16;
}
#endif

/**
 * oil_vectoradd_s_f32:
 * @d:
 * @dstr:
 * @s1:
 * @sstr1:
 * @s2:
 * @sstr2:
 * @n:
 *
 * Adds each element of @s1 to @s2 and clamps the result to the range
 * [-1,1] and places the result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
VECTORADD_S_DEFINE_IMPL (f32,f32);

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_vectoradd_s_f32() {
        return &_oil_function_class_vectoradd_s_f32;
}
#endif

/**
 * oil_vectoradd_s_f64:
 * @d:
 * @dstr:
 * @s1:
 * @sstr1:
 * @s2:
 * @sstr2:
 * @n:
 *
 * Adds each element of @s1 to @s2 and clamps the result to the range
 * [-1,1] and places the result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
VECTORADD_S_DEFINE_IMPL (f64,f64);



#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_vectoradd_s_f64() {
		return &_oil_function_class_vectoradd_s_f64;
}
#endif




#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_vectoradd_s_s8_ref() {
		return &_oil_function_impl_vectoradd_s_s8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_vectoradd_s_u8_ref() {
		return &_oil_function_impl_vectoradd_s_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_vectoradd_s_s16_ref() {
		return &_oil_function_impl_vectoradd_s_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_vectoradd_s_u16_ref() {
		return &_oil_function_impl_vectoradd_s_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_vectoradd_s_f32_ref() {
		return &_oil_function_impl_vectoradd_s_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_vectoradd_s_f64_ref() {
		return &_oil_function_impl_vectoradd_s_f64_ref;
}
#endif



#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_vectoradd_s_s8 ()	{
	oil_function_class_ptr_vectoradd_s_s8 = __oil_function_class_vectoradd_s_s8();
	return &oil_function_class_ptr_vectoradd_s_s8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_vectoradd_s_u8 ()	{
	oil_function_class_ptr_vectoradd_s_u8 = __oil_function_class_vectoradd_s_u8();
	return &oil_function_class_ptr_vectoradd_s_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_vectoradd_s_s16 ()	{
	oil_function_class_ptr_vectoradd_s_s16 = __oil_function_class_vectoradd_s_s16();
	return &oil_function_class_ptr_vectoradd_s_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_vectoradd_s_u16 ()	{
	oil_function_class_ptr_vectoradd_s_u16 = __oil_function_class_vectoradd_s_u16();
	return &oil_function_class_ptr_vectoradd_s_u16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_vectoradd_s_f32 ()	{
	oil_function_class_ptr_vectoradd_s_f32 = __oil_function_class_vectoradd_s_f32();
	return &oil_function_class_ptr_vectoradd_s_f32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_vectoradd_s_f64 ()	{
	oil_function_class_ptr_vectoradd_s_f64 = __oil_function_class_vectoradd_s_f64();
	return &oil_function_class_ptr_vectoradd_s_f64->func;
	}
#endif

