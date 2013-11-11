// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#define _STLP_DO_IMPORT_CSTD_FUNCTIONS

#include <limits>
#include <cmath>
#include <complex>
//We also test math functions imported from stdlib.h or
//defined in cstdlib
#include <cstdlib>

#include "math_aux.h"
#include "cppunit/cppunit_proxy.h"

//This test purpose is to check the right import of math.h C symbols
//into the std namespace so we do not use the using namespace std
//specification

//
// TestCase class
//
class CMathTest : public CPPUNIT_NS::TestCase // codescanner::missingcclass
{
  CPPUNIT_TEST_SUITE(CMathTest);
  CPPUNIT_TEST(test);
  CPPUNIT_TEST(complex_double_cov1);
  CPPUNIT_TEST(complex_double_cov2);
  CPPUNIT_TEST(complex_double_cov3);
  CPPUNIT_TEST(complex_double_cov4);
  CPPUNIT_TEST(complex_float_cov1);
  CPPUNIT_TEST(complex_float_cov2);
  CPPUNIT_TEST(complex_float_cov3);
  CPPUNIT_TEST(complex_float_cov4);
  CPPUNIT_TEST(complex_longdouble_cov1);
  CPPUNIT_TEST(complex_longdouble_cov2);
  CPPUNIT_TEST(complex_longdouble_cov3);
  CPPUNIT_TEST(complex_longdouble_cov4);
  CPPUNIT_TEST(complex_trigonometric_cov);
  CPPUNIT_TEST(complex_cmath_cov);
  CPPUNIT_TEST(complex_templates_cov1);
  CPPUNIT_TEST(complex_templates_cov2);
  CPPUNIT_TEST(complex_constructors_cov1);
  CPPUNIT_TEST(complex_constructors_cov2);
  CPPUNIT_TEST_SUITE_END();

  protected:
    void test();
    void complex_double_cov1();
    void complex_double_cov2();
    void complex_double_cov3();
    void complex_double_cov4();
    void complex_float_cov1();
    void complex_float_cov2();
    void complex_float_cov3();
    void complex_float_cov4();
    void complex_longdouble_cov1();
    void complex_longdouble_cov2();
    void complex_longdouble_cov3();
    void complex_longdouble_cov4();
    void complex_trigonometric_cov();
    void complex_cmath_cov();
    void complex_templates_cov1();
    void complex_templates_cov2();
    void complex_constructors_cov1();
    void complex_constructors_cov2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CMathTest);

