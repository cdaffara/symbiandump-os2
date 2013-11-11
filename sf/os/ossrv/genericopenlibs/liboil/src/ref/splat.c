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
#include <string.h>

/**
 * oil_splat_u8:
 * @dest:
 * @dstr:
 * @s1_1:
 * @n:
 *
 * Copies the constant source value @s1_1 to each element in @dest.
 */
OIL_DEFINE_CLASS(splat_u8,"uint8_t *dest, int dstr, uint8_t *s1_1, int n");
/**
 * oil_splat_u32:
 * @dest:
 * @dstr:
 * @s1_1:
 * @n:
 *
 * Copies the constant source value @s1_1 to each element in @dest.
 */
OIL_DEFINE_CLASS(splat_u32,"uint32_t *dest, int dstr, uint32_t *s1_1, int n");
/**
 * oil_splat_u8_ns:
 * @dest:
 * @s1_1:
 * @n:
 *
 * Copies the constant source value @s1_1 to each element in @dest.
 */
OIL_DEFINE_CLASS(splat_u8_ns,"uint8_t *dest, uint8_t *s1_1, int n");
/**
 * oil_splat_u16_ns:
 * @dest:
 * @s1_1:
 * @n:
 *
 * Copies the constant source value @s1_1 to each element in @dest.
 */
OIL_DEFINE_CLASS(splat_u16_ns,"uint16_t *dest, uint16_t *s1_1, int n");
/**
 * oil_splat_u32_ns:
 * @dest:
 * @s1_1:
 * @n:
 *
 * Copies the constant source value @s1_1 to each element in @dest.
 */
OIL_DEFINE_CLASS(splat_u32_ns,"uint32_t *dest, uint32_t *s1_1, int n");


static void splat_u8_ref (uint8_t *dest, int dstr, uint8_t *param, int n)
{
  int i;
  for(i=0;i<n;i++){
    OIL_GET(dest,i*dstr, uint8_t) = *param;
  }
}
OIL_DEFINE_IMPL_REF(splat_u8_ref, splat_u8);

static void splat_u32_ref (uint32_t *dest, int dstr, uint32_t *param, int n)
{
  int i;
  for(i=0;i<n;i++){
    OIL_GET(dest,i*dstr, uint32_t) = *param;
  }
}
OIL_DEFINE_IMPL_REF(splat_u32_ref, splat_u32);


static void splat_u8_ns_ref (uint8_t *dest, uint8_t *param, int n)
{
  int i;
  for(i=0;i<n;i++){
    dest[i] = *param;
  }
}
OIL_DEFINE_IMPL_REF(splat_u8_ns_ref, splat_u8_ns);

static void splat_u16_ns_ref (uint16_t *dest, uint16_t *param, int n)
{
  int i;
  for(i=0;i<n;i++){
    dest[i] = *param;
  }
}
OIL_DEFINE_IMPL_REF(splat_u16_ns_ref, splat_u16_ns);

static void splat_u32_ns_ref (uint32_t *dest, uint32_t *param, int n)
{
  int i;
  for(i=0;i<n;i++){
    dest[i] = *param;
  }
}
OIL_DEFINE_IMPL_REF(splat_u32_ns_ref, splat_u32_ns);





#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_splat_u8() {
		return &_oil_function_class_splat_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_splat_u32() {
		return &_oil_function_class_splat_u32;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_splat_u8_ns() {
		return &_oil_function_class_splat_u8_ns;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_splat_u16_ns() {
		return &_oil_function_class_splat_u16_ns;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_splat_u32_ns() {
		return &_oil_function_class_splat_u32_ns;
}
#endif



#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_splat_u8_ref() {
		return &_oil_function_impl_splat_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_splat_u32_ref() {
		return &_oil_function_impl_splat_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_splat_u8_ns_ref() {
		return &_oil_function_impl_splat_u8_ns_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_splat_u16_ns_ref() {
		return &_oil_function_impl_splat_u16_ns_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_splat_u32_ns_ref() {
		return &_oil_function_impl_splat_u32_ns_ref;
}
#endif



#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_splat_u8 ()	{
	oil_function_class_ptr_splat_u8 = __oil_function_class_splat_u8();
	return &oil_function_class_ptr_splat_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_splat_u32 ()	{
	oil_function_class_ptr_splat_u32 = __oil_function_class_splat_u32();
	return &oil_function_class_ptr_splat_u32->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_splat_u8_ns ()	{
	oil_function_class_ptr_splat_u8_ns = __oil_function_class_splat_u8_ns();
	return &oil_function_class_ptr_splat_u8_ns->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_splat_u16_ns ()	{
	oil_function_class_ptr_splat_u16_ns = __oil_function_class_splat_u16_ns();
	return &oil_function_class_ptr_splat_u16_ns->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_splat_u32_ns ()	{
	oil_function_class_ptr_splat_u32_ns = __oil_function_class_splat_u32_ns();
	return &oil_function_class_ptr_splat_u32_ns->func;
	}
#endif

