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

#define LOG_FILE "c:\\logs\\testsuite_convert_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define SIZE    20

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_convert");
    close_log_file();
}

#define SIGN_BIT_8  0x80
#define SIGN_BIT_16 0x8000
#define SIGN_BIT_32 0x80000000

#define MAX_SIGNED_INT_32  0x7FFFFFFF
#define MAX_SIGNED_INT_16  0x7FFF
#define MAX_SIGNED_INT_16  0x7FFF
#define MAX_SIGNED_INT_8   0x7F

#define MAX_UNSIGNED_INT16  0xFFFF
#define MAX_UNSIGNED_INT8   0xFF

float f32src[] = {0.0, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.0, 11.1, 12.2, 13.3, 14.4, 15.5, 16.6, 17.7, 18.8, 19.9};
double f64src[] = {0.0, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.0, 11.1, 12.2, 13.3, 14.4, 15.5, 16.6, 17.7, 18.8, 19.9};

void test_oil_convert_s16_f32()
    {
    int16_t output[SIZE];
    int i = 0;
   
    oil_convert_s16_f32(output, f32src, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_s16_f64()
    {
    int16_t output[SIZE];
    int i = 0;
    
    oil_convert_s16_f64(output, f64src, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_s16_s32()
    {
    int32_t input[SIZE];
    int16_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        input[i] = i;
    
    oil_convert_s16_s32(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_s16_s8()
    {
    int8_t input[SIZE];
    int16_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        input[i] = i;
    
    oil_convert_s16_s8(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_s16_u16()
    {
    uint16_t input[SIZE];
    int16_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_16;
        }
    
    oil_convert_s16_u16(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != i)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != MAX_SIGNED_INT_16)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_SIGNED_INT_16,output[i]);
                assert_failed = 1;
                }
            }
        }
    }

void test_oil_convert_s16_u32()
    {
    uint32_t input[SIZE];
    int16_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_32;
        }
    
    oil_convert_s16_u32(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != i)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != MAX_SIGNED_INT_16)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_SIGNED_INT_16,output[i]);
                assert_failed = 1;
                }
            }
        }
    }

void test_oil_convert_s16_u8()
    {
    uint8_t input[SIZE];
    int16_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_8;
        }
    
    oil_convert_s16_u8(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != input[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,input[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_s32_f64()
    {
    double input[SIZE];
    int32_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        input[i] = i;
    
    oil_convert_s32_f64(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_s32_s16()
    {
    int16_t input[SIZE];
    int32_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        input[i] = i;
    
    oil_convert_s32_s16(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_s32_s8()
    {
    int8_t input[SIZE];
    int32_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        input[i] = i;
    
    oil_convert_s32_s8(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_s32_u16()
    {
    uint16_t input[SIZE];
    int32_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_16;
        }
    
    oil_convert_s32_u16(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != i)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != input[i])
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,input[i],output[i]);
                assert_failed = 1;
                }
            }
        }
    }

void test_oil_convert_s32_u32()
    {
    uint32_t input[SIZE];
    int32_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_32;
        }
    
    oil_convert_s32_u32(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != i)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != MAX_SIGNED_INT_32)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_SIGNED_INT_32,output[i]);
                assert_failed = 1;
                }
            }
        }
    }

void test_oil_convert_s32_u8()
    {
    uint8_t input[SIZE];
    int32_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_8;
        }
    
    oil_convert_s32_u8(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != input[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,input[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_s8_f32()
    {
    int8_t output[SIZE];
    int i = 0;
   
    oil_convert_s8_f32(output, f32src, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_s8_f64()
    {
    int8_t output[SIZE];
    int i = 0;
   
    oil_convert_s8_f64(output, f64src, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_s8_s16()
    {
    int16_t input[SIZE];
    int8_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | 0x100;
        }
    
    oil_convert_s8_s16(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != i)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != MAX_SIGNED_INT_8)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_SIGNED_INT_8,output[i]);
                assert_failed = 1;            
                }
            }
        }
    }

void test_oil_convert_s8_s32()
    {
    int32_t input[SIZE];
    int8_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | 0x10000;
        }
    
    oil_convert_s8_s32(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != i)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != MAX_SIGNED_INT_8)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_SIGNED_INT_8,output[i]);
                assert_failed = 1;            
                }
            }
        }    
    }