//
// tests implementation
//
void CMathTest::test()
{
  int int_val = -1;
  long long_val = -1l;
  float float_val = -1.0f;
  double double_val = -1.0;
#if !defined (_STLP_NO_LONG_DOUBLE)
  long double long_double_val = -1.0l;
#endif

  CPPUNIT_CHECK( are_equals(std::abs(int_val), -int_val) );
  CPPUNIT_CHECK( are_equals(std::abs(long_val), -long_val) );
  CPPUNIT_CHECK( are_equals(std::labs(long_val), -long_val) );
  CPPUNIT_CHECK( are_equals(std::abs(float_val), -float_val) );
  CPPUNIT_CHECK( are_equals(std::abs(double_val), -double_val) );
#if !defined (_STLP_NO_LONG_DOUBLE)
  CPPUNIT_CHECK( are_equals(std::abs(long_double_val), -long_double_val) );
#endif

  CPPUNIT_CHECK( are_equals(std::fabs(float_val), -float_val) );
  CPPUNIT_CHECK( are_equals(std::fabs(double_val), -double_val) );
#if !defined (_STLP_NO_LONG_DOUBLE)
  CPPUNIT_CHECK( are_equals(std::fabs(long_double_val), -long_double_val) );
#endif

  std::div_t div_res = std::div(3, 2);
  CPPUNIT_CHECK( div_res.quot == 1 );
  CPPUNIT_CHECK( div_res.rem == 1 );
  std::ldiv_t ldiv_res = std::ldiv(3l, 2l);
  CPPUNIT_CHECK( ldiv_res.quot == 1l );
  CPPUNIT_CHECK( ldiv_res.rem == 1l );
  ldiv_res = std::div(3l, 2l);
  CPPUNIT_CHECK( ldiv_res.quot == 1l );
  CPPUNIT_CHECK( ldiv_res.rem == 1l );

  std::srand(2);
  int rand_val = std::rand();
  CPPUNIT_CHECK( rand_val >= 0 && rand_val <= RAND_MAX );

  CPPUNIT_CHECK( are_equals(std::floor(1.5), 1.0) );
  CPPUNIT_CHECK( are_equals(std::ceil(1.5), 2.0) );
  CPPUNIT_CHECK( are_equals(std::fmod(1.5, 1.0), 0.5) );
  CPPUNIT_CHECK( are_equals(std::sqrt(4.0), 2.0) );
  CPPUNIT_CHECK( are_equals(std::pow(2.0, 2), 4.0) );
  /*
   * Uncomment the following to check that it generates an ambiguous call
   * as there is no Standard pow(int, int) function only pow(double, int),
   * pow(float, int) and some others...
   * If it do not generate a compile time error it should at least give
   * the good result.
   */
  //CPPUNIT_CHECK( are_equals(std::pow(10, -2), 0.01) );
  CPPUNIT_CHECK( are_equals(std::pow(10.0, -2), 0.01) );
  CPPUNIT_CHECK( are_equals(std::exp(0.0), 1.0) );
  CPPUNIT_CHECK( are_equals(std::log(std::exp(1.0)), 1.0) );
  CPPUNIT_CHECK( are_equals(std::log10(100.0), 2.0) );
#if !defined (STLPORT) || !defined (_STLP_USING_PLATFORM_SDK_COMPILER) || !defined (_WIN64)
  CPPUNIT_CHECK( are_equals(std::modf(100.5, &double_val), 0.5) );
  CPPUNIT_CHECK( are_equals(double_val, 100.0) );
#endif
  double_val = std::frexp(8.0, &int_val);
  CPPUNIT_CHECK( are_equals(double_val * std::pow(2.0, int_val), 8.0) );
  CPPUNIT_CHECK( are_equals(std::ldexp(1.0, 2), 4.0) );
  CPPUNIT_CHECK( are_equals(std::cos(std::acos(1.0)), 1.0) );
  CPPUNIT_CHECK( are_equals(std::sin(std::asin(1.0)), 1.0) );
  CPPUNIT_CHECK( are_equals(std::tan(std::atan(1.0)), 1.0) );
  CPPUNIT_CHECK( are_equals(std::tan(std::atan2(1.0, 1.0)), 1.0) );
  CPPUNIT_CHECK( are_equals(std::cosh(0.0), 1.0) );
  CPPUNIT_CHECK( are_equals(std::sinh(0.0), 0.0) );
#if !defined (STLPORT) || !defined (_STLP_USING_PLATFORM_SDK_COMPILER) || !defined (_M_AMD64)
  CPPUNIT_CHECK( are_equals(std::tanh(0.0), 0.0) );
#endif

  CPPUNIT_CHECK( are_equals(std::floor(1.5f), 1.0f) );
  CPPUNIT_CHECK( are_equals(std::ceil(1.5f), 2.0f) );
  CPPUNIT_CHECK( are_equals(std::fmod(1.5f, 1.0f), 0.5f) );
  CPPUNIT_CHECK( are_equals(std::sqrt(4.0f), 2.0f) );
  CPPUNIT_CHECK( are_equals(std::pow(2.0f, 2), 4.0f) );
  CPPUNIT_CHECK( are_equals(std::exp(0.0f), 1.0f) );
  CPPUNIT_CHECK( are_equals(std::log(std::exp(1.0f)), 1.0f) );
  CPPUNIT_CHECK( are_equals(std::log10(100.0f), 2.0f) );
#if !defined (STLPORT) || !defined (_STLP_USING_PLATFORM_SDK_COMPILER) || !defined (_WIN64)
  CPPUNIT_CHECK( are_equals(std::modf(100.5f, &float_val), 0.5f) );
  CPPUNIT_CHECK( are_equals(float_val, 100.0f) );
#endif
  float_val = std::frexp(8.0f, &int_val);
  CPPUNIT_CHECK( are_equals(float_val * std::pow(2.0f, int_val), 8.0f) );
  CPPUNIT_CHECK( are_equals(std::ldexp(1.0f, 2), 4.0f) );
  CPPUNIT_CHECK( are_equals(std::cos(std::acos(1.0f)), 1.0f) );
  CPPUNIT_CHECK( are_equals(std::sin(std::asin(1.0f)), 1.0f) );
  CPPUNIT_CHECK( are_equals(std::tan(std::atan(1.0f)), 1.0f) );
  CPPUNIT_CHECK( are_equals(std::tan(std::atan2(1.0f, 1.0f)), 1.0f) );
  CPPUNIT_CHECK( are_equals(std::cosh(0.0f), 1.0f) );
  CPPUNIT_CHECK( are_equals(std::sinh(0.0f), 0.0f) );
#if !defined (STLPORT) || !defined (_STLP_USING_PLATFORM_SDK_COMPILER) || !defined (_M_AMD64)
  CPPUNIT_CHECK( are_equals(std::tanh(0.0f), 0.0f) );
#endif

#if !defined (_STLP_NO_LONG_DOUBLE)
  CPPUNIT_CHECK( are_equals(std::floor(1.5l), 1.0l) );
  CPPUNIT_CHECK( are_equals(std::ceil(1.5l), 2.0l) );
  CPPUNIT_CHECK( are_equals(std::fmod(1.5l, 1.0l), 0.5l) );
  CPPUNIT_CHECK( are_equals(std::sqrt(4.0l), 2.0l) );
  CPPUNIT_CHECK( are_equals(std::pow(2.0l, 2), 4.0l) );
  CPPUNIT_CHECK( are_equals(std::exp(0.0l), 1.0l) );
  CPPUNIT_CHECK( are_equals(std::log(std::exp(1.0l)), 1.0l) );
  CPPUNIT_CHECK( are_equals(std::log10(100.0l), 2.0l) );
#  if !defined (STLPORT) || !defined (_STLP_USING_PLATFORM_SDK_COMPILER) || !defined (_WIN64)
  CPPUNIT_CHECK( are_equals(std::modf(100.5l, &long_double_val), 0.5l) );
  CPPUNIT_CHECK( are_equals(long_double_val, 100.0l) );
#  endif
  long_double_val = std::frexp(8.0l, &int_val);
  CPPUNIT_CHECK( are_equals(long_double_val * std::pow(2.0l, int_val), 8.0l) );
  CPPUNIT_CHECK( are_equals(std::ldexp(1.0l, 2), 4.0l) );
  CPPUNIT_CHECK( are_equals(std::cos(std::acos(1.0l)), 1.0l) );
  CPPUNIT_CHECK( are_equals(std::sin(std::asin(1.0l)), 1.0l) );
  CPPUNIT_CHECK( are_equals(std::tan(0.0l), 0.0l) );
  CPPUNIT_CHECK( are_equals(std::atan(0.0l), 0.0l) );
  CPPUNIT_CHECK( are_equals(std::atan2(0.0l, 1.0l), 0.0l) );
  CPPUNIT_CHECK( are_equals(std::cosh(0.0l), 1.0l) );
  CPPUNIT_CHECK( are_equals(std::sinh(0.0l), 0.0l) );
#  if !defined (STLPORT) || !defined (_STLP_USING_PLATFORM_SDK_COMPILER) || !defined (_M_AMD64)
  CPPUNIT_CHECK( are_equals(std::tanh(0.0l), 0.0l) );
#  endif
#endif

  CPPUNIT_CHECK( are_equals(std::sqrt(std::sqrt(std::sqrt(256.0))), 2.0) );
  CPPUNIT_CHECK( are_equals(std::sqrt(std::sqrt(std::sqrt(256.0f))), 2.0f) );
#if !defined (_STLP_NO_LONG_DOUBLE)
  CPPUNIT_CHECK( are_equals(std::sqrt(std::sqrt(std::sqrt(256.0l))), 2.0l) );
#endif
}
void CMathTest::complex_double_cov1()
	{
	using namespace std;
	double pi = 3.14159265359;
	// Test Case for abs,arg abd SQRT
	{
	complex <double> c1( polar ( 5.0 ) ); 
	complex <double> c2 ( polar ( 5.0 , pi / 6 ) );
	double absc1 = abs ( c1 );
	double argc1 = arg ( c1 );
	CPPUNIT_CHECK( absc1 == 5 );
	CPPUNIT_CHECK( argc1 == 0 );
	double normc2 = norm ( c2 );
	double sqrtnormc2 = sqrt ( normc2 );
	CPPUNIT_CHECK(sqrtnormc2 == 5);
	}
	// Test case for the real,imag and conj
	{
	complex <double> c1 ( 4.0 , 3.0 );
	double dr1 = real ( c1 );
	CPPUNIT_CHECK(dr1 == 4);
	double di1 = imag ( c1 );
	CPPUNIT_CHECK(di1 == 3);
	complex <double> c2 = conj ( c1 );
	double dr2 = real ( c2 );
	CPPUNIT_CHECK(dr2 == 4);
	double di2 = imag ( c2 );
	CPPUNIT_CHECK(di2 == -3);
	}
	// test case for the operator "=" , the 3 templates 
	{
	complex <double> cl1 ( polar (3.0 , pi / 6 ) );
	complex <double> cr1a ( polar (2.0 , pi /3 ) );
	complex <double> cr1b ( polar (2.0 , pi / 3 ) );
	CPPUNIT_CHECK(cl1 != cr1a);
	CPPUNIT_CHECK(cr1b == cr1a);
	
	double cl3a =3;
	double cl3b =5;
	complex <double> cr3a ( 3 , 4 );
	complex <double> cr3b ( 5 ,0 );
	CPPUNIT_CHECK(cl3a != cr3a);
	CPPUNIT_CHECK(cl3b == cr3b);
	
	complex <int> cl2a ( 3 , 4 );
	complex <int> cl2b ( 5 ,0 );
	int cr2a =3;
	CPPUNIT_CHECK(cl2a != cr2a);
	}
}
void CMathTest::complex_double_cov2()
	{
	using namespace std;
	double pi = 3.14159265359;
	// test case for the operator "*" , the 3 templates 
	{
	complex <double> cl1 ( polar (3.0 , pi / 6 ) );
	complex <double> cr1 ( 5,0 );
	complex <double> cs1 = cl1 * cr1;	
	complex <double> cl2 ( polar ( 3.0 , pi / 6 ) );
	double cr2 =5;
	complex <double> cs2 = cl2 * cr2;
	double cl3 = 5;
	complex <double> cr3 ( polar (3.0 , pi / 6 ) );
	complex <double> cs3 = cl3 * cr3;
	
	CPPUNIT_CHECK(cs1 == cs2);
	CPPUNIT_CHECK(cs3 == cs2);
	}
	// test case for the operator "+" , the 3 templates 
	{
	complex <double> cl1 ( polar (3.0 , pi / 6 ) );
	complex <double> cr1 ( 5,0 );
	complex <double> cs1 = cl1 + cr1;	
	complex <double> cl2 ( polar ( 3.0 , pi / 6 ) );
	double cr2 =5;
	complex <double> cs2 = cl2 + cr2;
	double cl3 = 5;
	complex <double> cr3 ( polar (3.0 , pi / 6 ) );
	complex <double> cs3 = cl3 + cr3;
	
	complex <double> cr4 ( 3.0 , 4.0 );
	complex <double> cs4 = + cr4;
	   
	CPPUNIT_CHECK(cs4 == cr4);
	CPPUNIT_CHECK(cs1 == cs2);
	CPPUNIT_CHECK(cs3 == cs2);
	}
	// test case for the operator "-" , the 3 templates 
	{
	complex <double> cl1 (3 , 4);
	complex <double> cr1 ( 2,3 );
	complex <double> cs1 = cl1 - cr1;
	complex <double> cs11 ( 1,1 );
	CPPUNIT_CHECK(cs1 == cs11);
	
	complex <double> cl2 (3 , 4);
	double cr2 =5;
	complex <double> cs2 = cl2 - cr2;
	complex <double> cs22 (-2 , 4);
	CPPUNIT_CHECK(cs2 == cs22);
	
	double cl3 = 5;
	complex <double> cr3 (3 , 4);
	complex <double> cs3 = cl3 - cr3;
	complex <double> cs33 (2 , -4);
	CPPUNIT_CHECK(cs3 == cs33);
	
	complex <double> cr4 ( 3 , 4 );
	complex <double> cs4 = -cr4;
	complex <double> cs44 (-3 , -4);
	CPPUNIT_CHECK(cs4 == cs44);
	}
	// test case for the operator "/" , the 3 templates 
	{
	complex <double> cl1 (4 , 4);
	complex <double> cr1 ( 2,2 );
	complex <double> cs1 = cl1/cr1;
	complex <double> cs11 ( 2,0 );
	CPPUNIT_CHECK(cs1 == cs11);
	
	complex <double> cl2 (4 , 4);
	double cr2 =2;
	complex <double> cs2 = cl2/cr2;
	complex <double> cs22 (2 , 2);
	CPPUNIT_CHECK(cs2 == cs22);
	
	double cl3 = 4;
	complex <double> cr3 (2 , 2);
	complex <double> cs3 = cl3/cr3;
	complex <double> cs33 (1 , -1);
	CPPUNIT_CHECK(cs3 == cs33);
	}
}

