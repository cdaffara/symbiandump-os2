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
#include <liboil/liboiltest.h>
#include <liboil/liboilrandom.h>


static void
wavelet_test (OilTest *test)
{
  int16_t *data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC1);
  int i;

  for(i=0;i<test->n;i++){
    data[i] = oil_rand_u8();
  }
}

static void
rshift_test (OilTest *test)
{
  int16_t *data;
  int i;

  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC1);
  for(i=0;i<test->n;i++){
    data[i] = oil_rand_s16()>>1;
  }

  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC2);
  data[0] = (1<<3);
  data[1] = 4;
}

static void
lshift_test (OilTest *test)
{
  int16_t *data;

  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC2);
  data[0] = 12;
}
 
static void
combine2_test (OilTest *test)
{
  int16_t *data;

  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC3);
  data[0] = 1;
  data[1] = 1;
  data[2] = 1;
  data[3] = 1;
}

static void
combine4_test (OilTest *test)
{
  int16_t *data;

  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC5);
  data[0] = 4;
  data[1] = 4;
  data[2] = 4;
  data[3] = 4;
  data[4] = 8;
  data[5] = 4;
}

static void
add2_test (OilTest *test)
{
  int16_t *data;
  int i;

  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC1);
  for(i=0;i<test->n;i++){
    data[i] = oil_rand_s16()>>4;
  }

  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC2);
  for(i=0;i<test->n;i++){
    data[i] = oil_rand_s16()>>4;
  }

  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC3);
  for(i=0;i<test->n;i++){
    data[i] = oil_rand_s16()>>4;
  }

  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC4);
  data[0] = 1;
  data[1] = 1;
}
 
OIL_DEFINE_CLASS_FULL (deinterleave,
    "int16_t *d_2xn, int16_t *s_2xn, int n", wavelet_test);
OIL_DEFINE_CLASS (deinterleave2_s16,
    "int16_t *d1_n, int16_t *d2_n, int16_t *s_2xn, int n");
OIL_DEFINE_CLASS_FULL (interleave,
    "int16_t *d_2xn, int16_t *s_2xn, int n", wavelet_test);
OIL_DEFINE_CLASS (interleave2_s16,
    "int16_t *d_2xn, int16_t *s1_n, int16_t *s2_n, int n");
OIL_DEFINE_CLASS_FULL (synth_daub97,
    "int16_t *d_2xn, int16_t *s_2xn, int n", wavelet_test);
OIL_DEFINE_CLASS_FULL (split_daub97,
    "int16_t *d_2xn, int16_t *s_2xn, int n", wavelet_test);
OIL_DEFINE_CLASS_FULL (split_approx97,
    "int16_t *d_2xn, int16_t *s_2xn, int n", wavelet_test);
OIL_DEFINE_CLASS_FULL (synth_approx97,
    "int16_t *d_2xn, int16_t *s_2xn, int n", wavelet_test);
OIL_DEFINE_CLASS_FULL (split_53,
    "int16_t *d_2xn, int16_t *s_2xn, int n", wavelet_test);
OIL_DEFINE_CLASS_FULL (synth_53,
    "int16_t *d_2xn, int16_t *s_2xn, int n", wavelet_test);
OIL_DEFINE_CLASS_FULL (split_135,
    "int16_t *d_2xn, int16_t *s_2xn, int n", wavelet_test);
OIL_DEFINE_CLASS_FULL (synth_135,
    "int16_t *d_2xn, int16_t *s_2xn, int n", wavelet_test);
OIL_DEFINE_CLASS_FULL (add_const_rshift_s16,
    "int16_t *d1, int16_t *s1, int16_t *s2_2, int n", rshift_test);
OIL_DEFINE_CLASS_FULL (lshift_s16,
    "int16_t *d1, int16_t *s1, int16_t *s2_1, int n", lshift_test);
OIL_DEFINE_CLASS (multiply_and_add_s16,
    "int16_t *d, int16_t *src1, int16_t *src2, int16_t *src3, int n");
OIL_DEFINE_CLASS (multiply_and_add_s16_u8,
    "int16_t *d, int16_t *src1, int16_t *src2, uint8_t *src3, int n");
OIL_DEFINE_CLASS (multiply_and_acc_6xn_s16_u8, "int16_t *i1_6xn, int is1, "
    "int16_t *s1_6xn, int ss1, uint8_t *s2_6xn, int ss2, int n");
OIL_DEFINE_CLASS (multiply_and_acc_8xn_s16_u8, "int16_t *i1_8xn, int is1, "
    "int16_t *s1_8xn, int ss1, uint8_t *s2_8xn, int ss2, int n");
OIL_DEFINE_CLASS (multiply_and_acc_12xn_s16_u8, "int16_t *i1_12xn, int is1, "
    "int16_t *s1_12xn, int ss1, uint8_t *s2_12xn, int ss2, int n");
OIL_DEFINE_CLASS (multiply_and_acc_16xn_s16_u8, "int16_t *i1_16xn, int is1, "
    "int16_t *s1_16xn, int ss1, uint8_t *s2_16xn, int ss2, int n");
OIL_DEFINE_CLASS (multiply_and_acc_24xn_s16_u8, "int16_t *i1_24xn, int is1, "
    "int16_t *s1_24xn, int ss1, uint8_t *s2_24xn, int ss2, int n");
OIL_DEFINE_CLASS_FULL (combine2_8xn_u8, "uint8_t *d_8xn, int ds1, "
    "uint8_t *s1_8xn, int ss1, uint8_t *s2_8xn, int ss2, int16_t *s3_4, int n", combine2_test);
OIL_DEFINE_CLASS_FULL (combine2_12xn_u8, "uint8_t *d_12xn, int ds1, "
    "uint8_t *s1_12xn, int ss1, uint8_t *s2_12xn, int ss2, int16_t *s3_4, int n", combine2_test);
OIL_DEFINE_CLASS_FULL (combine2_16xn_u8, "uint8_t *d_16xn, int ds1, "
    "uint8_t *s1_16xn, int ss1, uint8_t *s2_16xn, int ss2, int16_t *s3_4, int n", combine2_test);
OIL_DEFINE_CLASS_FULL (combine4_8xn_u8, "uint8_t *d_8xn, int ds1, "
    "uint8_t *s1_8xn, int ss1, uint8_t *s2_8xn, int ss2, uint8_t *s3_8xn, "
    "int ss3, uint8_t *s4_8xn, int ss4, int16_t *s5_6, int n", combine4_test);
OIL_DEFINE_CLASS_FULL (combine4_12xn_u8, "uint8_t *d_12xn, int ds1, "
    "uint8_t *s1_12xn, int ss1, uint8_t *s2_12xn, int ss2, uint8_t *s3_12xn, "
    "int ss3, uint8_t *s4_12xn, int ss4, int16_t *s5_6, int n", combine4_test);
OIL_DEFINE_CLASS_FULL (combine4_16xn_u8, "uint8_t *d_16xn, int ds1, "
    "uint8_t *s1_16xn, int ss1, uint8_t *s2_16xn, int ss2, uint8_t *s3_16xn, "
    "int ss3, uint8_t *s4_16xn, int ss4, int16_t *s5_6, int n", combine4_test);
