//
// atomic_count_test.cpp
//
// Copyright 2005 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/detail/atomic_count.hpp>
#include <boost/detail/lightweight_test.hpp>

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif

int main()
{
    std_log(LOG_FILENAME_LINE,"[Test Case for atomic_count_test]");
    boost::detail::atomic_count n( 4 );

    BOOST_TEST( n == 4L );

    ++n;

    BOOST_TEST( n == 5L );
    BOOST_TEST( --n != 0L );

    boost::detail::atomic_count m( 0 );

    BOOST_TEST( m == 0 );

    ++m;

    BOOST_TEST( m == 1 );

    ++m;

    BOOST_TEST( m == 2 );
    BOOST_TEST( --m != 0 );
    BOOST_TEST( --m == 0 );

#ifdef __SYMBIAN32__
    int failures = boost::report_errors();	  
	if(failures)
	{
		std_log(LOG_FILENAME_LINE,"Result : Failed"); 
		assert_failed = true;
	}
	else
	{
		std_log(LOG_FILENAME_LINE,"Result : Passed");
	}  
	std_log(LOG_FILENAME_LINE,"[End Test Case ]");
#endif
	testResultXml("atomic_count_test");
	close_log_file();
	return failures;
}