void CMathTest::complex_double_cov3()
	{
	using namespace std;
	double pi = 3.14159265359;
	// test case for the operator "*=" , the 3 templates 
	{
	complex <double> cl1 ( polar (3.0 , pi / 6 ) );
	complex <double> cr1 ( 5,0 );
	complex <double> cs1 = cl1 * cr1;	
	complex <double> cs11 (10,0);	
	complex <int> cint1 ( 2,0 );
	complex <double> csresult (20,0);	
	cl1 *= cr1;
	CPPUNIT_CHECK(cs1 == cl1);
	
	double val = 2;
	cr1 *= val;
	CPPUNIT_CHECK(cr1 == cs11);	
	cs11 *= cint1;
	CPPUNIT_CHECK(csresult == cs11);	
	}
	// test case for the operator "+=" , the 3 templates 
	{
	complex <double> cl1 ( polar (3.0 , pi / 6 ) );
	complex <double> cr1 ( 5,0 );
	complex <double> cs1 = cl1 + cr1;	
	complex <double> cs11 (7,0);	
	complex <int> cint1 ( 2,0 );
	complex <double> csresult (9,0);
	cl1 += cr1;
	CPPUNIT_CHECK(cs1 == cl1);
	
	double val = 2;
	cr1 += val;
	CPPUNIT_CHECK(cr1 == cs11);	
	cs11 += cint1;
	CPPUNIT_CHECK(csresult == cs11);	
	}
}

