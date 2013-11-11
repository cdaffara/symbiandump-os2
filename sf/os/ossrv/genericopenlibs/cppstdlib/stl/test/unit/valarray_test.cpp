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

#include <valarray>

#include "cppunit/cppunit_proxy.h"

#if !defined (STLPORT) || defined(_STLP_USE_NAMESPACES)
using namespace std;
#endif

//
// TestCase class
//
class ValarrayTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(ValarrayTest);
  CPPUNIT_TEST(transcendentals);
  CPPUNIT_TEST(valarray_cov1);
  CPPUNIT_TEST(valarray_cov2);
  CPPUNIT_TEST(valarray_cov3);
  CPPUNIT_TEST(valarray_cov4);
  CPPUNIT_TEST(valarray_cov5);
  CPPUNIT_TEST(valarray_cov6);
  CPPUNIT_TEST(valarray_cov7);
  CPPUNIT_TEST(valarray_cov8);
  CPPUNIT_TEST(valarray_cov9);
  CPPUNIT_TEST(valarray_cov10);
  CPPUNIT_TEST(valarray_cov11);
  CPPUNIT_TEST(valarray_cov12);
  CPPUNIT_TEST_SUITE_END();

protected:
  void transcendentals();
  void valarray_cov1();
  void valarray_cov2();
  void valarray_cov3();
  void valarray_cov4();
  void valarray_cov5();
  void valarray_cov6();
  void valarray_cov7();
  void valarray_cov8();
  void valarray_cov9();
  void valarray_cov10();
  void valarray_cov11();
  void valarray_cov12();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ValarrayTest);

int MyApplyFunc( int n )
	{
	   return n*2;
	}


