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

#define LOG_FILE "c:\\logs\\testsuite_yuv2rgb_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#define MAX_SIZE 10
#define RES_SIZE 40

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_yuv2rgb");
    close_log_file();
}


void test_yuv2rgbx_u8()
    {
    uint8_t src1[MAX_SIZE], src2[MAX_SIZE], src3[MAX_SIZE];
    uint8_t res[RES_SIZE],check[RES_SIZE]={0,135,0,0,0,134,0,0,0,134,0,0,0,134,0,0,0,134,0,0,0,134,0,0,0,134,0,0,0,134,0,0,0,134,0,0,0,134,0,0};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
	{
          src1[i]=i+1;
          src2[i]=i;
          src3[i]=i+2;
	}
    
    for(i=0;i<RES_SIZE;i++)
        res[i]=0;
        
    oil_yuv2rgbx_u8(res,src1,src2,src3,MAX_SIZE);
       
	for(i=0;i<RES_SIZE;i++)
    if(res[i] == check[i])
        {
         std_log(LOG_FILENAME_LINE,"oil_yuv2rgbx_u8 successful, res[%d] = %d", i,res[i]); 
        }
    else
        {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_yuv2rgbx_u8 unsuccessful, Expected =%d,Obtained =%d",check[i],res[i]);
        }
    }


void test_yuv2rgbx_sub2_u8()
    {
    uint8_t src1[MAX_SIZE], src2[MAX_SIZE], src3[MAX_SIZE];
    uint8_t res[RES_SIZE],check[RES_SIZE]={0,134,0,0,0,135,0,0,0,135,0,0,0,136,0,0,0,136,0,0,0,137,0,0,0,137,0,0,0,138,0,0,0,138,0,0,0,139,0,0};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
    {
          src1[i]=i;
          src2[i]=i+1;
          src3[i]=i+1;
    }
    
    for(i=0;i<RES_SIZE;i++)
        res[i]=0;
        
    oil_yuv2rgbx_sub2_u8(res,src1,src2,src3,MAX_SIZE);
    
    for(i=0;i<RES_SIZE;i++)
    if(res[i] == check[i])
        {
         std_log(LOG_FILENAME_LINE,"oil_yuv2rgbx_sub2_u8 successful, res[%d] = %d", i,res[i]); 
        }
    else
        {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_yuv2rgbx_sub2_u8 unsuccessful, Expected =%d,Obtained =%d",check[i],res[i]);
        }
    }



void test_yuv2rgbx_sub4_u8()
    {
    uint8_t src1[MAX_SIZE], src2[MAX_SIZE], src3[MAX_SIZE];
    uint8_t res[RES_SIZE],check[RES_SIZE]={0,137,0,0,0,138,0,0,0,139,0,0,0,140,0,0,0,140,0,0,0,141,0,0,0,142,0,0,0,143,0,0,0,0,0,0,0,0,0,0};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
    {
          src1[i]=i+2;
          src2[i]=i+1;
          src3[i]=i;
    }
    
    for(i=0;i<RES_SIZE;i++)
        res[i]=0;
        
    oil_yuv2rgbx_sub4_u8(res,src1,src2,src3,MAX_SIZE);
    
    for(i=0;i<RES_SIZE;i++)
    if(res[i] == check[i])
        {
         std_log(LOG_FILENAME_LINE,"oil_yuv2rgbx_sub4_u8 successful, res[%d] = %d", i,res[i]); 
        }
    else
        {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_yuv2rgbx_sub4_u8 unsuccessful,Expected =%d,Obtained =%d",check[i],res[i]);
        }
    }


int main()
    {
    std_log(LOG_FILENAME_LINE,"Test started testsuite_yuv2rgb8x8");
    oil_init ();

    std_log(LOG_FILENAME_LINE,"oil_yuv2rgbx_u8");
    test_yuv2rgbx_u8(); 
    
    std_log(LOG_FILENAME_LINE,"oil_yuv2rgbx_sub2_u8");
    test_yuv2rgbx_sub2_u8(); 
    
    std_log(LOG_FILENAME_LINE,"oil_yuv2rgbx_sub4_u8");
    test_yuv2rgbx_sub4_u8(); 

      
    if(assert_failed)
           std_log(LOG_FILENAME_LINE,"Test Fail");
    else
           std_log(LOG_FILENAME_LINE,"Test Successful");
    create_xml(0);
    return 0;
    }
