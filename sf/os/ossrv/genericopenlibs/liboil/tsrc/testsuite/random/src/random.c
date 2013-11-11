/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <liboil/liboil.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>
#include <stdlib.h>

#include <liboil/globals.h>

#define LOG_FILE "c:\\logs\\testsuite_random_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define SIZE    20

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_random");
    close_log_file();
}

void test_oil_random_s64()
    {
    int i = 0;
    int pos = SIZE;
    
    oil_type_s64 output1[SIZE];
    oil_type_s64 output2[SIZE];
    
    oil_random_s64(output1, SIZE);
    oil_random_s64(output2, SIZE);
    
    for(i=0; i<SIZE; i++)
        {
        if(output1[i] != output2[i])
            pos = i;
        }
    
    if(pos == SIZE)
        {
        std_log(LOG_FILENAME_LINE,"oil_random_s64 returns same array everytime");
        assert_failed = 1;
        }
    }

void test_oil_random_u64()
    {
    int i = 0;
    int pos = SIZE;
    
    oil_type_u64 output1[SIZE];
    oil_type_u64 output2[SIZE];
    
    oil_random_u64(output1, SIZE);
    oil_random_u64(output2, SIZE);
    
    for(i=0; i<SIZE; i++)
        {
        if(output1[i] != output2[i])
            pos = i;
        }
    
    if(pos == SIZE)
        {
        std_log(LOG_FILENAME_LINE,"oil_random_s64 returns same array everytime");
        assert_failed = 1;
        }
    }

int main (int argc, char *argv[])
{
  oil_init ();

  test_oil_random_s64();
  test_oil_random_u64();
  
  if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Failed");
  else
      std_log(LOG_FILENAME_LINE,"Test Successful");

  create_xml(0);
  return 0;
}