//
// tests implementation
//
// For the moment this test is just a complitation test
// everyone is welcome to do a real good unit test for
// valarray functionality.
void ValarrayTest::transcendentals()
{
#ifdef __SUNPRO_CC
  using std::abs;
#endif
  {
    valarray<double> darray;
    valarray<double> tmp;
    tmp = abs(darray);
    tmp = acos(darray);
    tmp = asin(darray);
    tmp = atan(darray);
    tmp = atan2(darray, tmp);
    tmp = atan2(1.0, darray);
    tmp = atan2(darray, 1.0);
    tmp = cos(darray);
    tmp = cosh(darray);
    tmp = sin(darray);
    tmp = sinh(darray);
    tmp = tan(darray);
#if !defined (STLPORT) || !defined (_STLP_USING_PLATFORM_SDK_COMPILER) || !defined (_M_AMD64)
    tmp = tanh(darray);
#endif
    tmp = exp(darray);
    tmp = log(darray);
    tmp = log10(darray);
    tmp = pow(darray, tmp);
    tmp = pow(1.0, darray);
    tmp = pow(darray, 1.0);
    tmp = sqrt(darray);
  }
  {
    valarray<float> farray;
    valarray<float> tmp;
    tmp = abs(farray);
    tmp = acos(farray);
    tmp = asin(farray);
    tmp = atan(farray);
    tmp = atan2(farray, tmp);
    tmp = atan2(1.0f, farray);
    tmp = atan2(farray, 1.0f);
    tmp = cos(farray);
    tmp = cosh(farray);
    tmp = sin(farray);
    tmp = sinh(farray);
    tmp = tan(farray);
#if !defined (STLPORT) || !defined (_STLP_USING_PLATFORM_SDK_COMPILER) || !defined (_M_AMD64)
    tmp = tanh(farray);
#endif
    tmp = exp(farray);
    tmp = log(farray);
    tmp = log10(farray);
    tmp = pow(farray, tmp);
    tmp = pow(1.0f, farray);
    tmp = pow(farray, 1.0f);
    tmp = sqrt(farray);
  }
#if !defined (STLPORT) || !defined (_STLP_NO_LONG_DOUBLE)
  {
    valarray<long double> ldarray;
    valarray<long double> tmp;
    tmp = abs(ldarray);
    tmp = acos(ldarray);
    tmp = asin(ldarray);
    tmp = atan(ldarray);
    tmp = atan2(ldarray, tmp);
    tmp = atan2(1.0l, ldarray);
    tmp = atan2(ldarray, 1.0l);
    tmp = cos(ldarray);
    tmp = cosh(ldarray);
    tmp = sin(ldarray);
    tmp = sinh(ldarray);
    tmp = tan(ldarray);
#  if !defined (STLPORT) || !defined (_STLP_USING_PLATFORM_SDK_COMPILER) || !defined (_M_AMD64)
    tmp = tanh(ldarray);
#  endif
    tmp = exp(ldarray);
    tmp = log(ldarray);
    tmp = log10(ldarray);
    tmp = pow(ldarray, tmp);
    tmp = pow(1.0l, ldarray);
    tmp = pow(ldarray, 1.0l);
    tmp = sqrt(ldarray);
  }
#endif
}
void ValarrayTest::valarray_cov1()
	{
		{/*
		int i;
		valarray<int> va ( 10 );
		
		for ( i = 0 ; i < 10 ; i += 1 )
		      va [ i ] =  2 * ( i + 1 );
		CPPUNIT_ASSERT(va.size()==10);
		
		float af1 [] = {0, 1, 2, 3};
		valarray <float> vf1 (af1, 4); 
		CPPUNIT_ASSERT(vf1.size()==4);
		
		valarray <float> vf2 (10, 3.0f); 		
		CPPUNIT_ASSERT(vf2.size()==10);
		
		valarray <float> vf3 (vf2); 		
		CPPUNIT_ASSERT(vf3.size()==10);
		
		slice Slice ( 2 , 4 , 3 );
		
		valarray<int> vaSlice = va [ Slice ];

		CPPUNIT_ASSERT(vaSlice [ 0 ] == 6);
		CPPUNIT_ASSERT(vaSlice [ 1 ] == 12);
		CPPUNIT_ASSERT(vaSlice [ 2 ] == 18);*/
		}
		{/*
		int i;

		valarray<int> va ( 20 ), vaResult;
		for ( i = 0 ; i < 20 ; i+=1 ) 
		      va [ i ] =  i;
		valarray<size_t> Len ( 2 ), Stride ( 2 );
		Len [0] = 4;
		Len [1] = 4;
		Stride [0] = 7;
		Stride [1] = 4;

		gslice vaGSlice ( 0, Len, Stride );
		vaResult = va [ vaGSlice ];
		CPPUNIT_ASSERT(vaResult [ 0 ] == 0);
		CPPUNIT_ASSERT(vaResult [ 1 ] == 4);
		CPPUNIT_ASSERT(vaResult [ 2 ] == 8);
		CPPUNIT_ASSERT(vaResult [ 3 ] == 12);
		CPPUNIT_ASSERT(vaResult [ 4 ] == 7);
		CPPUNIT_ASSERT(vaResult [ 5 ] == 11);
		CPPUNIT_ASSERT(vaResult [ 6 ] == 15);
		CPPUNIT_ASSERT(vaResult [ 7 ] == 19);*/
		}
		{
		int i;

		valarray<int> va ( 10 );
		for ( i = 0 ; i < 10 ; i += 2 )
		      va [ i ] =  i;
		for ( i = 1 ; i < 10 ; i += 2 )
		      va [ i ] =  -1;
		// Use masked subsets to assign a value of 10
		// to all elements grrater than 3 in value
		va [va > 3 ] = 10;
		CPPUNIT_ASSERT(va [ 0 ] == 0);
		CPPUNIT_ASSERT(va [ 1 ] == -1);
		CPPUNIT_ASSERT(va [ 2 ] == 2);
		CPPUNIT_ASSERT(va [ 3 ] == -1);
		CPPUNIT_ASSERT(va [ 4 ] == 10);
		CPPUNIT_ASSERT(va [ 5 ] == -1);
		CPPUNIT_ASSERT(va [ 6 ] == 10);
		CPPUNIT_ASSERT(va [ 7 ] == -1);
		CPPUNIT_ASSERT(va [ 8 ] == 10);
		CPPUNIT_ASSERT(va [ 9 ] == -1);
		}
		{
		int i;

		valarray<int> va ( 10 );
		for ( i = 0 ; i < 10 ; i += 2 )
		      va [ i ] =  i;
		for ( i = 1 ; i < 10 ; i += 2 )
		      va [ i ] =  -1;
		// Select 2nd, 4th & 6th elements
		// and assign a value of 10 to them
		valarray<size_t> indx ( 3 );
		indx [0] = 2;
		indx [1] = 4;
		indx [2] = 6;
		va[indx] = 10;
		CPPUNIT_ASSERT(va [ 0 ] == 0);
		CPPUNIT_ASSERT(va [ 1 ] == -1);
		CPPUNIT_ASSERT(va [ 2 ] == 10);
		CPPUNIT_ASSERT(va [ 3 ] == -1);
		CPPUNIT_ASSERT(va [ 4 ] == 10);
		CPPUNIT_ASSERT(va [ 5 ] == -1);
		CPPUNIT_ASSERT(va [ 6 ] == 10);
		CPPUNIT_ASSERT(va [ 7 ] == -1);
		CPPUNIT_ASSERT(va [ 8 ] == 8);
		CPPUNIT_ASSERT(va [ 9 ] == -1);
		}
	}
