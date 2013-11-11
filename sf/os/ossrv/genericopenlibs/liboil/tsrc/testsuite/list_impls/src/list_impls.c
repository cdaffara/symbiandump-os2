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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>

#include <liboil/liboil.h>
#include <liboil/liboilfunction.h>

#define LOG_FILE "c:\\logs\\testsuite_list_impls_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_list_impls");
    close_log_file();
}

int main (int argc, char *argv[])
{
  int failed = 0;
  int n_classes;
  int n_impls;
  int i, flag;

  std_log(LOG_FILENAME_LINE,"Test started testsuite_list_impls");
  oil_init ();

  n_classes = oil_class_get_n_classes();

  n_impls = 0;
  for(i=0;i<n_classes;i++){
    OilFunctionClass *klass = oil_class_get_by_index (i);
    OilFunctionImpl *impl;

    flag = 0;
    for (impl = klass->first_impl; impl; impl=impl->next) {
    std_log(LOG_FILENAME_LINE, "%s:%s\n", klass->name, impl->name);
      flag = 1;
    }
    if(flag == 0)
        {
        assert_failed= 1;
        std_log(LOG_FILENAME_LINE, "Failed in class -> %s\n", klass->name);
        break;
        }
  }
  if(assert_failed)
        std_log(LOG_FILENAME_LINE,"Test Fail");
  else
        std_log(LOG_FILENAME_LINE,"Test Successful");
  create_xml(0);
  return 0;
}

