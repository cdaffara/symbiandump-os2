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

#define LOG_FILE "c:\\logs\\testsuite_mas_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define SIZE    20

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_mas");
    close_log_file();
}

void test_oil_mas10_u8()
    {
    uint8_t input1[2*SIZE];
    int16_t input2[SIZE/2];
    int16_t input3[SIZE/10];
    uint8_t output[SIZE];
    uint8_t linux_output[] = {17,20,23,26,29,31,34,37,40,43,45,48,51,54,57,60,62,65,68,71};
    int i,j;
    
    for(i = 0; i < SIZE; i++)
        {
        input1[i] = i;
        if(i < SIZE/2)
            input2[i] = i*2;
        if(i < SIZE/10)
            input3[i] = i*5;
        }
    
    for(j = SIZE; j < 2*SIZE; j++)
        input1[j] = j;
    
    oil_mas10_u8(output, input1, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas10_u8_l15()
    {
    uint8_t input1[2*SIZE];
    int16_t input2[SIZE/2];
    int16_t input3[SIZE/10];
    uint8_t output[SIZE];
    uint8_t linux_output[] = {17,20,23,26,29,31,34,37,40,43,45,48,51,54,57,60,62,65,68,71};
    int i,j;
    
    for(i = 0; i < SIZE; i++)
        {
        input1[i] = i;
        if(i < SIZE/2)
            input2[i] = i*2;
        if(i < SIZE/10)
            input3[i] = i*5;
        }
    
    for(j = SIZE; j < 2*SIZE; j++)
        input1[j] = j;
    
    oil_mas10_u8_l15(output, input1, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas10_u8_sym_l15()
    {
    uint8_t input1[2*SIZE];
    int16_t input2[SIZE/2];
    int16_t input3[SIZE/10];
    uint8_t output[SIZE];
    uint8_t linux_output[] = {17,20,23,26,29,31,34,37,40,43,45,48,51,54,57,60,62,65,68,71};
    int i,j;
    
    for(i = 0; i < SIZE; i++)
        {
        input1[i] = i;
        if(i < SIZE/2)
            input2[i] = i*2;
        if(i < SIZE/10)
            input3[i] = i*5;
        }
    
    for(j = SIZE; j < 2*SIZE; j++)
        input1[j] = j;
    
    oil_mas10_u8_sym_l15(output, input1, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas12_addc_rshift_decim2_u8()
    {
    uint8_t input[2*SIZE + 12];
    int16_t taps[12];
    int16_t offsetshift[2];
    uint8_t output[SIZE];
    uint8_t linux_output[] = {31,39,48,56,64,72,81,89,97,105,114,122,130,138,147,155,163,171,180,188};
    int i,j;
    
    for(i = 0; i < SIZE; i++)
        {
        input[i] = i;
        if(i < 12)
            taps[i] = i*2;
        if(i < 2)
            offsetshift[i] = i*5;
        }
    
    for(j = SIZE; j < 52; j++)
        input[j] = j;
    
    oil_mas12_addc_rshift_decim2_u8(output, input, taps, offsetshift, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas2_across_add_s16()
    {
    int16_t input1[SIZE];
    int16_t input2[SIZE];
    int16_t input3[SIZE];
    int16_t input4[2];
    int16_t input5[2];
    int16_t output[SIZE];
    uint8_t linux_output[] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
    int i;
    
    for(i = 0; i < SIZE; i++)
        {
        input1[i] = i+3;
        input2[i] = i*2;
        input3[i] = i*5;
        
        if(i < 2)
            {
            input4[i] = i+10;
            input5[i] = i+20;
            }
        }
    
    oil_mas2_across_add_s16(output, input1, input2, input3, input4, input5, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas2_add_s16()
    {
    int16_t input1[SIZE];
    int16_t input2[SIZE+1];
    int16_t input3[2];
    int16_t input4[2];
    int16_t output[SIZE];
    int16_t linux_output[] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
    int i;
    
    for(i = 0; i <= SIZE; i++)
        {
        input2[i] = i*2;
         
        if(i < SIZE)
            {
            input1[i] = i+3;
            }
        
        if(i < 2)
            {
            input3[i] = i*5;
            input4[i] = i+10;
            }
        }
    
    oil_mas2_add_s16(output, input1, input2, input3, input4, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas4_across_add_s16()
    {
    int16_t input1[SIZE];
    int16_t input2[100];
    int16_t input3[4];
    int16_t input4[2];
    int16_t output[SIZE];
    int16_t linux_output[] = {19,23,26,30,33,37,40,44,47,51,54,58,61,65,68,72,75,79,82,86};
    int i;
    
    for(i = 0; i < 100; i++)
        {
        input2[i] = i;
         
        if(i < SIZE)
            {
            input1[i] = i+3;
            }
        
        if(i < 4)
            {
            input3[i] = i*5;
            }
        
        if(i < 2)
            {
            input4[i] = i+10;
            }
        }
    
    oil_mas4_across_add_s16(output, input1, input2, 5, input3, input4, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas4_add_s16()
    {
    int16_t input1[SIZE];
    int16_t input2[SIZE+3];
    int16_t input3[4];
    int16_t input4[2];
    int16_t output[SIZE];
    int16_t linux_output[] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
    int i;
    
    for(i = 0; i < SIZE+3; i++)
        {
        input2[i] = i;
         
        if(i < SIZE)
            {
            input1[i] = i+3;
            }
        
        if(i < 4)
            {
            input3[i] = i*5;
            }
        
        if(i < 2)
            {
            input4[i] = i+10;
            }
        }
    
    oil_mas4_add_s16(output, input1, input2, input3, input4, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas8_across_add_s16()
    {
    int16_t input1[SIZE];
    int16_t input2[SIZE*8];
    int16_t input3[8];
    int16_t input4[2];
    int16_t output[SIZE];
    int16_t linux_output[] = {139,150,161,172,183,194,205,216,227,238,249,260,271,282,294,305,316,327,338,349};
    int i;
    
    for(i = 0; i < SIZE*8; i++)
        {
        input2[i] = i;
         
        if(i < SIZE)
            {
            input1[i] = i+3;
            }
        
        if(i < 8)
            {
            input3[i] = i*5;
            }
        
        if(i < 2)
            {
            input4[i] = i+10;
            }
        }
    
    oil_mas8_across_add_s16(output, input1, input2, 5, input3, input4, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas8_across_u8()
    {
    uint8_t input1[SIZE*8];
    int16_t input2[8];
    int16_t input3[2];
    uint8_t output[SIZE];
    int16_t linux_output[] = {35,36,38,39,41,43,44,46,48,49,51,52,54,56,57,59,61,62,64,65};
    int i;
    
    for(i = 0; i < SIZE*8; i++)
        {
        input1[i] = i;
         
        if(i < 8)
            {
            input2[i] = i+3;
            }
        
        if(i < 2)
            {
            input3[i] = i*5;
            }
        }
    
    oil_mas8_across_u8(output, input1, 5, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas8_add_s16()
    {
    int16_t input1[SIZE];
    int16_t input2[SIZE+7];
    int16_t input3[8];
    int16_t input4[2];
    int16_t output[SIZE];
    int16_t linux_output[] = {3,4,5,7,8,9,10,11,12,13,15,16,17,18,19,20,21,23,24,25};
    int i;
    
    for(i = 0; i < SIZE+7; i++)
        {
        input2[i] = i;
         
        if(i < SIZE)
            {
            input1[i] = i+3;
            }
        
        if(i < 8)
            {
            input3[i] = i*5;
            }
        
        if(i < 2)
            {
            input4[i] = i*10;
            }
        }
    
    oil_mas8_add_s16(output, input1, input2, input3, input4, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas8_addc_rshift_decim2_u8()
    {
    uint8_t input1[2*(SIZE+9)];
    int16_t taps[8];
    int16_t offsetshift[2];
    uint8_t output[SIZE];
    int16_t linux_output[] = {7,10,13,16,20,23,26,29,33,36,39,42,46,49,52,55,59,62,65,68};
    int i;
    
    for(i = 0; i < 2*(SIZE+9); i++)
        {
        input1[i] = i;
         
        if(i < 8)
            {
            taps[i] = i+3;
            }
        
        if(i < 2)
            {
            offsetshift[i] = i*5;
            }
        }
    
    oil_mas8_addc_rshift_decim2_u8(output, input1, taps, offsetshift, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    
    }

void test_oil_mas8_u8()
    {
    uint8_t input1[SIZE+7];
    int16_t input2[8];
    int16_t input3[2];
    uint8_t output[SIZE];
    int16_t linux_output[] = {7,8,10,11,13,15,16,18,20,21,23,24,26,28,29,31,33,34,36,37};
    int i;
    
    for(i = 0; i < SIZE+7; i++)
        {
        input1[i] = i;
         
        if(i < 8)
            {
            input2[i] = i+3;
            }
        
        if(i < 2)
            {
            input3[i] = i*5;
            }
        }
    
    oil_mas8_u8(output, input1, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas8_u8_l15()
    {
    uint8_t input1[SIZE+7];
    int16_t input2[8];
    int16_t input3[2];
    uint8_t output[SIZE];
    int16_t linux_output[] = {7,8,10,11,13,15,16,18,20,21,23,24,26,28,29,31,33,34,36,37};
    int i;
    
    for(i = 0; i < SIZE+7; i++)
        {
        input1[i] = i;
         
        if(i < 8)
            {
            input2[i] = i+3;
            }
        
        if(i < 2)
            {
            input3[i] = i*5;
            }
        }
    
    oil_mas8_u8_l15(output, input1, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

void test_oil_mas8_u8_sym_l15()
    {
    uint8_t input1[SIZE+7];
    int16_t input2[8];
    int16_t input3[2];
    uint8_t output[SIZE];
    int16_t expected_output[] = {7,8,10,11,13,15,16,18,20,21,23,24,26,28,29,31,33,34,36,37};
    int i;
    
    for(i = 0; i < SIZE+7; i++)
        {
        input1[i] = i;
         
        if(i < 8)
            {
            input2[i] = i+3;
            }
        
        if(i < 2)
            {
            input3[i] = i*5;
            }
        }
    
    oil_mas8_u8_sym_l15(output, input1, input2, input3, SIZE);
    
    for(i = 0; i < SIZE; i++)
        {
        if(output[i] != expected_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,expected_output[i],output[i]);
            assert_failed = 1;
            }
        }
    }

int main (int argc, char *argv[])
{
  oil_init ();

  std_log(LOG_FILENAME_LINE,"START oil_mas10_u8 TEST");
  test_oil_mas10_u8();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas10_u8_l15 TEST");
  test_oil_mas10_u8_l15();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas10_u8_sym_l15 TEST");
  test_oil_mas10_u8_sym_l15();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas12_addc_rshift_decim2_u8 TEST");
  test_oil_mas12_addc_rshift_decim2_u8();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas2_across_add_s16 TEST");
  test_oil_mas2_across_add_s16();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas2_add_s16 TEST");
  test_oil_mas2_add_s16();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas4_across_add_s16 TEST");
  test_oil_mas4_across_add_s16();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas4_add_s16 TEST");
  test_oil_mas4_add_s16();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas8_across_add_s16 TEST");
  test_oil_mas8_across_add_s16();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas8_across_u8 TEST");
  test_oil_mas8_across_u8();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas8_add_s16 TEST");
  test_oil_mas8_add_s16();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas8_addc_rshift_decim2_u8 TEST");
  test_oil_mas8_addc_rshift_decim2_u8();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas8_u8 TEST");
  test_oil_mas8_u8();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas8_u8_l15 TEST");
  test_oil_mas8_u8_l15();
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  std_log(LOG_FILENAME_LINE,"START oil_mas8_u8_sym_l15 TEST");
  test_oil_mas8_u8_sym_l15(); //Getting different result than linux
  std_log(LOG_FILENAME_LINE,"END TEST\n");
  
  if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Failed");
  else
      std_log(LOG_FILENAME_LINE,"Test Successful");

  create_xml(0);
  return 0;
}

