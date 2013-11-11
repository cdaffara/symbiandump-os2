//=======================================================================
// Copyright 2001 Indiana University.
// Author: Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
//#include <boost/test/minimal.hpp>
#include <iostream>


#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif

//int test_main(int,char*[])
int main(int,char*[])
{
  using namespace boost;
  using namespace std;
  typedef adjacency_list<vecS, vecS, bidirectionalS, no_property, 
    property<int, edge_weight_t>, no_property, vecS> Graph;
	
	const std::size_t n = 3;
  typedef std::pair<std::size_t, std::size_t> E;
  E edge_array[] = { E(0,1), E(0,2), E(0,1) };
  const std::size_t m = sizeof(edge_array) / sizeof(E);
  Graph g(edge_array, edge_array + m, n);
  
  for (std::size_t i = 0; i < m; ++i)
  {
   //std::cout<< edges(g).first[i]<<std::endl;
   edges(g).first[i];
          
  }
	
  #ifdef __SYMBIAN32__
	  std_log(LOG_FILENAME_LINE,"[End Test Case ]");
  	testResultXml("adj_list_ra_edgelist");
	  close_log_file();
  #endif
  return 0;
}
