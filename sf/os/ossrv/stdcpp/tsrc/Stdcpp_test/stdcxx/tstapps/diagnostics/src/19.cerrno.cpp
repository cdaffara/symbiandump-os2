

/***************************************************************************
 *
 * 19.cerrno.cpp - test exercising the lib.errno
 *
 * $Id: 22.locale.num.get.cpp 290831 2005-09-21 21:15:42Z sebor $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

// include only <cerrno> here to prevent namespace pollution
#include <cerrno>

/**************************************************************************/

const char* const errors[] = {

#ifndef errno
    "errno",
#endif   // errno

#ifndef EDOM
    "EDOM not defined",
#elif EDOM <= 0
    "EDOM not positive",
#endif   // EDOM

#ifndef EILSEQ
    "EILSEQ",   // lwg issue 288
#elif EILSEQ <= 0
    "EILSEQ not positive",
#elif EILSEQ == EDOM
    "EILSEQ not distinct from EDOM",
#endif   // EILSEQ

#ifndef ERANGE
    "ERANGE not defined",
#elif ERANGE <= 0
    "ERANGE not positive",
#elif ERANGE == EDOM
    "ERANGE not distinct from EDOM",
#elif ERANGE == EILSEQ
    "ERANGE not distinct from EILSEQ",
#endif   // ERANGE

    0
};

/**************************************************************************/

// include all other headers here
#include <any.h>      // for rw_any_t
#include <driver.h>   // for rw_test(), ...
#include"std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
int failures=0;

/**************************************************************************/

template <class T> void set_errno_value (T, int) { /* empty */ }
void set_errno_value (int &errno_ref, int val) { errno_ref = val; }

/**************************************************************************/

static int
errno_at_startup;

static int
run_test (int, char**)
{
    rw_info (0, 0, 0, "exercising the contents of the <cerrno> header");

    for (unsigned i = 0; errors [i]; ++i) {
      if(0!=errors)
      {  
        failures++;
       std_log(LOG_FILENAME_LINE,"Reason : Failing for i=%d",i);
        rw_assert (0 == errors [i], 0, 0,
                        "macro %s", errors [i]);
       }                 
    }

    // get the type of errno
    const char* const errno_type = rw_any_t (errno).type_name ();

    if(!( 'i' == errno_type [0] && 'n' == errno_type [1]&& 't' == errno_type [2]
               && '\0' == errno_type [3]))
       {
         failures++;
         std_log(LOG_FILENAME_LINE,"Reason : Failing ");               
         }
     
    // 7.5, p2 of C99: the type of errno must be int
    rw_assert (   'i' == errno_type [0]
               && 'n' == errno_type [1]
               && 't' == errno_type [2]
               && '\0' == errno_type [3],
               0, 0, 
               "the type of errno is int, got %s", errno_type);

     if(!(0 == errno_at_startup))
       {
         failures++;
         std_log(LOG_FILENAME_LINE,"Reason : Failing as errno is not 0 at program startup ");               
         }
    
    // 7.5, p3 of C99: errno must be 0 at program startup
    rw_assert (0 == errno_at_startup, 0, 0,
               "errno == 0 at program startup, got %d", errno_at_startup);

#ifndef EDOM
#  define EDOM   33 /* Solaris value */
#endif   // EDOM

    // 7.5, p2 of C99: errno must be a modifiable lvalue
    set_errno_value (errno, int (EDOM));
     
     if(!(EDOM == errno))
       {
         failures++;
         std_log(LOG_FILENAME_LINE,"Reason : Failing");               
         }

          
    rw_assert (EDOM == errno, 0, 0,
               "errno == %d (%{#*m}, got %d (%{#m}) "
               "(errno not a modifiable lvalue?)",
               EDOM, EDOM, errno, errno);

#ifndef ERANGE
#  define ERANGE   34 /* Solaris value */
#endif   // ERANGE

    set_errno_value (errno, int (ERANGE));
    
    if(!(ERANGE == errno))
       {
         failures++;
         std_log(LOG_FILENAME_LINE,"Reason : Failing");               
         }

    
    rw_assert (ERANGE == errno, 0, 0,
               "errno == %d (%{#*m}, got %d (%{#m}) "
               "(errno not a modifiable lvalue?)",
               ERANGE, ERANGE, errno, errno);

#ifndef EILSEQ
#  define EILSEQ   84 /* Solaris value */
#endif   // EILSEQ

    set_errno_value (errno, int (EILSEQ));
     
     if(!(EILSEQ == errno))
       {
         failures++;
         std_log(LOG_FILENAME_LINE,"Reason : Failing");               
         }


    rw_assert (EILSEQ == errno, 0, 0,
               "errno == %d (%{#*m}, got %d (%{#m}) "
               "(errno not a modifiable lvalue?)",
               EILSEQ, EILSEQ, errno, errno);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    errno_at_startup = errno;

    std_log(LOG_FILENAME_LINE,"[Test Case for cerrno]");
    
    #ifndef __SYMBIAN32__
    return rw_test (argc, argv, __FILE__,
                    "lib.errno",
                    0 /* no comment */, run_test,
                    0 /* no command line options */, 0);
    #else                
      rw_test (argc, argv, __FILE__,
                    "lib.errno",
                    0 /* no comment */, run_test,
                    0 /* no command line options */, 0);
    if(failures)
      {
            assert_failed = true;
       std_log(LOG_FILENAME_LINE,"Result: Failed");
      }
     else
      {

       std_log(LOG_FILENAME_LINE,"Result: Passed");
      }
  
   std_log(LOG_FILENAME_LINE,"[End Test Case]");  
   
   testResultXml("19_cerrno");
    close_log_file();
 #endif
                    
}