void ValarrayTest::valarray_cov2()
	{
		{
		int i;

		valarray<int> va ( 10 );
		for ( i = 0 ; i < 10 ; i+=1 )
		      va [ i ] =  i;
		CPPUNIT_ASSERT(va.sum ( ) == 45);
		CPPUNIT_ASSERT(va.min ( ) == 0);
		CPPUNIT_ASSERT(va.max ( ) == 9);
		}
		{
		int i;
	    valarray<int> va1 ( 3 );
		for ( i = 0 ; i < 3 ; i+=1 )
		      va1 [ i ] =  i;
		va1.resize(5, 10);
		
		CPPUNIT_ASSERT(va1 [ 0 ] == 10);
		CPPUNIT_ASSERT(va1 [ 1 ] == 10);
		CPPUNIT_ASSERT(va1 [ 2 ] == 10);
		CPPUNIT_ASSERT(va1 [ 3 ] == 10);
		CPPUNIT_ASSERT(va1 [ 4 ] == 10);
		CPPUNIT_ASSERT(va1.size ( ) == 5);
		}
		{
		int i;

		valarray<int> vaR ( 3 ),vaApplied(3);
		for ( i = 0 ; i < 3 ; i+=1 )
		      vaR [ i ] =  i;
		
		vaApplied = vaR.apply( MyApplyFunc );
		CPPUNIT_ASSERT(vaApplied [ 0 ] == 0);
		CPPUNIT_ASSERT(vaApplied [ 1 ] == 2);
		CPPUNIT_ASSERT(vaApplied [ 2 ] == 4);
		}
		{
		int i;

		valarray<int> vaL2 ( 4 );
		valarray<int> vaNOT2 ( 4 );
		for ( i = 0 ; i < 3 ; i += 2 )
		      vaL2 [ i ] =  i;
		for ( i = 1 ; i < 3 ; i += 2 )
		      vaL2 [ i ] =  -2 * i;	
		vaNOT2 = ~vaL2;
		CPPUNIT_ASSERT(vaNOT2 [ 0 ] == -1);
		CPPUNIT_ASSERT(vaNOT2 [ 1 ] == 1);
		CPPUNIT_ASSERT(vaNOT2 [ 2 ] == -3);
		}
	}
void ValarrayTest::valarray_cov3()
	{
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3);
		vaL[0] = 1;vaR[0] = 1;
		vaL[1] = 0;vaR[1] = 2;
		vaL[2] = 1;vaR[2] = 3;
		vaL1 = vaL;
		
		vaL |= vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 1);
		CPPUNIT_ASSERT(vaL [ 1 ] == 2);
		CPPUNIT_ASSERT(vaL [ 2 ] == 3);
		
		vaL1 |= 3;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 3);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == 3);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 3);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3);
		vaL[0] = 1;vaR[0] = 0;
		vaL[1] = 0;vaR[1] = 0;
		vaL[2] = 1;vaR[2] = 1;
		vaL1 = vaL;
		
		vaL ^= vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 1);
		CPPUNIT_ASSERT(vaL [ 1 ] == 0);
		CPPUNIT_ASSERT(vaL [ 2 ] == 0);
		
		vaL1 ^= 0;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 1);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == 0);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 1);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3);
		vaL[0] = 64;vaR[0] = 0;
		vaL[1] = -64;vaR[1] = 1;
		vaL[2] = 64;vaR[2] = 2;
		vaL1 = vaL;
		
		vaL >>= vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 64);
		CPPUNIT_ASSERT(vaL [ 1 ] == -32);
		CPPUNIT_ASSERT(vaL [ 2 ] == 16);
		
		vaL1 >>= 0;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 64);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -64);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 64);
		}
	}
