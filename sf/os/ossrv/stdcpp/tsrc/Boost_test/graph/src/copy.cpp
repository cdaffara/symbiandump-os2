// Copyright (C) Vladimir Prus 2003.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>
#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif

int failed_tests = 0;
using namespace boost;

class copier {
public:
    template<class V1, class V2>
    void operator()(const V1&, const V2&) const {}
};

int main()
{
    adjacency_list<vecS, vecS, directedS, property<vertex_root_t, int> > g1, g2;
    adjacency_list<vecS, setS, directedS, property<vertex_index_t, int> > g3;

    copy_graph(g1, g2);
    copier c;
    copy_graph(g3, g1, vertex_copy(c));
#ifdef __SYMBIAN32__
	if(failed_tests)
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
#ifdef __SYMBIAN32__
	testResultXml("copy");
	close_log_file();
#endif

return failed_tests;
}
