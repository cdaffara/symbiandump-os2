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
#include <liboil/liboil.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


#define LOG_FILE "c:\\logs\\testsuite_proto4_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_proto4");
    close_log_file();
}

int main (int argc, char *argv[])
{
  OilFunctionClass *klass;
  int i;
  int n;
  //xmlfile = "proto4";
  std_log(LOG_FILENAME_LINE, "Test Started testsuite_proto4");

  oil_init ();

  n = oil_class_get_n_classes ();
  for (i=0;i<n; i++ ){
    klass = oil_class_get_by_index(i);

    if(klass->prototype) {
      OilPrototype *proto;
      char *string;

      proto = oil_prototype_from_string (klass->prototype);     /*Converts the string s containing C prototype that follows Liboil parameter naming rules into a OilPrototype.*/
      if (proto == NULL) {
        std_log(LOG_FILENAME_LINE, "ERROR bad prototype");
        printf ("ERROR bad prototype for %s\n", klass->name);
        continue;
      }

      string = oil_prototype_to_string (proto);                 /* Converts a prototype into the corresponding C style declaration. */

      printf("%s (%s)\n", klass->name, string);

      oil_prototype_free (proto);
      free(string);
    } else {
      std_log(LOG_FILENAME_LINE, "ERROR no prototype");
      printf("ERROR no prototype for %s\n", klass->name);
    }
  }
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  return 0;
}