void ValarrayTest::valarray_cov4()
	{
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3);
		vaL[0] = 1;vaR[0] = 0;
		vaL[1] = -1;vaR[1] = 1;
		vaL[2] = 1;vaR[2] = 2;
		vaL1 = vaL;
		
		vaL <<= vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 1);
		CPPUNIT_ASSERT(vaL [ 1 ] == -2);
		CPPUNIT_ASSERT(vaL [ 2 ] == 4);
		
		vaL1 <<= 2;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 4);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -4);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 4);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3);
		vaL[0] = 100;vaR[0] = 2;
		vaL[1] = -100;vaR[1] = 4;
		vaL[2] = 100;vaR[2] = 10;
		vaL1 = vaL;
		
		vaL /= vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 50);
		CPPUNIT_ASSERT(vaL [ 1 ] == -25);
		CPPUNIT_ASSERT(vaL [ 2 ] == 10);
		
		vaL1 /= 2;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 50);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -50);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 50);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3);
		vaL[0] = 10;vaR[0] = 0;
		vaL[1] = -10;vaR[1] = 1;
		vaL[2] = 10;vaR[2] = 2;
		vaL1 = vaL;
		
		vaL -= vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 10);
		CPPUNIT_ASSERT(vaL [ 1 ] == -11);
		CPPUNIT_ASSERT(vaL [ 2 ] == 8);
		
		vaL1 -= 2;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 8);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -12);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 8);
		}
	}
void ValarrayTest::valarray_cov5()
	{
		{
		valarray<int> vaL ( 3 ), vaR ( 3 );
		vaL[0] = 0;
		vaL[1] = -10;
		vaL[2] = 10;
		
		vaR = -vaL;
		CPPUNIT_ASSERT(vaR [ 0 ] == 0);
		CPPUNIT_ASSERT(vaR [ 1 ] == 10);
		CPPUNIT_ASSERT(vaR [ 2 ] == -10);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3);
		vaL[0] = 10;vaR[0] = 0;
		vaL[1] = -10;vaR[1] = 1;
		vaL[2] = 10;vaR[2] = 2;
		vaL1 = vaL;
		
		vaL += vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 10);
		CPPUNIT_ASSERT(vaL [ 1 ] == -9);
		CPPUNIT_ASSERT(vaL [ 2 ] == 12);
		
		vaL1 += 2;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 12);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -8);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 12);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3);
		vaL[0] = 2;vaR[0] = 0;
		vaL[1] = -1;vaR[1] = 1;
		vaL[2] = 2;vaR[2] = 2;
		vaL1 = vaL;
		
		vaL *= vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 0);
		CPPUNIT_ASSERT(vaL [ 1 ] == -1);
		CPPUNIT_ASSERT(vaL [ 2 ] == 4);
		
		vaL1 *= 2;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 4);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -2);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 4);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3);
		vaL[0] = 0;vaR[0] = 0;
		vaL[1] = 1;vaR[1] = 1;
		vaL[2] = 2;vaR[2] = 2;
		vaL1 = vaL;
		
		vaL &= vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 0);
		CPPUNIT_ASSERT(vaL [ 1 ] == 1);
		CPPUNIT_ASSERT(vaL [ 2 ] == 2);
		
		vaL1 &= 0;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 0);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == 0);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 0);
		}
	}
