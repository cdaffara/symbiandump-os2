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
// concept_checks.cpp -
//   make sure the types meet concept requirements
//
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include "boost/concept_check.hpp"
#include "boost/multi_array/concept_checks.hpp"
#include "boost/multi_array.hpp"
//#include "boost/test/test_tools.hpp"

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif
#include "boost/cstdlib.hpp"
#define BOOST_INCLUDE_MAIN

#include "boost/array.hpp"

int
main(int,char*[])
{
  const int ndims=3;
  typedef boost::multi_array<int,ndims> array;
  typedef boost::multi_array_ref<int,ndims> array_ref;
  typedef boost::const_multi_array_ref<int,ndims> const_array_ref;
  typedef array::array_view<ndims>::type array_view;
  typedef array::const_array_view<ndims>::type const_array_view;
  typedef array::subarray<ndims>::type subarray;
  typedef array::const_subarray<ndims>::type const_subarray;

boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<array,ndims> >();
boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<array_ref,ndims> >();
boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<const_array_ref,ndims> >();
boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<array_view,ndims> >();
boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<const_array_view,ndims> >();
boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<subarray,ndims> >();
boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<const_subarray,ndims> >();

boost::function_requires<
  boost::detail::multi_array::MutableMultiArrayConcept<array,ndims> >();
boost::function_requires<
  boost::detail::multi_array::MutableMultiArrayConcept<array_ref,ndims> >();
boost::function_requires<
  boost::detail::multi_array::MutableMultiArrayConcept<array_view,ndims> >();
boost::function_requires<
  boost::detail::multi_array::MutableMultiArrayConcept<subarray,ndims> >();


#ifdef __SYMBIAN32__

	std_log(LOG_FILENAME_LINE,"[End Test Case ]");
   	testResultXml("concept_checks");
	close_log_file();
#endif
  return 0;
}
