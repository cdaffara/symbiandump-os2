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

#include <e32std.h>
#include <bitset>
#include <algorithm>
#include <sstream>
#include <iostream>
#include "cppunit/cppunit_proxy.h"

#if !defined (STLPORT) || defined(_STLP_USE_NAMESPACES)
using namespace std;
#endif

//
// TestCase class
//
class BitsetTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(BitsetTest);
  CPPUNIT_TEST(bitset1);
  CPPUNIT_TEST(iostream);
  CPPUNIT_TEST(bitset_cov1);
  CPPUNIT_TEST(bitset_cov2);
  CPPUNIT_TEST(bitset_cov3);
  CPPUNIT_TEST_SUITE_END();

protected:
  void bitset1();
  void iostream();
  void bitset_cov1();
  void bitset_cov2();
  void bitset_cov3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(BitsetTest);

//
// tests implementation
//
void BitsetTest::bitset1()
{
  bitset<13U> b1(0xFFFF);
  bitset<13U> b2(0x1111);
  CPPUNIT_ASSERT(b1.size()==13);
  CPPUNIT_ASSERT(b1==0x1FFF);
  CPPUNIT_ASSERT(b2.size()==13);
  CPPUNIT_ASSERT(b2==0x1111);

  b1 = b1^(b2<<2);
  CPPUNIT_ASSERT(b1==0x1BBB);

  CPPUNIT_ASSERT(b1.count()==10);
  CPPUNIT_ASSERT(b2.count()==4);

#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  size_t __pos = b2._Find_first();
  CPPUNIT_ASSERT( __pos == 0 );
  __pos = b2._Find_next(__pos);
  CPPUNIT_ASSERT( __pos == 4 );
  __pos = b2._Find_next(__pos);
  CPPUNIT_ASSERT( __pos == 8 );
  __pos = b2._Find_next(__pos);
  CPPUNIT_ASSERT( __pos == 12 );
  __pos = b2._Find_next(__pos);
  CPPUNIT_ASSERT( __pos == 13 );
#endif

#if !defined (STLPORT) || !defined (_STLP_NO_MEMBER_TEMPLATES) && !defined (_STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS)
  string representation = b2.to_string<char, char_traits<char>, allocator<char> >();
  CPPUNIT_ASSERT( representation == "1000100010001" );
#  if !defined (STLPORT) || !defined (_STLP_NO_WCHAR_T)
  wstring wrepresentation = b2.to_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >();
  CPPUNIT_ASSERT( wrepresentation == L"1000100010001" );
#  endif
#else
  CPPUNIT_ASSERT( b2.to_string() == "1000100010001" );
#endif
}

void BitsetTest::iostream()
{
  {
    stringstream sstr;
    bitset<13U> b(0x1111);
    sstr << b;
    CPPUNIT_ASSERT( sstr.str() == "1000100010001" );

    bitset<13U> b1;
    sstr >> b1;
    CPPUNIT_ASSERT( b1.test(0) );
    CPPUNIT_ASSERT( b1.test(4) );
    CPPUNIT_ASSERT( b1.test(8) );
    CPPUNIT_ASSERT( b1.test(12) );
  }
#if !defined (STLPORT) || !defined (_STLP_NO_WCHAR_T)
  {
    wstringstream sstr;
    bitset<13U> b(0x1111);
    sstr << b;
    CPPUNIT_ASSERT( sstr.str() == L"1000100010001" );

    bitset<13U> b1;
    sstr >> b1;
    CPPUNIT_ASSERT( b1.test(0) );
    CPPUNIT_ASSERT( b1.test(4) );
    CPPUNIT_ASSERT( b1.test(8) );
    CPPUNIT_ASSERT( b1.test(12) );
  }
#endif
}
void BitsetTest::bitset_cov1()
	{
	__UHEAP_MARK;
		{
		bitset<5> b1 ( 6 );
	    bitset<5> rb1;
	    
		CPPUNIT_ASSERT( b1.any() );
		CPPUNIT_ASSERT( !b1.none() );
		rb1 = b1.reset ( );
	    CPPUNIT_ASSERT( !rb1.any() );
	    CPPUNIT_ASSERT( rb1.none() );
		}
		{
		bitset<5> b1 ( 7 );
		unsigned long int i;
		i = b1.to_ulong( );
		CPPUNIT_ASSERT( i == 7 );
		}
		{
		bitset<5> b1 ( 6 );
		bitset<5> bs1;
		bitset<5> b1s0;
		unsigned long int i;
		b1s0 = b1.set( 0 );
		i = b1s0.to_ulong( );
		CPPUNIT_ASSERT( i == 7 );

		bs1 = b1.set( );
		i = bs1.to_ulong( );
		CPPUNIT_ASSERT( i == 31 );
		}
		{
		bitset<5> b1 ( 6 );
		bitset<5> fb1;
		bitset<5> f3b1;
		unsigned long int i;
		
		fb1 = b1.flip ( );
		i = fb1.to_ulong( );
		CPPUNIT_ASSERT( i == 25 );	
		
		f3b1 = b1.flip ( 3 );
		i = f3b1.to_ulong( );
		CPPUNIT_ASSERT( i == 17 );	
		}
		{
		bitset<5> b1 ( 2 );
 	    bitset<5> b2 ( 6 );
 	    
 	    //x [i] = bool
 	    b1[ 0 ] = true;
 	    CPPUNIT_ASSERT( b1.to_ulong( ) == 3 );
 	    
 	    //x [i] = y [j]
 	    b2 [4] = b1 [0];
 	    CPPUNIT_ASSERT( b2.to_ulong( ) == 22 );
 	    
        //b = ~x [i]
 	    bool b = ~b2 [4];
 	    CPPUNIT_ASSERT( b == false );

 	    //b = x [i]
 	    b = b2 [4];
	    CPPUNIT_ASSERT( b == true );

	    //x [i] . flip ( )
	    b2 [4].flip( );
	    CPPUNIT_ASSERT( b2.to_ulong( ) == 6 );
		}
		__UHEAP_MARKEND;
	}