void ValarrayTest::valarray_cov6()
	{
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3);
		vaL[0] = 53;vaR[0] = 1;
		vaL[1] = -67;vaR[1] = 4;
		vaL[2] = 53;vaR[2] = 7;
		vaL1 = vaL;
		
		vaL %= vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 0);
		CPPUNIT_ASSERT(vaL [ 1 ] == -3);
		CPPUNIT_ASSERT(vaL [ 2 ] == 4);
		
		vaL1 %= 53;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 0);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -14);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 0);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3),vaL2(3);
		vaL[0] = 1;vaR[0] = 1;
		vaL[1] = 0;vaR[1] = 2;
		vaL[2] = 1;vaR[2] = 3;
		vaL1 = vaL;
		vaL2 = vaL;		
		
		vaL = vaL | vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 1);
		CPPUNIT_ASSERT(vaL [ 1 ] == 2);
		CPPUNIT_ASSERT(vaL [ 2 ] == 3);
		
		vaL1 = vaL1 | 3;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 3);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == 3);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 3);
		vaL2 = 3 | vaL2;
		CPPUNIT_ASSERT(vaL2 [ 0 ] == 3);
		CPPUNIT_ASSERT(vaL2 [ 1 ] == 3);
		CPPUNIT_ASSERT(vaL2 [ 2 ] == 3);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3),vaL2(3);
		vaL[0] = 1;vaR[0] = 0;
		vaL[1] = 0;vaR[1] = 0;
		vaL[2] = 1;vaR[2] = 1;
		vaL1 = vaL;
		vaL2 = vaL;
		
		vaL = vaL ^ vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 1);
		CPPUNIT_ASSERT(vaL [ 1 ] == 0);
		CPPUNIT_ASSERT(vaL [ 2 ] == 0);
		
		vaL1 = vaL1 ^ 0;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 1);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == 0);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 1);
		vaL2 = 0 ^ vaL2;
		CPPUNIT_ASSERT(vaL2 [ 0 ] == 1);
		CPPUNIT_ASSERT(vaL2 [ 1 ] == 0);
		CPPUNIT_ASSERT(vaL2 [ 2 ] == 1);
		}
	}
void ValarrayTest::valarray_cov7()
	{
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3),vaL2(3);
		vaL[0] = 64;vaR[0] = 0;
		vaL[1] = -64;vaR[1] = 1;
		vaL[2] = 64;vaR[2] = 2;
		vaL1 = vaL;
		vaL2 = vaL;
		
		vaL = vaL >> vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 64);
		CPPUNIT_ASSERT(vaL [ 1 ] == -32);
		CPPUNIT_ASSERT(vaL [ 2 ] == 16);
		
		vaL1 = vaL1 >> 0;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 64);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -64);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 64);
		vaL2 = 0 >> vaL2;
		CPPUNIT_ASSERT(vaL2 [ 0 ] == 0);
		CPPUNIT_ASSERT(vaL2 [ 1 ] == 0);
		CPPUNIT_ASSERT(vaL2 [ 2 ] == 0);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 );
		valarray<bool> vaLb1(3),vaLb2(3),vaLb3(3);
		vaL[0] = 1;vaR[0] = 0;
		vaL[1] = 0;vaR[1] = 0;
		vaL[2] = 1;vaR[2] = 2;
		
		vaLb1 = (vaL >= vaR);
		CPPUNIT_ASSERT(vaLb1 [ 0 ] == true);
		CPPUNIT_ASSERT(vaLb1 [ 1 ] == true);
		CPPUNIT_ASSERT(vaLb1 [ 2 ] == false);
		
		vaLb2 = (vaL >= 0);
		CPPUNIT_ASSERT(vaLb2 [ 0 ] == true);
		CPPUNIT_ASSERT(vaLb2 [ 1 ] == true);
		CPPUNIT_ASSERT(vaLb2 [ 2 ] == true);
		vaLb3 = (0 >= vaL);
		CPPUNIT_ASSERT(vaLb3 [ 0 ] == false);
		CPPUNIT_ASSERT(vaLb3 [ 1 ] == true);
		CPPUNIT_ASSERT(vaLb3 [ 2 ] == false);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 );
		valarray<bool> vaLb1(3),vaLb2(3),vaLb3(3);
		vaL[0] = 1;vaR[0] = 0;
		vaL[1] = 0;vaR[1] = 0;
		vaL[2] = 1;vaR[2] = 2;
		
		vaLb1 = (vaL > vaR);
		CPPUNIT_ASSERT(vaLb1 [ 0 ] == true);
		CPPUNIT_ASSERT(vaLb1 [ 1 ] == false);
		CPPUNIT_ASSERT(vaLb1 [ 2 ] == false);
		
		vaLb2 = (vaL > 0);
		CPPUNIT_ASSERT(vaLb2 [ 0 ] == true);
		CPPUNIT_ASSERT(vaLb2 [ 1 ] == false);
		CPPUNIT_ASSERT(vaLb2 [ 2 ] == true);
		vaLb3 = (0 > vaL);
		CPPUNIT_ASSERT(vaLb3 [ 0 ] == false);
		CPPUNIT_ASSERT(vaLb3 [ 1 ] == false);
		CPPUNIT_ASSERT(vaLb3 [ 2 ] == false);
		}
	}