void CMathTest::complex_double_cov4()
	{
	using namespace std;
	double pi = 3.14159265359;
	// test case for the operator "-=" , the 3 templates 
	{
	complex <double> cl1 ( polar (3.0 , pi / 6 ) );
	complex <double> cr1 ( 5,0 );
	complex <double> cs1 = cl1 - cr1;	
	complex <double> cs11 (3,0);	
	complex <int> cint1 ( 2,0 );
	complex <double> csresult (1,0);
	cl1 -= cr1;
	CPPUNIT_CHECK(cs1 == cl1);
	
	double val = 2;
	cr1 -= val;
	CPPUNIT_CHECK(cr1 == cs11);	
	cs11 -= cint1;
	CPPUNIT_CHECK(csresult == cs11);
	}
	// test case for the operator "/=" , the 3 templates 
	{
	complex <double> cl1 ( 10,0 );
	complex <double> cr1 ( 5,0 );
	complex <double> cs1 = cl1/cr1;	
	complex <double> cs11 (1,0);
	complex <int> cint1 ( 1,0 );
	complex <double> csresult (1,0);	
	cl1 /= cr1;
	CPPUNIT_CHECK(cs1 == cl1);
	
	double val = 5;
	cr1 /= val;
	CPPUNIT_CHECK(cr1 == cs11);
	cs11 /= cint1;
	CPPUNIT_CHECK(csresult == cs11);
	}
	// test case for the operator "=" , the 3 templates 
	{
	complex <double> cl1 ( 3.0 , 4.0 );
	complex <double> cr1 ( 2.0 , -1.0 );
	cl1  = cr1;
	CPPUNIT_CHECK(cl1 == cr1);
	complex <double> cl2 ( -2 , 4 );
	double cr2 =5.0;
	cl2 = cr2;
	CPPUNIT_CHECK(cl2 == cr2);
	complex<double> cl3(3.0, 4.0);
	cl2 = cl3;
	CPPUNIT_CHECK(cl2 == cl3);
	}
}

void CMathTest::complex_float_cov1()
	{
		using namespace std;
		// test case for the abs,arg and sqrt 
		{
		complex <float> c1( polar ( 5.0 ) ); 
		complex <float> c2 ( 5,0 );
		float absc1 = abs ( c1 );
		float argc1 = arg ( c1 );
		CPPUNIT_CHECK( absc1 == 5 );
		CPPUNIT_CHECK( argc1 == 0 );
		float normc2 = norm ( c2 );
		float val = 25;
		float sqrtnormc2 = sqrt ( normc2 );
		CPPUNIT_CHECK(normc2 == val);
		CPPUNIT_CHECK(sqrtnormc2 == 5);
		}
		// test case for the real,imag and conj
		{
		complex <float> c1 ( 4.0 , 3.0 );
		float dr1 = real ( c1 );
	    CPPUNIT_CHECK(dr1 == 4);
	    float di1 = imag ( c1 );
	    CPPUNIT_CHECK(di1 == 3);
	    complex <float> c2 = conj ( c1 );
		float dr2 = real ( c2 );
		CPPUNIT_CHECK(dr2 == 4);
		float di2 = imag ( c2 );
		CPPUNIT_CHECK(di2 == -3);
		}
		// test case for the operator "=" , the 3 templates 
		{
	    complex <float> cl1 ( 7,7 );
	    complex <float> cr1a ( 5,7 );
	    complex <float> cr1b ( 5,7 );
		CPPUNIT_CHECK(cl1 != cr1a);
		CPPUNIT_CHECK(cr1b == cr1a);
		
		float cl3a =3;
		float cl3b =5;
	    complex <float> cr3a ( 3 , 4 );
	    complex <float> cr3b ( 5 ,0 );
	    CPPUNIT_CHECK(cl3a != cr3a);
	    CPPUNIT_CHECK(cl3b == cr3b);
	    
	    complex <int> cl2a ( 3 , 4 );
	    complex <int> cl2b ( 5 ,0 );
	    int cr2a =3;
	    CPPUNIT_CHECK(cl2a != cr2a);
		}
	}
void CMathTest::complex_float_cov2()
	{
	using namespace std;
		// test case for the operator "*" , the 3 templates 
		{
	    complex <float> cl1 ( 7,5 );
		complex <float> cr1 ( 5,0 );
		complex <float> cs1 = cl1 * cr1;	
		complex <float> cl2 ( 7,5 );
		float cr2 =5;
		complex <float> cs2 = cl2 * cr2;
		float cl3 = 5;
		complex <float> cr3 ( 7,5 );
		complex <float> cs3 = cl3 * cr3;
		
		CPPUNIT_CHECK(cs1 == cs2);
		CPPUNIT_CHECK(cs3 == cs2);
		}
		// test case for the operator "+" , the 3 templates 
		{
	    complex <float> cl1 ( 7,0 );
		complex <float> cr1 ( 5,0 );
		complex <float> cs1 = cl1 + cr1;	
		complex <float> cl2 ( 7,0 );
		float cr2 =5;
		complex <float> cs2 = cl2 + cr2;
		float cl3 = 5;
		complex <float> cr3 ( 7,0 );
		complex <float> cs3 = cl3 + cr3;
		
		complex <float> cr4 ( 3.0 , 4.0 );
	    complex <float> cs4 = + cr4;
		   
	    CPPUNIT_CHECK(cs4 == cr4);
		CPPUNIT_CHECK(cs1 == cs2);
		CPPUNIT_CHECK(cs3 == cs2);
		}
		// test case for the operator "-" , the 3 templates 
		{
	    complex <float> cl1 (3 , 4);
		complex <float> cr1 ( 2,3 );
		complex <float> cs1 = cl1 - cr1;
		complex <float> cs11 ( 1,1 );
	    CPPUNIT_CHECK(cs1 == cs11);
	    
		complex <float> cl2 (3 , 4);
		float cr2 =5;
		complex <float> cs2 = cl2 - cr2;
		complex <float> cs22 (-2 , 4);
		CPPUNIT_CHECK(cs2 == cs22);
		
		float cl3 = 5;
		complex <float> cr3 (3 , 4);
		complex <float> cs3 = cl3 - cr3;
		complex <float> cs33 (2 , -4);
		CPPUNIT_CHECK(cs3 == cs33);
		
		complex <float> cr4 ( 3 , 4 );
	    complex <float> cs4 = -cr4;
		complex <float> cs44 (-3 , -4);
		CPPUNIT_CHECK(cs4 == cs44);
		}
		// test case for the operator "/" , the 3 templates 
		{
	    complex <float> cl1 (4 , 4);
		complex <float> cr1 ( 2,2 );
		complex <float> cs1 = cl1/cr1;
		complex <float> cs11 ( 2,0 );
	    CPPUNIT_CHECK(cs1 == cs11);
	    
		complex <float> cl2 (4 , 4);
		float cr2 =2;
		complex <float> cs2 = cl2/cr2;
		complex <float> cs22 (2 , 2);
		CPPUNIT_CHECK(cs2 == cs22);
		
		float cl3 = 4;
		complex <float> cr3 (2 , 2);
		complex <float> cs3 = cl3/cr3;
		complex <float> cs33 (1 , -1);
		CPPUNIT_CHECK(cs3 == cs33);
		}
	}
