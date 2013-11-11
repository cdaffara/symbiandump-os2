/*
 * LIBOIL - Library of Optimized Inner Loops
 * Copyright (c) 2004 David A. Schleef <ds@schleef.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <liboil/liboil.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <liboil/liboilprototype.h>
#include <liboil/globals.h>

#define LOG_FILE "c:\\logs\\testsuite_dso_check_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_dso_check");
    close_log_file();
}

int main (int argc, char *argv[])
{
  OilFunctionClass *klass;
  int i;
  int n;

  std_log(LOG_FILENAME_LINE,"Test started testsuite_dso_check");
  oil_init ();

  //std_log(LOG_FILENAME_LINE,"null is at %p\n", &oil_function_class_ptr_null);
  oil_null ();

  n = oil_class_get_n_classes ();
  std_log(LOG_FILENAME_LINE,"No. of classes %d\n", n);
  assert_failed = (n == 415) ? assert_failed : 1; 
  
  for (i=0;i<n; i++ ){
    klass = oil_class_get_by_index(i);
    assert_failed = (klass && klass->name && klass->chosen_impl && klass->prototype) ? assert_failed : 1; 
    std_log(LOG_FILENAME_LINE,"%d %p %s\n", i, klass, klass->name);
  }

  if(assert_failed)
          std_log(LOG_FILENAME_LINE,"Test Fail");
  else
          std_log(LOG_FILENAME_LINE,"Test Successful");
  create_xml(0);
  return 0;
}


