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

#define LOG_FILE "c:\\logs\\testsuite_diff_log.txt"
#include "std_log_result.h"
#include "utils.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define RES_SIZE 1
#define MAX_SIZE 64
#define MAX_SIZE1 10
#define MAX_SIZE2 15


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_diff");
    close_log_file();
}


void test_average_s16_u8(void)
{
  int i;
  int16_t dest[MAX_SIZE];
  uint8_t src1[MAX_SIZE],src2[MAX_SIZE], src3[MAX_SIZE];
  int16_t check[MAX_SIZE] = {0,2,3,5,6,8,9,11,5,6,8,9,11,12,14,15,9,11,12,14,15,17,18,20,14,15,17,18,20,21,23,24,18,20,21,23,24,26,27,29,23,24,26,27,29,30,32,33,27,29,30,32,33,35,36,38,32,33,35,36,38,39,41,42};
  for(i=0;i<MAX_SIZE;i++)
  {
    dest[i]=0;
    src1[i]=i*3;
    src2[i]=i;
    src3[i]=i*2;
    std_log(LOG_FILENAME_LINE,"src1[%d] = %d, src2[%d] = %d, src3[%d]=%d ", i,src1[i],i,src2[i],i,src3[i]);
  }

  oil_diff8x8_average_s16_u8 (dest,src1,2,src2,1,src3,1);

  for(i=0;i<MAX_SIZE;i++)
  {
     if(dest[i] == check[i])
         {
         std_log(LOG_FILENAME_LINE,"oil_diff8x8_average_s16_u8 successful, dest[%d]=%d",i,dest[i]);
         }
     else
         {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_diff8x8_average_s16_u8 unsuccessful,Expected =%d,Obtained =%d",check[i],dest[i]);
         }
  }
}