void CMathTest::complex_float_cov3()
	{
		using namespace std;
		// test case for the operator "*=" , the 3 templates 
		{
	    complex <float> cl1 ( 7,0 );
		complex <float> cr1 ( 5,0 );
		complex <float> cs1 = cl1 * cr1;	
		complex <float> cs11 (10,0);	
		complex <int> cint1 ( 2,0 );
		complex <float> csresult (20,0);	
		cl1 *= cr1;
		CPPUNIT_CHECK(cs1 == cl1);
	
		float val = 2;
		cr1 *= val;
		CPPUNIT_CHECK(cr1 == cs11);	
		cs11 *= cint1;
		CPPUNIT_CHECK(csresult == cs11);	
		}
		// test case for the operator "+=" , the 3 templates 
		{
	    complex <float> cl1 ( 7,0 );
		complex <float> cr1 ( 5,0 );
		complex <float> cs1 = cl1 + cr1;	
		complex <float> cs11 (7,0);	
		complex <int> cint1 ( 2,0 );
		complex <float> csresult (9,0);
		cl1 += cr1;
		CPPUNIT_CHECK(cs1 == cl1);
	
		float val = 2;
		cr1 += val;
		CPPUNIT_CHECK(cr1 == cs11);	
		cs11 += cint1;
		CPPUNIT_CHECK(csresult == cs11);	
		}
		// test case for the operator "-=" , the 3 templates 
		{
	    complex <float> cl1 ( 7,7 );
		complex <float> cr1 ( 5,0 );
		complex <float> cs1 = cl1 - cr1;	
		complex <float> cs11 (3,0);	
		complex <int> cint1 ( 2,0 );
		complex <float> csresult (1,0);
		cl1 -= cr1;
		CPPUNIT_CHECK(cs1 == cl1);
	
		float val = 2;
		cr1 -= val;
		CPPUNIT_CHECK(cr1 == cs11);	
		cs11 -= cint1;
		CPPUNIT_CHECK(csresult == cs11);
		}
	}
void CMathTest::complex_float_cov4()
	{
		using namespace std;
		// test case for the operator "/=" , the 3 templates 
		{
	    complex <float> cl1 ( 10,0 );
		complex <float> cr1 ( 5,0 );
		complex <float> cs1 = cl1/cr1;	
		complex <float> cs11 (1,0);
		complex <int> cint1 ( 1,0 );
		complex <float> csresult (1,0);	
		cl1 /= cr1;
		CPPUNIT_CHECK(cs1 == cl1);
	
		float val = 5;
		cr1 /= val;
		CPPUNIT_CHECK(cr1 == cs11);
		cs11 /= cint1;
		CPPUNIT_CHECK(csresult == cs11);	
		}
		// test case for the operator "=" , the 3 templates 
		{
		complex <float> cl1 ( 3.0 , 4.0 );
		complex <float> cr1 ( 2.0 , -1.0 );
		cl1  = cr1;
		CPPUNIT_CHECK(cl1 == cr1);
		complex <float> cl2 ( -2 , 4 );
		float cr2 =5.0;
		cl2 = cr2;
		CPPUNIT_CHECK(cl2 == cr2);
		complex<float> cl3(3.0, 4.0);
		cl2 = cl3;
		CPPUNIT_CHECK(cl2 == cl3);
		}
}

void CMathTest::complex_longdouble_cov1()
		{
		using namespace std;
		// test case for the abs,arg and sqrt
		{
		complex <long double> c1( polar ( 5.0 ) ); 
		complex <long double> c2 ( 5,0 );
		long double absc1 = abs ( c1 );
		long double argc1 = arg ( c1 );
		CPPUNIT_CHECK( absc1 == 5 );
		CPPUNIT_CHECK( argc1 == 0 );
		long double normc2 = norm ( c2 );
		long double val = 25;
		long double sqrtnormc2 = sqrt ( normc2 );
		CPPUNIT_CHECK(normc2 == val);
		CPPUNIT_CHECK(sqrtnormc2 == 5);
		}
		// test case for the real,imag and conj 
		{
		complex <long double> c1 ( 4.0 , 3.0 );
		long double dr1 = real ( c1 );
	    CPPUNIT_CHECK(dr1 == 4);
	    long double di1 = imag ( c1 );
	    CPPUNIT_CHECK(di1 == 3);
	    complex <long double> c2 = conj ( c1 );
		long double dr2 = real ( c2 );
		CPPUNIT_CHECK(dr2 == 4);
		long double di2 = imag ( c2 );
		CPPUNIT_CHECK(di2 == -3);
		}
		// test case for the operator "=" , the 3 templates 
		{
	    complex <long double> cl1 ( 7,7 );
	    complex <long double> cr1a ( 5,7 );
	    complex <long double> cr1b ( 5,7 );
		CPPUNIT_CHECK(cl1 != cr1a);
		CPPUNIT_CHECK(cr1b == cr1a);
		
		long double cl3a =3;
		long double cl3b =5;
	    complex <long double> cr3a ( 3 , 4 );
	    complex <long double> cr3b ( 5 ,0 );
	    CPPUNIT_CHECK(cl3a != cr3a);
	    CPPUNIT_CHECK(cl3b == cr3b);
	    
	    complex <int> cl2a ( 3 , 4 );
	    complex <int> cl2b ( 5 ,0 );
	    int cr2a =3;
	    CPPUNIT_CHECK(cl2a != cr2a);
		}
	}
