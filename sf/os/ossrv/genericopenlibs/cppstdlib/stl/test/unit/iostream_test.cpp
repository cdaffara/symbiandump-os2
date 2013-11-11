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

#include <string>
#include <e32std.h>

#if !defined (STLPORT) || !defined (_STLP_USE_NO_IOSTREAMS)
#  include <sstream>
#  include <fstream>
#  include <iostream>
#  include <cassert>
#  include "cppunit/cppunit_proxy.h"

#  if !defined (STLPORT) || defined(_STLP_USE_NAMESPACES)
using namespace std;
#  endif

//
// TestCase class
//
class IOStreamTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(IOStreamTest);
  CPPUNIT_TEST(manipulators);
  CPPUNIT_TEST(stringstream_cov);
//  CPPUNIT_TEST(iostream_cov1);
  CPPUNIT_TEST(iostream_cov2);
  CPPUNIT_TEST_SUITE_END();

private:
  void manipulators();
  void stringstream_cov();
//  void iostream_cov1();
  void iostream_cov2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(IOStreamTest);
void testfn (ios_base::event ev, ios_base& /*iosobj*/, int /*index*/)
	{
	static int i =0;
	  switch (ev)
	  {
	    case ios_base::copyfmt_event:
	    	assert(0);
	    case ios_base::imbue_event:
    	assert(i==0);
	    i++;
	    break;
	    case ios_base::erase_event:
    	assert(i==1);
	    break;
	  }
	}

//
// tests implementation
//
void IOStreamTest::manipulators()
{
  {
    istringstream istr;
    istr.str("bar");

    istr >> ws;
    CPPUNIT_ASSERT( istr.good() );

    string foo;
    istr >> foo;
    CPPUNIT_ASSERT( istr.eof() );
    CPPUNIT_ASSERT( !istr.fail() );
    CPPUNIT_ASSERT( foo == "bar" );

    istr >> ws;
    CPPUNIT_ASSERT( istr.eof() );
    CPPUNIT_ASSERT( !istr.fail() );
    istr.clear();
  }

  {
    istringstream istr;
    istr.str("  bar  ");

    istr >> ws;
    CPPUNIT_ASSERT( istr.good() );

    string foo;
    istr >> foo;
    CPPUNIT_ASSERT( !istr.eof() );
    CPPUNIT_ASSERT( !istr.fail() );
    CPPUNIT_ASSERT( foo == "bar" );

    istr >> ws;
    CPPUNIT_ASSERT( istr.eof() );
    CPPUNIT_ASSERT( !istr.fail() );
    istr.clear();
  }
}

void IOStreamTest::stringstream_cov()
	{
	__UHEAP_MARK;
	basic_string<char> i( "stdcpp" );
	stringstream ss1,ss2,ss3;
	ss1.rdbuf( )->str( i );
	string foo1,foo2,foo3;
    ss1 >> foo1;
    CPPUNIT_ASSERT(foo1 == "stdcpp");
    ss2 << "pips";
    ss2 >> foo2;
    CPPUNIT_ASSERT(foo2 == "pips");
    ss3.rdbuf( )->str( "stdcppwithpips" );
    ss3 >> foo3;
    CPPUNIT_ASSERT(foo3 == "stdcppwithpips");
    __UHEAP_MARKEND;
	}
#if 0 
void IOStreamTest::iostream_cov1()
	{
//	__UHEAP_MARK;
		{
		static const int i = ios_base::xalloc();
		cout.iword( i ) = 11;
		cin.iword( i ) = 13;
		CPPUNIT_ASSERT(cout.iword( i ) == 11);
		CPPUNIT_ASSERT(cin.iword( i ) == 13);
		}
	//__UHEAP_MARKEND;
	}
#endif
void IOStreamTest::iostream_cov2()
	{
	//__UHEAP_MARK;
	ofstream filestr;
    filestr.register_callback (testfn,0);
	bool x = filestr.sync_with_stdio();
	CPPUNIT_ASSERT(x);
	filestr.imbue (cout.getloc());
	  
    //__UHEAP_MARKEND;
	}
#endif
