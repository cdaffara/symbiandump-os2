// Copyright 2002 The Trustees of Indiana University.

// Use, modification and distribution is subject to the Boost Software 
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Boost.MultiArray Library
//  Authors: Ronald Garcia
//           Jeremy Siek
//           Andrew Lumsdaine
//  See http://www.boost.org/libs/multi_array for documentation.

//
// test out my new storage_order stuff
//
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include "boost/test/minimal.hpp"

#include "boost/multi_array/storage_order.hpp"

#include <boost/detail/lightweight_test.hpp>

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif
int 
test_main(int,char*[]) {

  using namespace boost;

  array<std::size_t,5> c_ordering = {{4,3,2,1,0}};;
  array<std::size_t,5> fortran_ordering = {{0,1,2,3,4}};
  array<bool,5> ascending = {{true,true,true,true,true}};
  general_storage_order<5> c_storage(c_ordering.begin(),
                                     ascending.begin());
  general_storage_order<5> fortran_storage(fortran_ordering.begin(),
                                           ascending.begin());
 
  BOOST_TEST(c_storage == (general_storage_order<5>) c_storage_order());
  BOOST_TEST(fortran_storage ==
             (general_storage_order<5>) fortran_storage_order());
 #ifdef __SYMBIAN32__
if(boost::minimal_test::errors_counter() != 0)
   assert_failed = true;
   	testResultXml("storage_order_convert");
	close_log_file();
#endif

  return boost::report_errors();
}
