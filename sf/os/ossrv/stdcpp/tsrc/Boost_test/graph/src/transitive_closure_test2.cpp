// Copyright (C) 2004 Jeremy Siek <jsiek@cs.indiana.edu>
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/transitive_closure.hpp>
#include <iostream>

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif

unsigned int failed_tests = 0;
using namespace std;

using namespace boost;
typedef adjacency_list<> graph_t;

int main(int argc, char *argv[]) {
  graph_t g(5),g_TC;

  add_edge(0,2,g);
  add_edge(1,0,g);
  add_edge(1,2,g);
  add_edge(1,4,g);
  add_edge(3,0,g);
  add_edge(3,2,g);
  add_edge(4,2,g);
  add_edge(4,3,g);

  transitive_closure(g,g_TC);

  cout << "original graph: 0->2, 1->0, 1->2, 1->4, 3->0, 3->2, 4->2, 4->3"
       << endl;
  cout << "transitive closure: ";

  graph_t::edge_iterator i,iend;
  for(tie(i,iend) = edges(g_TC);i!=iend;++i) {
    cout << source(*i,g_TC) << "->" << target(*i,g_TC) << " ";
  }
  cout << endl;

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
	testResultXml("transitive_closure_test2");
	close_log_file();
#endif
 
}
