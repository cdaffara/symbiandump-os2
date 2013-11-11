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

#define LOG_FILE "c:\\logs\\testsuite_err8x8_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#define MAX_SIZE 64
#define RES_SIZE 1

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_err8x8");
    close_log_file();
}


void test_err_intra8x8_u8()
    {
    uint8_t err1[MAX_SIZE];
    uint32_t res[RES_SIZE]={0}, check[RES_SIZE]={967680};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
          err1[i]=(i+2)*3;
        
    oil_err_intra8x8_u8(res, err1,2);
    
    std_log(LOG_FILENAME_LINE,"OUTPUT");
    if(res[0] == check[0])
        {
         std_log(LOG_FILENAME_LINE,"oil_err_intra8x8_u8 successful, res = %d", res[0]); 
        }
    else
        {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_err_intra8x8_u8 unsuccessful, Expected =%d,Obtained =%d",check[0],res[0]);
        }
    }


void test_err_inter8x8_u8()
    {
    uint8_t err1[MAX_SIZE],err2[MAX_SIZE];
    uint32_t res[RES_SIZE]={0}, check[RES_SIZE]={365568};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
        {
          err1[i]=(i+2)*3;
          err2[i]=(i+1)*2;
        }
    
    oil_err_inter8x8_u8(res,err1,2,err2,1);
    
    if(res[0] == check[0])
        {
         std_log(LOG_FILENAME_LINE,"oil_err_inter8x8_u8 successful, res = %d", res[0]); 
        }
    else
        {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_err_inter8x8_u8 unsuccessful, Expected =%d,Obtained =%d",check[0],res[0]);
        }
    }

void test_err_inter8x8_u8_avg()
    {
    uint8_t err1[MAX_SIZE],err2[MAX_SIZE],err3[MAX_SIZE];
    uint32_t res[RES_SIZE]={0}, check[RES_SIZE]={821248};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
        {
          err1[i]=(i+2)*3;
          err2[i]=(i+1)*2;
          err3[i]=(i+1);
        }
    oil_err_inter8x8_u8_avg(res,err1,3,err2,err3,2);
    if(res[0] == check[0])
        {
         std_log(LOG_FILENAME_LINE,"oil_err_intra8x8_u8_avg successful, res = %d", res[0]); 
        }
    else
        {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_err_intra8x8_u8_avg unsuccessful, Expected =%d,Obtained =%d",check[0],res[0]);
        }
    }
int main()
    {
    std_log(LOG_FILENAME_LINE,"Test started testsuite_err8x8");
    oil_init ();

    std_log(LOG_FILENAME_LINE,"oil_err_intra8x8_u8");
    test_err_intra8x8_u8(); 

    std_log(LOG_FILENAME_LINE,"oil_err_inter8x8_u8");
    test_err_inter8x8_u8();
    
    std_log(LOG_FILENAME_LINE,"oil_err_intra8x8_u8_avg");
    test_err_inter8x8_u8_avg();
    
    
    if(assert_failed)
           std_log(LOG_FILENAME_LINE,"Test Fail");
    else
           std_log(LOG_FILENAME_LINE,"Test Successful");
    create_xml(0);
    return 0;
    }