OIL_DEFINE_CLASS_FULL (add2_rshift_add_s16, "int16_t *d, int16_t *s1, "
    "int16_t *s2, int16_t *s3, int16_t *s4_2, int n", add2_test);
OIL_DEFINE_CLASS_FULL (add2_rshift_sub_s16, "int16_t *d, int16_t *s1, "
    "int16_t *s2, int16_t *s3, int16_t *s4_2, int n", add2_test);
OIL_DEFINE_CLASS (avg2_8xn_u8, "uint8_t *d_8xn, int ds1, "
    "uint8_t *s1_8xn, int ss1, uint8_t *s2_8xn, int ss2, int n");
OIL_DEFINE_CLASS (avg2_12xn_u8, "uint8_t *d_12xn, int ds1, "
    "uint8_t *s1_12xn, int ss1, uint8_t *s2_12xn, int ss2, int n");
OIL_DEFINE_CLASS (avg2_16xn_u8, "uint8_t *d_16xn, int ds1, "
    "uint8_t *s1_16xn, int ss1, uint8_t *s2_16xn, int ss2, int n");

	
#ifdef __SYMBIAN32__
 
#endif
void
deinterleave_ref (int16_t *d_2xn, int16_t *s_2xn, int n)
{
  int i;

  for(i=0;i<n;i++) {
    d_2xn[i] = s_2xn[2*i];
    d_2xn[n + i] = s_2xn[2*i + 1];
  }
}
OIL_DEFINE_IMPL_REF (deinterleave_ref, deinterleave);

#ifdef __SYMBIAN32__
 
#endif
void
deinterleave2_s16_ref (int16_t *d1_n, int16_t *d2_n, int16_t *s_2xn, int n)
{
  int i;

  for(i=0;i<n;i++) {
    d1_n[i] = s_2xn[2*i];
    d2_n[i] = s_2xn[2*i + 1];
  }
}
OIL_DEFINE_IMPL_REF (deinterleave2_s16_ref, deinterleave2_s16);

#ifdef __SYMBIAN32__
 
#endif
void
interleave_ref (int16_t *d_2xn, int16_t *s_2xn, int n)
{
  int i;

  for(i=0;i<n;i++) {
    d_2xn[2*i] = s_2xn[i];
    d_2xn[2*i + 1] = s_2xn[n + i];
  }
}
OIL_DEFINE_IMPL_REF (interleave_ref, interleave);

#ifdef __SYMBIAN32__
 
#endif
void
interleave2_s16_ref (int16_t *d_2xn, int16_t *s1_n, int16_t *s2_n, int n)
{
  int i;

  for(i=0;i<n;i++) {
    d_2xn[2*i] = s1_n[i];
    d_2xn[2*i + 1] = s2_n[i];
  }
}
OIL_DEFINE_IMPL_REF (interleave2_s16_ref, interleave2_s16);

#ifdef __SYMBIAN32__
 
#endif
void
synth_daub97_ref (int16_t *d_2xn, int16_t *s_2xn, int n)
{
  int i;

  if (n==0) return;
  /* predict */
  d_2xn[0] = s_2xn[0] - ((1817 * s_2xn[1]) >> 11);
  for(i=2;i<n*2;i+=2){
    d_2xn[i] = s_2xn[i] - ((1817 * (s_2xn[i-1] + s_2xn[i+1])) >> 12);
  }
  for(i=1;i<n*2-2;i+=2){
    d_2xn[i] = s_2xn[i] - ((3616 * (d_2xn[i-1] + d_2xn[i+1])) >> 12);
  }
  d_2xn[n*2-1] = s_2xn[n*2-1] - ((3616 * d_2xn[n*2-2]) >> 11);

  /* update */
  d_2xn[0] += (217 * d_2xn[1]) >> 11;
  for(i=2;i<n*2;i+=2){
    d_2xn[i] += (217 * (d_2xn[i-1] + d_2xn[i+1])) >> 12;
  }
  for(i=1;i<n*2-2;i+=2){
    d_2xn[i] += (6497 * (d_2xn[i-1] + d_2xn[i+1])) >> 12;
  }
  d_2xn[n*2-1] += (6497 * d_2xn[n*2-2]) >> 11;
}
OIL_DEFINE_IMPL_REF (synth_daub97_ref, synth_daub97);

#ifdef __SYMBIAN32__
 
#endif
void
split_daub97_ref (int16_t *d_2xn, int16_t *s_2xn, int n)
{
  int i;

  if (n==0) return;
  /* predict */
  for(i=1;i<n*2-2;i+=2){
    d_2xn[i] = s_2xn[i] - ((6497 * (s_2xn[i-1] + s_2xn[i+1])) >> 12);
  }
  d_2xn[n*2-1] = s_2xn[n*2-1] - ((6497 * s_2xn[n*2-2]) >> 11);
  d_2xn[0] = s_2xn[0] - ((217 * d_2xn[1]) >> 11);
  for(i=2;i<n*2;i+=2){
    d_2xn[i] = s_2xn[i] - ((217 * (d_2xn[i-1] + d_2xn[i+1])) >> 12);
  }

  /* update */
  for(i=1;i<n*2-2;i+=2){
    d_2xn[i] += (3616 * (d_2xn[i-1] + d_2xn[i+1])) >> 12;
  }
  d_2xn[n*2-1] += (3616 * d_2xn[n*2-2]) >> 11;
  d_2xn[0] += (1817 * d_2xn[1]) >> 11;
  for(i=2;i<n*2;i+=2){
    d_2xn[i] += (1817 * (d_2xn[i-1] + d_2xn[i+1])) >> 12;
  }
}
OIL_DEFINE_IMPL_REF (split_daub97_ref, split_daub97);


#ifdef __SYMBIAN32__
 
#endif
void
split_approx97_ref (int16_t *d_2xn, int16_t *s_2xn, int n)
{
  int i;

  if (n==0) return;
  if (n==1) {
    d_2xn[1] = s_2xn[1] - s_2xn[0];
    d_2xn[0] = s_2xn[0] + (d_2xn[1] >> 1);
  } else if (n==2) {
    /* predict */
    d_2xn[1] = s_2xn[1] - ((9*(s_2xn[0] + s_2xn[2]) - (s_2xn[2] + s_2xn[2])) >> 4);
    d_2xn[3] = s_2xn[3] - ((9*s_2xn[2] - s_2xn[0]) >> 3);

    /* update */
    d_2xn[0] = s_2xn[0] + (d_2xn[1] >> 1);
    d_2xn[2] = s_2xn[2] + ((d_2xn[1] + d_2xn[3]) >> 2);
  } else {
    /* predict */
    d_2xn[1] = s_2xn[1] - ((9*(s_2xn[0] + s_2xn[2]) - (s_2xn[2] + s_2xn[4])) >> 4);
    for(i=3;i<n*2-4;i+=2){
      d_2xn[i] = s_2xn[i] - ((9*(s_2xn[i-1] + s_2xn[i+1]) - (s_2xn[i-3] + s_2xn[i+3])) >> 4);
    }
    d_2xn[n*2-3] = s_2xn[n*2-3] - ((9*(s_2xn[n*2-4] + s_2xn[n*2-2]) - (s_2xn[n*2-6] + s_2xn[n*2-2])) >> 4);
    d_2xn[n*2-1] = s_2xn[n*2-1] - ((9*s_2xn[n*2-2] - s_2xn[n*2-4]) >> 3);

    /* update */
    d_2xn[0] = s_2xn[0] + (d_2xn[1] >> 1);
    for(i=2;i<n*2;i+=2){
      d_2xn[i] = s_2xn[i] + ((d_2xn[i-1] + d_2xn[i+1]) >> 2);
    }
  }

}
OIL_DEFINE_IMPL_REF (split_approx97_ref, split_approx97);

