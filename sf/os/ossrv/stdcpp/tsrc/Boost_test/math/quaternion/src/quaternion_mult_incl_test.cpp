// test file for quaternion.hpp

//  (C) Copyright Hubert Holin 2001.
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include "quaternion_mi1.h"
#include "quaternion_mi2.h"

#include <boost/test/included/test_exec_monitor.hpp>
int test_main(int,char *[])
{

	quaternion_mi1();
  	quaternion_mi2();
	return 0;
	
}
