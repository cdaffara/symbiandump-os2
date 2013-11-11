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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_FILE "c:\\logs\\memcpy-speed_logs.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("memcpy-speed");
    close_log_file();
}

#include <liboil/liboil.h>
#include <liboil/liboilprofile.h>
#include <liboil/liboilfunction.h>
#include <liboil/globals.h>

#define ALIGN(ptr,n) ((void *)((unsigned long)(ptr) & (~(unsigned long)(n-1))))

int
main(int argc, char *argv[])
{
  char *s=NULL, *d=NULL;
  uint32_t *src, *dest;
  int16_t res=0;
  OilProfile prof;
  double ave, std;
  int i,j, cnt;
  double cpufreq;
  OilFunctionClass *klass;
  OilFunctionImpl *impl;
  int the_class;
  
  std_log(LOG_FILENAME_LINE,"Test Started memcpy-speed");
  
  oil_init ();
  
  s = malloc(512*1024+1024);
  d = malloc(512*1024+1024);
//  src = ((void *)((unsigned long)(s) |  0xFF ));
  //dest = ((void *)((unsigned long)(d) |  0xFF ));
    src = ((void *)((unsigned long)(s) ));
  dest = ((void *)((unsigned long)(d) ));

  for(the_class=0; the_class<3; the_class++)
  { 
      cpufreq = 1788e6;    
      switch(the_class) {
        case 0:
          klass = oil_class_get ("splat_u32_ns");
          break;
        case 1:
          klass = oil_class_get ("copy_u8");
          break;
        case 2:
          klass = oil_class_get ("sum_s16");
          break;
      }
    
      for(impl=klass->first_impl;impl;impl=impl->next) {
      std_log(LOG_FILENAME_LINE,"impl %s\n", impl->name);
    
        if (!oil_impl_is_usable(impl)) {
        std_log(LOG_FILENAME_LINE,"  not usable\n");
          continue;
        }
    
        oil_class_choose_by_name (klass, impl->name);
    
        for(i=10;i<20;i++){
          oil_profile_init (&prof);
          for(j=0;j<10;j++){
            switch(the_class) {
              case 0:
                oil_profile_start(&prof);
                oil_splat_u32_ns (dest, src, 1<<(i-2));
                oil_profile_stop(&prof);
                for(cnt=0; cnt<(1<<(i-2)); cnt++){
                    if(dest[cnt]!=*src){
                        std_log(LOG_FILENAME_LINE,"Failed at the_class=%d, cnt=%d, i=%d, j=%d, impl->name=%s\n", the_class, cnt, i, j, impl->name);
                        assert_failed =1;
                    }
                }
                
                break;
              case 1:
                oil_profile_start(&prof);
                oil_memcpy (dest, src, 1<<i);
                oil_profile_stop(&prof);
                for(cnt=0; cnt<(1<<(i-2)); cnt++){      //cnt is checked with 1<<(i-2) because dest & src are of type uint32_t*
                    if(dest[cnt]!=src[cnt]){
                        std_log(LOG_FILENAME_LINE,"Failed at the_class=%d, cnt=%d, i=%d, j=%d, impl->name=%s\n", the_class, cnt, i, j, impl->name);
                        assert_failed =1;
                    }
                }
                
                break;
              case 2:
                  {
                int16_t* src1 = (int16_t*)src;
                int16_t* dest1 = (int16_t*)dest;
                oil_profile_start(&prof);
                oil_sum_s16 (dest1, src1, 1<<(i-1));
                oil_profile_stop(&prof);
                res=0;
                for(cnt=0; cnt<(1<<(i-1)); cnt++){
                    res += src1[cnt];
                }
                if(*dest1 != res){
                    std_log(LOG_FILENAME_LINE,"Failed at the_class=%d, impl->name=%s\n", the_class, impl->name);
                    assert_failed =1;
                }
                
                  }
                break;
            }
          }
          oil_profile_get_ave_std (&prof, &ave, &std);                    
          std_log(LOG_FILENAME_LINE,"%d: %10.4g %10.4g %10.4g\n", i, ave, std,
            ave/(1<<i));
          
        }
      }
      
 }
  free(s);
  free(d);
  
  if(!assert_failed)
      std_log(LOG_FILENAME_LINE, "Test Passed");
  else
      std_log(LOG_FILENAME_LINE, "Test Failed");
  create_xml(0);
  return 0;
}
