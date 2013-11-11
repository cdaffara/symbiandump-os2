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

#include <utility>
#include <vector>
#include <algorithm>

#include "cppunit/cppunit_proxy.h"

#if !defined (STLPORT) || defined(_STLP_USE_NAMESPACES)
using namespace std;
#endif

//
// TestCase class
//
class PairTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(PairTest);
  CPPUNIT_TEST(pair0);
  CPPUNIT_TEST(pair_cov);
  CPPUNIT_TEST_SUITE_END();

protected:
  void pair0();
  void pair_cov();
};

CPPUNIT_TEST_SUITE_REGISTRATION(PairTest);

//
// tests implementation
//
void PairTest::pair0()
{
  pair<int, int> p = make_pair(1, 10);

  CPPUNIT_ASSERT(p.first==1);
  CPPUNIT_ASSERT(p.second==10);
}
void PairTest::pair_cov()
	{
	pair<int, int> p1 = make_pair(1, 10);
	pair<int, int> p2 = make_pair(2, 10);
	bool val;
	
	val = (p1 >= p2);
	CPPUNIT_ASSERT(val == false);
	val = (p1 > p2);
	CPPUNIT_ASSERT(val == false);
	val = (p1 <= p2);
	CPPUNIT_ASSERT(val == true);
	val = (p1 < p2);
	CPPUNIT_ASSERT(val == true);
	val = (p1 != p2);
	CPPUNIT_ASSERT(val == true);
	val = (p1 == p2);
	CPPUNIT_ASSERT(val == false);
	}
