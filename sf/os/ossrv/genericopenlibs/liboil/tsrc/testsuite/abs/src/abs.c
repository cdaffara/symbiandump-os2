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


#include <liboil/liboil.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>
#include <stdlib.h>

#include <liboil/globals.h>

#define LOG_FILE "c:\\logs\\testsuite_abs_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_abs");
    close_log_file();
}

void test(void)
{
  int i;
  float *src;
  float *dest;

  src = malloc(100*sizeof(float));
  dest = malloc(100*sizeof(float));

  for(i=0;i<100;i++){
    src[i] = (i-50)*100;
  }

  //_oil_function_class_ptr_abs_f32_f32(dest, 4, src, 4, 100);
  oil_abs_f32_f32 (dest, 4, src, 4, 100);
   
 
  for (i=0; i<100; i++) {
      float f = (src[i] < 0) ? -1*src[i] : src[i]; 
      if(dest[i] != f){
      std_log(LOG_FILENAME_LINE,"Failed at %g", src[i]);
          assert_failed = 1;
      }
      std_log(LOG_FILENAME_LINE,"%d %g %g\n", i, src[i], dest[i]);
  }
}

int main (int argc, char *argv[])
{
  OilFunctionClass *klass;
  OilFunctionImpl *impl;

  std_log(LOG_FILENAME_LINE, "Test Started testsuite_abs");
  oil_init ();

  klass = (OilFunctionClass *)oil_class_get ("abs_f32_f32");
  oil_class_optimize(klass);

  std_log(LOG_FILENAME_LINE, "class=%s\n", klass->name);
  for (impl = klass->first_impl; impl; impl=impl->next) {
      std_log(LOG_FILENAME_LINE,"impl=%p\n", impl);
      std_log(LOG_FILENAME_LINE,"  func=%p\n", impl->func);
      std_log(LOG_FILENAME_LINE,"  name=%s\n", impl->name);
      std_log(LOG_FILENAME_LINE,"  flags=%08x\n", impl->flags);

  }
  impl = klass->chosen_impl;
  std_log(LOG_FILENAME_LINE,"chosen=%p\n", impl);
  impl = klass->reference_impl;
  std_log(LOG_FILENAME_LINE,"ref=%p\n", impl);

  test();

  if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Fail");
  else
      std_log(LOG_FILENAME_LINE,"Test Successful");
  create_xml(0);
  return 0;
}

