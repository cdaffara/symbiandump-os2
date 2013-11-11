/*
 * Copyright (c) 2005
 *	Eric Anholt.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
//Portions Copyright (c)  2008-2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <liboil/liboilclasses.h>
#include <liboil/liboilfunction.h>

#ifdef HAVE_GCC_ASM 
#include <mmintrin.h>  
#endif

static void
copy_u8_mmx (uint8_t *dest, const uint8_t *src, int n)
{
#ifdef HAVE_GCC_ASM 
  for (; n >= 8; n -= 8) {
    *(__m64 *)dest = *(__m64 *)src;
    src += 8;
    dest += 8;
  }
  for (; n > 0; n--) {
    *dest++ = *src++;
  }
  _mm_empty();
#endif  
}
OIL_DEFINE_IMPL_FULL (copy_u8_mmx, copy_u8, OIL_IMPL_FLAG_MMX);

static void
copy_u8_mmx_unroll4 (uint8_t *dest, const uint8_t *src, int n)
{
#ifdef HAVE_GCC_ASM 
  for (; n >= 32; n -= 32) {
    ((__m64 *)dest)[0] = ((__m64 *)src)[0];
    ((__m64 *)dest)[1] = ((__m64 *)src)[1];
    ((__m64 *)dest)[2] = ((__m64 *)src)[2];
    ((__m64 *)dest)[3] = ((__m64 *)src)[3];
    src += 32;
    dest += 32;
  }
  for (; n >= 8; n -= 8) {
    *(__m64 *)dest = *(__m64 *)src;
    src += 8;
    dest += 8;
  }
  for (; n > 0; n--) {
    *dest++ = *src++;
  }
  _mm_empty();
#endif  
}
OIL_DEFINE_IMPL_FULL (copy_u8_mmx_unroll4, copy_u8, OIL_IMPL_FLAG_MMX);


#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_copy_u8_mmx() {
		return &_oil_function_impl_copy_u8_mmx;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_copy_u8_mmx_unroll4() {
		return &_oil_function_impl_copy_u8_mmx_unroll4;
}
#endif

