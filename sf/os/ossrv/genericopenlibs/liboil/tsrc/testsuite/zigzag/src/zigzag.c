/*
 * Copyright (c) 2005
 *	Eric Anholt.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <liboil/liboil.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>
#include <string.h>
#include <liboil/globals.h>

#define LOG_FILE "c:\\logs\\testsuite_zigzag_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_zigzag");
    close_log_file();
}

int main (int argc, char *argv[])
{
  int16_t data[64], datatmp[64];
  int i;

  std_log(LOG_FILENAME_LINE, "Test Started testsuite_zigzag");
  oil_init();

  for (i = 0; i < 64; i++)
    data[i] = i;

  oil_zigzag8x8_s16(datatmp, 8 * sizeof(int16_t), data, 8 * sizeof(int16_t));
  oil_unzigzag8x8_s16(data, 8 * sizeof(int16_t), datatmp, 8 * sizeof(int16_t));

  for (i = 0; i < 64; i++) {
    if (data[i] != i) {
    std_log(LOG_FILENAME_LINE, "entry %d was %d, should have been %d\n", i, data[i], i);
      assert_failed = 1;
    }
  }
  
  if(assert_failed)
          std_log(LOG_FILENAME_LINE,"Test Fail");
  else
          std_log(LOG_FILENAME_LINE,"Test Successful");
  create_xml(0);
  return 0;
}