void test_diff_const128_s16_u8()
    {
    int i;
    int16_t dest[MAX_SIZE];
    uint8_t src1[MAX_SIZE];
    int16_t check[MAX_SIZE]= {-28,-18,-8,2,12,22,32,42,-18,-8,2,12,22,32,42,52,-8,2,12,22,32,42,52,62,2,12,22,32,42,52,62,72,12,22,32,42,52,62,72,82,22,32,42,52,62,72,82,92,32,42,52,62,72,82,92,102,42,52,62,72,82,92,102,112}; 
    for(i=0;i<MAX_SIZE;i++)
        {
         dest[i]=0;
         src1[i]=(i+10)*10;
         std_log(LOG_FILENAME_LINE,"src1[%d] = %d", i,src1[i]);
        }
    
    oil_diff8x8_const128_s16_u8(dest,src1,1);
    
    for(i=0;i<MAX_SIZE;i++)
        {
        if(dest[i]== check[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_diff8x8_const128_s16_u8 successful,dest[%d]=%d",i,dest[i]);
            }
        else
            {
            assert_failed=1;
            std_log(LOG_FILENAME_LINE,"oil_diff8x8_const128_s16_u8 unsuccessful,Expected =%d,Obtained =%d",check[i],dest[i]);
            } 
        }
    }


void test_diff8x8_s16_u8()
    {
    int i;
    uint8_t src1[MAX_SIZE], src2[MAX_SIZE];
    int16_t dest[MAX_SIZE],check[MAX_SIZE]= {3,4,5,6,7,8,9,10,7,8,9,10,11,12,13,14,11,12,13,14,15,16,17,18,15,16,17,18,19,20,21,22,19,20,21,22,23,24,25,26,23,24,25,26,27,28,29,30,27,28,29,30,31,32,33,34,31,32,33,34,35,36,37,38};
    for(i=0;i<MAX_SIZE;i++)
        {
         dest[i]=0;
         src1[i]=(i+1)*3;
         src2[i]=i*2;
         std_log(LOG_FILENAME_LINE,"src1[%d]=%d, src2[%d]=%d", i,src1[i],i,src2[i]);
        }
    
    oil_diff8x8_s16_u8(dest,src1,2,src2,1);
    
    for(i=0;i<MAX_SIZE;i++)
     {
        if(dest[i]== check[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_diff8x8_s16_u8 successful, dest[%d]=%d", i,dest[i]);
            }
        else
            {
            assert_failed=1;
            std_log(LOG_FILENAME_LINE,"oil_diff8x8_s16_u8 unsuccessful,Expected =%d,Obtained =%d",check[i],dest[i]);
            } 
     }
    }

void test_squaresum_f32()
    {
    int i;
    float fsrc1[MAX_SIZE1];
    float fdest[RES_SIZE]={0},fcheck[RES_SIZE]= {1140.00000000000000};
    
    for(i=0;i<MAX_SIZE1;i++)
        {
         fsrc1[i]= i*2;
         std_log(LOG_FILENAME_LINE,"fsrc1[%d]=%15.14f", i,fsrc1[i]);
        }
    
    oil_squaresum_f32(fdest,fsrc1,MAX_SIZE1);
     
    if(comparefloats(fdest[0],fcheck[0])==0)
        {
        std_log(LOG_FILENAME_LINE,"oil_squaresum_f32 successful, fdest = %15.14f", fdest[0]);
        }
    else
        {
        assert_failed=1;
        std_log(LOG_FILENAME_LINE,"oil_squaresum_f32 unsuccessful,Expected =%15.14f,Obtained =%15.14f",fcheck[0],fdest[0]);
        } 
    }


void test_diff_squaresum_f32()
    {
    int i;
    float fsrc1[MAX_SIZE1], fsrc2[MAX_SIZE1];
    float fdest[RES_SIZE]={0},fcheck[RES_SIZE]= {930.00000000000000};
    
    for(i=0;i<MAX_SIZE1;i++)
        {
         fsrc1[i]= i*2;
         fsrc2[i]=i;
         std_log(LOG_FILENAME_LINE,"fsrc1[%d]=%15.14f, fsrc2[%d]=%15.14f", i,fsrc1[i],i,fsrc2[i]);
        }
    
    oil_diffsquaresum_f32(fdest,fsrc1,4,fsrc2,2,MAX_SIZE1);
     
     if(comparefloats(fdest[0],fcheck[0])==0)
          {
            std_log(LOG_FILENAME_LINE,"oil_diffsquaresum_f32 successful, fdest= %15.14f", fdest[0]);
          }
         else
          {
           assert_failed=1;
           std_log(LOG_FILENAME_LINE,"oil_diffsquaresum_f32 unsuccessful,Expected =%15.14f,Obtained =%15.14f",fcheck[0],fdest[0]);
          } 
    }


void test_squaresum_f64()
    {
    int i;
    double dsrc1[MAX_SIZE1];
    double ddest[RES_SIZE]={0},dcheck[RES_SIZE]={19668.00000000000000};
    
    for(i=0;i<MAX_SIZE1;i++)
        {
         dsrc1[i]= i*(i+1);
         std_log(LOG_FILENAME_LINE,"dsrc1[%d]=%15.14f", i,dsrc1[i]);
        }
    
    oil_squaresum_f64(ddest,dsrc1,MAX_SIZE1);
     
    if(comparefloats(ddest[0],dcheck[0])==0)
       {
        std_log(LOG_FILENAME_LINE,"oil_squaresum_f64 successful, ddest=%15.14f", ddest[0]);
       }
    else
       {
        assert_failed=1;
        std_log(LOG_FILENAME_LINE,"oil_squaresum_f64 unsuccessful,Expected =%15.14f,Obtained =%15.14f",dcheck[0],ddest[0]);
       } 
    }


void test_diff_squaresum_f64()
    {
    int i;
    double dsrc1[MAX_SIZE1], dsrc2[MAX_SIZE1];
    double ddest[RES_SIZE]={0},dcheck[RES_SIZE]= {539.25000000000000};
    
    for(i=0;i<MAX_SIZE1;i++)
        {
         dsrc1[i]= i*(i+1);
         dsrc2[i]=i*(0.5);
         std_log(LOG_FILENAME_LINE,"dsrc1[%d]=%15.14f, dsrc2[%d]=%15.14f", i,dsrc1[i],i,dsrc2[i]);
        }
    
    oil_diffsquaresum_f64(ddest,dsrc1,4,dsrc2,2,MAX_SIZE1);
    
        if(comparefloats(ddest[0],dcheck[0])==0)
          {
          std_log(LOG_FILENAME_LINE,"oil_diffsquaresum_f64 successful, ddest= %15.14f", ddest[0]);
          }
         else
          {
           assert_failed=1;
           std_log(LOG_FILENAME_LINE,"oil_diffsquaresum_f64 unsuccessful,Expected =%15.14f,Obtained =%15.14f",dcheck[0],ddest[0]);
          } 
    }

void test_squaresum_shifted_s16()
    {
    int16_t arr1[MAX_SIZE2];
    uint32_t res[RES_SIZE]={0},check[RES_SIZE]={526};
    int i;
    
    for(i=0;i<MAX_SIZE2;i++)
        {
         arr1[i]=(i+100)*10;
        }
    
    oil_squaresum_shifted_s16(res,arr1,MAX_SIZE2);
    
    if(res[0]==check[0])
         {
         std_log(LOG_FILENAME_LINE,"oil_squaresum_shifted_s16 successful, res= %d",res[0]);
         }
    else
        {
        assert_failed=1;
        std_log(LOG_FILENAME_LINE,"oil_squaresum_shifted_s16 unsuccessful,Expected =%d,Obtained =%d",check[0],res[0]);
        }
    }


void test_sum_f64()
    {
     double arr[MAX_SIZE2];
     double res[RES_SIZE]={0},check[RES_SIZE]={40.0};
     int i;
     
     for(i=0;i<MAX_SIZE2;i++)
        arr[i]=(i+3.5)*2;
       
     oil_sum_f64(res,arr,2,MAX_SIZE2);
     
     if(comparefloats(res[0],check[0])==0)
         {
         std_log(LOG_FILENAME_LINE,"oil_sum_f64 successful, res= %15.14f",res[0]);
         }
     else
         {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_sum_f64 unsuccessful,Expected =%15.14f,Obtained =%15.14f",check[0],res[0]);
         }
    }

void test_addc_s16()
    {
    int16_t arr1[MAX_SIZE2], arr2[MAX_SIZE2],sum[MAX_SIZE2];
    int res[MAX_SIZE2]={0,4,8,12,16,20,24,28,32,36,40,44,48,52,56};
    int i;
    
    for(i=0;i<MAX_SIZE2;i++)
        {
          sum[i]=0;
          arr1[i]=i*4;
          arr2[i]=i*3;
        }
    
    oil_addc_s16(sum,arr1,arr2,MAX_SIZE2);
    
    for(i=0;i<MAX_SIZE2;i++)
        {
          if(sum[i] == res[i])
              {
                std_log(LOG_FILENAME_LINE,"oil_addc_s16 successful,sum[%d]=%d",i,sum[i]);
              }
          else
              {
                assert_failed = 1;
                std_log(LOG_FILENAME_LINE,"oil_addc_s16 unsuccessful,Expected =%d,Obtained =%d",res[i],sum[i]);
              }
        }
    }


void test_addc_rshift_s16()
    {
    int16_t arr1[MAX_SIZE2],arr2[MAX_SIZE2],sum[MAX_SIZE2];
    int res[MAX_SIZE2]={0,0,1,1,2,2,3,3,4,4,5,5,6,6,7};
    int i;
    
    for(i=0;i<MAX_SIZE2;i++)
        {
          sum[i]=0;
          arr1[i]=i*4;
          arr2[i]=i*3;
        }
    
    oil_addc_rshift_s16(sum,arr1,arr2,MAX_SIZE2);
    
    for(i=0;i<MAX_SIZE2;i++)
        {
          if(sum[i] == res[i])
              {
                std_log(LOG_FILENAME_LINE,"oil_addc_rshift_s16 successful,sum[%d]=%d", sum[i]);
              }
          else
              {
                assert_failed = 1;
                std_log(LOG_FILENAME_LINE,"oil_addc_rshift_s16 unsuccessful,Expected =%d,Obtained =%d",res[i],sum[i]);
             }
        }
    }


int main ()
{
  std_log(LOG_FILENAME_LINE,"Test started testsuite_diff");
  oil_init ();
  
  std_log(LOG_FILENAME_LINE,"AVERAGE TEST");
  test_average_s16_u8();
  
  std_log(LOG_FILENAME_LINE,"DIFF8X8 TEST");
  test_diff8x8_s16_u8();
  
  std_log(LOG_FILENAME_LINE,"DIFF_CONST128 TEST");
  test_diff_const128_s16_u8();
  
  std_log(LOG_FILENAME_LINE,"SQUARESUM_F32 TEST");
  test_squaresum_f32();
  
  std_log(LOG_FILENAME_LINE,"DIFF_SQUARESUM32 TEST");
  test_diff_squaresum_f32();
  
  std_log(LOG_FILENAME_LINE,"SQUARESUM_F64 TEST");
  test_squaresum_f64();
  
  std_log(LOG_FILENAME_LINE,"DIFF_SQUARESUM64 TEST");
  test_diff_squaresum_f64(); 

  std_log(LOG_FILENAME_LINE,"SHIFT_SQUARESUM TEST");
  test_squaresum_shifted_s16(); 
  
  std_log(LOG_FILENAME_LINE,"SUM_F64 TEST");
  test_sum_f64(); 
  
  std_log(LOG_FILENAME_LINE,"ADDC_16 TEST");
  test_addc_s16(); 
  
  std_log(LOG_FILENAME_LINE,"ADDC_16_RSHIFT TEST");
  test_addc_rshift_s16();   

  if(assert_failed)
         std_log(LOG_FILENAME_LINE,"Test Fail");
  else
         std_log(LOG_FILENAME_LINE,"Test Successful");
   
  create_xml(0);
  return 0;
}