void ValarrayTest::valarray_cov8()
	{
		{
		valarray<int> vaL ( 3 ), vaR ( 3 );
		valarray<bool> vaLb1(3),vaLb2(3),vaLb3(3);
		vaL[0] = 1;vaR[0] = 0;
		vaL[1] = 0;vaR[1] = 0;
		vaL[2] = 1;vaR[2] = 2;
		
		vaLb1 = (vaL == vaR);
		CPPUNIT_ASSERT(vaLb1 [ 0 ] == false);
		CPPUNIT_ASSERT(vaLb1 [ 1 ] == true);
		CPPUNIT_ASSERT(vaLb1 [ 2 ] == false);
		
		vaLb2 = (vaL == 0);
		CPPUNIT_ASSERT(vaLb2 [ 0 ] == false);
		CPPUNIT_ASSERT(vaLb2 [ 1 ] == true);
		CPPUNIT_ASSERT(vaLb2 [ 2 ] == false);
		vaLb3 = (0 == vaL);
		CPPUNIT_ASSERT(vaLb3 [ 0 ] == false);
		CPPUNIT_ASSERT(vaLb3 [ 1 ] == true);
		CPPUNIT_ASSERT(vaLb3 [ 2 ] == false);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 );
		valarray<bool> vaLb1(3),vaLb2(3),vaLb3(3);
		vaL[0] = 1;vaR[0] = 0;
		vaL[1] = 0;vaR[1] = 0;
		vaL[2] = 1;vaR[2] = 2;
		
		vaLb1 = (vaL <= vaR);
		CPPUNIT_ASSERT(vaLb1 [ 0 ] == false);
		CPPUNIT_ASSERT(vaLb1 [ 1 ] == true);
		CPPUNIT_ASSERT(vaLb1 [ 2 ] == true);
		
		vaLb2 = (vaL <= 0);
		CPPUNIT_ASSERT(vaLb2 [ 0 ] == false);
		CPPUNIT_ASSERT(vaLb2 [ 1 ] == true);
		CPPUNIT_ASSERT(vaLb2 [ 2 ] == false);
		vaLb3 = (0 <= vaL);
		CPPUNIT_ASSERT(vaLb3 [ 0 ] == true);
		CPPUNIT_ASSERT(vaLb3 [ 1 ] == true);
		CPPUNIT_ASSERT(vaLb3 [ 2 ] == true);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3),vaL2(3);
		vaL[0] = 1;vaR[0] = 0;
		vaL[1] = -1;vaR[1] = 1;
		vaL[2] = 1;vaR[2] = 2;
		vaL1 = vaL;
		vaL2 = vaL;
		
		vaL = vaL << vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 1);
		CPPUNIT_ASSERT(vaL [ 1 ] == -2);
		CPPUNIT_ASSERT(vaL [ 2 ] == 4);
		
		vaL1 = vaL1 << 2;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 4);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -4);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 4);
		vaL2 = 2 << vaL2;
		CPPUNIT_ASSERT(vaL2 [ 0 ] == 4);
		CPPUNIT_ASSERT(vaL2 [ 1 ] == 0);
		CPPUNIT_ASSERT(vaL2 [ 2 ] == 4);
		}
	}
