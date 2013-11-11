/* example for using class array<>
 * (C) Copyright Nicolai M. Josuttis 2001.
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include <algorithm>
#include <functional>
#include <boost/array.hpp>
#include "print.hpp"
using namespace std;
using namespace boost;

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif

int main()
{
	std_log(LOG_FILENAME_LINE,"[Test Case for array2]");
	
	int failures = 0 ;
    // create and initialize array
    array<int,10> a = { { 1, 2, 3, 4, 5 } };

    print_elements(a);

    // modify elements directly
    for (unsigned i=0; i<a.size(); ++i) {
        ++a[i];
    }
    print_elements(a);

    // change order using an STL algorithm
    reverse(a.begin(),a.end());
    print_elements(a);

    // negate elements using STL framework
    transform(a.begin(),a.end(),    // source
              a.begin(),            // destination
              negate<int>());       // operation
    print_elements(a);
    
    if(a[0] != -1)
    failures++;
    if(a[1] != -1)
    failures++;
    if(a[2] != -1)
    failures++;
    if(a[3] != -1)
    failures++;
    if(a[4] != -1)
    failures++;
    if(a[5] != -6)
    failures++;
    if(a[6] != -5)
    failures++;
    if(a[7] != -4)
    failures++;
    if(a[8] != -3)
    failures++;
    if(a[9] != -2)
    failures++;
    
    
   if(failures)
   {
   std_log(LOG_FILENAME_LINE,"Result : Failed"); 
		assert_failed = true; 
   }
   else
   std_log(LOG_FILENAME_LINE,"Result : Passed");  
    
#ifdef __SYMBIAN32__
	testResultXml("array2");
	close_log_file();
#endif
    return 0;  // makes Visual-C++ compiler happy
}