#ifdef __SYMBIAN32__
 
#endif
void
synth_approx97_ref (int16_t *d_2xn, int16_t *s_2xn, int n)
{
  int i;

  if (n==0) return;
  if (n==1) {
    d_2xn[0] = s_2xn[0] - (s_2xn[1] >> 1);
    d_2xn[1] = s_2xn[1] + d_2xn[0];
  } else if (n==2) {
    /* predict */
    d_2xn[0] = s_2xn[0] - (s_2xn[1] >> 1);
    d_2xn[2] = s_2xn[2] - ((s_2xn[1] + s_2xn[3]) >> 2);

    /* update */
    d_2xn[1] = s_2xn[1] + ((9*(d_2xn[0] + d_2xn[2]) - (d_2xn[2] + d_2xn[2])) >> 4);
    d_2xn[3] = s_2xn[3] + ((9*d_2xn[2] - d_2xn[0]) >> 3);
  } else {
    /* predict */
    d_2xn[0] = s_2xn[0] - (s_2xn[1] >> 1);
    for(i=2;i<n*2;i+=2){
      d_2xn[i] = s_2xn[i] - ((s_2xn[i-1] + s_2xn[i+1]) >> 2);
    }

    /* update */
    d_2xn[1] = s_2xn[1] + ((9*(d_2xn[0] + d_2xn[2]) - (d_2xn[2] + d_2xn[4])) >> 4);
    for(i=3;i<n*2-4;i+=2){
      d_2xn[i] = s_2xn[i] + ((9*(d_2xn[i-1] + d_2xn[i+1]) - (d_2xn[i-3] + d_2xn[i+3])) >> 4);
    }
    d_2xn[n*2-3] = s_2xn[n*2-3] + ((9*(d_2xn[n*2-4] + d_2xn[n*2-2]) - (d_2xn[n*2-6] + d_2xn[n*2-2])) >> 4);
    d_2xn[n*2-1] = s_2xn[n*2-1] + ((9*d_2xn[n*2-2] - d_2xn[n*2-4]) >> 3);
  }
}
OIL_DEFINE_IMPL_REF (synth_approx97_ref, synth_approx97);

#ifdef __SYMBIAN32__
 
#endif
void
split_53_ref (int16_t *d_2xn, int16_t *s_2xn, int n)
{
  int i;

  if (n==0) return;
  if (n == 1) {
    d_2xn[1] = s_2xn[1] - s_2xn[0];
    d_2xn[0] = s_2xn[0] + (d_2xn[1] >> 1);
  } else {
    d_2xn[1] = s_2xn[1] - ((s_2xn[0] + s_2xn[2]) >> 1);
    d_2xn[0] = s_2xn[0] + (d_2xn[1] >> 1);
    for(i=2;i<n*2-2;i+=2){
      d_2xn[i+1] = s_2xn[i+1] - ((s_2xn[i] + s_2xn[i+2]) >> 1);
      d_2xn[i] = s_2xn[i] + ((d_2xn[i-1] + d_2xn[i+1]) >> 2);
    }
    d_2xn[n*2-1] = s_2xn[n*2-1] - s_2xn[n*2-2];
    d_2xn[n*2-2] = s_2xn[n*2-2] + ((d_2xn[n*2-3] + d_2xn[n*2-1]) >> 2);
  }
}
OIL_DEFINE_IMPL_REF (split_53_ref, split_53);

#ifdef __SYMBIAN32__
 
#endif
void
synth_53_ref (int16_t *d_2xn, int16_t *s_2xn, int n)
{
  int i;

  if (n==0) return;
  if (n == 1) {
    d_2xn[0] = s_2xn[0] - (s_2xn[1] >> 1);
    d_2xn[1] = s_2xn[1] + d_2xn[0];
  } else {
    d_2xn[0] = s_2xn[0] - (s_2xn[1] >> 1);
    for(i=2;i<n*2-2;i+=2){
      d_2xn[i] = s_2xn[i] - ((s_2xn[i-1] + s_2xn[i+1]) >> 2);
      d_2xn[i-1] = s_2xn[i-1] + ((d_2xn[i] + d_2xn[i-2]) >> 1);
    }
    d_2xn[n*2-2] = s_2xn[n*2-2] - ((s_2xn[n*2-3] + s_2xn[n*2-1]) >> 2);
    d_2xn[n*2-3] = s_2xn[n*2-3] + ((d_2xn[n*2-2] + d_2xn[n*2-4]) >> 1);
    d_2xn[n*2-1] = s_2xn[n*2-1] + d_2xn[n*2-2];
  }
}
OIL_DEFINE_IMPL_REF (synth_53_ref, synth_53);


#ifdef __SYMBIAN32__
 
#endif
void
split_135_ref (int16_t *d_2xn, int16_t *s_2xn, int n)
{
  int i;

  if (n==0) return;
  if (n==1) {
    d_2xn[1] = s_2xn[1] - (s_2xn[0]);
    d_2xn[0] = s_2xn[0] + (d_2xn[1]>>1);
  } else if (n==2) {
    /* predict */
    d_2xn[1] = s_2xn[1] - ((9*(s_2xn[0] + s_2xn[2]) - (s_2xn[2] + s_2xn[2])) >> 4);
    d_2xn[3] = s_2xn[3] - ((9*s_2xn[2] - s_2xn[0]) >> 3);

    /* update */
    d_2xn[0] = s_2xn[0] + ((9*d_2xn[1] - d_2xn[3]) >> 4);
    d_2xn[2] = s_2xn[2] + ((9*(d_2xn[1] + d_2xn[3]) - (d_2xn[1] + d_2xn[1])) >> 5);
  } else {
    /* predict */
    d_2xn[1] = s_2xn[1] - ((9*(s_2xn[0] + s_2xn[2]) - (s_2xn[2] + s_2xn[4])) >> 4);
    for(i=3;i<n*2-4;i+=2){
      d_2xn[i] = s_2xn[i] - ((9*(s_2xn[i-1] + s_2xn[i+1]) - (s_2xn[i-3] + s_2xn[i+3])) >> 4);
    }
    d_2xn[n*2-3] = s_2xn[n*2-3] - ((9*(s_2xn[n*2-4] + s_2xn[n*2-2]) - (s_2xn[n*2-6] + s_2xn[n*2-2])) >> 4);
    d_2xn[n*2-1] = s_2xn[n*2-1] - ((9*s_2xn[n*2-2] - s_2xn[n*2-4]) >> 3);

    /* update */
    d_2xn[0] = s_2xn[0] + ((9*d_2xn[1] - d_2xn[3]) >> 4);
    d_2xn[2] = s_2xn[2] + ((9*(d_2xn[1] + d_2xn[3]) - (d_2xn[1] + d_2xn[5])) >> 5);
    for(i=4;i<n*2-2;i+=2){
      d_2xn[i] = s_2xn[i] + ((9*(d_2xn[i-1] + d_2xn[i+1]) - (d_2xn[i-3] + d_2xn[i+3])) >> 5);
    }
    d_2xn[n*2-2] = s_2xn[n*2-2] + ((9*(d_2xn[n*2-3] + d_2xn[n*2-1]) - (d_2xn[n*2-5] + d_2xn[n*2-1])) >> 5);
  }

}
OIL_DEFINE_IMPL_REF (split_135_ref, split_135);

