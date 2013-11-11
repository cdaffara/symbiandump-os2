// Copyright (C) 2006 Trustees of Indiana University
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/


#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <map>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif
struct vertex_properties {
  std::string name;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & BOOST_SERIALIZATION_NVP(name);
  }  
};

struct edge_properties {
  std::string name;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & BOOST_SERIALIZATION_NVP(name);
  }  
};

using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS, 
               vertex_properties, edge_properties> Graph;

typedef graph_traits<Graph>::vertex_descriptor vd_type;


typedef adjacency_list<vecS, vecS, undirectedS, 
               vertex_properties> Graph_no_edge_property;

int main()
{
  {
    std::ofstream ofs("./kevin-bacon2.dat");
    archive::xml_oarchive oa(ofs);
    Graph g;
		vertex_properties vp;
		vp.name = "A";
		vd_type A = add_vertex( vp, g );
		vp.name = "B";
		vd_type B = add_vertex( vp, g );

		edge_properties ep;
		ep.name = "a";
		add_edge( A, B, ep, g);

    oa << BOOST_SERIALIZATION_NVP(g);

    Graph_no_edge_property g_n;
    oa << BOOST_SERIALIZATION_NVP(g_n);
  }

  {
    std::ifstream ifs("./kevin-bacon2.dat");
    archive::xml_iarchive ia(ifs);
    Graph g;
    ia >> BOOST_SERIALIZATION_NVP(g);

		if  (!( g[*(vertices( g ).first)].name == "A" )) return -1;

    Graph_no_edge_property g_n;
    ia >> BOOST_SERIALIZATION_NVP(g_n);
  }
  
  #ifdef __SYMBIAN32__
  
  	std_log(LOG_FILENAME_LINE,"[End Test Case ]");

	testResultXml("serialize");
	close_log_file();
#endif
  return 0;
}
