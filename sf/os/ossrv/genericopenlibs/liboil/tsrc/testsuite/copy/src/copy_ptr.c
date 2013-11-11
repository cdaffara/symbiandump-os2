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

#define LOG_FILE "c:\\logs\\testsuite_copyptr_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#define MAX_SIZE 64
#define RES_SIZE 1
#define SIZE 10


uint8_t src1[MAX_SIZE], src2[MAX_SIZE];

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_copyptr");
    close_log_file();
}


void test_copy_u8()
{
  uint8_t src[SIZE],dest[SIZE],check[SIZE]={0,0,0,1,2,3,4,5,0,0};  
  int i;

  for(i=0;i<SIZE;i++)
  {
    dest[i]=0;
    src[i]=i;
  }

  oil_copy_u8 (dest+3,src+1,5);

  for(i=0;i<SIZE;i++)
      if(dest[i]==check[i])
       {
       std_log(LOG_FILENAME_LINE,"oil_copy_u8 successful, dest[%d]=%d",i,dest[i]);
       }
      else
       {
       assert_failed=1;
       std_log(LOG_FILENAME_LINE,"oil_copy_u8 unsuccessful,Expected =%d,Actual =%d",check[i],dest[i]);
       }
}

void test_copy8x8_u8()
{   
    uint8_t dest[MAX_SIZE];
    uint8_t check[MAX_SIZE] = {0,2,4,6,8,10,12,14,15,16,17,18,19,20,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
    {
      src1[i]=i;
      dest[i]=0;
      std_log(LOG_FILENAME_LINE,"src1[%d]=%d",i,src1[i]);
    }

    oil_copy8x8_u8(dest,1,src1,2);
    
    for (i=0;i<MAX_SIZE;i++)
        if(check[i] == dest[i])
          {
          std_log(LOG_FILENAME_LINE,"oil_copy8x8_u8 successful,dest[%d]=%d",i,dest[i]);
          }
        else
         {
         assert_failed = 1;
         std_log(LOG_FILENAME_LINE,"oil_copy8x8_u8 unsuccessful,Expected =%d,Actual =%d",check[i],dest[i]);
         } 
}


void test_compare_u8()
{  
  uint32_t dest1[RES_SIZE]={0},check[RES_SIZE]={10};
  int i;

  for(i=0;i<MAX_SIZE;i++)
  {
    src1[i]=i;
    src2[i]=i;
  }

  src1[10]=20;
  src2[20]=10;
  
  oil_compare_u8(dest1,src1,src2,MAX_SIZE);

  if(dest1[0] == check[0])
   {
    std_log(LOG_FILENAME_LINE,"oil_compare_u8 successful");
   }
  else
   {
   assert_failed = 1; 
   std_log(LOG_FILENAME_LINE,"oil_compare_u8 unsuccessful,Expected =%d,Actual =%d",check[0],dest1[0]);
   }
}


void test_testzero_u8()
{
    uint32_t dest1[RES_SIZE]={0},check[RES_SIZE]={15};
    int i;

    for(i=0;i<MAX_SIZE;i++)
    {
      src1[i]=i+1;
      std_log(LOG_FILENAME_LINE,"src1[%d]=%d",i,src1[i]);
    }
    
    src1[15]=0;
    
    oil_testzero_u8(dest1,src1,MAX_SIZE);
    
    if(dest1[0] == check[0])
     {
     std_log(LOG_FILENAME_LINE,"oil_testzero_u8 successful");
     }
    else
     {
      assert_failed = 1;
      std_log(LOG_FILENAME_LINE,"oil_testzero_u8 unsuccessful,Expected =%d,Actual =%d",check[0],dest1[0]);
     }
}

int main (void)
{
  std_log(LOG_FILENAME_LINE,"Test started testsuite_copyptr");
  oil_init ();

  std_log(LOG_FILENAME_LINE,"COPY");
  test_copy_u8();

  std_log(LOG_FILENAME_LINE,"COPY8X8");
  test_copy8x8_u8();

  std_log(LOG_FILENAME_LINE,"COMPARE TEST");
  test_compare_u8();
  
  std_log(LOG_FILENAME_LINE,"TESTZERO");
  test_testzero_u8();
  
  if(assert_failed)
        std_log(LOG_FILENAME_LINE,"Test Fail");
  else
        std_log(LOG_FILENAME_LINE,"Test Successful");
  create_xml(0);
  return 0;
}

