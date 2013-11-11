//=======================================================================
// Copyright 2001 University of Notre Dame.
// Author: Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

/* 
   Sample output:

   filtered out-edges:
   A --> 
   B --> 
   C --> E 
   D --> E 
   E --> 
 */

#include <boost/config.hpp>
#include <iostream>
#include <boost/graph/vector_as_graph.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif



struct constant_target {
  constant_target() { }
  constant_target(int t) : target(t) { }
  bool operator()(const std::pair<int,int>& e) const {
    return e.second == target;
  }
  int target;
};


int main()
{
  using namespace boost;
  
  enum { A, B, C, D, E, N };
  const char* name = "ABCDE";
  typedef std::vector < std::list < int > > Graph;
  Graph g(N);
  g[A].push_back(B);
  g[A].push_back(C);
  g[C].push_back(D);
  g[C].push_back(E);
  g[D].push_back(E);
  g[E].push_back(C);

  constant_target filter(E);
  filtered_graph<Graph, constant_target> fg(g, filter);

  std::cout << "filtered out-edges:" << std::endl;
  print_graph(fg, name);
  
  #ifdef __SYMBIAN32__
  	std_log(LOG_FILENAME_LINE,"[End Test Case ]");
 	testResultXml("filtered_vec_as_graph");
	close_log_file();
  #endif
  
  return 0;
}
