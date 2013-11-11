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

#define LOG_FILE "c:\\logs\\testsuite_splat_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#define MAX_SIZE 10
#define SIZE 1


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_splat");
    close_log_file();
}

void test_splat_u16_ns()
    {
    int i;
    uint16_t sp1[SIZE]={10},res[MAX_SIZE];
    uint16_t check[MAX_SIZE]={10,10,10,10,10,10,10,10,10,10};
    
    for(i=0;i<MAX_SIZE;i++)
        res[i]=0;
        
    oil_splat_u16_ns(res,sp1,MAX_SIZE);
    
    for(i=0;i<MAX_SIZE;i++)
    if(check[i] == res[i])
          {
          std_log(LOG_FILENAME_LINE,"oil_splat_u16_ns successful, res[%d]=%d", i,res[i]);
          }
     else
         {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_splat_u16_ns unsuccessful, Expected =%d,Obtained =%d",check[i],res[i]);
         }
    }


void test_splat_u32()
    {
    int i;
    uint32_t sp1[SIZE]={5},res[MAX_SIZE];
    uint32_t check[MAX_SIZE]={84215045,84215045,1285,0,0,0,0,0,0,0};
    
    for(i=0;i<MAX_SIZE;i++)
        res[i]=0;
        
    oil_splat_u32(res,1,sp1,MAX_SIZE);
       
    for(i=0;i<MAX_SIZE;i++)
    if(check[i] == res[i])
          {
          std_log(LOG_FILENAME_LINE,"oil_splat_u32 successful,res[%d]=%d",i,res[i]);
          }
     else
         {
         assert_failed=1;
         std_log(LOG_FILENAME_LINE,"oil_splat_u32 unsuccessful, Expected =%d,Obtained =%d",check[i],res[i]);
         }
    }

int main ()
{
  std_log(LOG_FILENAME_LINE,"Test started testsuite_splat");
  oil_init ();
  
  std_log(LOG_FILENAME_LINE,"oil_splat_u16_ns");
  test_splat_u16_ns();
  
  std_log(LOG_FILENAME_LINE,"oil_splat_u32");
  test_splat_u32();
  
  if(assert_failed)
         std_log(LOG_FILENAME_LINE,"Test Fail");
  else
         std_log(LOG_FILENAME_LINE,"Test Successful");
   
  create_xml(0);
  return 0;
}

