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
#include <liboil/liboiltest.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>
#include <stdlib.h>

#include <liboil/globals.h>

#define LOG_FILE "c:\\logs\\testsuite_liboiltest_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define SIZE    20

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_liboiltest");
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

void test()
    {
    OilTest *test;
    OilParameter *p;
    int16_t *data;
    int n;
    int footer;
    int footer_increment = 10;
    OilFunctionClass *klass;
    OilFunctionImpl *impl;
    
    klass = oil_class_get("abs_f32_f32");
    
    if(klass != NULL)
        {
        test = (OilTest *)oil_test_new(klass); 
        
        if(test != NULL)
            {
            impl = (OilFunctionImpl*)calloc(sizeof(OilFunctionImpl), 0);
            impl->func = (void*)abs_f32_f32_test;
            impl->name = "abs_f32_f32_test";
            impl->klass = klass;
            
            oil_test_set_impl(test, impl);
            
            if(test->impl != impl)
                {
                std_log(LOG_FILENAME_LINE,"oil_test_set_impl failed. errno = %d", errno);
                assert_failed = 1;
                }
            
            p = &test->params[1];
            footer = p->test_footer;
            oil_test_set_test_footer(test, p, OIL_TEST_FOOTER+footer_increment);
            
            if(p->test_footer != footer+footer_increment)
                {
                std_log(LOG_FILENAME_LINE,"oil_test_set_test_footer failed. errno = %d", errno);
                assert_failed = 1;
                }
            
            data = (int16_t *)oil_test_get_source_data (test, OIL_ARG_SRC1);
            n = oil_test_get_arg_pre_n (test, OIL_ARG_SRC1);
            
            oil_test_cleanup(test);
            oil_test_free(test);
            }
        else
            {
            std_log(LOG_FILENAME_LINE,"oil_test_new returned NULL. errno = %d", errno);
            assert_failed = 1;
            }
        }
    else
        {
        std_log(LOG_FILENAME_LINE,"oil_class_get returned NULL. errno = %d", errno);
        assert_failed = 1;
        }
    }

int main (int argc, char *argv[])
{
    oil_init ();
 
    test();
    
    if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Failed");
    else
      std_log(LOG_FILENAME_LINE,"Test Successful");
    
    create_xml(0);
    return 0;
}