#ifdef __SYMBIAN32__
 
#endif
void
synth_135_ref (int16_t *d_2xn, int16_t *s_2xn, int n)
{
  int i;

  if (n==0) return;
  if (n==1) {
    d_2xn[0] = s_2xn[0] - (s_2xn[1]>>1);
    d_2xn[1] = s_2xn[1] + (d_2xn[0]);
  } else if (n==2) {
    /* predict */
    d_2xn[0] = s_2xn[0] - ((9*s_2xn[1] - s_2xn[3]) >> 4);
    d_2xn[2] = s_2xn[2] - ((9*(s_2xn[1] + s_2xn[3]) - (s_2xn[1] + s_2xn[1])) >> 5);

    /* update */
    d_2xn[1] = s_2xn[1] + ((9*(d_2xn[0] + d_2xn[2]) - (d_2xn[2] + d_2xn[2])) >> 4);
    d_2xn[3] = s_2xn[3] + ((9*d_2xn[2] - d_2xn[0]) >> 3);
  } else {
    /* predict */
    d_2xn[0] = s_2xn[0] - ((9*s_2xn[1] - s_2xn[3]) >> 4);
    d_2xn[2] = s_2xn[2] - ((9*(s_2xn[1] + s_2xn[3]) - (s_2xn[1] + s_2xn[5])) >> 5);
    for(i=4;i<n*2-2;i+=2){
      d_2xn[i] = s_2xn[i] - ((9*(s_2xn[i-1] + s_2xn[i+1]) - (s_2xn[i-3] + s_2xn[i+3])) >> 5);
    }
    d_2xn[n*2-2] = s_2xn[n*2-2] - ((9*(s_2xn[n*2-3] + s_2xn[n*2-1]) - (s_2xn[n*2-5] + s_2xn[n*2-1])) >> 5);

    /* update */
    d_2xn[1] = s_2xn[1] + ((9*(d_2xn[0] + d_2xn[2]) - (d_2xn[2] + d_2xn[4])) >> 4);
    for(i=3;i<n*2-4;i+=2){
      d_2xn[i] = s_2xn[i] + ((9*(d_2xn[i-1] + d_2xn[i+1]) - (d_2xn[i-3] + d_2xn[i+3])) >> 4);
    }
    d_2xn[n*2-3] = s_2xn[n*2-3] + ((9*(d_2xn[n*2-4] + d_2xn[n*2-2]) - (d_2xn[n*2-6] + d_2xn[n*2-2])) >> 4);
    d_2xn[n*2-1] = s_2xn[n*2-1] + ((9*d_2xn[n*2-2] - d_2xn[n*2-4]) >> 3);
  }
}
OIL_DEFINE_IMPL_REF (synth_135_ref, synth_135);




static void
lift_test (OilTest *test)
{
  int16_t *data;
  int i;

  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC1);
  for(i=0;i<test->n;i++){
    data[i] = oil_rand_u8();
  }
  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC2);
  for(i=0;i<test->n;i++){
    data[i] = oil_rand_u8();
  }
  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC3);
  for(i=0;i<test->n;i++){
    data[i] = oil_rand_u8();
  }
}

OIL_DEFINE_CLASS_FULL (lift_add_shift1,
    "int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int n", lift_test);
OIL_DEFINE_CLASS_FULL (lift_sub_shift1,
    "int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int n", lift_test);
OIL_DEFINE_CLASS_FULL (lift_add_shift2,
    "int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int n", lift_test);
OIL_DEFINE_CLASS_FULL (lift_sub_shift2,
    "int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int n", lift_test);
OIL_DEFINE_CLASS_FULL (lift_add_mult_shift12,
    "int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int16_t *s4_1, int n", lift_test);
OIL_DEFINE_CLASS_FULL (lift_sub_mult_shift12,
    "int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int16_t *s4_1, int n", lift_test);
OIL_DEFINE_CLASS_FULL (lift_add_135,
    "int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int16_t *s4, int16_t *s5, int n", lift_test);
OIL_DEFINE_CLASS_FULL (lift_sub_135,
    "int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int16_t *s4, int16_t *s5, int n", lift_test);


	#ifdef __SYMBIAN32__
 
#endif
void
lift_add_shift1_ref (int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int n)
{
  int i;
  for(i=0;i<n;i++) {
    d[i] = s1[i] + ((s2[i] + s3[i])>>1);
  }
}
OIL_DEFINE_IMPL_REF (lift_add_shift1_ref, lift_add_shift1);

#ifdef __SYMBIAN32__
 
#endif
void
lift_sub_shift1_ref (int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int n)
{
  int i;
  for(i=0;i<n;i++) {
    d[i] = s1[i] - ((s2[i] + s3[i])>>1);
  }
}
OIL_DEFINE_IMPL_REF (lift_sub_shift1_ref, lift_sub_shift1);

#ifdef __SYMBIAN32__
 
#endif
void
lift_add_shift2_ref (int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int n)
{
  int i;
  for(i=0;i<n;i++) {
    d[i] = s1[i] + ((s2[i] + s3[i])>>2);
  }
}
OIL_DEFINE_IMPL_REF (lift_add_shift2_ref, lift_add_shift2);

#ifdef __SYMBIAN32__
 
#endif
void
lift_sub_shift2_ref (int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int n)
{
  int i;
  for(i=0;i<n;i++) {
    d[i] = s1[i] - ((s2[i] + s3[i])>>2);
  }
}
OIL_DEFINE_IMPL_REF (lift_sub_shift2_ref, lift_sub_shift2);

#ifdef __SYMBIAN32__
 
#endif
void
lift_add_mult_shift12_ref (int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int16_t *s4, int n)
{
  int i;
  for(i=0;i<n;i++) {
    d[i] = s1[i] + ((s4[0]*(s2[i] + s3[i]))>>12);
  }
}
OIL_DEFINE_IMPL_REF (lift_add_mult_shift12_ref, lift_add_mult_shift12);

#ifdef __SYMBIAN32__
 
#endif
void
lift_sub_mult_shift12_ref (int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int16_t *s4, int n)
{
  int i;
  for(i=0;i<n;i++) {
    d[i] = s1[i] - ((s4[0]*(s2[i] + s3[i]))>>12);
  }
}
OIL_DEFINE_IMPL_REF (lift_sub_mult_shift12_ref, lift_sub_mult_shift12);

#ifdef __SYMBIAN32__
 
#endif
void
lift_add_135_ref (int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3,
    int16_t *s4, int16_t *s5, int n)
{
  int i;
  for(i=0;i<n;i++) {
    d[i] = s1[i] + ((9*(s3[i-1] + s4[i+1]) - (s2[i-3] + s5[i+3])) >> 4);
  }
}
OIL_DEFINE_IMPL_REF (lift_add_135_ref, lift_add_135);