void ValarrayTest::valarray_cov9()
	{
		{
		valarray<int> vaL ( 3 ), vaR ( 3 );
		valarray<bool> vaLb1(3),vaLb2(3),vaLb3(3);
		vaL[0] = 1;vaR[0] = 0;
		vaL[1] = 0;vaR[1] = 0;
		vaL[2] = 1;vaR[2] = 2;
		
		vaLb1 = (vaL < vaR);
		CPPUNIT_ASSERT(vaLb1 [ 0 ] == false);
		CPPUNIT_ASSERT(vaLb1 [ 1 ] == false);
		CPPUNIT_ASSERT(vaLb1 [ 2 ] == true);
		
		vaLb2 = (vaL < 0);
		CPPUNIT_ASSERT(vaLb2 [ 0 ] == false);
		CPPUNIT_ASSERT(vaLb2 [ 1 ] == false);
		CPPUNIT_ASSERT(vaLb2 [ 2 ] == false);
		vaLb3 = (0 < vaL);
		CPPUNIT_ASSERT(vaLb3 [ 0 ] == true);
		CPPUNIT_ASSERT(vaLb3 [ 1 ] == false);
		CPPUNIT_ASSERT(vaLb3 [ 2 ] == true);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3),vaL2(3);
		vaL[0] = 100;vaR[0] = 2;
		vaL[1] = -100;vaR[1] = 4;
		vaL[2] = 100;vaR[2] = 10;
		vaL1 = vaL;
		vaL2 = vaL;
		
		vaL = vaL / vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 50);
		CPPUNIT_ASSERT(vaL [ 1 ] == -25);
		CPPUNIT_ASSERT(vaL [ 2 ] == 10);
		
		vaL1 = vaL1 / 2;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 50);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -50);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 50);
		vaL2 = 1000 / vaL2 ;
		CPPUNIT_ASSERT(vaL2 [ 0 ] == 10);
		CPPUNIT_ASSERT(vaL2 [ 1 ] == -10);
		CPPUNIT_ASSERT(vaL2 [ 2 ] == 10);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3),vaL2(3);
		vaL[0] = 10;vaR[0] = 0;
		vaL[1] = -10;vaR[1] = 1;
		vaL[2] = 10;vaR[2] = 2;
		vaL1 = vaL;
		vaL2 = vaL;
		
		vaL = vaL - vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 10);
		CPPUNIT_ASSERT(vaL [ 1 ] == -11);
		CPPUNIT_ASSERT(vaL [ 2 ] == 8);
		
		vaL1 = vaL1 - 2;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 8);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -12);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 8);
		vaL2 = 2 - vaL2;
		CPPUNIT_ASSERT(vaL2 [ 0 ] == -8);
		CPPUNIT_ASSERT(vaL2 [ 1 ] == 12);
		CPPUNIT_ASSERT(vaL2 [ 2 ] == -8);
		}
	}
void ValarrayTest::valarray_cov10()
	{
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3),vaL2(3);
		vaL[0] = 10;vaR[0] = 0;
		vaL[1] = -10;vaR[1] = 1;
		vaL[2] = 10;vaR[2] = 2;
		vaL1 = vaL;
		vaL2 = vaL;
		
		vaL = vaL + vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 10);
		CPPUNIT_ASSERT(vaL [ 1 ] == -9);
		CPPUNIT_ASSERT(vaL [ 2 ] == 12);
		
		vaL1 = vaL1 + 2;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 12);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -8);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 12);
		vaL2 = 2 + vaL2;
		CPPUNIT_ASSERT(vaL2 [ 0 ] == 12);
		CPPUNIT_ASSERT(vaL2 [ 1 ] == -8);
		CPPUNIT_ASSERT(vaL2 [ 2 ] == 12);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3),vaL2(3);
		vaL[0] = 10;vaR[0] = 0;
		vaL[1] = -10;vaR[1] = 1;
		vaL[2] = 10;vaR[2] = 2;
		vaL1 = vaL;
		vaL2 = vaL;
		
		vaL = vaL * vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 0);
		CPPUNIT_ASSERT(vaL [ 1 ] == -10);
		CPPUNIT_ASSERT(vaL [ 2 ] == 20);
		
		vaL1 = vaL1 * 2;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 20);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -20);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 20);
		vaL2 = 2 * vaL2;
		CPPUNIT_ASSERT(vaL2 [ 0 ] == 20);
		CPPUNIT_ASSERT(vaL2 [ 1 ] == -20);
		CPPUNIT_ASSERT(vaL2 [ 2 ] == 20);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3),vaL2(3);
		vaL[0] = 0;vaR[0] = 0;
		vaL[1] = 1;vaR[1] = 1;
		vaL[2] = 2;vaR[2] = 2;
		vaL1 = vaL;
		vaL2 = vaL;
		
		vaL = vaL & vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 0);
		CPPUNIT_ASSERT(vaL [ 1 ] == 1);
		CPPUNIT_ASSERT(vaL [ 2 ] == 2);
		
		vaL1 = vaL1 & 0;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 0);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == 0);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 0);
		vaL2 = 0 & vaL2;
		CPPUNIT_ASSERT(vaL2 [ 0 ] == 0);
		CPPUNIT_ASSERT(vaL2 [ 1 ] == 0);
		CPPUNIT_ASSERT(vaL2 [ 2 ] == 0);
		}
	}
