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

#define LOG_FILE "c:\\logs\\testsuite_swab_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define SIZE    20

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_swab");
    close_log_file();
}

void test_oil_swab_u16()
    {
    //uint16_t * d_n, const uint16_t * s_n, int n
    uint16_t output[SIZE];
    uint16_t input[SIZE];
    uint16_t linux_output[] = {768,1024,1280,1536,1792,2048,2304,2560,2816,3072,3328,3584,3840,4096,4352,4608,4864,5120,5376,5632};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        output[i] = 0;
        input[i] = i+3;
        }
    
    oil_swab_u16(output, input, SIZE);
    
    for(i=0; i<SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_swab_u32()
    {
    //uint32_t * d_n, const uint32_t * s_n, int n
    uint32_t output[SIZE];
    uint32_t input[SIZE];
    uint32_t linux_output[] = {50331648,67108864,83886080,100663296,117440512,134217728,150994944,167772160,184549376,201326592,218103808,234881024,251658240,268435456,285212672,301989888,318767104,335544320,352321536,369098752};
    int i = 0;
    
    for(i=0; i<SIZE; i++)
        {
        output[i] = 0;
        input[i] = i+3;
        }
    
    oil_swab_u32(output, input, SIZE);
    
    for(i=0; i<SIZE; i++)
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
  oil_init ();

  test_oil_swab_u16();
  test_oil_swab_u32();
  
  if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Failed");
  else
      std_log(LOG_FILENAME_LINE,"Test Successful");

  create_xml(0);
  return 0;
}

