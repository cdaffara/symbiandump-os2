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
#include <liboil/liboilprototype.h>
#include <liboil/liboiltest.h>
#include <liboil/liboilcpu.h>
#include <liboil/liboilrandom.h>
#include <liboil/liboil.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define LOG_FILE "c:\\logs\\testsuite_stride_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_stride");
    close_log_file();
}

void hist(OilTest *Test);

int verbose = 0;
int fail = 0;

int main (int argc, char *argv[])
{
  OilFunctionClass *klass;
  OilFunctionImpl *impl;
  OilTest *test;
  int i;
  int n;
  int j;
  int ret;
  unsigned int cpu_flags;
  //xmlfile = "stride";
  std_log(LOG_FILENAME_LINE, "Test Started testsuite_stride");

  if (argc > 1 && strcmp(argv[1],"-v") == 0) {
    verbose = 1;
  }
  oil_init ();

  cpu_flags = oil_cpu_get_flags ();
  n = oil_class_get_n_classes ();
  
  for (i=0;i<n; i++ )
  {
      klass = oil_class_get_by_index(i);
      std_log(LOG_FILENAME_LINE,"Class Name %s %d\n",klass->name, i);

      test = oil_test_new (klass);

      if (test == NULL) 
      {
          std_log(LOG_FILENAME_LINE,"class \"%s\" has  bad prototype\n", klass->name);
          assert_failed = fail = 1; 
          continue;
      }
      oil_test_set_iterations (test, 1);
      test->n = 1 + oil_rand_u8();
      test->m = 1 + oil_rand_u8();

      std_log(LOG_FILENAME_LINE, "ref impl %s",klass->reference_impl->name);
      oil_test_check_impl (test, klass->reference_impl);        //Testing whether an appropriate implementation is suitable or not
                                                                //Runs the testing procedure described by test on the implementation impl
      
      for(j=0;j<OIL_ARG_LAST;j++)
      {
          if (test->params[j].is_stride) 
          {
              test->params[j].value += oil_type_sizeof(test->params[j].type) *
              (oil_rand_u8()&0xf);
          }
      }
      test->inited = 0;

      for (impl = klass->first_impl; impl; impl = impl->next) 
      {
          std_log(LOG_FILENAME_LINE,"  %s\n", impl->name);
          if ((impl->flags & OIL_CPU_FLAG_MASK) & ~cpu_flags){
              std_log(LOG_FILENAME_LINE, "Not supported");
          } 
          else 
          {
              ret = oil_test_check_impl (test, impl);
              if (!ret) 
              {
                  assert_failed = fail = 1;
                  oil_test_free (test);
                  std_log(LOG_FILENAME_LINE, "Test Failed");
                  create_xml(1);
                  return fail;
              }
              
          }
          
      }

      oil_test_free (test);
  }
  
  if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Fail");
  else
      std_log(LOG_FILENAME_LINE,"Test Successful");
  create_xml(0);
  return fail;
}

void
hist(OilTest *test)
{
  double ave;
  double std;
  int max_i;
  double off;
  double s;
  double s2;
  int i;
  int n;
  double x;

  do {
    s = s2 = 0;
    n = 0;
    max_i = -1;
    for(i=0;i<10;i++){
      x = test->prof.hist_time[i];
      s2 += x * x * test->prof.hist_count[i];
      s += x * test->prof.hist_count[i];
      n += test->prof.hist_count[i];
      if (test->prof.hist_count[i] > 0) {
        if (max_i == -1 || test->prof.hist_time[i] > test->prof.hist_time[max_i]) {
          max_i = i;
        }
      }
    }

    ave = s / n;
    std = sqrt (s2 - s * s / n + n*n) / (n-1);
    off = (test->prof.hist_time[max_i] - ave)/std;

    printf("    ave=%g std=%g max=%ld off=%g %s\n",
        ave, std, test->prof.hist_time[max_i], off, (off>4.0)?"yes":"no");

    if (off > 4.0) {
      test->prof.hist_count[max_i] = 0;
    }
  } while (off > 4.0);
  printf("    ave=%g std=%g\n", ave, std);
}


