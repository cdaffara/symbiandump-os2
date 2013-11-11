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

#define LOG_FILE "c:\\logs\\testsuite_wavelet_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define SIZE    20

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_wavelet");
    close_log_file();
}

void test_oil_add2_rshift_add_s16()
    {
    //int16_t * d, const int16_t * s1, const int16_t * s2, const int16_t * s3, const int16_t * s4_2, int n
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t input3[SIZE];
    int16_t input4[2];
    int16_t output[SIZE];
    int16_t linux_output[] = {0,1,2,3,5,6,7,9,10,11,13,14,15,17,18,19,21,22,23,24};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i;
        input2[i] = i*2;
        input3[i] = i*3;
        if(i < 2)
            input4[i] = i*4;
        }
    
    oil_add2_rshift_add_s16(output, input1, input2, input3, input4, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_add2_rshift_sub_s16()
    {
    //int16_t * d, const int16_t * s1, const int16_t * s2, const int16_t * s3, const int16_t * s4_2, int n
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t input3[SIZE];
    int16_t input4[2];
    int16_t output[SIZE];
    int16_t linux_output[] = {0,1,2,3,3,4,5,5,6,7,7,8,9,9,10,11,11,12,13,14};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i;
        input2[i] = i*2;
        input3[i] = i*3;
        if(i < 2)
            input4[i] = i*4;
        }
    
    oil_add2_rshift_sub_s16(output, input1, input2, input3, input4, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_add_const_rshift_s16()
    {
    //int16_t * d1, const int16_t * s1, const int16_t * s2_2, int n
    int16_t input1[SIZE];
    int16_t input2[2];
    int16_t output[SIZE];
    int16_t linux_output[] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i;
        if(i < 2)
            input2[i] = i*2;
        }
    
    oil_add_const_rshift_s16(output, input1, input2, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_avg2_12xn_u8()
    {
    //uint8_t * d_12xn, int ds1, const uint8_t * s1_12xn, int ss1, const uint8_t * s2_12xn, int ss2, int n
    uint8_t input1[SIZE*12];
    uint8_t input2[SIZE*12];
    uint8_t output[SIZE*12];
    int16_t linux_output[] = {0,2,6,7,11,13,17,18,22,24,28,29,33,35,39,40,44,46,50,51,55,57,61,62,66,68,72,73,77,79,83,84,88,90,94,95,99,101,105,106,108,109,111,112,114,115,117,118,120,121,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<SIZE*12; i++)
        {
        output[i] = 0;
        input1[i] = i;
        input2[i] = i*2;
        }
    
    oil_avg2_12xn_u8(output, 2, input1, 3, input2, 4, SIZE);
    
    for(i = 0; i < SIZE*12; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_avg2_16xn_u8()
    {
    //uint8_t * d_16xn, int ds1, const uint8_t * s1_16xn, int ss1, const uint8_t * s2_16xn, int ss2, int n
    uint8_t input1[SIZE*16];
    uint8_t input2[SIZE*16];
    uint8_t output[SIZE*16];
    int16_t linux_output[] = {0,2,6,7,11,13,17,18,22,24,28,29,33,35,39,40,44,46,50,51,55,57,61,62,66,68,72,73,77,79,83,84,88,90,94,95,99,101,105,106,108,109,111,112,114,115,117,118,120,121,123,124,126,127,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<SIZE*16; i++)
        {
        output[i] = 0;
        input1[i] = i;
        input2[i] = i*2;
        }
    
    oil_avg2_16xn_u8(output, 2, input1, 3, input2, 4, SIZE);
    
    for(i = 0; i < SIZE*16; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_avg2_8xn_u8()
    {
    //uint8_t * d_8xn, int ds1, const uint8_t * s1_8xn, int ss1, const uint8_t * s2_8xn, int ss2, int n
    uint8_t input1[SIZE*8];
    uint8_t input2[SIZE*8];
    uint8_t output[SIZE*8];
    int16_t linux_output[] = {0,2,6,7,11,13,17,18,22,24,28,29,33,35,39,40,44,46,50,51,55,57,61,62,66,68,72,73,77,79,83,84,88,90,94,95,99,101,105,106,108,109,111,112,114,115,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<SIZE*8; i++)
        {
        output[i] = 0;
        input1[i] = i;
        input2[i] = i*2;
        }
    
    oil_avg2_8xn_u8(output, 2, input1, 3, input2, 4, SIZE);
    
    for(i = 0; i < SIZE*8; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_combine2_12xn_u8()
    {
    //uint8_t * d_12xn, int ds1, const uint8_t * s1_12xn, int ss1, const uint8_t * s2_12xn, int ss2, const int16_t * s3_4, int n
    uint8_t input1[SIZE*12];
    uint8_t input2[SIZE*12];
    int16_t input3[4];
    uint8_t output[SIZE*12];
    int16_t linux_output[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<SIZE*12; i++)
        {
        output[i] = 0;
        input1[i] = i;
        input2[i] = i*2;
        
        if(i < 4)
            input3[i] = i*3;
        }
    
    oil_combine2_12xn_u8(output, 2, input1, 3, input2, 4, input3, SIZE);
    
    for(i = 0; i < SIZE*12; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_combine2_16xn_u8()
    {
    //uint8_t * d_16xn, int ds1, const uint8_t * s1_16xn, int ss1, const uint8_t * s2_16xn, int ss2, const int16_t * s3_4, int n
    uint8_t input1[SIZE*16];
    uint8_t input2[SIZE*16];
    int16_t input3[4];
    uint8_t output[SIZE*16];
    int16_t linux_output[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    int i = 0;
    
    for(i=0; i<SIZE*16; i++)
        {
        output[i] = 1;
        input1[i] = i;
        input2[i] = i*2;
        
        if(i < 4)
            input3[i] = i*3;
        }
    
    oil_combine2_16xn_u8(output, 2, input1, 3, input2, 4, input3, SIZE);
    
    for(i = 0; i < SIZE*16; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_combine2_8xn_u8()
    {
    //uint8_t * d_8xn, int ds1, const uint8_t * s1_8xn, int ss1, const uint8_t * s2_8xn, int ss2, const int16_t * s3_4, int n
    uint8_t input1[SIZE*8];
    uint8_t input2[SIZE*8];
    int16_t input3[4];
    uint8_t output[SIZE*8];
    int16_t linux_output[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    int i = 0;
    
    for(i=0; i<SIZE*8; i++)
        {
        output[i] = 2;
        input1[i] = i;
        input2[i] = i*2;
        
        if(i < 4)
            input3[i] = i*3;
        }
    
    oil_combine2_8xn_u8(output, 2, input1, 3, input2, 4, input3, SIZE);
    
    for(i = 0; i < SIZE*8; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_combine4_12xn_u8()
    {
    //uint8_t * d_12xn, int ds1, const uint8_t * s1_12xn, int ss1, const uint8_t * s2_12xn, int ss2, const uint8_t * s3_12xn, int ss3, const uint8_t * s4_12xn, int ss4, const int16_t * s5_6, int n
    uint8_t input1[SIZE*12];
    uint8_t input2[SIZE*12];
    uint8_t input3[SIZE*12];
    uint8_t input4[SIZE*12];
    int16_t input5[6];
    uint8_t output[SIZE*12];
 	uint8_t expected_output[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	   
    int i = 0;
    
    for(i=0; i<SIZE*12; i++)
        {
        output[i] = 1;
        input1[i] = i;
        input2[i] = i+2;
        input3[i] = i+3;
        input4[i] = i+4;
        
        if(i < 6)
            input5[i] = i+5;
        }
    
    oil_combine4_12xn_u8(output, 1, input1, 1, input2, 2, input3, 3, input4, 4, input5, SIZE);
    
    for(i = 0; i < SIZE*12; i++)
        {
        if(output[i] != expected_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,expected_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_combine4_16xn_u8()
    {
    //uint8_t * d_16xn, int ds1, const uint8_t * s1_16xn, int ss1, const uint8_t * s2_16xn, int ss2, const uint8_t * s3_16xn, int ss3, const uint8_t * s4_16xn, int ss4, const int16_t * s5_6, int n
    uint8_t input1[SIZE*16];
    uint8_t input2[SIZE*16];
    uint8_t input3[SIZE*16];
    uint8_t input4[SIZE*16];
    int16_t input5[6];
    uint8_t output[SIZE*16];
    uint8_t expected_output[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    int i = 0;
    
    for(i=0; i<SIZE*16; i++)
        {
        output[i] = 1;
        input1[i] = i;
        input2[i] = i+2;
        input3[i] = i+3;
        input4[i] = i+4;
        
        if(i < 6)
            input5[i] = i+5;
        }
    
    oil_combine4_16xn_u8(output, 1, input1, 1, input2, 2, input3, 3, input4, 4, input5, SIZE);
    
    for(i = 0; i < SIZE*16; i++)
        {
        if(output[i] != expected_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,expected_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_combine4_8xn_u8()
    {
    //uint8_t * d_8xn, int ds1, const uint8_t * s1_8xn, int ss1, const uint8_t * s2_8xn, int ss2, const uint8_t * s3_8xn, int ss3, const uint8_t * s4_8xn, int ss4, const int16_t * s5_6, int n
    uint8_t input1[SIZE*8];
    uint8_t input2[SIZE*8];
    uint8_t input3[SIZE*8];
    uint8_t input4[SIZE*8];
    int16_t input5[6];
    uint8_t output[SIZE*8];
    uint8_t expected_output[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    int i = 0;
    
    for(i=0; i<SIZE*8; i++)
        {
        output[i] = 1;
        input1[i] = i;
        input2[i] = i+2;
        input3[i] = i+3;
        input4[i] = i+4;
        
        if(i < 6)
            input5[i] = i+5;
        }
    
    oil_combine4_8xn_u8(output, 1, input1, 1, input2, 2, input3, 3, input4, 4, input5, SIZE);
    
    for(i = 0; i < SIZE*8; i++)
        {
        if(output[i] != expected_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,expected_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_deinterleave()
    {
    //int16_t * d_2xn, const int16_t * s_2xn, int n
    int16_t input[SIZE*2];
    int16_t output[SIZE*2];
    int16_t linux_output[] = {0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39};
    int i = 0;
    
    for(i=0; i<SIZE*2; i++)
        {
        output[i] = 0;
        input[i] = i;
        }
    
    oil_deinterleave(output, input, SIZE);
    
    for(i = 0; i < SIZE*2; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_deinterleave2_s16()
    {
    //int16_t * d1_n, int16_t * d2_n, const int16_t * s_2xn, int n
    int16_t input[SIZE*2];
    int16_t output1[SIZE];
    int16_t output2[SIZE];
    int16_t linux_output1[] = {0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38};
    int16_t linux_output2[] = {1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39};
    int i = 0;
    
    for(i=0; i<SIZE*2; i++)
        {
        if(i < SIZE)
            {
            output1[i] = i+1;
            output2[i] = i+2;
            }
        input[i] = i;
        }
    
    oil_deinterleave2_s16(output1, output2, input, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output1[i] != linux_output1[i])
            {
            std_log(LOG_FILENAME_LINE, "output1[%d]: expected value - %d, actual value - %d", i,linux_output1[i],output1[i]);
            assert_failed = 1;
            }
        
        if(output2[i] != linux_output2[i])
            {
            std_log(LOG_FILENAME_LINE, "output2[%d]: expected value - %d, actual value - %d", i,linux_output2[i],output2[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_interleave()
    {
    //int16_t * d_2xn, const int16_t * s_2xn, int n
    int16_t input[SIZE*2];
    int16_t output[SIZE*2];
    int16_t linux_output[] = {0,20,1,21,2,22,3,23,4,24,5,25,6,26,7,27,8,28,9,29,10,30,11,31,12,32,13,33,14,34,15,35,16,36,17,37,18,38,19,39};
    int i = 0;
    
    for(i=0; i<SIZE*2; i++)
        {
        output[i] = 0;
        input[i] = i;
        }
    
    oil_interleave(output, input, SIZE);
    
    for(i = 0; i < SIZE*2; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_interleave2_s16()
    {
    //int16_t * d_2xn, const int16_t * s1_n, const int16_t * s2_n, int n
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t output[SIZE*2];
    int16_t linux_output[] = {1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11};
    int i = 0;
    
    for(i=0; i<SIZE*2; i++)
        {
        if(i < SIZE)
            {
            input1[i] = i+1;
            input2[i] = i+2;
            }
        output[i] = 0;
        }
    
    oil_interleave2_s16(output, input1, input2, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_lift_add_135()
    {
    //int16_t * d, const int16_t * s1, const int16_t * s2, const int16_t * s3, const int16_t * s4, const int16_t * s5, int n
    int16_t output[SIZE];
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t input3[SIZE];
    int16_t input4[SIZE];
    int16_t input5[SIZE];
    int16_t linux_output[] = {15,5,7,10,12,14,16,18,20,22,24,26,28,30,32,34,36,39,41,32};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
        input3[i] = i+3;
        input4[i] = i+4;
        input5[i] = i+5;

        output[i] = 0;
        }
    
    oil_lift_add_135(output, input1, input2, input3, input4, input5, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_lift_add_mult_shift12()
    {
    //int16_t * d, const int16_t * s1, const int16_t * s2, const int16_t * s3, const int16_t * s4_1, int n
    int16_t output[SIZE];
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t input3[SIZE];
    int16_t input4[1];
    int16_t linux_output[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    int i = 0;
    
    input4[0] = 4;
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
        input3[i] = i+3;
        
        output[i] = 0;
        }
    
    oil_lift_add_mult_shift12(output, input1, input2, input3, input4, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_lift_add_shift1()
    {
    //int16_t * d, const int16_t * s1, const int16_t * s2, const int16_t * s3, int n
    int16_t output[SIZE];
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t input3[SIZE];
    int16_t linux_output[] = {3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39,41};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
        input3[i] = i+3;
        
        output[i] = 0;
        }
    
    oil_lift_add_shift1(output, input1, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_lift_add_shift2()
    {
    //int16_t * d, const int16_t * s1, const int16_t * s2, const int16_t * s3, int n
    int16_t output[SIZE];
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t input3[SIZE];
    int16_t linux_output[] = {2,3,5,6,8,9,11,12,14,15,17,18,20,21,23,24,26,27,29,30};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
        input3[i] = i+3;
        
        output[i] = 0;
        }
    
    oil_lift_add_shift2(output, input1, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_lift_sub_135()
    {
    //int16_t * d, const int16_t * s1, const int16_t * s2, const int16_t * s3, const int16_t * s4, const int16_t * s5, int n
    int16_t output[SIZE];
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t input3[SIZE];
    int16_t input4[SIZE];
    int16_t input5[SIZE];
    int16_t linux_output[] = {-13,-1,-1,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-3,-3,8};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
        input3[i] = i+3;
        input4[i] = i+4;
        input5[i] = i+5;

        output[i] = 0;
        }
    
    oil_lift_sub_135(output, input1, input2, input3, input4, input5, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_lift_sub_mult_shift12()
    {
    //int16_t * d, const int16_t * s1, const int16_t * s2, const int16_t * s3, const int16_t * s4_1, int n
    int16_t output[SIZE];
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t input3[SIZE];
    int16_t input4[1];
    int16_t linux_output[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    int i = 0;
    
    input4[0] = 4;
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
        input3[i] = i+3;
        
        output[i] = 0;
        }
    
    oil_lift_sub_mult_shift12(output, input1, input2, input3, input4, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_lift_sub_shift1()
    {
    //int16_t * d, const int16_t * s1, const int16_t * s2, const int16_t * s3, int n
    int16_t output[SIZE];
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t input3[SIZE];
    int16_t linux_output[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
        input3[i] = i+3;
        
        output[i] = 0;
        }
    
    oil_lift_sub_shift1(output, input1, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_lift_sub_shift2()
    {
    //int16_t * d, const int16_t * s1, const int16_t * s2, const int16_t * s3, int n
    int16_t output[SIZE];
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t input3[SIZE];
    int16_t linux_output[] = {0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
        input3[i] = i+3;
        
        output[i] = 0;
        }
    
    oil_lift_sub_shift2(output, input1, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_lshift_s16()
    {
    //int16_t * d1, const int16_t * s1, const int16_t * s2_1, int n
    int16_t output[SIZE];
    int16_t input1[SIZE];
    int16_t input2[1];
    int16_t linux_output[] = {1024,2048,3072,4096,5120,6144,7168,8192,9216,10240,11264,12288,13312,14336,15360,16384,17408,18432,19456,20480};
    int i = 0;
    
    input2[0] = 10;
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i+1;
               
        output[i] = 0;
        }
    
    oil_lshift_s16(output, input1, input2, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_multiply_and_acc_12xn_s16_u8()
    {
    //int16_t * i1_12xn, int is1, const int16_t * s1_12xn, int ss1, const uint8_t * s2_12xn, int ss2, int n
    int16_t input1[SIZE*12];
    uint8_t input2[SIZE*12];
    int16_t output[SIZE*12];
    int16_t linux_output[] = {2049,14105,-20630,-27882,1088,9482,6037,-508,-1416,11797,14588,22258,-22445,28216,-31104,-12536,9646,26705,29900,10241,25291,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<SIZE*12; i++)
        {
        input1[i] = i+1;
        input2[i] = i+1;
               
        output[i] = 0;
        }
    
    oil_multiply_and_acc_12xn_s16_u8(output, 1, input1, 2, input2, 3, SIZE);
    
    for(i = 0; i < SIZE*12; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_multiply_and_acc_16xn_s16_u8()
    {
    //int16_t * i1_16xn, int is1, const int16_t * s1_16xn, int ss1, const uint8_t * s2_16xn, int ss2, int n
    int16_t input1[SIZE*16];
    uint8_t input2[SIZE*16];
    int16_t output[SIZE*16];
    int16_t linux_output[] = {2562,15902,-16778,-21204,11363,24125,25819,25184,30958,-13657,-8553,1687,16812,-28204,-2288,28512,-32621,-9442,23776,-7240,19842,30750,16744,-30913,10112,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<SIZE*16; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
               
        output[i] = 0;
        }
    
    oil_multiply_and_acc_16xn_s16_u8(output, 1, input1, 2, input2, 3, SIZE);
    
    for(i = 0; i < SIZE*16; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_multiply_and_acc_24xn_s16_u8()
    {
    //int16_t * i1_24xn, int is1, const int16_t * s1_24xn, int ss1, const uint8_t * s2_24xn, int ss2, int n
    int16_t input1[SIZE*24];
    uint8_t input2[SIZE*24];
    int16_t output[SIZE*24];
    int16_t linux_output[] = {2562,15902,-16778,-21204,11363,24125,25819,25184,30958,-13657,-8553,1687,16812,-28204,-2288,28512,-1083,-25540,20682,6507,-2529,-6425,-5181,1206,15666,28340,30493,13130,-31716,17517,20765,-29431,-11505,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<SIZE*24; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
               
        output[i] = 0;
        }
    
    oil_multiply_and_acc_24xn_s16_u8(output, 1, input1, 2, input2, 3, SIZE);
    
    for(i = 0; i < SIZE*24; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_multiply_and_acc_6xn_s16_u8()
    {
    //int16_t * i1_6xn, int is1, const int16_t * s1_6xn, int ss1, const uint8_t * s2_6xn, int ss2, int n
    int16_t input1[SIZE*6];
    uint8_t input2[SIZE*6];
    int16_t output[SIZE*6];
    int16_t linux_output[] = {2562,15902,-16778,-21204,11363,24125,3235,19354,6945,31545,-12086,19342,-13979,10284,17859,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<SIZE*6; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
               
        output[i] = 0;
        }
    
    oil_multiply_and_acc_6xn_s16_u8(output, 1, input1, 2, input2, 3, SIZE);
    
    for(i = 0; i < SIZE*6; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_multiply_and_acc_8xn_s16_u8()
    {
    //int16_t * i1_8xn, int is1, const int16_t * s1_8xn, int ss1, const uint8_t * s2_8xn, int ss2, int n
    int16_t input1[SIZE*8];
    uint8_t input2[SIZE*8];
    int16_t output[SIZE*8];
    int16_t linux_output[] = {2562,15902,-16778,-21204,11363,24125,25819,25184,-2412,19336,-14808,-27096,-26260,-21039,-20171,-32394,-910,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<SIZE*8; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
               
        output[i] = 0;
        }
    
    oil_multiply_and_acc_8xn_s16_u8(output, 1, input1, 2, input2, 3, SIZE);
    
    for(i = 0; i < SIZE*8; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    
    }

void test_oil_multiply_and_add_s16()
    {
    //int16_t * d, const int16_t * src1, const int16_t * src2, const int16_t * src3, int n
    int16_t output[SIZE];
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t input3[SIZE];
    int16_t linux_output[] = {7,14,23,34,47,62,79,98,119,142,167,194,223,254,287,322,359,398,439,482};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
        input3[i] = i+3;
        
        output[i] = 0;
        }
    
    oil_multiply_and_add_s16(output, input1, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_multiply_and_add_s16_u8()
    {
    //int16_t * d, const int16_t * src1, const int16_t * src2, const uint8_t * src3, int n
    int16_t output[SIZE];
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    uint8_t input3[SIZE];
    int16_t linux_output[] = {7,14,23,34,47,62,79,98,119,142,167,194,223,254,287,322,359,398,439,482};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        input1[i] = i+1;
        input2[i] = i+2;
        input3[i] = i+3;
        
        output[i] = 0;
        }
    
    oil_multiply_and_add_s16_u8(output, input1, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_split_135()
    {
    //int16_t * d_2xn, const int16_t * s_2xn, int n
    int16_t output[SIZE*2];
    int16_t input1[SIZE*2];
    int16_t linux_output[] = {1,1,3,0,4,0,7,0,9,0,11,0,13,0,15,0,17,0,19,0,21,0,23,0,25,0,27,0,29,0,31,0,33,0,35,0,36,0,39,1};
    int i = 0;
    
    for(i=0; i<SIZE*2; i++)
        {
        input1[i] = i+1;
        output[i] = 0;
        }
    
    oil_split_135(output, input1, SIZE);
    
    for(i = 0; i < SIZE*2; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_split_53()
    {
    //int16_t * d_2xn, const int16_t * s_2xn, int n
    int16_t output[SIZE*2];
    int16_t input1[SIZE*2];
    int16_t linux_output[] = {1,0,3,0,5,0,7,0,9,0,11,0,13,0,15,0,17,0,19,0,21,0,23,0,25,0,27,0,29,0,31,0,33,0,35,0,37,0,39,1};
    int i = 0;
    
    for(i=0; i<SIZE*2; i++)
        {
        input1[i] = i+1;
        output[i] = 0;
        }
    
    oil_split_53(output, input1, SIZE);
    
    for(i = 0; i < SIZE*2; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_split_approx97()
    {
    //int16_t * d_2xn, const int16_t * s_2xn, int n
    int16_t output[SIZE*2];
    int16_t input1[SIZE*2];
    int16_t linux_output[] = {1,1,3,0,5,0,7,0,9,0,11,0,13,0,15,0,17,0,19,0,21,0,23,0,25,0,27,0,29,0,31,0,33,0,35,0,37,0,39,1};
    int i = 0;
    
    for(i=0; i<SIZE*2; i++)
        {
        input1[i] = i+1;
        output[i] = 0;
        }
    
    oil_split_approx97(output, input1, SIZE);
    
    for(i = 0; i < SIZE*2; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_split_daub97()
    {
    //int16_t * d_2xn, const int16_t * s_2xn, int n
    int16_t output[SIZE*2];
    int16_t input1[SIZE*2];
    int16_t linux_output[] = {2,1,4,1,7,1,9,1,12,1,14,0,16,0,19,1,21,0,24,1,26,1,29,0,31,1,34,1,36,1,39,1,41,1,43,0,46,0,48,1};
    int i = 0;
    
    for(i=0; i<SIZE*2; i++)
        {
        input1[i] = i+1;
        output[i] = 0;
        }
    
    oil_split_daub97(output, input1, SIZE);
    
    for(i = 0; i < SIZE*2; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_synth_135()
    {
    //int16_t * d_2xn, const int16_t * s_2xn, int n
    int16_t output[SIZE*2];
    int16_t input1[SIZE*2];
    int16_t linux_output[] = {1,3,2,6,3,9,4,12,5,15,6,18,7,21,8,24,9,27,10,30,11,33,12,36,13,39,14,42,15,45,16,48,17,51,18,54,19,57,20,60};
    int i = 0;
    
    for(i=0; i<SIZE*2; i++)
        {
        input1[i] = i+1;
        output[i] = 0;
        }
    
    oil_synth_135(output, input1, SIZE);
    
    for(i = 0; i < SIZE*2; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_synth_53()
    {
    //int16_t * d_2xn, const int16_t * s_2xn, int n
    int16_t output[SIZE*2];
    int16_t input1[SIZE*2];
    int16_t linux_output[] = {0,3,2,6,3,9,4,12,5,15,6,18,7,21,8,24,9,27,10,30,11,33,12,36,13,39,14,42,15,45,16,48,17,51,18,54,19,57,20,60};
    int i = 0;
    
    for(i=0; i<SIZE*2; i++)
        {
        input1[i] = i+1;
        output[i] = 0;
        }
    
    oil_synth_53(output, input1, SIZE);
    
    for(i = 0; i < SIZE*2; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_synth_approx97()
    {
    //int16_t * d_2xn, const int16_t * s_2xn, int n
    int16_t output[SIZE*2];
    int16_t input1[SIZE*2];
    int16_t linux_output[] = {0,2,2,6,3,9,4,12,5,15,6,18,7,21,8,24,9,27,10,30,11,33,12,36,13,39,14,42,15,45,16,48,17,51,18,54,19,57,20,60};
    int i = 0;
    
    for(i=0; i<SIZE*2; i++)
        {
        input1[i] = i+1;
        output[i] = 0;
        }
    
    oil_synth_approx97(output, input1, SIZE);
    
    for(i = 0; i < SIZE*2; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_synth_daub97()
    {
    //int16_t * d_2xn, const int16_t * s_2xn, int n
    int16_t output[SIZE*2];
    int16_t input1[SIZE*2];
    int16_t linux_output[] = {0,3,1,6,1,8,1,10,2,13,2,16,3,20,3,22,3,25,4,27,4,29,4,33,5,37,6,40,6,42,6,44,6,46,6,51,8,55,8,57};
    int i = 0;
    
    for(i=0; i<SIZE*2; i++)
        {
        input1[i] = i+1;
        output[i] = 0;
        }
    
    oil_synth_daub97(output, input1, SIZE);
    
    for(i = 0; i < SIZE*2; i++)
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

    std_log(LOG_FILENAME_LINE,"START oil_add2_rshift_add_s16 TEST");
    test_oil_add2_rshift_add_s16();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_add2_rshift_sub_s16 TEST");
    test_oil_add2_rshift_sub_s16();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_add_const_rshift_s16 TEST");
    test_oil_add_const_rshift_s16();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_avg2_12xn_u8 TEST");
    test_oil_avg2_12xn_u8();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_avg2_16xn_u8 TEST");
    test_oil_avg2_16xn_u8();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_avg2_8xn_u8 TEST");
    test_oil_avg2_8xn_u8();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_combine2_12xn_u8 TEST");
    test_oil_combine2_12xn_u8();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_combine2_16xn_u8 TEST");
    test_oil_combine2_16xn_u8();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_combine2_8xn_u8 TEST");
    test_oil_combine2_8xn_u8();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_combine4_12xn_u8 TEST");
    test_oil_combine4_12xn_u8(); //case fails...getting diff result than linux one.
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_combine4_16xn_u8 TEST");
    test_oil_combine4_16xn_u8(); //case fails...getting diff result than linux one.
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_combine4_8xn_u8 TEST");
    test_oil_combine4_8xn_u8(); //case fails...getting diff result than linux one.
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_deinterleave TEST");
    test_oil_deinterleave();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_deinterleave2_s16 TEST");
    test_oil_deinterleave2_s16();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_interleave TEST");
    test_oil_interleave();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_interleave2_s16 TEST");
    test_oil_interleave2_s16();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_lift_add_135 TEST");
    test_oil_lift_add_135();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_lift_add_mult_shift12 TEST");
    test_oil_lift_add_mult_shift12();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_lift_add_shift1 TEST");
    test_oil_lift_add_shift1();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_interleave2_s16 TEST");
    test_oil_lift_add_shift2();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_lift_sub_135 TEST");
    test_oil_lift_sub_135();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_lift_sub_mult_shift12 TEST");
    test_oil_lift_sub_mult_shift12();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_lift_sub_shift1 TEST");
    test_oil_lift_sub_shift1();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_lift_sub_shift2 TEST");
    test_oil_lift_sub_shift2();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_lshift_s16 TEST");
    test_oil_lshift_s16();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_multiply_and_acc_12xn_s16_u8 TEST");
    test_oil_multiply_and_acc_12xn_s16_u8();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_multiply_and_acc_16xn_s16_u8 TEST");
    test_oil_multiply_and_acc_16xn_s16_u8();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_multiply_and_acc_24xn_s16_u8 TEST");
    test_oil_multiply_and_acc_24xn_s16_u8();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_multiply_and_acc_6xn_s16_u8 TEST");
    test_oil_multiply_and_acc_6xn_s16_u8();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_multiply_and_acc_8xn_s16_u8 TEST");
    test_oil_multiply_and_acc_8xn_s16_u8();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_multiply_and_add_s16 TEST");
    test_oil_multiply_and_add_s16();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_multiply_and_add_s16_u8 TEST");
    test_oil_multiply_and_add_s16_u8();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_split_135 TEST");
    test_oil_split_135();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_split_53 TEST");
    test_oil_split_53();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_split_approx97 TEST");
    test_oil_split_approx97();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_split_daub97 TEST");
    test_oil_split_daub97();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_synth_135 TEST");
    test_oil_synth_135();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_synth_53 TEST");
    test_oil_synth_53();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_synth_approx97 TEST");
    test_oil_synth_approx97();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    std_log(LOG_FILENAME_LINE,"START oil_synth_daub97 TEST");
    test_oil_synth_daub97();
    std_log(LOG_FILENAME_LINE,"END TEST\n");
    
    if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Failed");
    else
      std_log(LOG_FILENAME_LINE,"Test Successful");
    
    create_xml(0);
    
    return 0;
    }

