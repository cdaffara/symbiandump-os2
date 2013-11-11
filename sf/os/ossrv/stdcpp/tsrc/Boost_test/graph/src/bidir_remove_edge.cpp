//  (C) Copyright Jeremy Siek 2004 
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/cstdlib.hpp>
#include <boost/test/minimal.hpp>
#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif
using namespace boost;

struct edge_prop {
  int weight;
};

int
test_main(int, char*[])
{
  {
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
      boost::no_property, edge_prop> graph;
    typedef boost::graph_traits<graph>::edge_descriptor edge;

    graph g(2);

    edge_prop p = { 42 };
    edge e; bool b;
    tie(e, b) = add_edge(0, 1, p, g);
    assert( num_edges(g) == 1 );
    assert( g[e].weight == 42 );
    remove_edge(e, g);
    assert( num_edges(g) == 0 );
  }
  {
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS> graph;
    typedef boost::graph_traits<graph>::edge_descriptor edge;

    graph g(2);

    edge e; bool b;
    tie(e, b) = add_edge(0, 1, g);
    assert( num_edges(g) == 1 );
    remove_edge(e, g);
    assert( num_edges(g) == 0 );
  }
  
   #ifdef __SYMBIAN32__
	std_log(LOG_FILENAME_LINE,"[End Test Case ]");

	testResultXml("bidir_remove_edge");
	close_log_file();
#endif
  return boost::exit_success;
}
