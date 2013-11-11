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

#include <algorithm>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <e32std.h>

#include "cppunit/cppunit_proxy.h"

#if !defined (STLPORT) || defined(_STLP_USE_NAMESPACES)
using namespace std;
#endif

//
// TestCase class
//
class StackTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(StackTest);
  CPPUNIT_TEST(stack1);
  CPPUNIT_TEST(stack2);
  CPPUNIT_TEST(stack_cov);
  CPPUNIT_TEST(stack_excep);
  CPPUNIT_TEST_SUITE_END();

protected:
  void stack1();
  void stack2();
  void stack_cov();
  void stack_excep();
};

CPPUNIT_TEST_SUITE_REGISTRATION(StackTest);

//
// tests implementation
//
void StackTest::stack1()
{
  stack<int, deque<int> > s;
  s.push(42);
  s.push(101);
  s.push(69);
  CPPUNIT_ASSERT(s.top()==69);
  s.pop();
  CPPUNIT_ASSERT(s.top()==101);
  s.pop();
  CPPUNIT_ASSERT(s.top()==42);
  s.pop();
  CPPUNIT_ASSERT(s.empty());
}
void StackTest::stack2()
{
  stack<int, list<int> > s;
  s.push(42);
  s.push(101);
  s.push(69);
  CPPUNIT_ASSERT(s.top()==69);
  s.pop();
  CPPUNIT_ASSERT(s.top()==101);
  s.pop();
  CPPUNIT_ASSERT(s.top()==42);
  s.pop();
  CPPUNIT_ASSERT(s.empty());
}
void StackTest::stack_cov()
	{
	  __UHEAP_MARK;
		{
		typedef stack<char> Mystack; 
		Mystack c1,c2; 
	    
	    c1.push('a'); 
	    c1.push('b'); 
	    c1.push('c'); 
	    
	    c2.push('a'); 
	    c2.push('b'); 
	    c2.push('d'); 
	    
	    CPPUNIT_ASSERT( (c1<c2) == true);
	    CPPUNIT_ASSERT( (c1<=c2) == true);
	    CPPUNIT_ASSERT( (c1==c2) == false);
	    CPPUNIT_ASSERT( (c1>c2) == false);
	    CPPUNIT_ASSERT( (c1>=c2) == false);
	    CPPUNIT_ASSERT( (c1!=c2) == true);
	    
	    CPPUNIT_ASSERT( c1.size() == 3);
	    c1.pop();
	    CPPUNIT_ASSERT( c1.size() == 2);

		}
		__UHEAP_MARKEND;
	}
void StackTest::stack_excep()
	{
	__UHEAP_MARK;
	typedef stack<char> Mystack;
	Mystack c1;
	c1.push('a'); 
    c1.push('b'); 
    c1.push('c'); 
    
    c1.pop();
    c1.pop();
    c1.pop();
    
    CPPUNIT_ASSERT( c1.size() == 0);
#if !defined (STLPORT) || defined (_STLP_USE_EXCEPTIONS)
    try
    	{
    	c1.pop();
    	}
    catch(...)
    	{
    	CPPUNIT_ASSERT( 0 );
    	}
    CPPUNIT_ASSERT( 1 );
#endif
	__UHEAP_MARKEND;
	}