void BitsetTest::bitset_cov2()
	{
	__UHEAP_MARK;
		{
		bitset<5> b1 ( 7 );
		bitset<5> b2;
		unsigned long int i;
		b2 = ~b1;
		i = b2.to_ulong( );
		CPPUNIT_ASSERT( i == 24 );
		}
		{
		bitset<5> b1 ( 7 );
		bitset<5> b2 ( 11 );
		unsigned long int i;
		b1 |= b2;
		i = b1.to_ulong( );
		CPPUNIT_ASSERT( i == 15 );
		i = b2.to_ulong( );
		CPPUNIT_ASSERT( i == 11 );
		}
		{
		bool b;
		bitset<2> b1 ( 2 );
		bitset<2> b2;
		b = b1[ 0 ];
		CPPUNIT_ASSERT( b == 0 );
		b = b1[ 1 ];
		CPPUNIT_ASSERT( b == 1 );
		b2[0] = b1[0];
	    b2[1] = b1[1];
	    CPPUNIT_ASSERT(b1 == b2);
		}
		{
		bitset<5> b1 ( 28 );
		b1 >>= 2;
		CPPUNIT_ASSERT( b1.to_ulong( ) == 7 );
		}
		{
		bitset<5> b1 ( 28 );
		bitset<5> b3 = (b1 >> 1);
		CPPUNIT_ASSERT( b3.to_ulong( ) == 14 );
		}
		{
		bitset<5> b1 ( 7 );
		bitset<5> b2 ( 11 );
		b1 &= b2;
		CPPUNIT_ASSERT( b1.to_ulong( ) == 3 );
		CPPUNIT_ASSERT( b2.to_ulong( ) == 11 );
		}
		{
		bitset<5> b1 ( 7 );
		bitset<5> b2 ( 2 );
		CPPUNIT_ASSERT( b1 != b2 );
		}
		__UHEAP_MARKEND;
	}
void BitsetTest::bitset_cov3()
	{
	__UHEAP_MARK;
		{
		bitset<5> b1 ( 13 );
		bitset<5> b1r3;
		b1r3 = b1.reset( 2 );
		CPPUNIT_ASSERT( b1r3.to_ulong( ) == 9 );
		}
		{
		string bitval1 ( "00011" );
		bitset<5> b1 ( bitval1 );
		CPPUNIT_ASSERT( b1.to_ulong( ) == 3 );
		string bitval2 ("11110011011");
		bitset<6> b2 ( bitval2, 3, 6 );
		CPPUNIT_ASSERT( b2.to_ulong( ) == 38 );
		bitset<9> b3 ( bitval2, 2 );
		CPPUNIT_ASSERT( b3.to_ulong( ) == 411 );
		}
		{
		bitset<5> b1 ( 7 );
		bitset<5> b2 ( 11 );
		unsigned long int i;
		b1 = b1 | b2;
		i = b1.to_ulong( );
		CPPUNIT_ASSERT( i == 15 );
		i = b2.to_ulong( );
		CPPUNIT_ASSERT( i == 11 );
		}
		{
		bitset<5> b1 ( 7 );
		bitset<5> b2 ( 11 );
		b1 = b1 & b2;
		CPPUNIT_ASSERT( b1.to_ulong( ) == 3 );
		CPPUNIT_ASSERT( b2.to_ulong( ) == 11 );
		}	
		__UHEAP_MARKEND;
	}
