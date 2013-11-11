//Portions Copyright (c)  2008-2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
/*
Copyright 2002,2003,2004,2005 David A. Schleef <ds@schleef.org>
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
  
  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
#include <liboil/liboilfunction.h>
#include <liboil/liboilclasses.h>


void
multiply_and_acc_6xn_s16_u8_c (int16_t *i1, int is1, int16_t *s1,
    int ss1, uint8_t *s2, int ss2, int n)
{
  int j;
  for(j=0;j<n;j++){
    i1[0] += s1[0]*s2[0];
    i1[1] += s1[1]*s2[1];
    i1[2] += s1[2]*s2[2];
    i1[3] += s1[3]*s2[3];
    i1[4] += s1[4]*s2[4];
    i1[5] += s1[5]*s2[5];

    i1 = OIL_OFFSET(i1,is1);
    s1 = OIL_OFFSET(s1,ss1);
    s2 = OIL_OFFSET(s2,ss2);
  }
}
OIL_DEFINE_IMPL (multiply_and_acc_6xn_s16_u8_c,
    multiply_and_acc_6xn_s16_u8);



#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_multiply_and_acc_6xn_s16_u8_c() {
		return &_oil_function_impl_multiply_and_acc_6xn_s16_u8_c;
}
#endif