#ifdef __SYMBIAN32__
 
#endif
void
lift_sub_135_ref (int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3,
    int16_t *s4, int16_t *s5, int n)
{
  int i;
  for(i=0;i<n;i++) {
    d[i] = s1[i] - ((9*(s3[i-1] + s4[i+1]) - (s2[i-3] + s5[i+3])) >> 4);
  }
}
OIL_DEFINE_IMPL_REF (lift_sub_135_ref, lift_sub_135);


#ifdef __SYMBIAN32__
 
#endif
void
add_const_rshift_s16_ref(int16_t *d1, int16_t *s1, int16_t *s3_2, int n)
{
  int i;
  for(i=0;i<n;i++){
    d1[i] = (s1[i] + s3_2[0])>>s3_2[1];
  }
}
OIL_DEFINE_IMPL_REF (add_const_rshift_s16_ref, add_const_rshift_s16);

#ifdef __SYMBIAN32__
 
#endif
void
lshift_s16_ref(int16_t *d1, int16_t *s1, int16_t *s3_1, int n)
{
  int i;
  for(i=0;i<n;i++){
    d1[i] = s1[i]<<s3_1[0];
  }
}
OIL_DEFINE_IMPL_REF (lshift_s16_ref, lshift_s16);

#ifdef __SYMBIAN32__
 
#endif
void
multiply_and_add_s16_ref (int16_t *d, int16_t *src1, int16_t *src2, int16_t *src3, int n)
{
  int i;
  for(i=0;i<n;i++){
    d[i] = src1[i] + src2[i]*src3[i];
  }
}
OIL_DEFINE_IMPL_REF (multiply_and_add_s16_ref, multiply_and_add_s16);

#ifdef __SYMBIAN32__
 
#endif
void
multiply_and_add_s16_u8_ref (int16_t *d, int16_t *src1, int16_t *src2,
    uint8_t *src3, int n)
{
  int i;
  for(i=0;i<n;i++){
    d[i] = src1[i] + src2[i]*src3[i];
  }
}
OIL_DEFINE_IMPL_REF (multiply_and_add_s16_u8_ref, multiply_and_add_s16_u8);

#ifdef __SYMBIAN32__
 
#endif
void
multiply_and_acc_6xn_s16_u8_ref (int16_t *i1, int is1, int16_t *s1,
    int ss1, uint8_t *s2, int ss2, int n)
{
  int i, j;
  for(j=0;j<n;j++){
    for(i=0;i<6;i++){
      i1[i] += s1[i]*s2[i];
    }
    i1 = OIL_OFFSET(i1,is1);
    s1 = OIL_OFFSET(s1,ss1);
    s2 = OIL_OFFSET(s2,ss2);
  }
}
OIL_DEFINE_IMPL_REF (multiply_and_acc_6xn_s16_u8_ref,
    multiply_and_acc_6xn_s16_u8);

#ifdef __SYMBIAN32__
 
#endif
void
multiply_and_acc_8xn_s16_u8_ref (int16_t *i1, int is1, int16_t *s1,
    int ss1, uint8_t *s2, int ss2, int n)
{
  int i, j;
  for(j=0;j<n;j++){
    for(i=0;i<8;i++){
      i1[i] += s1[i]*s2[i];
    }
    i1 = OIL_OFFSET(i1,is1);
    s1 = OIL_OFFSET(s1,ss1);
    s2 = OIL_OFFSET(s2,ss2);
  }
}
OIL_DEFINE_IMPL_REF (multiply_and_acc_8xn_s16_u8_ref,
    multiply_and_acc_8xn_s16_u8);

#ifdef __SYMBIAN32__
 
#endif
void
multiply_and_acc_12xn_s16_u8_ref (int16_t *i1, int is1, int16_t *s1,
    int ss1, uint8_t *s2, int ss2, int n)
{
  int i, j;
  for(j=0;j<n;j++){
    for(i=0;i<12;i++){
      i1[i] += s1[i]*s2[i];
    }
    i1 = OIL_OFFSET(i1,is1);
    s1 = OIL_OFFSET(s1,ss1);
    s2 = OIL_OFFSET(s2,ss2);
  }
}
OIL_DEFINE_IMPL_REF (multiply_and_acc_12xn_s16_u8_ref,
    multiply_and_acc_12xn_s16_u8);

#ifdef __SYMBIAN32__
 
#endif
void
multiply_and_acc_16xn_s16_u8_ref (int16_t *i1, int is1, int16_t *s1,
    int ss1, uint8_t *s2, int ss2, int n)
{
  int i, j;
  for(j=0;j<n;j++){
    for(i=0;i<16;i++){
      i1[i] += s1[i]*s2[i];
    }
    i1 = OIL_OFFSET(i1,is1);
    s1 = OIL_OFFSET(s1,ss1);
    s2 = OIL_OFFSET(s2,ss2);
  }
}
OIL_DEFINE_IMPL_REF (multiply_and_acc_16xn_s16_u8_ref,
    multiply_and_acc_16xn_s16_u8);

	#ifdef __SYMBIAN32__
 
#endif
void
multiply_and_acc_24xn_s16_u8_ref (int16_t *i1, int is1, int16_t *s1,
    int ss1, uint8_t *s2, int ss2, int n)
{
  int i, j;
  for(j=0;j<n;j++){
    for(i=0;i<24;i++){
      i1[i] += s1[i]*s2[i];
    }
    i1 = OIL_OFFSET(i1,is1);
    s1 = OIL_OFFSET(s1,ss1);
    s2 = OIL_OFFSET(s2,ss2);
  }
}
OIL_DEFINE_IMPL_REF (multiply_and_acc_24xn_s16_u8_ref,
    multiply_and_acc_24xn_s16_u8);

	#ifdef __SYMBIAN32__
 
#endif
void
combine4_8xn_u8_ref (uint8_t *d, int ds1,
    uint8_t *s1, int ss1,
    uint8_t *s2, int ss2,
    uint8_t *s3, int ss3,
    uint8_t *s4, int ss4,
    int16_t *s5_6, int n)
{
  int i;
  int j;
  for(j=0;j<n;j++){
    for(i=0;i<8;i++){
      int x = 0;
      x += s5_6[0] * s1[i];
      x += s5_6[1] * s2[i];
      x += s5_6[2] * s3[i];
      x += s5_6[3] * s4[i];
      d[i] = (x + s5_6[4]) >> s5_6[5];
    }
    s1 += ss1;
    s2 += ss2;
    s3 += ss3;
    s4 += ss4;
    d += ds1;
  }
}
OIL_DEFINE_IMPL_REF (combine4_8xn_u8_ref, combine4_8xn_u8);

#ifdef __SYMBIAN32__
 
