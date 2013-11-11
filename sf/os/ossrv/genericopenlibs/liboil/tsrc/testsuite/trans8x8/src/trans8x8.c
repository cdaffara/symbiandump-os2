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

#define LOG_FILE "c:\\logs\\testsuite_trans8x8_log.txt"
#include "std_log_result.h"
#include "utils.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define SIZE    8*8

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_trans8x8");
    close_log_file();
}

void test_oil_trans8x8_f64()
    {
    //double * d_8x8, int ds, const double * s_8x8, int ss
    double output[SIZE];
    double input[SIZE];
    double linux_output[SIZE] = {77.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,88.000000000000000,0.000000000000000,0.000000000000000,-2.000000000007314,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        output[i] = 0;
        input[i] = i+(i*10);
        }
    
    oil_trans8x8_f64(output, 0, input, 2);
    
    for(i=0; i<SIZE; i++)
        {
        if(comparefloats(output[i], linux_output[i]))
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_trans8x8_u16()
    {
    //uint16_t * d_8x8, int ds, const uint16_t * s_8x8, int ss
    uint16_t output[SIZE];
    uint16_t input[SIZE];
	uint16_t expected_output[SIZE] = {2816,5643,8470,11297,14124,16951,19778,22605,28259,33913,39567,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        output[i] = 0;
        input[i] = i+(i*10);
        }
    
    oil_trans8x8_u16(output, 1, input, 2);
    
    for(i=0; i<SIZE; i++)
        {
        if(output[i] != expected_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,expected_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_trans8x8_u32()
    {
    //uint32_t * d_8x8, int ds, const uint32_t * s_8x8, int ss
    uint32_t output[SIZE];
    uint32_t input[SIZE];
    uint32_t linux_output[SIZE] = {555092736,1296185132,0,1476417536,0,1660969728,0,1845521920,0,30976,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        output[i] = 0;
        input[i] = i+(i*10);
        }
    
    oil_trans8x8_u32(output, 1, input, 2);
    
    for(i=0; i<SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_trans8x8_u8()
    {
    //uint8_t * d_8x8, int ds, const uint8_t * s_8x8, int ss
    uint8_t output[SIZE];
    uint8_t input[SIZE];
    uint8_t linux_output[SIZE] = {0,11,22,33,44,55,66,77,99,121,143,165,187,209,231,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        output[i] = 0;
        input[i] = i+(i*10);
        }
    
    oil_trans8x8_u8(output, 1, input, 2);
    
    for(i=0; i<SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

int main (/*int argc, char *argv[]*/)
{
  oil_init ();
  
  std_log(LOG_FILENAME_LINE,"START oil_trans8x8_u8 TEST");
  test_oil_trans8x8_u8();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_trans8x8_u16 TEST");
  test_oil_trans8x8_u16(); // getting diff result than linux
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_trans8x8_u32 TEST");
  test_oil_trans8x8_u32();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_trans8x8_f64 TEST");
  test_oil_trans8x8_f64();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
      
  if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Failed");
  else
      std_log(LOG_FILENAME_LINE,"Test Successful");

  create_xml(0);
  return 0;
}

