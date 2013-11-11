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


#include <iostream>


struct Fun
{
    Fun()
    {
        std::cout << "fun" << std::endl;
    }
    
    template< class T >
    void operator()( T t )
    {
    }

    template< class T >
    Fun& operator,( T r ) 
    {
        return *this;
    }
};

template< class T >
Fun foo( T t )
{
    return Fun();
}



struct one
{
    one()
    {
        std::cout <<"one" << std::endl;
    }
};

struct two
{
    two()
    {
        std::cout <<"two" << std::endl;
    }
};

#include <boost/test/included/test_exec_monitor.hpp>
int test_main(int,char *[])
{
    foo( one() )( two() );
    Fun(),one(),two(),one();
    return 0;
}
