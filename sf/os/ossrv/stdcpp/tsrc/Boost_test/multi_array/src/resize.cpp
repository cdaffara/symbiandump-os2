// Copyright 2002 The Trustees of Indiana University.

// Use, modification and distribution is subject to the Boost Software 
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Boost.MultiArray Library
//  Authors: Ronald Garcia
//           Jeremy Siek
//           Andrew Lumsdaine
//  See http://www.boost.org/libs/multi_array for documentation.
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

//
// resize.cpp - Test of resizing multi_arrays
//

#include "boost/test/minimal.hpp"
#include "boost/multi_array.hpp"
#include <iostream>
#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif
using namespace std;


int test_main(int,char*[]) {

  typedef boost::multi_array<int,3> marray;


  int A_data[] = {
    0,1,2,3,
    4,5,6,7,
    8,9,10,11,

    12,13,14,15,
    16,17,18,19,
    20,21,22,23
  };

  int A_resize[] = {
    0,1,
    4,5,
    8,9,

    12,13,
    16,17,
    20,21,

    0,0,
    0,0,
    0,0,

    0,0,
    0,0,
    0,0
  };

  // resize through the extent_gen interface
  {
    marray A(boost::extents[2][3][4]);
    A.assign(A_data,A_data+(2*3*4));
    A.resize(boost::extents[4][3][2]);
    BOOST_CHECK(std::equal(A_resize,A_resize+(4*3*2),A.data()));
  }  

  // resize through the Collection
  {
    marray A(boost::extents[2][3][4]);
    A.assign(A_data,A_data+(2*3*4));
    boost::array<int,3> new_extents = {{4,3,2}};
    A.resize(new_extents);
    BOOST_CHECK(std::equal(A_resize,A_resize+(4*3*2),A.data()));
  }  

  // default construct all the new elements (in this case, all elements)
  {
    marray defaultA;
    defaultA.resize(boost::extents[2][3][4]);
    BOOST_CHECK(std::accumulate(defaultA.data(),
                               defaultA.data()+(2*3*4),0) == 0);
  }



  // verify the preservation of storage order
  {
    int tiling_graph_storage_order[] = {2, 0, 1}; 
    bool tiling_graph_index_order[] = {true, true, true};
  
    marray A(boost::extents[3][4][2],
             boost::general_storage_order<3>(tiling_graph_storage_order,
                                             tiling_graph_index_order)); 


    int value = 0;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 2; k++) {
          *(A.data() + value) = value;
          ++value;
        }
      }
    }

    // "Resize" to the same size
    A.resize(boost::extents[3][4][2]);

    int check = 0;
    for (int x = 0; x < 3; x++) { 
      for (int y = 0; y < 4; y++) {
        for (int z = 0; z < 2; z++) {
          BOOST_CHECK(*(A.data() + check) == check);
          ++check;
        }
      }
    }
  }
#ifdef __SYMBIAN32__
if(boost::minimal_test::errors_counter() != 0)
   assert_failed = true;
   	testResultXml("resize");
	close_log_file();
#endif


  return boost::exit_success;
}
