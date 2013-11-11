// Copyright 2006 The Trustees of Indiana University.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Douglas Gregor
//           Andrew Lumsdaine
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

// Make sure adjacency_list works with EdgeListS=setS
#include <boost/graph/adjacency_list.hpp>
#include <boost/test/minimal.hpp>
#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif
using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS, no_property, no_property,
                       no_property, setS> GraphType;

int test_main(int,char*[])
{
  GraphType g(10);
  add_vertex(g);
  add_edge(0, 5, g);
#ifdef __SYMBIAN32__
	std_log(LOG_FILENAME_LINE,"[End Test Case ]");

	testResultXml("adj_list_edge_list_set");
	close_log_file();
#endif
  return 0;
}