#endif
void
combine4_12xn_u8_ref (uint8_t *d, int ds1,
    uint8_t *s1, int ss1,
    uint8_t *s2, int ss2,
    uint8_t *s3, int ss3,
    uint8_t *s4, int ss4,
    int16_t *s5_6, int n)
{
  int i;
  int j;
  for(j=0;j<n;j++){
    for(i=0;i<12;i++){
      int x = 0;
      x += s5_6[0] * s1[i];
      x += s5_6[1] * s2[i];
      x += s5_6[2] * s3[i];
      x += s5_6[3] * s4[i];
      d[i] = (x + s5_6[4]) >> s5_6[5];
    }
    s1 += ss1;
    s2 += ss2;
    s3 += ss3;
    s4 += ss4;
    d += ds1;
  }
}
OIL_DEFINE_IMPL_REF (combine4_12xn_u8_ref, combine4_12xn_u8);

#ifdef __SYMBIAN32__
 
#endif
void
combine4_16xn_u8_ref (uint8_t *d, int ds1,
    uint8_t *s1, int ss1,
    uint8_t *s2, int ss2,
    uint8_t *s3, int ss3,
    uint8_t *s4, int ss4,
    int16_t *s5_6, int n)
{
  int i;
  int j;
  for(j=0;j<n;j++){
    for(i=0;i<16;i++){
      int x = 0;
      x += s5_6[0] * s1[i];
      x += s5_6[1] * s2[i];
      x += s5_6[2] * s3[i];
      x += s5_6[3] * s4[i];
      d[i] = (x + s5_6[4]) >> s5_6[5];
    }
    s1 += ss1;
    s2 += ss2;
    s3 += ss3;
    s4 += ss4;
    d += ds1;
  }
}
OIL_DEFINE_IMPL_REF (combine4_16xn_u8_ref, combine4_16xn_u8);

#ifdef __SYMBIAN32__
 
#endif
void
combine2_8xn_u8_ref (uint8_t *d, int ds1,
    uint8_t *s1, int ss1,
    uint8_t *s2, int ss2,
    int16_t *s3_4, int n)
{
  int i;
  int j;
  for(j=0;j<n;j++){
    for(i=0;i<8;i++){
      int x = 0;
      x += s3_4[0] * s1[i];
      x += s3_4[1] * s2[i];
      d[i] = (x + s3_4[2]) >> s3_4[3];
    }
    s1 += ss1;
    s2 += ss2;
    d += ds1;
  }
}
OIL_DEFINE_IMPL_REF (combine2_8xn_u8_ref, combine2_8xn_u8);

#ifdef __SYMBIAN32__
 
#endif
void
combine2_12xn_u8_ref (uint8_t *d, int ds1,
    uint8_t *s1, int ss1,
    uint8_t *s2, int ss2,
    int16_t *s3_4, int n)
{
  int i;
  int j;
  for(j=0;j<n;j++){
    for(i=0;i<12;i++){
      int x = 0;
      x += s3_4[0] * s1[i];
      x += s3_4[1] * s2[i];
      d[i] = (x + s3_4[2]) >> s3_4[3];
    }
    s1 += ss1;
    s2 += ss2;
    d += ds1;
  }
}
OIL_DEFINE_IMPL_REF (combine2_12xn_u8_ref, combine2_12xn_u8);

#ifdef __SYMBIAN32__
 
#endif
void
combine2_16xn_u8_ref (uint8_t *d, int ds1,
    uint8_t *s1, int ss1,
    uint8_t *s2, int ss2,
    int16_t *s3_4, int n)
{
  int i;
  int j;
  for(j=0;j<n;j++){
    for(i=0;i<16;i++){
      int x = 0;
      x += s3_4[0] * s1[i];
      x += s3_4[1] * s2[i];
      d[i] = (x + s3_4[2]) >> s3_4[3];
    }
    s1 += ss1;
    s2 += ss2;
    d += ds1;
  }
}
OIL_DEFINE_IMPL_REF (combine2_16xn_u8_ref, combine2_16xn_u8);

#ifdef __SYMBIAN32__
 
#endif
void
add2_rshift_add_s16_ref (int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3,
    int16_t *s4_2, int n)
{
  int i;
  for(i=0;i<n;i++) {
    d[i] = s1[i] + ((s2[i] + s3[i] + s4_2[0])>>s4_2[1]);
  }
}
OIL_DEFINE_IMPL_REF (add2_rshift_add_s16_ref, add2_rshift_add_s16);

#ifdef __SYMBIAN32__
 
#endif
void
add2_rshift_sub_s16_ref (int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3,
    int16_t *s4_2, int n)
{
  int i;
  for(i=0;i<n;i++) {
    d[i] = s1[i] - ((s2[i] + s3[i] + s4_2[0])>>s4_2[1]);
  }
}
OIL_DEFINE_IMPL_REF (add2_rshift_sub_s16_ref, add2_rshift_sub_s16);

#ifdef __SYMBIAN32__
 
#endif
void
avg2_8xn_u8_ref (uint8_t *d, int ds1, uint8_t *s1, int ss1,
    uint8_t *s2, int ss2, int n)
{
  int i;
  int j;
  for(j=0;j<n;j++){
    for(i=0;i<8;i++){
      d[i] = (s1[i] + s2[i] + 1)>>1;
    }
    s1 += ss1;
    s2 += ss2;
    d += ds1;
  }
}
OIL_DEFINE_IMPL_REF (avg2_8xn_u8_ref, avg2_8xn_u8);

#ifdef __SYMBIAN32__
 
#endif
void
avg2_12xn_u8_ref (uint8_t *d, int ds1, uint8_t *s1, int ss1,
    uint8_t *s2, int ss2, int n)
{
  int i;
  int j;
  for(j=0;j<n;j++){
    for(i=0;i<12;i++){
      d[i] = (s1[i] + s2[i] + 1)>>1;
    }
    s1 += ss1;
    s2 += ss2;
    d += ds1;
  }
}
OIL_DEFINE_IMPL_REF (avg2_12xn_u8_ref, avg2_12xn_u8);

#ifdef __SYMBIAN32__
 
#endif
void
avg2_16xn_u8_ref (uint8_t *d, int ds1, uint8_t *s1, int ss1,
    uint8_t *s2, int ss2, int n)
{
  int i;
  int j;
  for(j=0;j<n;j++){
    for(i=0;i<16;i++){
      d[i] = (s1[i] + s2[i] + 1)>>1;
    }
    s1 += ss1;
    s2 += ss2;
    d += ds1;
  }
}
OIL_DEFINE_IMPL_REF (avg2_16xn_u8_ref, avg2_16xn_u8);



