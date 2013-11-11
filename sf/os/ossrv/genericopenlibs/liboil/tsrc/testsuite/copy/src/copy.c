/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <liboil/liboil.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>
#include <string.h>
#include <liboil/globals.h>

#define LOG_FILE "c:\\logs\\testsuite_copy_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_copy");
    close_log_file();
}

uint8_t dest[200];
uint8_t src[200];


void test(void)
{
  int i;

  for(i=0;i<200;i++){
    dest[i]=0;
    src[i]=i;
  }

  oil_copy_u8 (dest + 8, src + 0, 64);

  for(i=0;i<100;i++){
      if(i<8 || i>71)
          assert_failed = (dest[i] == 0) ? assert_failed : 1; 
      else
          assert_failed = (dest[i] == (i-8)) ? assert_failed : 1;
      std_log(LOG_FILENAME_LINE,"%d\n",dest[i]);
  }
}

int main (int argc, char *argv[])
{
  OilFunctionClass *klass;
  OilFunctionImpl *impl;

  std_log(LOG_FILENAME_LINE,"Test started testsuite_copy");
  oil_init ();

  klass = (OilFunctionClass *)oil_class_get ("copy_u8");
  oil_class_optimize(klass);

  oil_class_choose_by_name (klass, "copy_u8_altivec");
  impl = klass->chosen_impl;
  if (oil_impl_is_runnable (impl)) {
      std_log(LOG_FILENAME_LINE,"chosen=%p\n", impl);
    impl = klass->reference_impl;
    std_log(LOG_FILENAME_LINE,"ref=%p\n", impl);
    test();
  }

  if(assert_failed)
        std_log(LOG_FILENAME_LINE,"Test Fail");
  else
        std_log(LOG_FILENAME_LINE,"Test Successful");
  create_xml(0);
  return 0;
}

