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
// index_bases - test of the index_base modifying facilities.
//

#include "boost/multi_array.hpp"

#include "boost/test/minimal.hpp"

#include "boost/array.hpp"
#include <vector>
#include <iostream>
#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif

int
test_main(int,char*[])
{
  typedef boost::multi_array<double, 3> array;
  typedef array::array_view<3>::type array_view;

  typedef array::extent_range erange;
  typedef array::index_range irange;

  array::extent_gen extents;
  array::index_gen indices;

  // Construct with nonzero bases
  {

    array A(extents[erange(1,4)][erange(2,5)][erange(3,6)]);
    //array_view E = A[indices[irange(1,2)][irange(1,2)][irange(1,2)]];
    array_view E = A[indices[irange(1,2)][irange(2,3)][irange(3,4)]];

  }
  
  #ifdef __SYMBIAN32__
	std_log(LOG_FILENAME_LINE,"[End Test Case ]");
   	testResultXml("vc_death");
	close_log_file();
#endif
  return boost::exit_success;
}
