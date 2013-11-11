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

#define LOG_FILE "c:\\logs\\testsuite_rowcoladd8x8_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#define MAX_SIZE 64
#define RES_SIZE 1

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_rowcoladd8x8");
    close_log_file();
}

void test_colsad8x8_u8()
    {
    uint8_t arr1[MAX_SIZE],arr2[MAX_SIZE];
    uint32_t res[RES_SIZE]={0}, check[RES_SIZE]={30};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
        {
         arr1[i]=i+3;
         arr2[i]=i+1;
        }
    oil_colsad8x8_u8(res,arr1,3,arr2,2);
    
    if(res[0]==check[0])
        {
        std_log(LOG_FILENAME_LINE,"oil_colsad8x8_u8 successful, res = %d", res[0]);
        }
    else
        {
        assert_failed=1;
        std_log(LOG_FILENAME_LINE,"oil_colsad8x8_u8 unsuccessful,Expected =%d,Obtained =%d",check[0],res[0]);
        }
    }


void test_rowsad8x8_u8()
    {
    uint8_t arr1[MAX_SIZE],arr2[MAX_SIZE];
    uint32_t res[RES_SIZE]={0}, check[RES_SIZE]={12};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
        {
         arr1[i]=i+5;
         arr2[i]=i+2;
        }
    
    oil_rowsad8x8_u8(res,arr1,arr2);
    
    if(res[0]==check[0])
        {
        std_log(LOG_FILENAME_LINE,"oil_rowsad8x8_u8 successful, res = %d", res[0]);
        }
    else
        {
        assert_failed=1;
        std_log(LOG_FILENAME_LINE,"oil_rowsad8x8_u8 unsuccessful,Expected =%d,Obtained =%d",check[0],res[0]);
        }
    }


int main ()
{
  std_log(LOG_FILENAME_LINE,"Test started testsuite_rowcoladd8x8");
  oil_init ();
  
  std_log(LOG_FILENAME_LINE,"oil_colsad8x8_u8");
  test_colsad8x8_u8();
  
  std_log(LOG_FILENAME_LINE,"oil_rowsad8x8_u8");
  test_rowsad8x8_u8();
    
  if(assert_failed)
         std_log(LOG_FILENAME_LINE,"Test Fail");
  else
         std_log(LOG_FILENAME_LINE,"Test Successful");
   
  create_xml(0);
  return 0;
}

