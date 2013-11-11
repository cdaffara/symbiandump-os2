//=======================================================================
// Copyright 2002 Indiana University.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/adjacency_list.hpp>
#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif

unsigned int failed_tests = 0;
using namespace boost;

// Check to make you can apply a vertex filter with the
// make_filtered_graph function, to fix bug #480175.

struct NotMuchOfAFilter
{
    template<class Vertex> bool operator()(Vertex key)
    const { return true; }
};

int main()
{
    adjacency_list<> graph;
    make_filtered_graph(graph, keep_all(), NotMuchOfAFilter());
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
	testResultXml("filter_graph_vp_test");
	close_log_file();
#endif
 
    return failed_tests;
}