void test_oil_convert_s8_u16()
    {
    uint16_t input[SIZE];
    int8_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_16;
        }
    
    oil_convert_s8_u16(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != i)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != MAX_SIGNED_INT_8)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_SIGNED_INT_8,output[i]);
                assert_failed = 1;            
                }
            }
        }    
    }

void test_oil_convert_s8_u32()
    {
    uint32_t input[SIZE];
    int8_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_32;
        }
    
    oil_convert_s8_u32(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != i)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != MAX_SIGNED_INT_8)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_SIGNED_INT_8,output[i]);
                assert_failed = 1;            
                }
            }
        }    
    }

void test_oil_convert_s8_u8()
    {
    uint8_t input[SIZE];
    int8_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_8;
        }
    
    oil_convert_s8_u8(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != i)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != MAX_SIGNED_INT_8)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_SIGNED_INT_8,output[i]);
                assert_failed = 1;            
                }
            }
        }    
    }

void test_oil_convert_u16_f32()
    {
    uint16_t output[SIZE];
    int i = 0;
   
    oil_convert_u16_f32(output, f32src, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_u16_f64()
    {
    uint16_t output[SIZE];
    int i = 0;
   
    oil_convert_u16_f64(output, f64src, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_u16_s16()
    {
    int16_t input[SIZE];
    uint16_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | 0x100;
        }
    
    oil_convert_u16_s16(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != input[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,input[i],output[i]);
            assert_failed = 1;            
            }
        }
    }

void test_oil_convert_u16_s32()
    {
    int32_t input[SIZE];
    uint16_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | 0x10000;
        }
    
    oil_convert_u16_s32(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != i)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != MAX_UNSIGNED_INT16)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_UNSIGNED_INT16,output[i]);
                assert_failed = 1;            
                }
            }
        }    
    }

void test_oil_convert_u16_u32()
    {
    uint32_t input[SIZE];
    uint16_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_32;
        }
    
    oil_convert_u16_u32(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != i)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != MAX_UNSIGNED_INT16)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_UNSIGNED_INT16,output[i]);
                assert_failed = 1;            
                }
            }
        }
    }

void test_oil_convert_u16_u8()
    {
    uint8_t input[SIZE];
    uint16_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_8;
        }
    
    oil_convert_u16_u8(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != input[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,input[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_u32_f64()
    {
    uint32_t output[SIZE];
    int i = 0;
   
    oil_convert_u32_f64(output, f64src, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_u32_s32()
    {
    int32_t input[SIZE];
    uint32_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | 0x10000;
        }
    
    oil_convert_u32_s32(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != input[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,input[i],output[i]);
            assert_failed = 1;
            }
        }    
    }

void test_oil_convert_u32_u16()
    {
    uint16_t input[SIZE];
    uint32_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_16;
        }
    
    oil_convert_u32_u16(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != input[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,input[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_u32_u8()
    {
    uint8_t input[SIZE];
    uint32_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_8;
        }
    
    oil_convert_u32_u8(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != input[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,input[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_u8_f32()
    {
    uint8_t output[SIZE];
    int i = 0;
   
    oil_convert_u8_f32(output, f32src, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_u8_f64()
    {
    uint8_t output[SIZE];
    int i = 0;
   
    oil_convert_u8_f64(output, f64src, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_u8_s16()
    {
    int16_t input[SIZE];
    uint8_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | 0x100;
        }
    
    oil_convert_u8_s16(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != input[i])
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,input[i],output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != MAX_UNSIGNED_INT8)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_UNSIGNED_INT8,output[i]);
                assert_failed = 1;
                }
            }
        }  
    }

void test_oil_convert_u8_s32()
    {
    int32_t input[SIZE];
    uint8_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | 0x10000;
        }
    
    oil_convert_u8_s32(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != input[i])
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,input[i],output[i]);
                assert_failed = 1;
                }
            }
        else
            {
            if(output[i] != MAX_UNSIGNED_INT8)
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_UNSIGNED_INT8,output[i]);
                assert_failed = 1;
                }
            }
        }  
    }

