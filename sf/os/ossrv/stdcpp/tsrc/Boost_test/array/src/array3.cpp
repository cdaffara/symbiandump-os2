/* example for using class array<>
 * (C) Copyright Nicolai M. Josuttis 2001.
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include <string>
#include <iostream>
#include <boost/array.hpp>

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif
template <class T>
void print_elements (const T& x);

int main()
{
std_log(LOG_FILENAME_LINE,"[Test Case for array3]");
int failures=0;
    // create array of four seasons
    boost::array<std::string,4> seasons = {
        { "spring", "summer", "autumn", "winter" }
    };

    // copy and change order
    boost::array<std::string,4> seasons_orig = seasons;
    for (unsigned i=seasons.size()-1; i>0; --i) {
        std::swap(seasons.at(i),seasons.at((i+1)%seasons.size()));
    }

    std::cout << "one way:   ";
    print_elements(seasons);

    // try swap()
    std::cout << "other way: ";
    std::swap(seasons,seasons_orig);
    print_elements(seasons);

    // try reverse iterators
    std::cout << "reverse:   ";
    for (boost::array<std::string,4>::reverse_iterator pos
           =seasons.rbegin(); pos<seasons.rend(); ++pos) {
        std::cout << " " << *pos;
    }
    std::cout << std::endl;
    
    if (seasons[3]!= "winter")
    		failures++;
    if (seasons[2] != "autumn")
    		failures++;
    if (seasons[1] != "summer")
    		failures++;
    if (seasons[0] != "spring")
    		failures++;
   
   if(failures)
   {
   std_log(LOG_FILENAME_LINE,"Result : Failed"); 
		assert_failed = true; 
   }
   else
   std_log(LOG_FILENAME_LINE,"Result : Passed");  
#ifdef __SYMBIAN32__
	testResultXml("array3");
	close_log_file();
#endif
    return 0;  // makes Visual-C++ compiler happy
}

template <class T>
void print_elements (const T& x)
{
    for (unsigned i=0; i<x.size(); ++i) {
        std::cout << " " << x[i];
    }
    std::cout << std::endl;
}

