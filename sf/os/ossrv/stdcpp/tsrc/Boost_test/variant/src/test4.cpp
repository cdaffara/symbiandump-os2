//-----------------------------------------------------------------------------
// boost-libs variant/test/test4.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2003
// Eric Friedman, Itay Maman
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include "boost/test/minimal.hpp"
#include "boost/variant.hpp"

#include "jobs.h"

#include <string>
#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif
struct class_a;

using boost::variant;

typedef variant<std::string, class_a, float> var_type_1;
typedef variant<std::string, class_a, short> var_type_2;

#include "class_a.h"

int test_main(int , char* [])
{
   using namespace boost;
std_log(LOG_FILENAME_LINE,"[Test Case for test4]");
   var_type_1 v1;
   var_type_2 v2;

   v1 = class_a();
   verify(v1, spec<class_a>(), "[V] class_a(5511)");

   verify(v2, spec<std::string>(), "[V] ");

   v2 = "abcde";
   verify(v2, spec<std::string>(), "[V] abcde");

   v2 = v1;
   verify(v2, spec<class_a>(), "[V] class_a(5511)");

   v2 = 5;
   v1 = v2;
#ifdef __SYMBIAN32__
   	testResultXml("test4");
	close_log_file();
#endif
   return boost::exit_success;
}