void CMathTest::complex_longdouble_cov2()
	{
	using namespace std;
		// test case for the operator "*" , the 3 templates 
		{
	    complex <long double> cl1 ( 7,5 );
		complex <long double> cr1 ( 5,0 );
		complex <long double> cs1 = cl1 * cr1;	
		complex <long double> cl2 ( 7,5 );
		long double cr2 =5;
		complex <long double> cs2 = cl2 * cr2;
		long double cl3 = 5;
		complex <long double> cr3 ( 7,5 );
		complex <long double> cs3 = cl3 * cr3;
		
		CPPUNIT_CHECK(cs1 == cs2);
		CPPUNIT_CHECK(cs3 == cs2);
		}
		// test case for the operator "+" , the 3 templates 
		{
	    complex <long double> cl1 ( 7,0 );
		complex <long double> cr1 ( 5,0 );
		complex <long double> cs1 = cl1 + cr1;	
		complex <long double> cl2 ( 7,0 );
		long double cr2 =5;
		complex <long double> cs2 = cl2 + cr2;
		long double cl3 = 5;
		complex <long double> cr3 ( 7,0 );
		complex <long double> cs3 = cl3 + cr3;
		
		complex <long double> cr4 ( 3.0 , 4.0 );
	    complex <long double> cs4 = + cr4;
		   
	    CPPUNIT_CHECK(cs4 == cr4);
		CPPUNIT_CHECK(cs1 == cs2);
		CPPUNIT_CHECK(cs3 == cs2);
		}
		// test case for the operator "-" , the 3 templates 
		{
	    complex <long double> cl1 (3 , 4);
		complex <long double> cr1 ( 2,3 );
		complex <long double> cs1 = cl1 - cr1;
		complex <long double> cs11 ( 1,1 );
	    CPPUNIT_CHECK(cs1 == cs11);
	    
		complex <long double> cl2 (3 , 4);
		long double cr2 =5;
		complex <long double> cs2 = cl2 - cr2;
		complex <long double> cs22 (-2 , 4);
		CPPUNIT_CHECK(cs2 == cs22);
		
		long double cl3 = 5;
		complex <long double> cr3 (3 , 4);
		complex <long double> cs3 = cl3 - cr3;
		complex <long double> cs33 (2 , -4);
		CPPUNIT_CHECK(cs3 == cs33);
		
		complex <long double> cr4 ( 3 , 4 );
	    complex <long double> cs4 = -cr4;
		complex <long double> cs44 (-3 , -4);
		CPPUNIT_CHECK(cs4 == cs44);
		}
		// test case for the operator "/" , the 3 templates 
		{
	    complex <long double> cl1 (4 , 4);
		complex <long double> cr1 ( 2,2 );
		complex <long double> cs1 = cl1/cr1;
		complex <long double> cs11 ( 2,0 );
	    CPPUNIT_CHECK(cs1 == cs11);
	    
		complex <long double> cl2 (4 , 4);
		long double cr2 =2;
		complex <long double> cs2 = cl2/cr2;
		complex <long double> cs22 (2 , 2);
		CPPUNIT_CHECK(cs2 == cs22);
		
		long double cl3 = 4;
		complex <long double> cr3 (2 , 2);
		complex <long double> cs3 = cl3/cr3;
		complex <long double> cs33 (1 , -1);
		CPPUNIT_CHECK(cs3 == cs33);
		}
	}
void CMathTest::complex_longdouble_cov3()
	{
	using namespace std;
		// test case for the operator "*=" , the 3 templates 
		{
	    complex <long double> cl1 ( 7,0 );
		complex <long double> cr1 ( 5,0 );
		complex <long double> cs1 = cl1 * cr1;	
		complex <long double> cs11 (10,0);	
		complex <int> cint1 ( 2,0 );
		complex <long double> csresult (20,0);	
		cl1 *= cr1;
		CPPUNIT_CHECK(cs1 == cl1);
	
		long double val = 2;
		cr1 *= val;
		CPPUNIT_CHECK(cr1 == cs11);	
		cs11 *= cint1;
		CPPUNIT_CHECK(csresult == cs11);	
		}
		// test case for the operator "+=" , the 3 templates 
		{
	    complex <long double> cl1 ( 7,0 );
		complex <long double> cr1 ( 5,0 );
		complex <long double> cs1 = cl1 + cr1;	
		complex <long double> cs11 (7,0);	
		complex <int> cint1 ( 2,0 );
		complex <long double> csresult (9,0);
		cl1 += cr1;
		CPPUNIT_CHECK(cs1 == cl1);
	
		long double val = 2;
		cr1 += val;
		CPPUNIT_CHECK(cr1 == cs11);	
		cs11 += cint1;
		CPPUNIT_CHECK(csresult == cs11);	
		}
	}
