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

#define LOG_FILE "c:\\logs\\testsuite_composite_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define SIZE    20

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_composite");
    close_log_file();
}

void test_oil_composite_add_argb()
    {
    uint32_t input[SIZE];
    uint32_t output[SIZE];
    int i = 0;
    
    for(i = 0; i< SIZE; i++)
        {
        input[i] = i*10;
        output[i] = i;
        }
    
    oil_composite_add_argb(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i+input[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i+input[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_composite_add_argb_const_src()
    {
    uint32_t constant = 50;
    uint32_t output[SIZE];
    int i = 0;
    
    for(i = 0; i< SIZE; i++)
        {
        output[i] = i;
        }
    
    oil_composite_add_argb_const_src(output, &constant, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i+constant)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i+constant,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_composite_add_u8()
    {
    uint8_t input[SIZE];
    uint8_t output[SIZE];
    int i = 0;
    
    for(i = 0; i< SIZE; i++)
        {
        input[i] = i*10;
        output[i] = i;
        }
    
    oil_composite_add_u8(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i+input[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i+input[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_composite_add_u8_const_src()
    {
    uint8_t constant = 50;
    uint8_t output[SIZE];
    int i = 0;
    
    for(i = 0; i< SIZE; i++)
        {
        output[i] = i;
        }
    
    oil_composite_add_u8_const_src(output, &constant, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i+constant)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i+constant,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_composite_in_argb()
    {
    uint32_t input[SIZE];
    uint8_t mask[SIZE];
    uint32_t output[SIZE];
    int i = 0;
    uint32_t linux_output[] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,2,2,2,2,3,3};
    
    for(i = 0; i< SIZE; i++)
        {
        input[i] = i;
        mask[i] = i*2;
        }
    
    oil_composite_in_argb(output, input, mask, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_composite_in_argb_const_mask()
    {
    uint32_t input[SIZE];
    uint8_t constant = 50;
    uint32_t output[SIZE];
    uint32_t linux_output[] = {0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4,4};
    int i = 0; 
    
    for(i = 0; i < SIZE; i++)
        {
        input[i] = i;
        }
    
    oil_composite_in_argb_const_mask(output, input, &constant, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_composite_in_argb_const_src()
    {
    uint32_t constant = 50;
    uint8_t mask[SIZE];
    uint32_t output[SIZE];
    uint32_t linux_output[] = {0,0,1,1,2,2,2,3,3,4,4,4,5,5,5,6,6,7,7,7};
    int i = 0; 
    
    for(i = 0; i< SIZE; i++)
        {
        mask[i] = i*2;
        }
    
    oil_composite_in_argb_const_src(output, &constant, mask, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_composite_in_over_argb()
    {
    uint32_t input[SIZE];
    uint8_t mask[SIZE];
    uint32_t output[SIZE];
    uint32_t linux_output[] = {0,10,20,30,40,50,60,70,81,91,101,111,121,131,142,152,162,172,183,193};
    int i = 0;
    
    for(i = 0; i< SIZE; i++)
        {
        input[i] = i;
        mask[i] = i*2;
        output[i] = i*10;
        }
    
    oil_composite_in_over_argb(output, input, mask, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_composite_in_over_argb_const_mask()
    {
    uint32_t input[SIZE];
    uint8_t constant = 50;
    uint32_t output[SIZE];
    uint32_t linux_output[] = {0,10,20,31,41,51,61,71,82,92,102,112,122,133,143,153,163,173,184,194};
    int i = 0;
    
    for(i = 0; i< SIZE; i++)
        {
        input[i] = i;
        output[i] = i*10;
        }
    
    oil_composite_in_over_argb_const_mask(output, input, &constant, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_composite_in_over_argb_const_src()
    {
    //uint32_t * i_n, const uint32_t * s1_1, const uint8_t * s2_n, int n
    uint32_t constant = 50;
    uint8_t mask[SIZE];
    uint32_t output[SIZE];
    uint32_t linux_output[] = {0,10,21,31,42,52,62,73,83,94,104,114,125,135,145,156,166,177,187,197};
    int i = 0;
    
    for(i = 0; i< SIZE; i++)
        {
        mask[i] = i*2;
        output[i] = i*10;
        }
    
    oil_composite_in_over_argb_const_src(output, &constant, mask, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_composite_over_argb()
    {
    uint32_t input[SIZE];
    uint32_t output[SIZE];
    uint32_t linux_output[] = {0,11,22,33,44,55,66,77,88,99,110,121,132,143,154,165,176,187,198,209};
    int i = 0;
    
    for(i = 0; i< SIZE; i++)
        {
        input[i] = i;
        output[i] = i*10;
        }
    
    oil_composite_over_argb(output, input, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_composite_over_argb_const_src()
    {
    uint32_t constant = 50;
    uint32_t output[SIZE];
    uint32_t linux_output[] = {50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200,210,220,230,240};
    int i = 0;
    
    for(i = 0; i< SIZE; i++)
        {
        output[i] = i*10;
        }
    
    oil_composite_over_argb_const_src(output, &constant, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_composite_over_u8()
    {
    uint8_t input[SIZE];
    uint8_t output[SIZE];
    uint32_t linux_output[] = {0,11,22,33,43,54,65,75,85,96,106,116,126,136,146,156,166,176,185,195};
    int i = 0;
    
    for(i = 0; i< SIZE; i++)
        {
        input[i] = i;
        output[i] = i*10;
        }
    
    oil_composite_over_u8(output, input, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

int main (int argc, char *argv[])
    {
    oil_init();
    
    std_log(LOG_FILENAME_LINE,"START oil_composite_add_argb TEST");
    test_oil_composite_add_argb();
    std_log(LOG_FILENAME_LINE,"END TEST");
    
    std_log(LOG_FILENAME_LINE,"START oil_composite_add_argb_const_src TEST");
    test_oil_composite_add_argb_const_src();
    std_log(LOG_FILENAME_LINE,"END TEST");
    
    std_log(LOG_FILENAME_LINE,"START oil_composite_add_u8 TEST");
    test_oil_composite_add_u8();
    std_log(LOG_FILENAME_LINE,"END TEST");
    
    std_log(LOG_FILENAME_LINE,"START oil_composite_add_u8_const_src TEST");
    test_oil_composite_add_u8_const_src();
    std_log(LOG_FILENAME_LINE,"END TEST");
    
    std_log(LOG_FILENAME_LINE,"START oil_composite_in_argb TEST");
    test_oil_composite_in_argb();
    std_log(LOG_FILENAME_LINE,"END TEST");
    
    std_log(LOG_FILENAME_LINE,"START oil_composite_in_argb_const_mask TEST");
    test_oil_composite_in_argb_const_mask();
    std_log(LOG_FILENAME_LINE,"END TEST");
    
    std_log(LOG_FILENAME_LINE,"START oil_composite_in_argb_const_src TEST");
    test_oil_composite_in_argb_const_src();
    std_log(LOG_FILENAME_LINE,"END TEST");
    
    std_log(LOG_FILENAME_LINE,"START oil_composite_in_over_argb TEST");
    test_oil_composite_in_over_argb();
    std_log(LOG_FILENAME_LINE,"END TEST");
    
    std_log(LOG_FILENAME_LINE,"START oil_composite_in_over_argb_const_mask TEST");
    test_oil_composite_in_over_argb_const_mask();
    std_log(LOG_FILENAME_LINE,"END TEST");
    
    std_log(LOG_FILENAME_LINE,"START oil_composite_add_argb TEST");
    test_oil_composite_in_over_argb_const_src();
    std_log(LOG_FILENAME_LINE,"END TEST");
  
    std_log(LOG_FILENAME_LINE,"START oil_composite_over_argb TEST");
    test_oil_composite_over_argb();
    std_log(LOG_FILENAME_LINE,"END TEST");
    
    std_log(LOG_FILENAME_LINE,"START oil_composite_over_argb_const_src TEST");
    test_oil_composite_over_argb_const_src();
    std_log(LOG_FILENAME_LINE,"END TEST");
    
    std_log(LOG_FILENAME_LINE,"START oil_composite_over_u8 TEST");
    test_oil_composite_over_u8();
    std_log(LOG_FILENAME_LINE,"END TEST");
    
    if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Failed");
    else
      std_log(LOG_FILENAME_LINE,"Test Successful");
    create_xml(0);
    return 0;
    }

