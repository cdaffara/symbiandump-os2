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

#include <vector>
#include <algorithm>
#include <list>
#include <deque>
#include <queue>
#include <e32std.h>

#include "cppunit/cppunit_proxy.h"

#if !defined (STLPORT) || defined(_STLP_USE_NAMESPACES)
using namespace std;
#endif

//
// TestCase class
//
class QueueTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(QueueTest);
  CPPUNIT_TEST(pqueue1);
  CPPUNIT_TEST(queue1);
  CPPUNIT_TEST(queue_cov);
  CPPUNIT_TEST(pqueue_cov1);
  CPPUNIT_TEST(pqueue_cov2);
  CPPUNIT_TEST_SUITE_END();

protected:
  void pqueue1();
  void queue1();
  void queue_cov();
  void pqueue_cov1();
  void pqueue_cov2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(QueueTest);

//
// tests implementation
//
void QueueTest::pqueue1()
{
  priority_queue<int, deque<int>, less<int> > q;
  q.push(42);
  q.push(101);
  q.push(69);

  CPPUNIT_ASSERT( q.top()==101 );
  q.pop();
  CPPUNIT_ASSERT( q.top()==69 );
  q.pop();
  CPPUNIT_ASSERT( q.top()==42 );
  q.pop();

  CPPUNIT_ASSERT(q.empty());
}
void QueueTest::queue1()
{
  queue<int, list<int> > q;
  q.push(42);
  q.push(101);
  q.push(69);

  CPPUNIT_ASSERT( q.front()==42 );
  q.pop();
  CPPUNIT_ASSERT( q.front()==101 );
  q.pop();
  CPPUNIT_ASSERT( q.front()==69 );
  q.pop();

  CPPUNIT_ASSERT(q.empty());
}
void QueueTest::queue_cov()
	{
	__UHEAP_MARK;
		{
		queue<int, list<int> > q;
		q.push(42);
		q.push(101);
		q.push(69);

		CPPUNIT_ASSERT( q.size()==3 );
		q.pop();
		CPPUNIT_ASSERT( q.size()==2 );
		q.pop();
		CPPUNIT_ASSERT( q.size()==1 );
		q.pop();

		CPPUNIT_ASSERT(q.empty());
		}
		{
		queue<int, list<int> > q;
		q.push(42);
		q.push(101);
		q.push(69);
		CPPUNIT_ASSERT(q.back() == 69);
		}
		{
		queue<int> myqueue;
		queue<int> const& cv = myqueue;
		myqueue.push(77);
		myqueue.push(16);	
		CPPUNIT_ASSERT(cv.front() == 77);
		CPPUNIT_ASSERT(cv.back() == 16);
		}
		 __UHEAP_MARKEND;
	 }
void QueueTest::pqueue_cov1()
	{
	__UHEAP_MARK;
		{
		priority_queue<int> pq;
		pq.push(42);
		pq.push(101);
		pq.push(69);

		CPPUNIT_ASSERT( pq.size()==3 );
		pq.pop();
		CPPUNIT_ASSERT( pq.size()==2 );
		pq.pop();
		CPPUNIT_ASSERT( pq.size()==1 );
		pq.pop();
		CPPUNIT_ASSERT(pq.empty());
		}
		{
		queue<int> myqueue;
		queue<int> myqueue1;
		bool flag;
		
		myqueue.push(77);
		myqueue.push(16);	
		myqueue1.push(77);
		myqueue1.push(17);	
		flag = myqueue < myqueue1;
		CPPUNIT_ASSERT(flag == true);
		}	
		{
		queue<int> myqueue;
		queue<int> myqueue1;
		bool flag;
		
		myqueue.push(77);
		myqueue.push(16);	
		myqueue1.push(77);
		myqueue1.push(16);	
		flag = (myqueue == myqueue1);
		CPPUNIT_ASSERT(flag == true);
		}	
		 __UHEAP_MARKEND;
	}
void QueueTest::pqueue_cov2()
	{
	__UHEAP_MARK;
	int myints[]= {10,60,50,20};
	priority_queue<int> first;
	priority_queue<int> second (myints,myints+3);
	priority_queue< int, vector<int>, greater<int> > third (myints,myints+3);
	CPPUNIT_ASSERT(first.empty() == true);
	CPPUNIT_ASSERT(second.size() == 3);
	CPPUNIT_ASSERT(third.top() == 10);
	third.pop();
	CPPUNIT_ASSERT(third.top() == 50);
	third.pop();
	CPPUNIT_ASSERT(third.top() == 60);
	 __UHEAP_MARKEND;
	}
