/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
//Portions Copyright (c)  2008-2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 

#include "liboil/liboilfunction.h"
#include "liboil/liboiltest.h"
#include "liboil/liboilrandom.h"

static void
shift_test (OilTest *test)
{
  int16_t *data;
  int i;
  int n;

if (0) {
  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC1);
  n = oil_test_get_arg_post_n (test, OIL_ARG_SRC1);
  for(i=0;i<n;i++){
    data[i] = CLAMP(data[i], -4096, 4095);
  }
}

  data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC2);
  //data[0] = 10000;
  data[1] = 8;
}

OIL_DEFINE_CLASS (addc_s16,
    "int16_t *d1, int16_t *s1, int16_t *s2_1, int n");
OIL_DEFINE_CLASS_FULL (addc_rshift_s16,
    "int16_t *d1, int16_t *s1, int16_t *s2_2, int n", shift_test);

void
addc_rshift_s16_ref (int16_t *d1, int16_t *s1, int16_t *s2_2, int n)
{
  int i;
  int16_t x;

  for(i=0;i<n;i++){
    x = s1[i] + s2_2[0];
    d1[i] = x>>s2_2[1];
  }
}
OIL_DEFINE_IMPL_REF (addc_rshift_s16_ref, addc_rshift_s16);

void
addc_s16_ref (int16_t *d1, uint16_t *s1, int16_t *s2_1, int n)
{
  int i;
  for(i=0;i<n;i++){
    d1[i] = s1[i] + s2_1[0];
  }
}
OIL_DEFINE_IMPL_REF (addc_s16_ref, addc_s16);



#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_addc_s16() {
		return &_oil_function_class_addc_s16;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionClass* __oil_function_class_addc_rshift_s16() {
		return &_oil_function_class_addc_rshift_s16;
}
#endif



#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_addc_rshift_s16_ref() {
		return &_oil_function_impl_addc_rshift_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_addc_s16_ref() {
		return &_oil_function_impl_addc_s16_ref;
}
#endif



#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_addc_s16 ()	{
	oil_function_class_ptr_addc_s16 = __oil_function_class_addc_s16();
	return &oil_function_class_ptr_addc_s16->func;
	}
#endif

#ifdef	__SYMBIAN32__
 
EXPORT_C void** _oil_function_class_ptr_addc_rshift_s16 ()	{
	oil_function_class_ptr_addc_rshift_s16 = __oil_function_class_addc_rshift_s16();
	return &oil_function_class_ptr_addc_rshift_s16->func;
	}
#endif

