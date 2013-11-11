/* simple example for using class array<>
 *
 * (C) Copyright Nicolai M. Josuttis 2001.
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Changelog:
 * 20 Jan 2001 - Removed boolalpha use since stock GCC doesn't support it
 *               (David Abrahams)
 */
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include <iostream>
#include <boost/array.hpp>

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif

int main()
{
    std_log(LOG_FILENAME_LINE,"[Test Case for array1]");
    // define special type name
    typedef boost::array<float,6> Array;

    // create and initialize an array
    Array a = { { 42 } };

    // access elements
    for (unsigned i=1; i<a.size(); ++i) {
        a[i] = a[i-1]+1;
    }

    // use some common STL container operations
    std::cout << "size:     " << a.size() << std::endl;
    std::cout << "empty:    " << (a.empty() ? "true" : "false") << std::endl;
    std::cout << "max_size: " << a.max_size() << std::endl;
    std::cout << "front:    " << a.front() << std::endl;
    std::cout << "back:     " << a.back() << std::endl;
    std::cout << "elems:    ";

    // iterate through all elements
    for (Array::const_iterator pos=a.begin(); pos<a.end(); ++pos) {
        std::cout << *pos << ' ';
    }
    std::cout << std::endl;

    // check copy constructor and assignment operator
    Array b(a);
    Array c;
    c = a;
    if (a==b && a==c) {
        std::cout << "copy construction and copy assignment are OK"
                  << std::endl;
        std_log(LOG_FILENAME_LINE,"Result : Passed");          
    }
    else {
        std::cout << "copy construction and copy assignment FAILED"
                  << std::endl;
        std_log(LOG_FILENAME_LINE,"Result : Failed"); 
		assert_failed = true;          
    }
	testResultXml("array1");
	close_log_file();
    return 0;  // makes Visual-C++ compiler happy
}