void test_oil_convert_u8_s8()
    {
    int8_t input[SIZE];
    uint8_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        input[i] = i;

    oil_convert_u8_s8(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(output[i] != i)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,i,output[i]);
            assert_failed = 1;
            }
        }  
    }

void test_oil_convert_u8_u16()
    {
    uint16_t input[SIZE];
    uint8_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_16;
        }
    
    oil_convert_u8_u16(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != input[i])
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,input[i],output[i]);
                assert_failed = 1;
                }
            }
        else if(output[i] != MAX_UNSIGNED_INT8)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_UNSIGNED_INT8,output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_convert_u8_u32()
    {
    uint32_t input[SIZE];
    uint8_t output[SIZE];
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            input[i] = i;
        else
            input[i] = i | SIGN_BIT_32;
        }
    
    oil_convert_u8_u32(output, input, SIZE);
    
    for(i = 0; i<SIZE; i++)
        {
        if(i < SIZE/2)
            {
            if(output[i] != input[i])
                {
                std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,input[i],output[i]);
                assert_failed = 1;
                }
            }
        else if(output[i] != MAX_UNSIGNED_INT8)
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,MAX_UNSIGNED_INT8,output[i]);
            assert_failed = 1;
            }
        }
    }

int main (int argc, char *argv[])
    {
    oil_init();
    
    std_log(LOG_FILENAME_LINE,"START oil_convert_s16_* TESTS");
    test_oil_convert_s16_f32();
    test_oil_convert_s16_f64();
    test_oil_convert_s16_s32();
    test_oil_convert_s16_s8();
    test_oil_convert_s16_u16();
    test_oil_convert_s16_u32();
    test_oil_convert_s16_u8();
    std_log(LOG_FILENAME_LINE,"END oil_convert_s16_* TESTS");
    
    std_log(LOG_FILENAME_LINE,"START oil_convert_s32_* TESTS");
    test_oil_convert_s32_f64();
    test_oil_convert_s32_s16();
    test_oil_convert_s32_s8();
    test_oil_convert_s32_u16();
    test_oil_convert_s32_u32();
    test_oil_convert_s32_u8();
    std_log(LOG_FILENAME_LINE,"END oil_convert_s32_* TESTS");
    
    std_log(LOG_FILENAME_LINE,"START oil_convert_s8_* TESTS");
    test_oil_convert_s8_f32();
    test_oil_convert_s8_f64();
    test_oil_convert_s8_s16();
    test_oil_convert_s8_s32();
    test_oil_convert_s8_u16();
    test_oil_convert_s8_u32();
    test_oil_convert_s8_u8();
    std_log(LOG_FILENAME_LINE,"END oil_convert_s8_* TESTS");
    
    std_log(LOG_FILENAME_LINE,"START oil_convert_u16_* TESTS");
    test_oil_convert_u16_f32();
    test_oil_convert_u16_f64();
    test_oil_convert_u16_s16();
    test_oil_convert_u16_s32();
    test_oil_convert_u16_u32();
    test_oil_convert_u16_u8();
    std_log(LOG_FILENAME_LINE,"END oil_convert_u16_* TESTS");
    
    std_log(LOG_FILENAME_LINE,"START oil_convert_u32_* TESTS");
    test_oil_convert_u32_f64();
    test_oil_convert_u32_s32();
    test_oil_convert_u32_u16();
    test_oil_convert_u32_u8();
    std_log(LOG_FILENAME_LINE,"END oil_convert_u32_* TESTS");
    
    std_log(LOG_FILENAME_LINE,"START oil_convert_u8_* TESTS");
    test_oil_convert_u8_f32();
    test_oil_convert_u8_f64();
    test_oil_convert_u8_s16();
    test_oil_convert_u8_s32();
    test_oil_convert_u8_s8();
    test_oil_convert_u8_u16();
    test_oil_convert_u8_u32();
    std_log(LOG_FILENAME_LINE,"END oil_convert_u8_* TESTS");
        
    if(assert_failed)
        std_log(LOG_FILENAME_LINE,"Test Failed");
    else
        std_log(LOG_FILENAME_LINE,"Test Successful");
    create_xml(0);
    return 0;
    }

