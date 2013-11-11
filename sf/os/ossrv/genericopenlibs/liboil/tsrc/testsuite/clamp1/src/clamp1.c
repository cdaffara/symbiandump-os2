// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#include <liboil/liboil.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>
#include <stdlib.h>
#include <liboil/globals.h>

#define SIZE 5

#define LOG_FILE "c:\\logs\\testsuite_clamp1_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

int i;
void test_clamp_s8()
    {
    int8_t dest[SIZE]={0,0,0,0,0};
    const int8_t src[SIZE]={2,2,2,2,2},s2_1[SIZE]={14,14,14,14,14},s3_1[SIZE]={1,1,1,1,1};
    int8_t linux_result[SIZE]={1,1,1,1,1};
    oil_clamp_s8 (dest,src,SIZE,s2_1,s3_1);
      for(i=0;i<SIZE;i++)
          {
          if(dest[i]!= linux_result[i])
              {
              std_log(LOG_FILENAME_LINE,"oil_clamp_s8 Fails \n");
              assert_failed = 1;
              break;
              }
           }
    }

void test_clamp_u8()
    {
    uint8_t dest[SIZE];
    const uint8_t  src[SIZE]={128,0,20,30,127}, s2_1[SIZE]={1,2,3,4,5}, s3_1[SIZE]={1,2,3,4,5};
    int8_t linux_result[SIZE]={1,1,1,1,1};
    oil_clamp_u8 (dest,src,SIZE,s2_1,s3_1);
      for(i=0;i<SIZE;i++)
          {
          if(dest[i]!= linux_result[i])
              {
              std_log(LOG_FILENAME_LINE,"oil_clamp_u8 Fails \n");
              assert_failed = 1;
              break;
              }
           }
    }

void test_clamp_s16()
    {
    int16_t dest[SIZE];
    const int16_t src[SIZE]={2,2,2,2,2},s2_1[SIZE]={14,14,14,14,14},s3_1[SIZE]={10,10,10,10,10};
    int16_t linux_result[SIZE]={10,10,10,10,10};
    oil_clamp_s16 (dest,src,SIZE,s2_1,s3_1);
      for(i=0;i<SIZE;i++)
          {
          if(dest[i]!= linux_result[i])
              {
              std_log(LOG_FILENAME_LINE,"oil_clamp_s16 Fails \n");
              assert_failed = 1;
              break;
              }
           }
    }

void test_clamp_u16()
    {
    uint16_t dest[SIZE];
    const uint16_t src[SIZE]={2,2,2,2,2},s2_1[SIZE]={14,14,14,14,14},s3_1[SIZE]={10,10,10,10,10};
    uint16_t linux_result[SIZE]={10,10,10,10,10};
    oil_clamp_u16 (dest,src,SIZE,s2_1,s3_1);
      for(i=0;i<SIZE;i++)
          {
          if(dest[i]!= linux_result[i])
              {
              std_log(LOG_FILENAME_LINE,"oil_clamp_u16 Fails \n");
              assert_failed = 1;
              break;
              }
           }
    }

void test_clamp_s32()
    {
    int32_t dest[SIZE];
    const int32_t src[SIZE]={2,2,2,2,2},s2_1[SIZE]={14,14,14,14,14},s3_1[SIZE]={10,10,10,10,10};
    int32_t linux_result[SIZE]={10,10,10,10,10};
    oil_clamp_s32 (dest,src,SIZE,s2_1,s3_1);
      for(i=0;i<SIZE;i++)
          {
          if(dest[i]!= linux_result[i])
              {
              std_log(LOG_FILENAME_LINE,"oil_clamp_s32 Fails \n");
              assert_failed = 1;
              break;
              }
           }
    }

void test_clamp_u32()
    {
    uint32_t dest[SIZE];
    const uint32_t  src[SIZE]={128,0,20,30,127}, s2_1[SIZE]={1,2,3,4,5}, s3_1[SIZE]={1,2,3,4,5};
    uint32_t linux_result[SIZE]={1,1,1,1,1};
    oil_clamp_u32 (dest,src,SIZE,s2_1,s3_1);
      for(i=0;i<SIZE;i++)
          {
          if(dest[i]!= linux_result[i])
              {
              std_log(LOG_FILENAME_LINE,"oil_clamp_u32 Fails \n");
              assert_failed = 1;
              break;
              }
           }
    }

void test_clamp_f32()
    {
    float  dest[5];
    const float src[SIZE]={2.1,2.1,2.1,2.1,2.1},s2_1[SIZE]={14.1,14.1,14.1,14.1,14.1},s3_1[SIZE]={10.1,10.1,10.1,10.1,10};
    float linux_result[SIZE]={10.1,10.1,10.1,10.1,10.1};
    oil_clamp_f32 (dest,src,SIZE,s2_1,s3_1);
    for(i=0;i<SIZE;i++)
          {
          if(dest[i]!= linux_result[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_clamp_f32 Fails \n");
            assert_failed = 1;
            break;
            }
          }
    }

void test_clamp_f64()
    {
    double dest[5];
    const double src[SIZE]={2.1,2.1,2.1,2.1,2.1},s2_1[SIZE]={14.1,14.1,14.1,14.1,14.1},s3_1[SIZE]={10.1,10.1,10.1,10.1,10};
    double linux_result[SIZE]={10.1,10.1,10.1,10.1,10.1};
    oil_clamp_f64(dest,src,SIZE,s2_1,s3_1);
    for(i=0;i<SIZE;i++)
           {
          if(dest[i]!= linux_result[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_clamp_f64 Fails \n");
            assert_failed = 1;
            break;
            }
           }
    }

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_clamp1");
    close_log_file();
}

int main (int argc, char *argv[])
{
  std_log(LOG_FILENAME_LINE, "Test Started testsuite_clamp1");
  oil_init ();
  
  test_clamp_s8();
  test_clamp_u8();
  test_clamp_s16();
  test_clamp_u16();
  test_clamp_s32();
  test_clamp_u32();
  test_clamp_f32();
  test_clamp_f64();
  if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Fail");
  else
      std_log(LOG_FILENAME_LINE,"Test Successful");
  create_xml(0);
  return 0;
}

