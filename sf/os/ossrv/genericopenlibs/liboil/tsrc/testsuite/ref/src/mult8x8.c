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



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <liboil/liboil.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>
#include <string.h>
#include <globals.h>

#define LOG_FILE "c:\\logs\\testsuite_mult8x8_log.txt"
#include "std_log_result.h"
#include "utils.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#define MAX_SIZE 64
#define MAX_SIZE1 10
#define RES_SIZE 1

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_mult8x8");
    close_log_file();
}


void test_mult8x8_s16()
    {
    int i;
    int16_t mult1[MAX_SIZE],mult2[MAX_SIZE],res[MAX_SIZE],check[MAX_SIZE]={2,16,42,80,108,130,154,180,208,238,14,48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
    for(i=0;i<MAX_SIZE;i++)
        {
        mult1[i]=i+1;
        mult2[i]=i+2;
        res[i]=0;
        }
    
    oil_mult8x8_s16(res,mult1,mult2,1,3,2);
    
    for(i=0;i<MAX_SIZE;i++)
    if(res[i] == check[i])
          {
          std_log(LOG_FILENAME_LINE,"oil_mult8x8_s16 successful, res[%d] = %d",i,res[i]);
          }
     else
         {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_mult8x8_s16 unsuccessful,Expected =%d,Obtained =%d",check[i],res[i]);
         }
    }

void test_multsum_f32()
    {
    float mult1[MAX_SIZE1],mult2[MAX_SIZE1];
    float res[RES_SIZE]={0},check[RES_SIZE]={404307968.00000000000000};
    int i;
    
    for(i=0;i<MAX_SIZE1;i++)
        {
         mult1[i]=(i+0.1)*3;
         mult2[i]=(i+2)*2;
        }
    oil_multsum_f32(res,mult1,3,mult2,1,MAX_SIZE1);
    
    if(comparefloats(res[0],check[0])==0)
        {
        std_log(LOG_FILENAME_LINE,"oil_multsum_f32 successful, res = %15.14f",res[0]);
        }
    else
        {
        assert_failed=1;
        std_log(LOG_FILENAME_LINE,"oil_multsum_f32 unsuccessful,Expected =%15.14f,Obtained =%15.14f",check[0],res[0]);
        }  
    }


void test_multsum_f64()
    {
    double mult1[MAX_SIZE1],mult2[MAX_SIZE1];
    double res[RES_SIZE]={0},check[RES_SIZE]={111.60000000000001};
    int i;
    
    for(i=0;i<MAX_SIZE1;i++)
        {
         mult1[i]=(i+1.1)*3;
         mult2[i]=(i+2)*2;
        }
    oil_multsum_f64(res,mult1,3,mult2,2,MAX_SIZE1);
    
    if(comparefloats(res[0],check[0])==0)
        {
        std_log(LOG_FILENAME_LINE,"oil_multsum_f64 successful, res = %15.14f",res[0]);
        }
    else
        {
        assert_failed=1;
        std_log(LOG_FILENAME_LINE,"oil_multsum_f64 unsuccessful,Expected =%15.14f,Obtained =%15.14f",check[0],res[0]);
        }  
    }

int main ()
{
  std_log(LOG_FILENAME_LINE,"Test started testsuite_mult8x8");
  oil_init ();
  
  std_log(LOG_FILENAME_LINE,"oil_mult8x8_s16");
  test_mult8x8_s16();
 
  std_log(LOG_FILENAME_LINE,"oil_multsum_f32");
  test_multsum_f32();
  
  std_log(LOG_FILENAME_LINE,"oil_multsum_f64");
  test_multsum_f64();
  
  if(assert_failed)
         std_log(LOG_FILENAME_LINE,"Test Fail");
  else
         std_log(LOG_FILENAME_LINE,"Test Successful");
   
  create_xml(0);
  return 0;
}

