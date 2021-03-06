/*
 * LIBOIL - Library of Optimized Inner Loops
 * Copyright (c) 2004 David A. Schleef <ds@schleef.org>
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

OIL_DECLARE_CLASS (copy8x8_u8);

#ifdef HAVE_UNALIGNED_ACCESS
static void
copy8x8_u8_ints (uint8_t *d1, int ds, const uint8_t *s1, int ss)
{
  int j;
  for (j=0;j<8;j++){
    ((uint32_t*)d1)[0] = ((uint32_t*)s1)[0];
    ((uint32_t*)d1)[1] = ((uint32_t*)s1)[1];

    d1+=ds;
    s1+=ss;
  }
}
OIL_DEFINE_IMPL (copy8x8_u8_ints, copy8x8_u8);
#endif

#ifdef HAVE_UNALIGNED_ACCESS
static void
copy8x8_u8_ints_unrolled (uint8_t *d1, int ds, const uint8_t *s1, int ss)
{
  ((uint32_t*)d1)[0] = ((uint32_t*)s1)[0];
  ((uint32_t*)d1)[1] = ((uint32_t*)s1)[1];
  d1+=ds; s1+=ss;
  ((uint32_t*)d1)[0] = ((uint32_t*)s1)[0];
  ((uint32_t*)d1)[1] = ((uint32_t*)s1)[1];
  d1+=ds; s1+=ss;
  ((uint32_t*)d1)[0] = ((uint32_t*)s1)[0];
  ((uint32_t*)d1)[1] = ((uint32_t*)s1)[1];
  d1+=ds; s1+=ss;
  ((uint32_t*)d1)[0] = ((uint32_t*)s1)[0];
  ((uint32_t*)d1)[1] = ((uint32_t*)s1)[1];
  d1+=ds; s1+=ss;
  ((uint32_t*)d1)[0] = ((uint32_t*)s1)[0];
  ((uint32_t*)d1)[1] = ((uint32_t*)s1)[1];
  d1+=ds; s1+=ss;
  ((uint32_t*)d1)[0] = ((uint32_t*)s1)[0];
  ((uint32_t*)d1)[1] = ((uint32_t*)s1)[1];
  d1+=ds; s1+=ss;
  ((uint32_t*)d1)[0] = ((uint32_t*)s1)[0];
  ((uint32_t*)d1)[1] = ((uint32_t*)s1)[1];
  d1+=ds; s1+=ss;
  ((uint32_t*)d1)[0] = ((uint32_t*)s1)[0];
  ((uint32_t*)d1)[1] = ((uint32_t*)s1)[1];
}
OIL_DEFINE_IMPL (copy8x8_u8_ints_unrolled, copy8x8_u8);
#endif


#ifdef HAVE_UNALIGNED_ACCESS
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_copy8x8_u8_ints() {
		return &_oil_function_impl_copy8x8_u8_ints;
}
#endif
#endif

#ifdef HAVE_UNALIGNED_ACCESS
#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_copy8x8_u8_ints_unrolled() {
		return &_oil_function_impl_copy8x8_u8_ints_unrolled;
}
#endif
#endif

