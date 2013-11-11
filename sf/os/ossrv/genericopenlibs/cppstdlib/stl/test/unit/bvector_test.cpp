/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
 */

#include <vector>
#include <e32std.h>
#include "cppunit/cppunit_proxy.h"

#if defined(_STLP_USE_NAMESPACES)
using namespace std;
#endif

//
// TestCase class
//
class BvectorTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(BvectorTest);
#if !defined (STLPORT) || defined (_STLP_NO_EXTENSIONS)
  CPPUNIT_IGNORE;
#endif
  CPPUNIT_TEST(bvec1);
  CPPUNIT_TEST(bvec_cov);
  CPPUNIT_TEST(bvec_cov1);
  CPPUNIT_TEST_SUITE_END();

protected:
  void bvec1();
  void bvec_cov();
  void bvec_cov1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(BvectorTest);

//
// tests implementation
//
void BvectorTest::bvec1()
{
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  bool ii[3]= {1,0,1};
  bit_vector b(3);

  CPPUNIT_ASSERT(b[0]==0);
  CPPUNIT_ASSERT(b[1]==0);
  CPPUNIT_ASSERT(b[2]==0);

  b[0] = b[2] = 1;

  CPPUNIT_ASSERT(b[0]==1);
  CPPUNIT_ASSERT(b[1]==0);
  CPPUNIT_ASSERT(b[2]==1);

  b.insert(b.begin(),(bool*)ii, ii+2);

  CPPUNIT_ASSERT(b[0]==1);
  CPPUNIT_ASSERT(b[1]==0);
  CPPUNIT_ASSERT(b[2]==1);
  CPPUNIT_ASSERT(b[3]==0);
  CPPUNIT_ASSERT(b[4]==1);

  bit_vector bb = b;
  if (bb != b)
    exit(1);

  b[0] |= 0;
  b[1] |= 0;
  b[2] |= 1;
  b[3] |= 1;
  CPPUNIT_ASSERT(!((b[0] != 1) || (b[1] != 0) || (b[2] != 1) || (b[3] != 1)));


  bb[0] &= 0;
  bb[1] &= 0;
  bb[2] &= 1;
  bb[3] &= 1;
  CPPUNIT_ASSERT(!((bb[0] != 0) || (bb[1] != 0) || (bb[2] != 1) || (bb[3] != 0)));
#endif
}
void BvectorTest::bvec_cov()
	{
//	__UHEAP_MARK;
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
		{
		bit_vector b1(3),b2(2);
		b1[0] = b1[2] = 1;
		b2[1] = 1;
	
		b1.swap(b2);
		CPPUNIT_ASSERT(b1.size() == 2);
		CPPUNIT_ASSERT(b2.size() == 3);
		
		CPPUNIT_ASSERT(*b1.rbegin() == 1);
		CPPUNIT_ASSERT(*b1.rend() == 0);
		
		b2.pop_back();
		CPPUNIT_ASSERT(b2.size() == 2);
		CPPUNIT_ASSERT(*b2.rbegin() == 0);
		bool val = b2[0];
		CPPUNIT_ASSERT(val == true);
		}
		{
		bit_vector b1(3),b2(2);
		b1[0] = b1[2] = 1;
		b2[1] = 1;
		
		b1.max_size();
		CPPUNIT_ASSERT(b1.front() == 1);
		CPPUNIT_ASSERT(b1.back() == 1);
		b1.erase(b1.begin());
		CPPUNIT_ASSERT(b1.size() == 2);
		b1.erase(b1.begin(),b1.end());
		CPPUNIT_ASSERT(b1.size() == 0);
		CPPUNIT_ASSERT(b1.empty() == true);
		const bool &res1 = b2.at(0);
		CPPUNIT_ASSERT(res1 == false);
		CPPUNIT_ASSERT(b2.at(1) == true);
		b2.clear();
		CPPUNIT_ASSERT(b2.empty() == true);
		}
#endif	
//	__UHEAP_MARKEND;
	}
void BvectorTest::bvec_cov1()
	{
//	__UHEAP_MARK;
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
		{
		bit_vector b1(5),b2(2);
		b1[0] = b1[2] = 1;
		b2[1] = 1;
		
		b1.assign(b1.begin()+1,b1.begin()+3);
		CPPUNIT_ASSERT(b1.size() == 2);
		b1.assign(7,1);
		CPPUNIT_ASSERT(b1.size() == 7);
		b2.resize(3,1);
		CPPUNIT_ASSERT(b2.size() == 3);
		}
		{
		bit_vector b1(2),b2(2);
		b1[1] = 1;	
		b2[0] = 1;
		b1.flip();
		CPPUNIT_ASSERT(b1.front() == 1);
		CPPUNIT_ASSERT(b1.back() == 0);
		CPPUNIT_ASSERT(b1 == b2);
		CPPUNIT_ASSERT(!(b1 < b2));
		
		b2.reserve(10);
		CPPUNIT_ASSERT(b2.capacity() >= 10);
		}
		{
		bit_vector b(2);
		b[1] = 1;
		b.insert(b.begin(),1);	
		CPPUNIT_ASSERT(b.size() == 3);
		CPPUNIT_ASSERT(b.front() == 1);
		}
#endif	
//	__UHEAP_MARKEND;
	}		