void CMathTest::complex_longdouble_cov4()
	{
	using namespace std;
		// test case for the operator "-=" , the 3 templates 
		{
	    complex <long double> cl1 ( 7,7 );
		complex <long double> cr1 ( 5,0 );
		complex <long double> cs1 = cl1 - cr1;	
		complex <long double> cs11 (3,0);	
		complex <int> cint1 ( 2,0 );
		complex <long double> csresult (1,0);
		cl1 -= cr1;
		CPPUNIT_CHECK(cs1 == cl1);
	
		long double val = 2;
		cr1 -= val;
		CPPUNIT_CHECK(cr1 == cs11);	
		cs11 -= cint1;
		CPPUNIT_CHECK(csresult == cs11);
		}
		// test case for the operator "/=" , the 3 templates 
		{
	    complex <long double> cl1 ( 10,0 );
		complex <long double> cr1 ( 5,0 );
		complex <long double> cs1 = cl1/cr1;	
		complex <long double> cs11 (1,0);
		complex <int> cint1 ( 1,0 );
		complex <long double> csresult (1,0);	
		cl1 /= cr1;
		CPPUNIT_CHECK(cs1 == cl1);
	
		long double val = 5;
		cr1 /= val;
		CPPUNIT_CHECK(cr1 == cs11);
		cs11 /= cint1;
		CPPUNIT_CHECK(csresult == cs11);	
		}
		// test case for the operator "=" , the 3 templates 
		{
		complex <long double> cl1 ( 3.0 , 4.0 );
		complex <long double> cr1 ( 2.0 , -1.0 );
		cl1  = cr1;
		CPPUNIT_CHECK(cl1 == cr1);
		complex <long double> cl2 ( -2 , 4 );
		long double cr2 =5.0;
		cl2 = cr2;
		CPPUNIT_CHECK(cl2 == cr2);
		complex<long double> cl3(3.0, 4.0);
		cl2 = cl3;
		CPPUNIT_CHECK(cl2 == cl3);
		}
}
void CMathTest::complex_trigonometric_cov()
	{
	using namespace std;
	
	complex <double> c1 ( 5,0 );
	complex <float> c2 ( 5,0 );
	complex <long double> c3 ( 5,0 );
	
	complex <double> cdresult;
	complex <float> cfresult;
	complex <long double> cldresult;
	
	cdresult = cos ( c1 );	
	cfresult = cos ( c2 );	
	cldresult = cos ( c3 );	
	
	cdresult = cosh ( c1 );	
	cfresult = cosh ( c2 );	
	cldresult = cosh ( c3 );	
	
	cdresult = sin ( c1 );	
	cfresult = sin ( c2 );	
	cldresult = sin ( c3 );	
	
	cdresult = sinh ( c1 );	
	cfresult = sinh ( c2 );	
	cldresult = sinh ( c3 );	
	
	cdresult = tan ( c1 );	
	cfresult = tan ( c2 );	
	cldresult = tan ( c3 );
	
	cdresult = tanh ( c1 );	
	cfresult = tanh ( c2 );	
	cldresult = tanh ( c3 );
	}

void CMathTest::complex_cmath_cov()
	{
	using namespace std;
		{
		complex <double> c1 ( 5,5 );
		complex <float> c2 ( 5,5 );
		complex <long double> c3 ( 5,5 );
		
		complex <double> cdresult;
		complex <float> cfresult;
		complex <long double> cldresult;
		
		CPPUNIT_CHECK( are_equals(abs(c1), (double)(sqrt(2.0)*5) ));		
		CPPUNIT_CHECK( are_equals(abs(c2), (float)(sqrt(2.0)*5) ));		
		CPPUNIT_CHECK( are_equals(abs(c3), (long double)(sqrt(2.0)*5) ));		
		
		double val1 = arg(c1);
		float val2 = arg(c2);
		long double val3 = arg(c3);
		
		cdresult = exp(c1);
		cfresult = exp(c2);
		cldresult = exp(c3);
		
		cdresult = log(c1);
		cfresult = log(c2);
		cldresult = log(c3);
		
		cdresult = log10(c1);
		cfresult = log10(c2);
		cldresult = log10(c3);
		}
		{
		complex <double> c1 ( 5,5 );
		complex <float> c2 ( 5,5 );
		complex <long double> c3 ( 5,5 );
		
		complex <double> cpower1 ( 5,5 );
		complex <float> cpower2 ( 5,5 );
		complex <long double> cpower3 ( 5,5 );
		
		complex <double> cdresult;
		complex <float> cfresult;
		complex <long double> cldresult;
		
		double dval = 2;
		float fval = 2;
		long double ldval = 2;
		
		cdresult = pow(c1 , cpower1);
		cdresult = pow(c1 , dval);
		cdresult = pow(c1 , (int)2);
		cdresult = pow(dval , cpower1);
		
		cfresult = pow(c2 , cpower2);
		cfresult = pow(c2 , fval);
		cfresult = pow(c2 , (int)2);
		cfresult = pow(fval , cpower2);
		
		cldresult = pow(c3 , cpower3);
		cldresult = pow(c3 , ldval);
		cldresult = pow(c3 , (int)2);
		cldresult = pow(ldval , cpower3);
		}
		{
		complex <double> c1 ( 5,5 );
		complex <float> c2 ( 5,5 );
		complex <long double> c3 ( 5,5 );
		
		complex <double> cdresult;
		complex <float> cfresult;
		complex <long double> cldresult;
		
		cdresult = sqrt(c1);
		cfresult = sqrt(c2);
		cldresult = sqrt(c3);
		}
		{
		double pi = 3.14159265359;
		complex <double> c1( polar ( (long double)5.0 , (long double)pi / 6 ) ); 
		complex <double> c2 ( polar ( (float)5.0 , (float)pi / 6 ) );
		double absc1 = abs ( c1 );
		double argc1 = arg ( c1 );
		CPPUNIT_CHECK( absc1 == 5 );
		float normc2 = norm ( c2 );
		float sqrtnormc2 = sqrt ( normc2 );
		CPPUNIT_CHECK(sqrtnormc2 == 5);
		}
	}

void CMathTest::complex_templates_cov1()
	{
	using namespace std;
		{
		complex <int> c1( 5,0 ); 
		complex <int> c2 ( polar ( 3 , 0) );
		complex <int> c3 ( 4 , 3 );
	    
		int dr1 = real ( c3 );
		CPPUNIT_CHECK(dr1 == 4);
	    int di1 = imag ( c3 );
		CPPUNIT_CHECK(di1 == 3);
		
		int normc2 = norm ( c2 );
		int sqrtnormc2 = sqrt ( (double)normc2 );
		CPPUNIT_CHECK(normc2 == 9);
		CPPUNIT_CHECK(sqrtnormc2 == 3);
		
		int absc1 = abs ( c1 );
		int argc1 = arg ( c1 );
		CPPUNIT_CHECK( absc1 == 5 );
		CPPUNIT_CHECK( argc1 == 0 );
		}
		{/*
    	complex <int> cl1 (4 , 4);
		complex <int> cr1 ( 2,2 );
		complex <int> cs1 = cl1/cr1;
		complex <int> cs11 ( 2,0 );
    	CPPUNIT_CHECK(cs1 == cs11);
    
		complex <int> cl2 (4 , 4);
		int cr2 =2;
		complex <int> cs2 = cl2/cr2;
		complex <int> cs22 (2 , 2);
		CPPUNIT_CHECK(cs2 == cs22);
	
		int cl3 = 4;
		complex <int> cr3 (2 , 2);
		complex <int> cs3 = cl3/cr3;
		complex <int> cs33 (1 , -1);
		CPPUNIT_CHECK(cs3 == cs33);*/
		}
		{
	    complex <int> cl1 ( 3,0 );
		complex <int> cr1 ( 5,0 );
		complex <int> cs1 = cl1 * cr1;	
		complex <int> cs11 (10,0);	
		complex <int> cint1 ( 2,0 );
		complex <float> cint11 ( 2,0 );
		complex <int> csresult (4,0);	
		cl1 *= cr1;
		CPPUNIT_CHECK(cs1 == cl1);

		int val = 2;
		cr1 *= val;
		CPPUNIT_CHECK(cr1 == cs11);	
		cint1 *= cint11;
		CPPUNIT_CHECK(csresult == cint1);	
		}
	}
