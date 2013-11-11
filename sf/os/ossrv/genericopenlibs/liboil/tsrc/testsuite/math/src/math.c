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
//Description:
//



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <liboil/liboil.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>
#include <string.h>
#include <globals.h>

#define LOG_FILE "c:\\logs\\testsuite_math_log.txt"
#include "std_log_result.h"
#include "utils.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#define MAX_SIZE 10
#define MAX_SIZE1 8
#define MAX_DSIZE 5

int16_t arr1[MAX_SIZE], arr2[MAX_SIZE], res1[MAX_SIZE];
uint8_t uarr1[MAX_SIZE],uarr2[MAX_SIZE1],uarr3[MAX_SIZE1], ures1[MAX_SIZE1];
float farr1[MAX_SIZE1], farr2[MAX_SIZE1],fres1[MAX_SIZE1];
double darr1[MAX_DSIZE], darr2[MAX_DSIZE],dres1[MAX_DSIZE];

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_math");
    close_log_file();
}

void test_add_s16()
    {
    int i;
    int16_t check[MAX_SIZE]={8,13,18,23,28,33,38,43,48,53};
    for(i=0;i<MAX_SIZE;i++)
        {
          res1[i]=0;
          arr1[i]=(i+2) *3;
          arr2[i]=(i+1)*2;
          std_log(LOG_FILENAME_LINE,"arr1[%d] = %d, arr2[%d] = %d", i,arr1[i],i,arr2[i]);
        }
    oil_add_s16(res1,arr1,arr2,MAX_SIZE);
    
    for(i=0;i<MAX_SIZE;i++)
        {
        if(res1[i] == check[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_add_s16 successful, res1[%d]=%d", i,res1[i]);
            }
        else
            {
             assert_failed = 1;
             std_log(LOG_FILENAME_LINE,"oil_add_s16 unsuccessful, Expected =%d,Obtained =%d",check[i],res1[i]);
            }
        }  
    }

void test_add_s16_u8()
    {
    int i;
    int16_t check[MAX_SIZE]={6,11,16,21,26,31,36,41,46,51};
    for(i=0;i<MAX_SIZE;i++)
        {
          res1[i]=0;
          arr1[i]=(i+2)*3;
          uarr1[i]=i*2;
          std_log(LOG_FILENAME_LINE,"arr1[%d] = %d, uarr1[%d] = %d", i,arr1[i],i,uarr1[i]);
        }
    oil_add_s16_u8(res1,arr1,uarr1,MAX_SIZE);
    
    for(i=0;i<MAX_SIZE;i++)
        {
        if(res1[i] == check[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_add_s16_u8 successful, res1[%d] = %d", i,res1[i]);
            }
        else
            {
             assert_failed = 1;
             std_log(LOG_FILENAME_LINE,"oil_add_s16_u8 unsuccessful,Expected =%d,Obtained =%d",check[i],res1[i]);
            }
        }  
    }




void test_add_f32()
    {
    int i;
    float fcheck[MAX_SIZE1]={8.30000019073486,12.10000038146973,15.89999961853027,19.70000076293945,23.50000000000000,27.29999923706055,31.10000038146973,34.90000152587891};
    for(i=0;i<MAX_SIZE1;i++)
        {
          fres1[i]=0;
          farr1[i]=(i+2.5)*3;
          farr2[i]=(i+1)*0.8;
          std_log(LOG_FILENAME_LINE,"farr1[%d] = %f, farr2[%d] = %f", i,farr1[i],i,farr2[i]);
        }
    oil_add_f32(fres1,farr1,farr2,MAX_SIZE1);
    
    for(i=0;i<MAX_SIZE1;i++)
        {
        if(comparefloats(fres1[i],fcheck[i])==0)
            {
            std_log(LOG_FILENAME_LINE,"oil_add_f32 successful, fres1[%d] = %15.14f", i,fres1[i]);
            }
        else
            {
             assert_failed = 1;
             std_log(LOG_FILENAME_LINE,"oil_add_f32 unsuccessful, Expected =%15.14f,Obtained =%15.14f",fcheck[i],fres1[i]);
            }
        }  
    }


void test_add_f64()
    {
    int i;
    double dcheck[MAX_DSIZE]={7.51500000000000,10.85500000000000,14.19500000000000,17.53500000000000,20.87500000000000};
    for(i=0;i<MAX_DSIZE;i++)
        {
          fres1[i]=0;
          darr1[i]=(i+2.25)*3.34;
          arr2[i]=(i+1.3)*0.18;
          std_log(LOG_FILENAME_LINE,"farr1[%d] = %15.14f, farr2[%d] = %15.14f", i,darr1[i],i,darr2[i]);
        }
    oil_add_f64(dres1,darr1,darr2,MAX_DSIZE);
    
    for(i=0;i<MAX_DSIZE;i++)
        {
        if(comparefloats(dres1[i],dcheck[i])==0)
            {
            std_log(LOG_FILENAME_LINE,"oil_add_f64 successful, dres1[%d] = %15.14f", i,dres1[i]);
            }
        else
            {
             assert_failed = 1;
             std_log(LOG_FILENAME_LINE,"oil_add_f64 unsuccessful, Expected =%15.14f,Obtained =%15.14f",dcheck[i],dres1[i]);
            } 
        }  
    }


void test_sub_s16()
    {
    int i;
    int16_t check[MAX_SIZE]={4,5,6,7,8,9,10,11,12,13};
    for(i=0;i<MAX_SIZE;i++)
        {
          res1[i]=0;
          arr1[i]=(i+2) *3;
          arr2[i]=(i+1)*2;
          std_log(LOG_FILENAME_LINE,"arr1[%d]=%d,arr2[%d]=%d", i,arr1[i],i,arr2[i]);
        }
    
    oil_subtract_s16(res1,arr1,arr2,MAX_SIZE);
    
    for(i=0;i<MAX_SIZE;i++)
        {
        if(res1[i] == check[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_subtract_s16 successful,res1[%d]=%d", i,res1[i]);
            }
        else
            {
             assert_failed = 1;
             std_log(LOG_FILENAME_LINE,"oil_subtract_s16 unsuccessful, Expected =%d,Obtained =%d",check[i],res1[i]);
            }
        }  
    }

void test_sub_s16_u8()
    {
    int i;
    int16_t check[MAX_SIZE]={6,7,8,9,10,11,12,13,14,15};
    for(i=0;i<MAX_SIZE;i++)
        {
          res1[i]=0;
          arr1[i]=(i+2)*3;
          uarr1[i]=i*2;
          std_log(LOG_FILENAME_LINE,"arr1[%d]=%d, uarr1[%d]=%d",i,arr1[i],i,uarr1[i]);
        }
    oil_subtract_s16_u8(res1,arr1,uarr1,MAX_SIZE);
    
    for(i=0;i<MAX_SIZE;i++)
        {
        if(res1[i] == check[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_subtract_s16_u8 successful, res1[%d] = %d", i,res1[i]);
            }
        else
            {
             assert_failed = 1;
             std_log(LOG_FILENAME_LINE,"oil_subtract_s16_u8 unsuccessful, Expected =%d,Obtained =%d",check[i],res1[i]);
            }
        }  
    }



void test_sub_f32()
    {
    int i;
    float fcheck[MAX_SIZE1]={6.69999980926514,8.89999961853027,11.10000038146973,13.30000019073486,15.50000000000000,17.70000076293945,19.89999961853027,22.10000038146973};
    for(i=0;i<MAX_SIZE1;i++)
        {
          fres1[i]=0;
          farr1[i]=(i+2.5)*3;
          farr2[i]=(i+1)*0.8;
          std_log(LOG_FILENAME_LINE,"farr1[%d]=%f,farr2[%d]=%f", i,farr1[i],i,farr2[i]);
        }
    oil_subtract_f32(fres1,farr1,farr2,MAX_SIZE1);
    
    for(i=0;i<MAX_SIZE1;i++)
        {
        if(comparefloats(fres1[i],fcheck[i])==0)
            {
            std_log(LOG_FILENAME_LINE,"oil_subtract_f32 successful,fres1[%d]=%15.14f", i,fres1[i]);
            }
        else
            {
             assert_failed = 1;
             std_log(LOG_FILENAME_LINE,"oil_subtract_f32 unsuccessful,Expected =%15.14f,Obtained =%15.14f",fcheck[i],fres1[i]);
            }
        }  
    }


void test_sub_f64()
    {
    int i;
    double dcheck[MAX_DSIZE]={7.28100000000000,10.44100000000000,13.60100000000000,16.76100000000000,19.92100000000000};
    for(i=0;i<MAX_DSIZE;i++)
        {
          dres1[i]=0;
          darr1[i]=(i+2.25)*3.34;
          darr2[i]=(i+1.3)*0.18;
          std_log(LOG_FILENAME_LINE,"darr1[%d] = %15.14f, darr2[%d] = %15.14f", i,darr1[i],i,darr2[i]);
        }
    oil_subtract_f64(dres1,darr1,darr2,MAX_DSIZE);
    
    for(i=0;i<MAX_DSIZE;i++)
        {
        if(comparefloats(dres1[i],dcheck[i])==0)
            {
            std_log(LOG_FILENAME_LINE,"oil_subtract_f64 successful, dres1[%d] = %15.14f", i,dres1[i]);
            }
        else
            {
             assert_failed = 1;
             std_log(LOG_FILENAME_LINE,"oil_subtract_f64 unsuccessful, Expected =%15.14f,Obtained =%15.14f",dcheck[i],dres1[i]);
            } 
        }  
    }


void test_mult_f32()
    {
    int i;
    float fcheck[MAX_SIZE1]={0.66799998283386,2.00400018692017,4.00799989700317,6.68000030517578,10.02000045776367,14.02799987792969,18.70399856567383,24.04800033569336};
    for(i=0;i<MAX_SIZE1;i++)
        {
          fres1[i]=0;
          farr1[i]=(i+2)*3.34;
          farr2[i]=(i+1)*0.1;
          std_log(LOG_FILENAME_LINE,"farr1[%d] = %f, farr2[%d] = %f", i,farr1[i],i,farr2[i]);
        }
    oil_multiply_f32(fres1,farr1,farr2,MAX_SIZE1);
    
    for(i=0;i<MAX_SIZE1;i++)
        {
        if(comparefloats(fres1[i],fcheck[i])==0)
            {
              std_log(LOG_FILENAME_LINE,"oil_multiply_f32 successful, fres1[%d] = %15.14f", i,fres1[i]);
             }
           else
               {
                assert_failed = 1;
                std_log(LOG_FILENAME_LINE,"oil_multiply_f32 unsuccessful, Expected =%15.14f,Obtained =%15.14f",fcheck[i],fres1[i]);
               } 
        }
    }

void test_mult_f64()
    {
    int i;
    double dcheck[MAX_DSIZE]={1.71943200000000,4.42483200000000,8.33263200000000,13.44283200000000,19.75543200000000};
    for(i=0;i<MAX_DSIZE;i++)
         {
           dres1[i]=0;
           darr1[i]=(i+2.2)*3.34;
           darr2[i]=(i+1.3)*0.18;
           std_log(LOG_FILENAME_LINE,"darr1[%d] = %15.14f, darr2[%d] = %15.14f", i,darr1[i],i,darr2[i]);
         }
    oil_multiply_f64(dres1,darr1,darr2,MAX_DSIZE);
     
    for(i=0;i<MAX_DSIZE;i++)
         {
         if(comparefloats(dres1[i],dcheck[i])==0)
             {
             std_log(LOG_FILENAME_LINE,"oil_multiply_f64 successful, dres1[%d] = %15.14f", i,dres1[i]);
             }
         else
             {
              assert_failed = 1;
              std_log(LOG_FILENAME_LINE,"oil_multiply_f64 unsuccessful, Expected =%15.14f,Obtained =%15.14f",dcheck[i],dres1[i]);
             } 
         }  
    }


void test_div_f32()
    {
    int i;
    float fcheck[MAX_SIZE1]={60.00000000000000,45.00000000000000,40.00000000000000,37.50000000000000,36.00000000000000,35.00000000000000,34.28571319580078,33.75000000000000};
    for(i=0;i<MAX_SIZE1;i++)
        {
          fres1[i]=0;
          farr1[i]=(i+2)*3;
          farr2[i]=(i+1)*0.1;
          std_log(LOG_FILENAME_LINE,"farr1[%d] = %f, farr2[%d] = %f", i,farr1[i],i,farr2[i]);
        }
    oil_divide_f32(fres1,farr1,farr2,MAX_SIZE1);
    
    for(i=0;i<MAX_SIZE1;i++)
        {
        if(comparefloats(fres1[i],fcheck[i])==0)
            {
              std_log(LOG_FILENAME_LINE,"oil_divide_f32 successful, fres1[%d] = %15.14f", i,fres1[i]);
              }
           else
               {
                assert_failed = 1;
                std_log(LOG_FILENAME_LINE,"oil_divide_f32 unsuccessful, Expected =%15.14f,Obtained =%15.14f",fcheck[i],fres1[i]);
               } 
        }
    }

void test_div_f64()
    {
    int i;
    double dcheck[MAX_DSIZE]={2.82615384615385,2.32347826086957,2.12545454545455,2.01953488372093,1.95358490566038};
    for(i=0;i<MAX_DSIZE;i++)
         {
           dres1[i]=0;
           darr1[i]=(i+2.2)*3.34;
           darr2[i]=(i+1.3)*2;
           std_log(LOG_FILENAME_LINE,"darr1[%d] = %15.14f, darr2[%d] = %15.14f", i,darr1[i],i,darr2[i]);
         }
    oil_divide_f64(dres1,darr1,darr2,MAX_DSIZE);
     
    for(i=0;i<MAX_DSIZE;i++)
         {
         if(comparefloats(dres1[i],dcheck[i])==0)
             {
             std_log(LOG_FILENAME_LINE,"oil_divide_f64 successful, dres1[%d] = %15.14f", i,dres1[i]);
             }
         else
             {
              assert_failed = 1;
              std_log(LOG_FILENAME_LINE,"oil_divide_f64 unsuccessful, Expected =%15.14f,Obtained =%15.14f",dcheck[i],dres1[i]);
             } 
         }  
    }



void test_minimum_f32()
    {
    int i;
    float fmarr1[MAX_SIZE] = {2.10, 0.23123, 532.2, 1, 0908.34, 23432.1, 11.34, 0.0002, 87.2324, 700};
    float fmarr2[MAX_SIZE] = { 86.3423, 1231.11, 65457.2, 0, 10.3, 700, 2.10, 8967.21, 3423.23,211.076 };
    float fmin1[MAX_SIZE];
    float fmcheck[MAX_SIZE]={2.09999990463257,0.23123000562191,532.20001220703125,0.00000000000000,10.30000019073486,700.00000000000000,2.09999990463257,0.00019999999495,87.23239898681641,211.07600402832031};
    for(i=0;i<MAX_SIZE;i++)
        {
          fmin1[i]=0;
          std_log(LOG_FILENAME_LINE,"fmarr1[%d] = %f, fmarr2[%d] = %f", i,fmarr1[i],i,fmarr2[i]);
        }
    oil_minimum_f32(fmin1,fmarr1,fmarr2,MAX_SIZE);
    
    for(i=0;i<MAX_SIZE;i++)
        {
        if(comparefloats(fmin1[i],fmcheck[i])==0)
            {
              std_log(LOG_FILENAME_LINE,"oil_minimum_f32 successful, fmin1[%d] = %15.14f", i,fmin1[i]);
             }
         else
            {
              assert_failed = 1;
              std_log(LOG_FILENAME_LINE,"oil_minimum_f32 unsuccessful, Expected =%15.14f,Obtained =%15.14f",fmcheck[i],fmin1[i]);
            } 
        }
    }


void test_minimum_f64()
    {
    int i;
    float dmarr1[MAX_SIZE] = {2324235.10, 0231.3223, 532.2, 1.3423421, 23432.1, 11.3423424, 340.0002, 87342.3424,3243.12,0};
    float dmarr2[MAX_SIZE] = { 2386.3423, 11131.11, 6545.2, 230, 110.31, 400.700, 122.10, 892467.22, 24323.23,3243.12 };
    float dmin1[MAX_SIZE];
    float dmcheck[MAX_SIZE]={2386.34228515625000,231.32229614257812,532.20001220703125,1.34234213829041,110.30999755859375,11.34234237670898,122.09999847412109,87342.34375000000000,3243.12011718750000,0.00000000000000};
    for(i=0;i<MAX_SIZE;i++)
        {
          dmin1[i]=0;
          std_log(LOG_FILENAME_LINE,"fmarr1[%d] = %15.14f, fmarr2[%d] = %15.14f", i,dmarr1[i],i,dmarr2[i]);
        }
    oil_minimum_f64(dmin1,dmarr1,dmarr2,MAX_SIZE);
    
    for(i=0;i<MAX_SIZE;i++)
        {
        if(comparefloats(dmin1[i],dmcheck[i])==0)
            {
              std_log(LOG_FILENAME_LINE,"oil_minimum_f64 successful, dmin1[%d] = %15.14f", i,dmin1[i]);
            }
        else
            {
              assert_failed = 1;
              std_log(LOG_FILENAME_LINE,"oil_minimum_f64 unsuccessful, Expected =%15.14f,Obtained =%15.14f",dmcheck[i],dmin1[i]);
            } 
        }
    }



void test_maximum_f32()
    {
    int i;
    float fmarr1[MAX_SIZE] = {2.10, 0.23123, 532.2, 1, 0908.34, 23432.1, 11.34, 0.0002, 87.2324, 700};
    float fmarr2[MAX_SIZE] = { 86.3423, 1231.11, 65457.2, 0, 10.3, 700, 2.10, 8967.21, 3423.23,211.076 };
    float fmax1[MAX_SIZE];
    float fmcheck[MAX_SIZE]={86.34230041503906,1231.10998535156250,65457.19921875000000,1.00000000000000,908.34002685546875,23432.09960937500000,11.34000015258789,8967.20996093750000,3423.22998046875000,700.00000000000000};
    for(i=0;i<MAX_SIZE;i++)
        {
          fmax1[i]=0;
          std_log(LOG_FILENAME_LINE,"fmarr1[%d] = %f, fmarr2[%d] = %f", i,fmarr1[i],i,fmarr2[i]);
        }
    oil_maximum_f32(fmax1,fmarr1,fmarr2,MAX_SIZE);
    
    for(i=0;i<MAX_SIZE;i++)
        {
        if(comparefloats(fmax1[i],fmcheck[i])==0)
            {
              std_log(LOG_FILENAME_LINE,"oil_maximum_f32 successful, fmax1[%d] = %15.14f", i,fmax1[i]);
            }
        else
            {
              assert_failed = 1;
              std_log(LOG_FILENAME_LINE,"oil_maximum_f32 unsuccessful,Expected =%15.14f,Obtained =%15.14f",fmcheck[i],fmax1[i]);
            } 
        }
    }


void test_maximum_f64()
    {
    int i;
    float dmarr1[MAX_SIZE] = {2324235.10, 0231.3223, 532.2, 1.3423421, 23432.1, 11.3423424, 340.0002, 87342.3424,3243.12};
    float dmarr2[MAX_SIZE] = {2386.3423, 11131.11, 6545.2, 230, 110.31, 400.700, 122.10, 892467.22, 24323.23,3243.12 };
    float dmax1[MAX_SIZE];
    float dmcheck[MAX_SIZE]={2324235.00000000000000,11131.11035156250000,6545.20019531250000,230.00000000000000,23432.09960937500000,400.70001220703125,340.00021362304688,892467.25000000000000,24323.23046875000000,3243.12011718750000};
    for(i=0;i<MAX_SIZE;i++)
        {
          dmax1[i]=0;
          std_log(LOG_FILENAME_LINE,"fmarr1[%d] = %15.14f, fmarr2[%d] = %15.14f", i,dmarr1[i],i,dmarr2[i]);
        }
    oil_maximum_f64(dmax1,dmarr1,dmarr2,MAX_SIZE);
    
    for(i=0;i<MAX_SIZE;i++)
        {
        if(comparefloats(dmax1[i],dmcheck[i])==0)
           {
            std_log(LOG_FILENAME_LINE,"oil_maximum_f64 successful, dmax1[%d] = %15.14f", i,dmax1[i]);
           }
        else
          {
           assert_failed = 1;
           std_log(LOG_FILENAME_LINE,"oil_maximum_f64 unsuccessful, Expected =%15.14f,Obtained =%15.14f",dmcheck[i],dmax1[i]);
          } 
        }
    }


void test_negative_f32()
    {
    int i;
    float fcheck[MAX_SIZE1]={-6.19999980926514,-9.30000019073486,-12.39999961853027,-15.50000000000000,-18.60000038146973,-21.70000076293945,-24.79999923706055,-27.89999961853027};
    for(i=0;i<MAX_SIZE1;i++)
        {
          fres1[i]=0;
          farr1[i]=(i+2)*3.1;
          std_log(LOG_FILENAME_LINE,"farr1[%d] = %f", i,farr1[i]);
        }
    oil_negative_f32(fres1,farr1,MAX_SIZE1);
    
    for(i=0;i<MAX_SIZE1;i++)
        {
        if(comparefloats(fres1[i],fcheck[i])==0)
            {
              std_log(LOG_FILENAME_LINE,"oil_negative_f32 successful, fres1[%d] = %15.14f", i,fres1[i]);
            }
           else
            {
             assert_failed = 1;
             std_log(LOG_FILENAME_LINE,"oil_negative_f32 unsuccessful, Expected =%15.14f,Obtained =%15.14f",fcheck[i],fres1[i]);
            } 
        }
    }



void test_inverse_f32()
    {
    int i;
    float fcheck[MAX_SIZE1]={1.00000000000000,0.66666668653488,0.50000000000000,0.40000000596046,0.33333334326744,0.28571429848671,0.25000000000000,0.22222222387791};
    for(i=0;i<MAX_SIZE1;i++)
        {
          fres1[i]=0;
          farr1[i]=(i+2)*0.5;
          std_log(LOG_FILENAME_LINE,"farr1[%d] = %f", i,farr1[i]);
        }
    oil_inverse_f32(fres1,farr1,MAX_SIZE1);
    
    for(i=0;i<MAX_SIZE1;i++)
        {
        if(comparefloats(fres1[i],fcheck[i])==0)
            {
             std_log(LOG_FILENAME_LINE,"oil_inverse_f32 successful, fres1[%d] = %15.14f", i,fres1[i]);
            }
        else
           {
            assert_failed = 1;
            std_log(LOG_FILENAME_LINE,"oil_inverse_f32 unsuccessful, Expected =%15.14f,Obtained =%15.14f",fcheck[i],fres1[i]);
           } 
        }
    }


void test_sign_f32()
    {
    int i;
    float fcheck[MAX_SIZE1]={0.30000001192093,3.29999995231628,6.30000019073486,9.30000019073486,12.30000019073486,15.30000019073486,18.29999923706055,21.29999923706055};
    for(i=0;i<MAX_SIZE1;i++)
        {
          fres1[i]=0;
          farr1[i]=(i+0.1)*3;
          std_log(LOG_FILENAME_LINE,"farr1[%d] = %f", i,farr1[i]);
        }
    oil_sign_f32(fres1,farr1,MAX_SIZE1);
    
    for(i=0;i<MAX_SIZE1;i++)
        {
        if(comparefloats(fres1[i],fcheck[i])==0)
            {
             std_log(LOG_FILENAME_LINE,"oil_sign_f32 successful, fres1[%d] = %15.14f", i,fres1[i]);
            }
        else
           {
            assert_failed = 1;
            std_log(LOG_FILENAME_LINE,"oil_sign_f32 unsuccessful, Expected =%15.14f,Obtained =%15.14f",fcheck[i],fres1[i]);
           } 
        }
    }


void test_floor_f32()
    {
    int i;
    float fcheck[MAX_SIZE1]={3.00000000000000,7.00000000000000,10.00000000000000,14.00000000000000,17.00000000000000,21.00000000000000,24.00000000000000,28.00000000000000};
    for(i=0;i<MAX_SIZE1;i++)
        {
          fres1[i]=0;
          farr1[i]=(i+1)*3.5;
          std_log(LOG_FILENAME_LINE,"farr1[%d] = %f", i,farr1[i]);
        }
    oil_floor_f32(fres1,farr1,MAX_SIZE1);
    
    for(i=0;i<MAX_SIZE1;i++)
        {
        if(comparefloats(fres1[i],fcheck[i])==0)
          {
           std_log(LOG_FILENAME_LINE,"oil_floor_f32 successful, fres1[%d] = %15.14f", i,fres1[i]);
          }
        else
         {
           assert_failed = 1;
           std_log(LOG_FILENAME_LINE,"oil_floor_f32 unsuccessful, Expected =%15.14f,Obtained =%15.14f",fcheck[i],fres1[i]);
         } 
        }
    }

void test_scalaradd_f32()
    {
    int i;
    float fcheck[MAX_SIZE1]={18.20000076293945,21.20000076293945,24.20000076293945,27.20000076293945,30.20000076293945,33.20000076293945,36.20000076293945,39.20000076293945}, add1[8] = {3.2};
    for(i=0;i<MAX_SIZE1;i++)
        {
          fres1[i]=0;
          farr1[i]=(i+5)*3;
          std_log(LOG_FILENAME_LINE,"farr1[%d] = %f", i,farr1[i]);
        }
    oil_scalaradd_f32_ns(fres1,farr1,add1,MAX_SIZE1);
    
    for(i=0;i<MAX_SIZE1;i++)
        {
        if(comparefloats(fres1[i],fcheck[i])==0)
            {
              std_log(LOG_FILENAME_LINE,"oil_scalaradd_f32_ns successful, fres1[%d] = %15.14f", i,fres1[i]);
            }
         else
            {
             assert_failed = 1;
             std_log(LOG_FILENAME_LINE,"oil_scalaradd_f32_ns unsuccessful, Expected =%15.14f,Obtained =%15.14f",fcheck[i],fres1[i]);
           } 
        }
    }


void test_scalarmult_f64()
    {
    int i;
    double dcheck[MAX_DSIZE]={9.00000000000000,15.00000000000000,21.00000000000000,27.00000000000000,33.00000000000000}, mul1[MAX_DSIZE] = {3};
    for(i=0;i<MAX_DSIZE;i++)
        {
          dres1[i]=0;
          darr1[i]=(i+1.5)*2;
          std_log(LOG_FILENAME_LINE,"darr1[%d] = %15.14f", i,darr1[i]);
        }
    oil_scalarmultiply_f64_ns(dres1,darr1,mul1,MAX_DSIZE);
    
    for(i=0;i<MAX_DSIZE;i++)
        {
        if(comparefloats(dres1[i],dcheck[i])==0)
            {
             std_log(LOG_FILENAME_LINE,"oil_scalarmultiply_f64_ns successful, dres1[%d] = %15.14f", i,dres1[i]);
            }
         else
            {
             assert_failed = 1;
             std_log(LOG_FILENAME_LINE,"oil_scalarmultiply_f64_ns unsuccessful, Expected =%15.14f,Obtained =%15.14f",dcheck[i],dres1[i]);
            } 
        }
    }


int main ()
{

  std_log(LOG_FILENAME_LINE,"Test started testsuite_math");
  oil_init (); 
  
  std_log(LOG_FILENAME_LINE,"ADD_S16 TEST");
  test_add_s16();
  
  std_log(LOG_FILENAME_LINE,"ADD_S16_U8 TEST");
  test_add_s16_u8();

  std_log(LOG_FILENAME_LINE,"ADD_F32 TEST");
  test_add_f32();
  
  std_log(LOG_FILENAME_LINE,"ADD_F64 TEST");
  test_add_f64();

  std_log(LOG_FILENAME_LINE,"SUB_S16 TEST");
  test_sub_s16();  

  std_log(LOG_FILENAME_LINE,"SUB_S16_U8 TEST");
  test_sub_s16_u8();
  
  std_log(LOG_FILENAME_LINE,"SUB_F32 TEST");
  test_sub_f32();

  std_log(LOG_FILENAME_LINE,"SUB_F64 TEST");
  test_sub_f64();
  
  std_log(LOG_FILENAME_LINE,"MULT_F32 TEST");
  test_mult_f32();
  
  std_log(LOG_FILENAME_LINE,"MULT_F64 TEST");
  test_mult_f64();
  
  std_log(LOG_FILENAME_LINE,"DIV_F32 TEST");
  test_div_f32();
  
  std_log(LOG_FILENAME_LINE,"DIV_F64 TEST");
  test_div_f64();

  std_log(LOG_FILENAME_LINE,"MIN_F32 TEST");
  test_minimum_f32();

  std_log(LOG_FILENAME_LINE,"MIN_F64 TEST");
  test_minimum_f64();
  
  std_log(LOG_FILENAME_LINE,"MAX_F32 TEST");
  test_maximum_f32();

  std_log(LOG_FILENAME_LINE,"MAX_F64 TEST");
  test_maximum_f64();
  
  std_log(LOG_FILENAME_LINE,"NEGATIVE_F32 TEST");
  test_negative_f32();

  std_log(LOG_FILENAME_LINE,"SIGN_F32 TEST");
  test_sign_f32();
  
  std_log(LOG_FILENAME_LINE,"INVERSE_F32 TEST");
  test_inverse_f32();

  std_log(LOG_FILENAME_LINE,"FLOOR_F32 TEST");
  test_floor_f32();
  
  std_log(LOG_FILENAME_LINE,"SCALARADD_F32 TEST");
  test_scalaradd_f32();
  
  std_log(LOG_FILENAME_LINE,"SCALARMULT_F64 TEST");
  test_scalarmult_f64();
  
   if(assert_failed)
         std_log(LOG_FILENAME_LINE,"Test Fail");
   else
         std_log(LOG_FILENAME_LINE,"Test Successful");
   
  create_xml(0);
  return 0;
}