#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_deinterleave() {
		return &_oil_function_class_deinterleave;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_deinterleave2_s16() {
		return &_oil_function_class_deinterleave2_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_interleave() {
		return &_oil_function_class_interleave;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_interleave2_s16() {
		return &_oil_function_class_interleave2_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_synth_daub97() {
		return &_oil_function_class_synth_daub97;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_split_daub97() {
		return &_oil_function_class_split_daub97;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_split_approx97() {
		return &_oil_function_class_split_approx97;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_synth_approx97() {
		return &_oil_function_class_synth_approx97;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_split_53() {
		return &_oil_function_class_split_53;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_synth_53() {
		return &_oil_function_class_synth_53;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_split_135() {
		return &_oil_function_class_split_135;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_synth_135() {
		return &_oil_function_class_synth_135;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_add_const_rshift_s16() {
		return &_oil_function_class_add_const_rshift_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_lshift_s16() {
		return &_oil_function_class_lshift_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_multiply_and_add_s16() {
		return &_oil_function_class_multiply_and_add_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_multiply_and_add_s16_u8() {
		return &_oil_function_class_multiply_and_add_s16_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_multiply_and_acc_6xn_s16_u8() {
		return &_oil_function_class_multiply_and_acc_6xn_s16_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_multiply_and_acc_8xn_s16_u8() {
		return &_oil_function_class_multiply_and_acc_8xn_s16_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_multiply_and_acc_12xn_s16_u8() {
		return &_oil_function_class_multiply_and_acc_12xn_s16_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_multiply_and_acc_16xn_s16_u8() {
		return &_oil_function_class_multiply_and_acc_16xn_s16_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_multiply_and_acc_24xn_s16_u8() {
		return &_oil_function_class_multiply_and_acc_24xn_s16_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_combine2_8xn_u8() {
		return &_oil_function_class_combine2_8xn_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_combine2_12xn_u8() {
		return &_oil_function_class_combine2_12xn_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_combine2_16xn_u8() {
		return &_oil_function_class_combine2_16xn_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_combine4_8xn_u8() {
		return &_oil_function_class_combine4_8xn_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_combine4_12xn_u8() {
		return &_oil_function_class_combine4_12xn_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_combine4_16xn_u8() {
		return &_oil_function_class_combine4_16xn_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_add2_rshift_add_s16() {
		return &_oil_function_class_add2_rshift_add_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_add2_rshift_sub_s16() {
		return &_oil_function_class_add2_rshift_sub_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_avg2_8xn_u8() {
		return &_oil_function_class_avg2_8xn_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_avg2_12xn_u8() {
		return &_oil_function_class_avg2_12xn_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_avg2_16xn_u8() {
		return &_oil_function_class_avg2_16xn_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_lift_add_shift1() {
		return &_oil_function_class_lift_add_shift1;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_lift_sub_shift1() {
		return &_oil_function_class_lift_sub_shift1;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_lift_add_shift2() {
		return &_oil_function_class_lift_add_shift2;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_lift_sub_shift2() {
		return &_oil_function_class_lift_sub_shift2;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_lift_add_mult_shift12() {
		return &_oil_function_class_lift_add_mult_shift12;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_lift_sub_mult_shift12() {
		return &_oil_function_class_lift_sub_mult_shift12;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_lift_add_135() {
		return &_oil_function_class_lift_add_135;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_lift_sub_135() {
		return &_oil_function_class_lift_sub_135;
}
#endif



#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_deinterleave_ref() {
		return &_oil_function_impl_deinterleave_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_deinterleave2_s16_ref() {
		return &_oil_function_impl_deinterleave2_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_interleave_ref() {
		return &_oil_function_impl_interleave_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_interleave2_s16_ref() {
		return &_oil_function_impl_interleave2_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_synth_daub97_ref() {
		return &_oil_function_impl_synth_daub97_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_split_daub97_ref() {
		return &_oil_function_impl_split_daub97_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_split_approx97_ref() {
		return &_oil_function_impl_split_approx97_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_synth_approx97_ref() {
		return &_oil_function_impl_synth_approx97_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_split_53_ref() {
		return &_oil_function_impl_split_53_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_synth_53_ref() {
		return &_oil_function_impl_synth_53_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_split_135_ref() {
		return &_oil_function_impl_split_135_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_synth_135_ref() {
		return &_oil_function_impl_synth_135_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_lift_add_shift1_ref() {
		return &_oil_function_impl_lift_add_shift1_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_lift_sub_shift1_ref() {
		return &_oil_function_impl_lift_sub_shift1_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_lift_add_shift2_ref() {
		return &_oil_function_impl_lift_add_shift2_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_lift_sub_shift2_ref() {
		return &_oil_function_impl_lift_sub_shift2_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_lift_add_mult_shift12_ref() {
		return &_oil_function_impl_lift_add_mult_shift12_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_lift_sub_mult_shift12_ref() {
		return &_oil_function_impl_lift_sub_mult_shift12_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_lift_add_135_ref() {
		return &_oil_function_impl_lift_add_135_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_lift_sub_135_ref() {
		return &_oil_function_impl_lift_sub_135_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_add_const_rshift_s16_ref() {
		return &_oil_function_impl_add_const_rshift_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_lshift_s16_ref() {
		return &_oil_function_impl_lshift_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_multiply_and_add_s16_ref() {
		return &_oil_function_impl_multiply_and_add_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_multiply_and_add_s16_u8_ref() {
		return &_oil_function_impl_multiply_and_add_s16_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_multiply_and_acc_6xn_s16_u8_ref() {
		return &_oil_function_impl_multiply_and_acc_6xn_s16_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_multiply_and_acc_8xn_s16_u8_ref() {
		return &_oil_function_impl_multiply_and_acc_8xn_s16_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_multiply_and_acc_12xn_s16_u8_ref() {
		return &_oil_function_impl_multiply_and_acc_12xn_s16_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_multiply_and_acc_16xn_s16_u8_ref() {
		return &_oil_function_impl_multiply_and_acc_16xn_s16_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_multiply_and_acc_24xn_s16_u8_ref() {
		return &_oil_function_impl_multiply_and_acc_24xn_s16_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_combine4_8xn_u8_ref() {
		return &_oil_function_impl_combine4_8xn_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_combine4_12xn_u8_ref() {
		return &_oil_function_impl_combine4_12xn_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_combine4_16xn_u8_ref() {
		return &_oil_function_impl_combine4_16xn_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_combine2_8xn_u8_ref() {
		return &_oil_function_impl_combine2_8xn_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_combine2_12xn_u8_ref() {
		return &_oil_function_impl_combine2_12xn_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_combine2_16xn_u8_ref() {
		return &_oil_function_impl_combine2_16xn_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_add2_rshift_add_s16_ref() {
		return &_oil_function_impl_add2_rshift_add_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_add2_rshift_sub_s16_ref() {
		return &_oil_function_impl_add2_rshift_sub_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_avg2_8xn_u8_ref() {
		return &_oil_function_impl_avg2_8xn_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_avg2_12xn_u8_ref() {
		return &_oil_function_impl_avg2_12xn_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_avg2_16xn_u8_ref() {
		return &_oil_function_impl_avg2_16xn_u8_ref;
}
#endif



#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_deinterleave ()	{
	oil_function_class_ptr_deinterleave = __oil_function_class_deinterleave();
	return &oil_function_class_ptr_deinterleave->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_deinterleave2_s16 ()	{
	oil_function_class_ptr_deinterleave2_s16 = __oil_function_class_deinterleave2_s16();
	return &oil_function_class_ptr_deinterleave2_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_interleave ()	{
	oil_function_class_ptr_interleave = __oil_function_class_interleave();
	return &oil_function_class_ptr_interleave->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_interleave2_s16 ()	{
	oil_function_class_ptr_interleave2_s16 = __oil_function_class_interleave2_s16();
	return &oil_function_class_ptr_interleave2_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_synth_daub97 ()	{
	oil_function_class_ptr_synth_daub97 = __oil_function_class_synth_daub97();
	return &oil_function_class_ptr_synth_daub97->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_split_daub97 ()	{
	oil_function_class_ptr_split_daub97 = __oil_function_class_split_daub97();
	return &oil_function_class_ptr_split_daub97->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_split_approx97 ()	{
	oil_function_class_ptr_split_approx97 = __oil_function_class_split_approx97();
	return &oil_function_class_ptr_split_approx97->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_synth_approx97 ()	{
	oil_function_class_ptr_synth_approx97 = __oil_function_class_synth_approx97();
	return &oil_function_class_ptr_synth_approx97->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_split_53 ()	{
	oil_function_class_ptr_split_53 = __oil_function_class_split_53();
	return &oil_function_class_ptr_split_53->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_synth_53 ()	{
	oil_function_class_ptr_synth_53 = __oil_function_class_synth_53();
	return &oil_function_class_ptr_synth_53->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_split_135 ()	{
	oil_function_class_ptr_split_135 = __oil_function_class_split_135();
	return &oil_function_class_ptr_split_135->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_synth_135 ()	{
	oil_function_class_ptr_synth_135 = __oil_function_class_synth_135();
	return &oil_function_class_ptr_synth_135->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_add_const_rshift_s16 ()	{
	oil_function_class_ptr_add_const_rshift_s16 = __oil_function_class_add_const_rshift_s16();
	return &oil_function_class_ptr_add_const_rshift_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_lshift_s16 ()	{
	oil_function_class_ptr_lshift_s16 = __oil_function_class_lshift_s16();
	return &oil_function_class_ptr_lshift_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_multiply_and_add_s16 ()	{
	oil_function_class_ptr_multiply_and_add_s16 = __oil_function_class_multiply_and_add_s16();
	return &oil_function_class_ptr_multiply_and_add_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_multiply_and_add_s16_u8 ()	{
	oil_function_class_ptr_multiply_and_add_s16_u8 = __oil_function_class_multiply_and_add_s16_u8();
	return &oil_function_class_ptr_multiply_and_add_s16_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_multiply_and_acc_6xn_s16_u8 ()	{
	oil_function_class_ptr_multiply_and_acc_6xn_s16_u8 = __oil_function_class_multiply_and_acc_6xn_s16_u8();
	return &oil_function_class_ptr_multiply_and_acc_6xn_s16_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_multiply_and_acc_8xn_s16_u8 ()	{
	oil_function_class_ptr_multiply_and_acc_8xn_s16_u8 = __oil_function_class_multiply_and_acc_8xn_s16_u8();
	return &oil_function_class_ptr_multiply_and_acc_8xn_s16_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_multiply_and_acc_12xn_s16_u8 ()	{
	oil_function_class_ptr_multiply_and_acc_12xn_s16_u8 = __oil_function_class_multiply_and_acc_12xn_s16_u8();
	return &oil_function_class_ptr_multiply_and_acc_12xn_s16_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_multiply_and_acc_16xn_s16_u8 ()	{
	oil_function_class_ptr_multiply_and_acc_16xn_s16_u8 = __oil_function_class_multiply_and_acc_16xn_s16_u8();
	return &oil_function_class_ptr_multiply_and_acc_16xn_s16_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_multiply_and_acc_24xn_s16_u8 ()	{
	oil_function_class_ptr_multiply_and_acc_24xn_s16_u8 = __oil_function_class_multiply_and_acc_24xn_s16_u8();
	return &oil_function_class_ptr_multiply_and_acc_24xn_s16_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_combine2_8xn_u8 ()	{
	oil_function_class_ptr_combine2_8xn_u8 = __oil_function_class_combine2_8xn_u8();
	return &oil_function_class_ptr_combine2_8xn_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_combine2_12xn_u8 ()	{
	oil_function_class_ptr_combine2_12xn_u8 = __oil_function_class_combine2_12xn_u8();
	return &oil_function_class_ptr_combine2_12xn_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_combine2_16xn_u8 ()	{
	oil_function_class_ptr_combine2_16xn_u8 = __oil_function_class_combine2_16xn_u8();
	return &oil_function_class_ptr_combine2_16xn_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_combine4_8xn_u8 ()	{
	oil_function_class_ptr_combine4_8xn_u8 = __oil_function_class_combine4_8xn_u8();
	return &oil_function_class_ptr_combine4_8xn_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_combine4_12xn_u8 ()	{
	oil_function_class_ptr_combine4_12xn_u8 = __oil_function_class_combine4_12xn_u8();
	return &oil_function_class_ptr_combine4_12xn_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_combine4_16xn_u8 ()	{
	oil_function_class_ptr_combine4_16xn_u8 = __oil_function_class_combine4_16xn_u8();
	return &oil_function_class_ptr_combine4_16xn_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_add2_rshift_add_s16 ()	{
	oil_function_class_ptr_add2_rshift_add_s16 = __oil_function_class_add2_rshift_add_s16();
	return &oil_function_class_ptr_add2_rshift_add_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_add2_rshift_sub_s16 ()	{
	oil_function_class_ptr_add2_rshift_sub_s16 = __oil_function_class_add2_rshift_sub_s16();
	return &oil_function_class_ptr_add2_rshift_sub_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_avg2_8xn_u8 ()	{
	oil_function_class_ptr_avg2_8xn_u8 = __oil_function_class_avg2_8xn_u8();
	return &oil_function_class_ptr_avg2_8xn_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_avg2_12xn_u8 ()	{
	oil_function_class_ptr_avg2_12xn_u8 = __oil_function_class_avg2_12xn_u8();
	return &oil_function_class_ptr_avg2_12xn_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_avg2_16xn_u8 ()	{
	oil_function_class_ptr_avg2_16xn_u8 = __oil_function_class_avg2_16xn_u8();
	return &oil_function_class_ptr_avg2_16xn_u8->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_lift_add_shift1 ()	{
	oil_function_class_ptr_lift_add_shift1 = __oil_function_class_lift_add_shift1();
	return &oil_function_class_ptr_lift_add_shift1->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_lift_sub_shift1 ()	{
	oil_function_class_ptr_lift_sub_shift1 = __oil_function_class_lift_sub_shift1();
	return &oil_function_class_ptr_lift_sub_shift1->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_lift_add_shift2 ()	{
	oil_function_class_ptr_lift_add_shift2 = __oil_function_class_lift_add_shift2();
	return &oil_function_class_ptr_lift_add_shift2->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_lift_sub_shift2 ()	{
	oil_function_class_ptr_lift_sub_shift2 = __oil_function_class_lift_sub_shift2();
	return &oil_function_class_ptr_lift_sub_shift2->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_lift_add_mult_shift12 ()	{
	oil_function_class_ptr_lift_add_mult_shift12 = __oil_function_class_lift_add_mult_shift12();
	return &oil_function_class_ptr_lift_add_mult_shift12->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_lift_sub_mult_shift12 ()	{
	oil_function_class_ptr_lift_sub_mult_shift12 = __oil_function_class_lift_sub_mult_shift12();
	return &oil_function_class_ptr_lift_sub_mult_shift12->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_lift_add_135 ()	{
	oil_function_class_ptr_lift_add_135 = __oil_function_class_lift_add_135();
	return &oil_function_class_ptr_lift_add_135->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_lift_sub_135 ()	{
	oil_function_class_ptr_lift_sub_135 = __oil_function_class_lift_sub_135();
	return &oil_function_class_ptr_lift_sub_135->func;
	}
#endif