void CMathTest::complex_templates_cov2()
	{
	using namespace std;
		{
	    complex <int> cl1 ( 3,0 );
		complex <int> cr1 ( 5,0 );
		complex <int> cs1 = cl1 + cr1;	
		complex <int> cs11 (7,0);	
		complex <int> cint1 ( 2,0 );
		complex <float> cint11 ( 2,0 );
		complex <int> csresult (4,0);	
		cl1 += cr1;
		CPPUNIT_CHECK(cs1 == cl1);

		int val = 2;
		cr1 += val;
		CPPUNIT_CHECK(cr1 == cs11);	
		cint1 += cint11;
		CPPUNIT_CHECK(csresult == cint1);	
		}
		{
	    complex <int> cl1 ( 3,0 );
		complex <int> cr1 ( 5,0 );
		complex <int> cs1 = cl1 - cr1;	
		complex <int> cs11 (3,0);	
		complex <int> cint1 ( 2,0 );
		complex <float> cint11 ( 2,0 );
		complex <int> csresult (0,0);	
		cl1 -= cr1;
		CPPUNIT_CHECK(cs1 == cl1);

		int val = 2;
		cr1 -= val;
		CPPUNIT_CHECK(cr1 == cs11);	
		cint1 -= cint11;
		CPPUNIT_CHECK(csresult == cint1);	
		}
		{/*
    	complex <int> cl1 ( 10,0 );
		complex <int> cr1 ( 5,0 );
		complex <int> cs1 = cl1/cr1;	
		complex <int> cs11 (1,0);
		complex <int> cint1 ( 5,0 );
		complex <double> cint11 ( 5,0 );
		complex <int> csresult (1,0);	
		cl1 /= cr1;
		CPPUNIT_CHECK(cs1 == cl1);

		int val = 5;
		cr1 /= val;
		CPPUNIT_CHECK(cr1 == cs11);
		cint1 /= cint11;
		CPPUNIT_CHECK(csresult == cint1);	*/
		}
		{
		complex <int> cl1 ( 3 , 4 );
		complex <int> cr1 ( 2 , -1 );
		cl1  = cr1;
		CPPUNIT_CHECK(cl1 == cr1);
		complex <int> cl2 ( -2 , 4 );
		int cr2 =5;
		cl2 = cr2;
		CPPUNIT_CHECK(cl2 == cr2);
		complex<int> cl3(3, 4);
		cl2 = cl3;
		CPPUNIT_CHECK(cl2 == cl3);
		complex<double> cld(5, 0);
		cl2 = cld;
		}
	}
void CMathTest::complex_constructors_cov1()
	{
	using namespace std;
		{
		complex<int> c1;
		int dr1 = real ( c1 );
	    CPPUNIT_CHECK(dr1 == 0);
	    int di1 = imag ( c1 );
	    CPPUNIT_CHECK(di1 == 0);
		}
		{
		complex<int> c1(3,4) ;
		complex<int> c2(c1);
		CPPUNIT_CHECK(c1 == c2);
		}
		{
		complex<double> c1(3.0,4.0) ;
		complex<int> c2(c1);
		double dr1 = real ( c1 );
	    CPPUNIT_CHECK(dr1 == 3);
	    double di1 = imag ( c1 );
	    CPPUNIT_CHECK(di1 == 4);
	    
	    int dr2 = real ( c2 );
	    CPPUNIT_CHECK(dr1 == 3);
	    int di2 = imag ( c2 );
	    CPPUNIT_CHECK(di1 == 4);
		}
		{
		complex<int> c1(5);
		int dr1 = real ( c1 );
	    CPPUNIT_CHECK(dr1 == 5);
	    int di1 = imag ( c1 );
	    CPPUNIT_CHECK(di1 == 0);
		}
	}
void CMathTest::complex_constructors_cov2()
	{
	using namespace std;
		{
		complex<float> c1(3.0,4.0) ;
		complex<long double> c2(4.0,5.0) ;
		complex<double> c3(c1);
		complex<double> c4(c2);
		
		float dr1 = real ( c1 );
	    CPPUNIT_CHECK(dr1 == 3);
	    float di1 = imag ( c1 );
	    CPPUNIT_CHECK(di1 == 4);
	    
	    double dr3 = real ( c3 );
	    CPPUNIT_CHECK(dr3 == 3);
	    double di3 = imag ( c3 );
	    CPPUNIT_CHECK(di3 == 4);
	    
	    double dr4 = real ( c4 );
	    CPPUNIT_CHECK(dr4 == 4);
	    double di4 = imag ( c4 );
	    CPPUNIT_CHECK(di4 == 5);
		}
		{
		complex<long double> c1(3.0,4.0) ;
		complex<float> c2(c1);
		long double dr1 = real ( c1 );
	    CPPUNIT_CHECK(dr1 == 3);
	    long double di1 = imag ( c1 );
	    CPPUNIT_CHECK(di1 == 4);
	    
	    float dr2 = real ( c2 );
	    CPPUNIT_CHECK(dr1 == 3);
	    float di2 = imag ( c2 );
	    CPPUNIT_CHECK(di1 == 4);
		}
		{
		complex<float> c1(3.0,4.0) ;
		complex<long double> c2(c1);
		float dr1 = real ( c1 );
	    CPPUNIT_CHECK(dr1 == 3);
	    float di1 = imag ( c1 );
	    CPPUNIT_CHECK(di1 == 4);
	    
	    long double dr2 = real ( c2 );
	    CPPUNIT_CHECK(dr1 == 3);
	    long double di2 = imag ( c2 );
	    CPPUNIT_CHECK(di1 == 4);
		}
	}
