/*
 * LIBOIL - Library of Optimized Inner Loops
 * Copyright (c) 2003 David A. Schleef <ds@schleef.org>
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
#include "liboil/dct/dct.h"

#ifdef __SYMBIAN32__
#ifdef __WINSCW__
#pragma warn_unusedarg off 
#endif//__WINSCW__
#endif//__SYMBIAN32__

/**
 * oil_dct36_f32:
 * @d_36:
 * @dstr:
 * @s_36:
 * @sstr:
 *
 * FIXME: This function is broken.
 */
OIL_DEFINE_CLASS(dct36_f32, "float *d_36, int dstr, float *s_36, int sstr");

static void
dct36_f32_ref(float *dest, int dstr, const float *src, int sstr)
{
#if 0
  int i;
  for(i=0;i<36;i++){
    dest[i] = src[i];
  }
#endif
}

OIL_DEFINE_IMPL_REF (dct36_f32_ref, dct36_f32);



#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_dct36_f32() {
		return &_oil_function_class_dct36_f32;
}
#endif



#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_dct36_f32_ref() {
		return &_oil_function_impl_dct36_f32_ref;
}
#endif



#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_dct36_f32 ()	{
	oil_function_class_ptr_dct36_f32 = __oil_function_class_dct36_f32();
	return &oil_function_class_ptr_dct36_f32->func;
	}
#endif

