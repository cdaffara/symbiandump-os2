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

#define LOG_FILE "c:\\logs\\testsuite_misc8x8_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#define MAX_SIZE 64


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_misc8x8");
    close_log_file();
}

void test_clipconv8x8_u8_s16()
    {
    int16_t arr[MAX_SIZE];
    uint8_t res[MAX_SIZE], check[MAX_SIZE]={2,4,6,4,6,8,6,8,10,8,10,12,10,12,14,12,14,16,14,16,18,16,18,20,22,24,26,28,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
	{
          arr[i]=(i+1)*2;
		  res[i]=0;
	}
        
    oil_clipconv8x8_u8_s16(res,3,arr,2);
    
    std_log(LOG_FILENAME_LINE,"OUTPUT");
    
	for(i=0;i<MAX_SIZE;i++)
    if(res[i] == check[i])
        {
         std_log(LOG_FILENAME_LINE,"oil_clipconv8x8_u8_s16 successful, res[%d] = %d", i,res[i]); 
        }
    else
        {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_clipconv8x8_u8_s16 unsuccessful, Expected =%d,Obtained =%d",check[i],res[i]);
        }
    }


void test_dequantize8x8_s16()
    {
    int16_t arr1[MAX_SIZE],arr2[MAX_SIZE],res[MAX_SIZE],check[MAX_SIZE]={4,18,40,70,96,126,160,198,240,30,80,134,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
        {
          arr1[i]=(i+1)*2;
          arr2[i]=i+2;
		  res[i]=0;
        }
    
    oil_dequantize8x8_s16(res,1,arr1,2,arr2,1);
    
	for(i=0;i<MAX_SIZE;i++)
    if(res[i] == check[i])
        {
         std_log(LOG_FILENAME_LINE,"oil_dequantize8x8_s16 successful, res[%d] = %d", i,res[i]); 
        }
    else
        {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_dequantize8x8_s16 unsuccessful,Expected =%d,Obtained =%d",check[i],res[i]);
        }
    }


int main()
    {
    std_log(LOG_FILENAME_LINE,"Test started testsuite_misc8x8");
    oil_init ();

    std_log(LOG_FILENAME_LINE,"oil_clipconv8x8_u8_s16");
    test_clipconv8x8_u8_s16(); 

    std_log(LOG_FILENAME_LINE,"oil_dequantize8x8_s16");
    test_dequantize8x8_s16();
      
    if(assert_failed)
           std_log(LOG_FILENAME_LINE,"Test Fail");
    else
           std_log(LOG_FILENAME_LINE,"Test Successful");
    create_xml(0);
    return 0;
    }
