//
// Boost.Pointer Container
//
//  Copyright Thorsten Ottosen 2003-2005. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/ptr_container/
//
 
#include <boost/test/unit_test.hpp>
#include "associative_test_data.hpp"
#include <boost/ptr_container/ptr_set.hpp>

#ifdef __ARMCC__
#pragma diag_suppress 3017
#endif

template< class SetDerived, class SetBase, class T >
void test_transfer()
{
    SetBase to;
    SetDerived from;
    from.insert( new T );
    from.insert( new T );
    transfer_test( from, to );
}

void test_set()
{    

    ptr_set_test< ptr_set<Base>, Base, Derived_class >();
    ptr_set_test< ptr_set<Value>, Value, Value >();

    ptr_set_test< ptr_multiset<Base>, Base, Derived_class >();
    ptr_set_test< ptr_multiset<Value>, Value, Value >();

    test_transfer< ptr_set<Derived_class>, ptr_set<Base>, Derived_class>();
    test_transfer< ptr_multiset<Derived_class>, ptr_multiset<Base>, Derived_class>();
    
    ptr_set<int> set;

    BOOST_CHECK_THROW( set.insert( 0 ), bad_ptr_container_operation );
    set.insert( new int(0) );
 	//set.insert( std::auto_ptr<int>( new int(1) ) );
    BOOST_CHECK_THROW( (set.replace(set.begin(), 0 )), bad_ptr_container_operation );
    BOOST_CHECK_THROW( (set.replace(set.begin(), std::auto_ptr<int>(0) )), bad_ptr_container_operation );


}

#include <boost/test/included/test_exec_monitor.hpp>
int test_main(int,char *[])
{
  
test_set();
  return 0;
}






