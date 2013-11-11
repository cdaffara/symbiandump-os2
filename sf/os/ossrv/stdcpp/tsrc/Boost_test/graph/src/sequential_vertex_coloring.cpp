// Copyright 2004 The Trustees of Indiana University.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Douglas Gregor
//           Andrew Lumsdaine

/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/test/minimal.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <utility>

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif
using namespace boost;

int test_main(int, char*[])
{
  typedef adjacency_list<listS, vecS, undirectedS> Graph;
  typedef graph_traits<Graph>::vertex_descriptor vertex_descriptor;
  typedef graph_traits<Graph>::vertices_size_type vertices_size_type;
  typedef property_map<Graph, vertex_index_t>::const_type vertex_index_map;

  typedef std::pair<int, int> Edge;
  enum nodes {A, B, C, D, E, n};
  Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E), 
                        Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), 
                        Edge(E, B) };
  int m = sizeof(edge_array) / sizeof(Edge);
  Graph g(edge_array, edge_array + m, n);

  // Test with the normal order
  std::vector<vertices_size_type> color_vec(num_vertices(g));
  iterator_property_map<vertices_size_type*, vertex_index_map, 
                        vertices_size_type, vertices_size_type&>
    color(&color_vec.front(), get(vertex_index, g));
  vertices_size_type num_colors = sequential_vertex_coloring(g, color);
  BOOST_CHECK(num_colors == 3);
  BOOST_CHECK(get(color, (vertices_size_type)A) == 0);
  BOOST_CHECK(get(color, (vertices_size_type)B) == 0);
  BOOST_CHECK(get(color, (vertices_size_type)C) == 1);
  BOOST_CHECK(get(color, (vertices_size_type)D) == 2);
  BOOST_CHECK(get(color, (vertices_size_type)E) == 1);
  
    #ifdef __SYMBIAN32__
  
  	std_log(LOG_FILENAME_LINE,"[End Test Case ]");

	testResultXml("sequential_vertex_coloring");
	close_log_file();
#endif
  return 0;
}

