/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <liboil/liboil.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>
#include <stdlib.h>

#include <liboil/globals.h>

#define LOG_FILE "c:\\logs\\testsuite_function_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define SIZE    20

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_function");
    close_log_file();
}

void abs_f32_f32_test(float * dest, int dstr, const float * src, int sstr, int n)
    {
    int i = 0;
    std_log(LOG_FILENAME_LINE,"abs_f32_f32_test is called");
    assert_failed = 0;
    
    for(i=0; i<SIZE; i++)
        dest[i] = 10;
    }

void test_oil_class_register_impl_by_name()
    {
    float output[SIZE];
    float input[SIZE];
    
    OilFunctionImpl *impl;
    
    impl = (OilFunctionImpl*)calloc(sizeof(OilFunctionImpl), 0);
	if(impl != NULL)
		{
		impl->func = (void*)abs_f32_f32_test;
		impl->name = "abs_f32_f32_test";
	    
		oil_class_register_impl_by_name("abs_f32_f32", impl);
	    
		assert_failed = 1; //will be set to 0, if abs_f32_f32_test is called
		oil_abs_f32_f32(output, 1, input, 2, SIZE);
		}
	else
		{
		std_log(LOG_FILENAME_LINE,"memory allocation failed. errno = %d", errno);
		assert_failed = 1;
		}
    }

void test_oil_class_register_impl()
    {
    float output[SIZE];
    float input[SIZE];
    
    OilFunctionClass *klass;
    OilFunctionImpl *impl;
    
	if(impl != NULL)
		{
		impl = (OilFunctionImpl*)calloc(sizeof(OilFunctionImpl), 0);
		impl->func = (void *)abs_f32_f32_test;
		impl->name = "abs_f32_f32_test";
	    
		klass = (OilFunctionClass *)oil_class_get ("abs_f32_f32");

		if(klass != NULL)
			{
			oil_class_register_impl(klass, impl);
		    
			assert_failed = 1; //will be set to 0, if abs_f32_f32_test is called
			oil_abs_f32_f32(output, 1, input, 2, SIZE);
			}
		else
			{
			std_log(LOG_FILENAME_LINE,"oil_class_get returned NULL. errno = %d", errno);
			assert_failed = 1;
			}
		}
	else
		{
		std_log(LOG_FILENAME_LINE,"memory allocation failed. errno = %d", errno);
		assert_failed = 1;
		}
    }

void test_oil_class_register_impl_full()
    {
    float output[SIZE];
    float input[SIZE];
    OilFunctionClass *klass;
    
    klass = (OilFunctionClass *)oil_class_get ("abs_f32_f32");
    
	if(klass != NULL)
		{
		oil_class_register_impl_full(klass, (void*)abs_f32_f32_test, "abs_f32_f32_test", OIL_IMPL_FLAG_OPT);
	    
		assert_failed = 1; //will be set to 0, if abs_f32_f32_test is called
		oil_abs_f32_f32(output, 1, input, 2, SIZE);
		}
	else
		{
		std_log(LOG_FILENAME_LINE,"oil_class_get returned NULL. errno = %d", errno);
		assert_failed = 1;
		}
    }

int main (int argc, char *argv[])
{
  oil_init();
  oil_init_no_optimize();
  oil_optimize("abs_f32_f32");
  
 
  test_oil_class_register_impl_by_name();
  test_oil_class_register_impl();
  test_oil_class_register_impl_full();
    
  if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Failed");
  else
      std_log(LOG_FILENAME_LINE,"Test Successful");
  create_xml(0);
  return 0;
}

