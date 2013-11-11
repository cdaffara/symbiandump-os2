// test file for special functions.

//  (C) Copyright Hubert Holin 2003.
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/


#include <iomanip>


#include <boost/mpl/list.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/test_case_template.hpp>


template<typename T>
struct string_type_name;

#define DEFINE_TYPE_NAME(Type)              \
template<> struct string_type_name<Type>    \
{                                           \
    static char const * _()                 \
    {                                       \
        return #Type;                       \
    }                                       \
}

DEFINE_TYPE_NAME(float);
DEFINE_TYPE_NAME(double);
#ifndef __SYMBIAN32__
DEFINE_TYPE_NAME(long double);
#endif

#ifdef __SYMBIAN32__
typedef boost::mpl::list<float,double>  test_types;
#else
typedef boost::mpl::list<float,double,long double>  test_types;
#endif

// Apple GCC 4.0 uses the "double double" format for its long double,
// which means that epsilon is VERY small but useless for
// comparisons. So, don't do those comparisons.
#if defined(__APPLE_CC__) && defined(__GNUC__) && __GNUC__ == 4 || defined (__SYMBIAN32__)
typedef boost::mpl::list<float,double>  near_eps_test_types;
#else
typedef boost::mpl::list<float,double,long double>  near_eps_test_types;
#endif

#include "sinc_test.hpp"
#include "sinhc_test.hpp"
#include "atanh_test.hpp"
#include "asinh_test.hpp"
#include "acosh_test.hpp"



#include <boost/test/included/test_exec_monitor.hpp>
int test_main(int,char *[])
{
 
    
#define BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(fct)   \
      BOOST_TEST_CASE_TEMPLATE(fct##_test, test_types);

#define BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR_NEAR_EPS(fct)   \
        BOOST_TEST_CASE_TEMPLATE(fct##_test, near_eps_test_types);
    
    
#define BOOST_SPECIAL_FUNCTIONS_COMMON_TEST             \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(atanh)     \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(asinh)     \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(acosh)     \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(sinc_pi)   \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(sinhc_pi)
    
#define BOOST_SPECIAL_FUNCTIONS_TEMPLATE_TEMPLATE_TEST          \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR_NEAR_EPS(sinc_pi_complex)   \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR_NEAR_EPS(sinhc_pi_complex)
    
    
#ifdef  BOOST_NO_TEMPLATE_TEMPLATES

#define BOOST_SPECIAL_FUNCTIONS_TEST    \
    BOOST_SPECIAL_FUNCTIONS_COMMON_TEST \
    BOOST_MESSAGE("Warning: no template templates; curtailed functionality.");
    
#else   /* BOOST_NO_TEMPLATE_TEMPLATES */

#define BOOST_SPECIAL_FUNCTIONS_TEST                \
    BOOST_SPECIAL_FUNCTIONS_COMMON_TEST             \
    BOOST_SPECIAL_FUNCTIONS_TEMPLATE_TEMPLATE_TEST
    
#endif  /* BOOST_NO_TEMPLATE_TEMPLATES */
    
    
    BOOST_SPECIAL_FUNCTIONS_TEST
    
    
#undef  BOOST_SPECIAL_FUNCTIONS_TEST
    
#undef  BOOST_SPECIAL_FUNCTIONS_TEMPLATE_TEMPLATE_TEST

#undef  BOOST_SPECIAL_FUNCTIONS_COMMON_TEST
    
#undef  BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR

#undef  BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR_NEAR_EPS
    
#ifdef    BOOST_SPECIAL_FUNCTIONS_TEST_VERBOSE
        
    using    ::std::numeric_limits;
    
    BOOST_MESSAGE("epsilon");

#ifdef __SYMBIAN32__    
    BOOST_MESSAGE( ::std::setw(15) << numeric_limits<float>::epsilon()
                << ::std::setw(15) << numeric_limits<double>::epsilon());
#else
	BOOST_MESSAGE( ::std::setw(15) << numeric_limits<float>::epsilon()
                << ::std::setw(15) << numeric_limits<double>::epsilon()               
                << ::std::setw(15) << numeric_limits<long double>::epsilon());
#endif
    
    BOOST_MESSAGE(" ");
    atanh_manual_check();
    asinh_manual_check();
    acosh_manual_check();
    sinc_pi_manual_check();
    sinhc_pi_manual_check();
    
#endif    /* BOOST_SPECIAL_FUNCTIONS_TEST_VERBOSE */
    
    return 0;
}