void ValarrayTest::valarray_cov11()
	{
		{
		valarray<int> vaL ( 3 ), vaR ( 3 ),vaL1(3),vaL2(3);
		vaL[0] = 53;vaR[0] = 1;
		vaL[1] = -67;vaR[1] = 4;
		vaL[2] = 53;vaR[2] = 7;
		vaL1 = vaL;
		vaL2 = vaL;
		
		vaL = vaL % vaR;
		CPPUNIT_ASSERT(vaL [ 0 ] == 0);
		CPPUNIT_ASSERT(vaL [ 1 ] == -3);
		CPPUNIT_ASSERT(vaL [ 2 ] == 4);
		
		vaL1 = vaL1 % 53;
		CPPUNIT_ASSERT(vaL1 [ 0 ] == 0);
		CPPUNIT_ASSERT(vaL1 [ 1 ] == -14);
		CPPUNIT_ASSERT(vaL1 [ 2 ] == 0);
		
		vaL2 = 67 % vaL2;
		CPPUNIT_ASSERT(vaL2 [ 0 ] == 14);
		CPPUNIT_ASSERT(vaL2 [ 1 ] == 0);
		CPPUNIT_ASSERT(vaL2 [ 2 ] == 14);
		}
		{
		valarray<int> vaL ( 3 ), vaR ( 3 );
		valarray<bool> vaLb1(3),vaLb2(3),vaLb3(3);
		vaL[0] = 1;vaR[0] = 0;
		vaL[1] = 0;vaR[1] = 0;
		vaL[2] = 1;vaR[2] = 2;
		
		vaLb1 = (vaL != vaR);
		CPPUNIT_ASSERT(vaLb1 [ 0 ] == true);
		CPPUNIT_ASSERT(vaLb1 [ 1 ] == false);
		CPPUNIT_ASSERT(vaLb1 [ 2 ] == true);
		
		vaLb2 = (vaL != 0);
		CPPUNIT_ASSERT(vaLb2 [ 0 ] == true);
		CPPUNIT_ASSERT(vaLb2 [ 1 ] == false);
		CPPUNIT_ASSERT(vaLb2 [ 2 ] == true);
		vaLb3 = (1 != vaL);
		CPPUNIT_ASSERT(vaLb3 [ 0 ] == false);
		CPPUNIT_ASSERT(vaLb3 [ 1 ] == true);
		CPPUNIT_ASSERT(vaLb3 [ 2 ] == false);
		}
	}
void ValarrayTest::valarray_cov12()
	{
		{
		valarray<int> vaL ( 4 ), vaL1(4),vaR ( 4 ) , vaR1 ( 4 );

		vaL[0] = 0;vaL1[0] = 7;
		vaL[1] = 1;vaL1[1] = 6;
		vaL[2] = 2;vaL1[2] = 5;
		vaL[3] = 3;vaL1[3] = 4;
		
		vaR = vaL.shift(3);
		CPPUNIT_ASSERT(vaR [ 0 ] == 3);
		CPPUNIT_ASSERT(vaR [ 1 ] == 0);
		CPPUNIT_ASSERT(vaR [ 2 ] == 0);
		CPPUNIT_ASSERT(vaR [ 3 ] == 0);
		vaR1 = vaL1.shift(-4);
		CPPUNIT_ASSERT(vaR1 [ 0 ] == 0);
		CPPUNIT_ASSERT(vaR1 [ 1 ] == 0);
		CPPUNIT_ASSERT(vaR1 [ 2 ] == 0);
		CPPUNIT_ASSERT(vaR1 [ 3 ] == 0);
		}
	}
