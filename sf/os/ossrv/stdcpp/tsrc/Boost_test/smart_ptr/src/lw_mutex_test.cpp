//
// lw_mutex_test.cpp
//
// Copyright 2005 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include <boost/detail/lightweight_mutex.hpp>

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif

// Sanity check only

boost::detail::lightweight_mutex m1;

int main()
{
	std_log(LOG_FILENAME_LINE,"[Test Case for lw_mutex_test]");
	
	#ifdef __ARMCC__
  #pragma diag_suppress 177
  #endif  
    boost::detail::lightweight_mutex::scoped_lock lock1( m1 );

    boost::detail::lightweight_mutex m2;
    boost::detail::lightweight_mutex m3;

    boost::detail::lightweight_mutex::scoped_lock lock2( m2 );
    boost::detail::lightweight_mutex::scoped_lock lock3( m3 );
 


#ifdef __SYMBIAN32__
	std_log(LOG_FILENAME_LINE,"Result : Passed");

	std_log(LOG_FILENAME_LINE,"[End Test Case ]");
#endif
	testResultXml("lw_mutex_test");
	close_log_file();
    return 0;
}
