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
#include <string>
#include <iostream>
#include <boost/array.hpp>

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif
int main()
{
std_log(LOG_FILENAME_LINE,"[Test Case for array4]");
    // array of arrays of seasons
    boost::array<boost::array<std::string,4>,2> seasons_i18n = {
        { { { "spring", "summer", "autumn", "winter", } },
          { { "Fruehling", "Sommer", "Herbst", "Winter" } }
        }
    };

    // for any array of seasons print seasons
    for (unsigned i=0; i<seasons_i18n.size(); ++i) {
        boost::array<std::string,4> seasons = seasons_i18n[i];
        for (unsigned j=0; j<seasons.size(); ++j) {
            std::cout << seasons[j] << " ";
        }
        std::cout << std::endl;
    }

    // print first element of first array
    std::cout << "first element of first array: "
              << seasons_i18n[0][0] << std::endl;

    // print last element of last array
    std::cout << "last element of last array: "
              << seasons_i18n[seasons_i18n.size()-1][seasons_i18n[0].size()-1]
              << std::endl;
   if(seasons_i18n[0][0]!= "spring"  && seasons_i18n[seasons_i18n.size()-1][seasons_i18n[0].size()-1] != "Winter")
   {
   std_log(LOG_FILENAME_LINE,"Result : Failed"); 
		assert_failed = true; 
   }
   else
   std_log(LOG_FILENAME_LINE,"Result : Passed"); 
#ifdef __SYMBIAN32__
	testResultXml("array4");
	close_log_file();
#endif

    return 0;  // makes Visual-C++ compiler happy
}

