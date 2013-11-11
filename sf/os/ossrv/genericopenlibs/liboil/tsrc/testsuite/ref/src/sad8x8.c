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
#include <globals.h>

#define LOG_FILE "c:\\logs\\testsuite_sad8x8_log.txt"
#include "std_log_result.h"
#include "utils.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#define MAX_SIZE 144
#define MAX_SIZE1 256
#define MAX_SIZE2 64
#define RES_SIZE 1

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_sad8x8");
    close_log_file();
}


void test_sad12xn_u8()
    {
    int i;
    uint8_t sad1[MAX_SIZE],sad2[MAX_SIZE];
    uint32_t result[RES_SIZE]={0}, check_res=3168;
    
    for(i=0;i<MAX_SIZE;i++)
        {
        sad1[i]=(i+1)*2;
        sad2[i]=(i+2)*1;
        }
    oil_sad12x12_u8(result,sad1,2,sad2,1);
    
    if(check_res == result[0])
          {
          std_log(LOG_FILENAME_LINE,"oil_sad12x12_u8 successful, result = %d", result[0]);
          }
     else
         {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_sad12x12_u8 unsuccessful, Expected =%d,Obtained =%d",check_res,result[0]);
         }
    }


void test_sad12xn_12x12_u8()
    {
    int i;
    uint8_t sad1[MAX_SIZE],sad2[MAX_SIZE];
    uint32_t res[MAX_SIZE],check[MAX_SIZE]={3168,3026,2886,2748,2614,2484,2358,2238,2124,2016,1916,1824,1740,1664,1596,1536,1484,1440,1404,1376,1356,1344,1340,1344,1356,1376,1404,1440,1484,1536,1596,1664,1740,1824,1916,2016,2124,2238,2358,2484,2614,2748,2886,3026,3168,3312,3456,3600,3744,3888,4032,4176,4320,4464,4608,4752,4896,5040,5184,5328,5472,5616,5760,5904,6048,6192,6336,6480,6624,6768,6912,7056,7200,7344,7488,7632,7776,7920,8064,8208,8352,8496,8640,8784,8928,9072,9216,9360,9504,9648,9792,9936,10080,10224,10368,10512,10656,10800,10944,11088,11232,11376,11520,11664,11808,11952,12096,12240,12384,12528,12672,12816,12960,13104,13248,13392,13536,13680,13824,13968,14112,14256,14388,14493,14556,14562,14496,14343,14088,13716,13212,12561,11748,10758,9744,8721,7704,6708,5748,4839,3996,3234,2568,2013};
    for(i=0;i<MAX_SIZE;i++)
        {
        sad1[i]=(i+1)*2;
        sad2[i]=(i+2)*1;
        res[i]=0;
        }
    oil_sad12x12_12xn_u8(res,sad1,2,sad2,1,MAX_SIZE);
    
    for(i=0;i<MAX_SIZE;i++)
        if(check[i]== res[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_sad12x12_12xn_u8 successful, res[%d] = %d", i,res[i]);
            }
        else
            {
             assert_failed=1;
             std_log(LOG_FILENAME_LINE,"oil_sad12x12_12xn_u8 unsuccessful,Expected =%d,Obtained =%d",check[i],res[i]);
            }
    }

void test_sad16xn_u8()
    {
    int i;
    uint8_t sad1[MAX_SIZE1],sad2[MAX_SIZE1];
    uint32_t result[RES_SIZE]={0},check_res = 2176;
    
    for(i=0;i<MAX_SIZE1;i++)
        {
        sad1[i]=i+3;
        sad2[i]=i+2;
        }
    oil_sad16x16_u8(result,sad1,2,sad2,1);
    
     if(check_res == result[0])
           {
            std_log(LOG_FILENAME_LINE,"oil_sad16x16_u8 successful, result = %d",result[0]);
           }
        else
            {
             assert_failed=1;
             std_log(LOG_FILENAME_LINE,"oil_sad16x16_u8 unsuccessful,Expected =%d,Obtained =%d",check_res,result[0]);
            }  
    }


void test_sad16xn_16x16_u8()
    {
    int i;
    uint8_t sad1[MAX_SIZE1],sad2[MAX_SIZE1];
    uint32_t res[MAX_SIZE1],check[MAX_SIZE1]={7680,7426,7174,6924,6678,6436,6198,5966,5740,5520,5308,5104,4908,4722,4546,4380,4226,4082,3948,3826,3714,3612,3522,3442,3372,3314,3266,3228,3202,3186,3180,3186,3202,3228,3266,3314,3372,3442,3522,3612,3714,3826,3948,4082,4226,4380,4546,4722,4908,5104,5308,5520,5740,5966,6198,6436,6678,6924,7174,7426,7680,7936,8192,8448,8704,8960,9216,9472,9728,9984,10240,10496,10752,11008,11264,11520,11776,12032,12288,12544,12800,13056,13312,13568,13824,14080,14336,14592,14848,15104,15360,15616,15872,16128,16384,16640,16896,17152,17408,17664,17920,18176,18432,18688,18944,19200,19456,19712,19968,20224,20480,20736,20992,21248,21504,21760,22016,22272,22528,22784,23040,23296,23552,23808,24064,24320,24576,24832,25088,25344,25600,25856,26112,26368,26624,26880,27136,27392,27648,27904,28160,28416,28672,28928,29184,29440,29696,29952,30208,30464,30720,30976,31232,31488,31744,32000,32256,32512,32768,33024,33280,33536,33792,34048,34304,34560,34816,35072,35328,35584,35840,36096,36352,36608,36864,37120,37376,37632,37888,38144,38400,38656,38912,39168,39424,39680,39936,40192,40448,40704,40960,41216,41472,41728,41984,42240,42496,42752,43008,43264,43520,43776,44032,44288,44544,44800,45056,45312,45568,45824,46080,46336,46592,46848,47104,47360,47616,47872,48128,48384,48640,48896,49152,49408,49592,49690,49688,49571,49324,48932,48380,47653,46736,45614,44272,42695,40868,38776,36404,33737,31096,28494,25944,23461,21060,18756,16564,14499,12576,10810,9216,7809,6604,5616,4860,4351};
    for(i=0;i<MAX_SIZE1;i++)
        {
        sad1[i]=(i+1)*2;
        sad2[i]=(i+2)*1;
        res[i]=0;
        }
    oil_sad16x16_16xn_u8(res,sad1,2,sad2,1,MAX_SIZE1);
    
    for(i=0;i<MAX_SIZE1;i++)
        if(check[i]== res[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_sad16x16_16xn_u8 successful, res[%d] = %d", i,res[i]);
            }
        else
            {
             assert_failed=1;
             std_log(LOG_FILENAME_LINE,"oil_sad16x16_16xn_u8 unsuccessful,Expected =%d,Obtained =%d",check[i],res[i]);
            }
    }


void test_sad8xn_u8()
    {
    int i;
    uint8_t sad1[MAX_SIZE2],sad2[MAX_SIZE2];
    uint32_t result[RES_SIZE]={0},check_res = 672;
    for(i=0;i<MAX_SIZE2;i++)
        {
        sad1[i]=i*3;
        sad2[i]=i*2;
        }
    oil_sad8x8_u8(result,sad1,2,sad2,2);
    
     if(check_res == result[0])
           {
            std_log(LOG_FILENAME_LINE,"oil_sad8x8_u8 successful, result = %d",result[0]);
           }
        else
            {
             assert_failed=1;
             std_log(LOG_FILENAME_LINE,"oil_sad8x8_u8 unsuccessful, Expected =%d,Obtained =%d",check_res,result[0]);
            }  
    }


void test_sad8xn_8x8_u8()
    {
    int i;
    uint8_t sad1[MAX_SIZE2],sad2[MAX_SIZE2];
    uint32_t res[MAX_SIZE2],check[MAX_SIZE2]={896,834,774,716,662,612,566,526,492,462,438,420,406,398,396,398,406,420,438,462,492,526,566,612,662,716,774,834,896,960,1024,1088,1152,1216,1280,1344,1408,1472,1536,1600,1664,1728,1792,1856,1920,1984,2048,2112,2176,2240,2324,2413,2492,2546,2560,2519,2408,2212,1972,1703,1420,1138,872,637};
    for(i=0;i<MAX_SIZE2;i++)
        {
        sad1[i]=(i+1)*2;
        sad2[i]=(i+2)*1;
        res[i]=0;
        }
    oil_sad8x8_8xn_u8(res,sad1,2,sad2,1,MAX_SIZE2);
    
    for(i=0;i<MAX_SIZE2;i++)
        if(check[i]== res[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_sad8x8_8xn_u8 successful, res[%d] = %d", i,res[i]);
            }
        else
            {
             assert_failed=1;
             std_log(LOG_FILENAME_LINE,"oil_sad8x8_8xn_u8 unsuccessful,Expected =%d,Obtained =%d",check[i],res[i]);
            }
    }



void test_sad8x8_s16()
    {
    int i;
    int16_t sad1[MAX_SIZE2],sad2[MAX_SIZE2];
    uint32_t res[MAX_SIZE2],check[MAX_SIZE2]={213120,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for(i=0;i<MAX_SIZE2;i++)
        {
        sad1[i]=(i+3)*2;
        sad2[i]=(i+2)*1;
        res[i]=0;
        }
    oil_sad8x8_s16(res,1,sad1,3,sad2,2);
    
    for(i=0;i<MAX_SIZE2;i++)
        if(check[i]== res[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_sad8x8_s16 successful, res[%d] = %d", i,res[i]);
            }
        else
            {
             assert_failed=1;
             std_log(LOG_FILENAME_LINE,"oil_sad8x8_s16 unsuccessful,Expected =%d,Obtained =%d",check[i],res[i]);
            }
    }


void test_sad8x8_s16_2()
    {
    int i;
    int16_t sad1[MAX_SIZE2],sad2[MAX_SIZE2];
    uint32_t res[RES_SIZE]={0},check[RES_SIZE]={196736};
    for(i=0;i<MAX_SIZE2;i++)
        {
        sad1[i]=(i+2)*2;
        sad2[i]=(i+1)*1;
        }
    oil_sad8x8_s16_2(res,sad1,3,sad2,2);
    
    if(check[0]== res[0])
       {
        std_log(LOG_FILENAME_LINE,"oil_sad8x8_s16_2 successful, res = %d",res[0]);
       }
    else
      {
       assert_failed=1;
       std_log(LOG_FILENAME_LINE,"oil_sad8x8_s16_2 unsuccessful,Expected =%d,Obtained =%d",check[0],res[0]);
      }
    }

void test_sad8x8_f64()
    {
    int i;
    double dsad1[MAX_SIZE2],dsad2[MAX_SIZE2],dres[MAX_SIZE2],dcheck[MAX_SIZE2]={64.00000000000000,0,};
    for(i=0;i<MAX_SIZE2;i++)
        {
        dsad1[i]=i*2;
        dsad2[i]=i*1;
        dres[i]=0;
        }
    oil_sad8x8_f64(dres,1,dsad1,3,dsad2,2);
    
    for(i=0;i<MAX_SIZE2;i++)
        if(comparefloats(dres[i],dcheck[i])==0)
            {
            std_log(LOG_FILENAME_LINE,"oil_sad8x8_f64 successful, dres[%d] = %15.14f", i,dres[i]);
            }
        else
            {
             assert_failed=1;
             std_log(LOG_FILENAME_LINE,"oil_sad8x8_f64 unsuccessful,Expected =%15.14f,Obtained =%15.14f",dcheck[i],dres[i]);
            }
    }



void test_sad8x8_f64_2()
    {
    int i;
    double dsad1[MAX_SIZE2],dsad2[MAX_SIZE2];
    double dres[RES_SIZE]={0},dcheck[MAX_SIZE]={64.00000000000000};
    for(i=0;i<MAX_SIZE2;i++)
        {
        dsad1[i]=i*2;
        dsad2[i]=i*1;
        }
    oil_sad8x8_f64_2(dres,dsad1,3,dsad2,2);
    
     if(comparefloats(dres[0],dcheck[0])==0)
        {
         std_log(LOG_FILENAME_LINE,"oil_sad8x8_f64_2 successful, dres = %15.14f",dres[0]);
        }
     else
        {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_sad8x8_f64_2 unsuccessful,Expected =%15.14f,Obtained =%15.14f",dcheck[0],dres[0]);
        }
    }

void test_sad8x8_u8_avg()
    {
    int i;
    uint8_t sad1[MAX_SIZE2],sad2[MAX_SIZE2],sad3[MAX_SIZE2];
    uint32_t res[RES_SIZE]={0},check[RES_SIZE]={1440};
    for(i=0;i<MAX_SIZE2;i++)
        {
        sad1[i]=(i+3)*2;
        sad2[i]=(i+2)*1;
        sad3[i]=(i+1)*1;
        }
    oil_sad8x8_u8_avg(res,sad1,3,sad2,sad3,2);
    
        if(check[0] == res[0])
            {
            std_log(LOG_FILENAME_LINE,"oil_sad8x8_u8_avg successful, res = %d", res[0]);
            }
        else
            {
             assert_failed=1;
             std_log(LOG_FILENAME_LINE,"oil_sad8x8_u8_avg unsuccessful,Expected =%d,Obtained =%d", check[0],res[0]);
            }
    }


int main ()
{
  std_log(LOG_FILENAME_LINE,"Test started testsuite_sad8x8");
  oil_init ();
  
  std_log(LOG_FILENAME_LINE,"test_sad12xn_u8");
  test_sad12xn_u8();
  
  std_log(LOG_FILENAME_LINE,"oil_sad12xn_12x12_u8");
  test_sad12xn_12x12_u8();
  
  std_log(LOG_FILENAME_LINE,"oil_sad16xn_u8");
  test_sad16xn_u8();
  
  std_log(LOG_FILENAME_LINE,"oil_sad16xn_16x16_u8");
  test_sad16xn_16x16_u8(); 

  std_log(LOG_FILENAME_LINE,"oil_sad8xn_u8");
  test_sad8xn_u8();
  
  std_log(LOG_FILENAME_LINE,"oil_sad8xn_8x8_u8");
  test_sad8xn_8x8_u8(); 

  std_log(LOG_FILENAME_LINE,"oil_sad8x8_s16");
  test_sad8x8_s16(); 

  std_log(LOG_FILENAME_LINE,"oil_sad8x8_s16_2");
  test_sad8x8_s16_2();
  
  std_log(LOG_FILENAME_LINE,"oil_sad8x8_f64");
  test_sad8x8_f64();
  
  std_log(LOG_FILENAME_LINE,"oil_sad8x8_f64_2");
  test_sad8x8_f64_2();  
  
  std_log(LOG_FILENAME_LINE,"oil_sad8x8_u8_avg");
  test_sad8x8_u8_avg();
  
  
  if(assert_failed)
         std_log(LOG_FILENAME_LINE,"Test Fail");
  else
         std_log(LOG_FILENAME_LINE,"Test Successful");
   
  create_xml(0);
  return 0;
}

