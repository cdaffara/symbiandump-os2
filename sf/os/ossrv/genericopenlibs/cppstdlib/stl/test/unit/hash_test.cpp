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

//Has to be first for StackAllocator swap overload to be taken
//into account (at least using GCC 4.0.1)
#include "stack_allocator.h"

#include <vector>
#include <algorithm>
#include <e32std.h>

#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
#  include <hash_map>
#  include <hash_set>
#    include <rope>
#  endif

#include <string>

#include "cppunit/cppunit_proxy.h"

#if defined (__MVS__)
const char star = 92;
#else
const char star = 42;
#endif

#if !defined (STLPORT) || defined (_STLP_USE_NAMESPACES)
using namespace std;
#endif

//
// TestCase class
//
class HashTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(HashTest);
#if !defined (STLPORT) || defined (_STLP_NO_EXTENSIONS)
  CPPUNIT_IGNORE;
#endif
  CPPUNIT_TEST(hmap1);
  CPPUNIT_TEST(hmmap1);
  CPPUNIT_TEST(hmset1);
  CPPUNIT_TEST(hset2);
  CPPUNIT_TEST(insert_erase);
  CPPUNIT_TEST(allocator_with_state);
  CPPUNIT_TEST(hash_map_cov1);
  CPPUNIT_TEST(hash_map_cov2);
  CPPUNIT_TEST(hash_map_cov3);
  CPPUNIT_TEST(hash_map_cov4);
  CPPUNIT_TEST(hash_map_cov5);
  CPPUNIT_TEST(hash_map_cov6);
  CPPUNIT_TEST(hash_multimap_cov1);
  CPPUNIT_TEST(hash_multimap_cov2);
  CPPUNIT_TEST(hash_multimap_cov3);
  CPPUNIT_TEST(hash_set_cov1);
  CPPUNIT_TEST(hash_set_cov2);
  CPPUNIT_TEST(hash_set_cov3);
  CPPUNIT_TEST(hash_set_cov4);
  CPPUNIT_TEST(hash_set_cov5);
  CPPUNIT_TEST(hash_set_cov6);
  CPPUNIT_TEST(hash_multiset_cov1);
  CPPUNIT_TEST(hash_multiset_cov2);
  CPPUNIT_TEST(hash_multiset_cov3);
  //CPPUNIT_TEST(equality);
  CPPUNIT_TEST_SUITE_END();

#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  typedef hash_multiset<char, hash<char>, equal_to<char> > hmset;
#endif

protected:
  void hmap1();
  void hmmap1();
  void hmset1();
  void hset2();
  void insert_erase();
  void hash_map_cov1();
  void hash_map_cov2();
  void hash_map_cov3();
  void hash_map_cov4();
  void hash_map_cov5();
  void hash_map_cov6();
  void hash_multimap_cov1();
  void hash_multimap_cov2();
  void hash_multimap_cov3();
  void hash_set_cov1();
  void hash_set_cov2();
  void hash_set_cov3();
  void hash_set_cov4();
  void hash_set_cov5();
  void hash_set_cov6();
  void hash_multiset_cov1();
  void hash_multiset_cov2();
  void hash_multiset_cov3();
  //void equality();
  void allocator_with_state();
};

CPPUNIT_TEST_SUITE_REGISTRATION(HashTest);

//
// tests implementation
//
void HashTest::hmap1()
{
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  typedef hash_map<char, crope, hash<char>, equal_to<char> > maptype;
  maptype m;
  // Store mappings between roman numerals and decimals.
  m['l'] = "50";
  m['x'] = "20"; // Deliberate mistake.
  m['v'] = "5";
  m['i'] = "1";
  CPPUNIT_ASSERT( !strcmp(m['x'].c_str(),"20") );
  m['x'] = "10"; // Correct mistake.
  CPPUNIT_ASSERT( !strcmp(m['x'].c_str(),"10") );

  CPPUNIT_ASSERT( !strcmp(m['z'].c_str(),"") );

  CPPUNIT_ASSERT( m.count('z')==1 );
  pair<maptype::iterator, bool> p = m.insert(pair<const char, crope>('c', crope("100")));

  CPPUNIT_ASSERT(p.second);

  p = m.insert(pair<const char, crope>('c', crope("100")));
  CPPUNIT_ASSERT(!p.second);

  //Some iterators compare check, really compile time checks
  maptype::iterator ite(m.begin());
  maptype::const_iterator cite(m.begin());
  cite = m.begin();
  maptype const& cm = m;
  cite = cm.begin();
  CPPUNIT_ASSERT( ite == cite );
  CPPUNIT_ASSERT( !(ite != cite) );
  CPPUNIT_ASSERT( cite == ite );
  CPPUNIT_ASSERT( !(cite != ite) );
#endif
}

void HashTest::hmmap1()
{
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  typedef hash_multimap<char, int, hash<char>,equal_to<char> > mmap;
  mmap m;
  CPPUNIT_ASSERT(m.count('X')==0);
  m.insert(pair<const char,int>('X', 10)); // Standard way.
  CPPUNIT_ASSERT(m.count('X')==1);
//  m.insert('X', 20); // Non-standard, but very convenient!
  m.insert(pair<const char,int>('X', 20));  // jbuck: standard way
  CPPUNIT_ASSERT(m.count('X')==2);
//  m.insert('Y', 32);
  m.insert(pair<const char,int>('Y', 32));  // jbuck: standard way
  mmap::iterator i = m.find('X'); // Find first match.

  CPPUNIT_ASSERT((*i).first=='X');
  CPPUNIT_ASSERT((*i).second==10);
  i++;
  CPPUNIT_ASSERT((*i).first=='X');
  CPPUNIT_ASSERT((*i).second==20);
  i++;
  CPPUNIT_ASSERT((*i).first=='Y');
  CPPUNIT_ASSERT((*i).second==32);
  i++;
  CPPUNIT_ASSERT(i==m.end());

  size_t count = m.erase('X');
  CPPUNIT_ASSERT(count==2);

  //Some iterators compare check, really compile time checks
  mmap::iterator ite(m.begin());
  mmap::const_iterator cite(m.begin());
  CPPUNIT_ASSERT( ite == cite );
  CPPUNIT_ASSERT( !(ite != cite) );
  CPPUNIT_ASSERT( cite == ite );
  CPPUNIT_ASSERT( !(cite != ite) );

  typedef hash_multimap<size_t, size_t> HMapType;
  HMapType hmap;

  //We fill the map to implicitely start a rehash.
  for (size_t counter = 0; counter < 3077; ++counter)
    hmap.insert(HMapType::value_type(1, counter));

  hmap.insert(HMapType::value_type(12325, 1));
  hmap.insert(HMapType::value_type(12325, 2));

  CPPUNIT_ASSERT( hmap.count(12325) == 2 );

  //At this point 23 goes to the same bucket as 12325, it used to reveal a bug.
  hmap.insert(HMapType::value_type(23, 0));

  CPPUNIT_ASSERT( hmap.count(12325) == 2 );
#endif
}

void HashTest::hmset1()
{
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  hmset s;
  CPPUNIT_ASSERT( s.count(star) == 0 );
  s.insert(star);
  CPPUNIT_ASSERT( s.count(star) == 1 );
  s.insert(star);
  CPPUNIT_ASSERT( s.count(star) == 2 );
  hmset::iterator i = s.find(char(40));
  CPPUNIT_ASSERT( i == s.end() );

  i = s.find(star);
  CPPUNIT_ASSERT( i != s.end() )
  CPPUNIT_ASSERT( *i == '*' );
  CPPUNIT_ASSERT( s.erase(star) == 2 );
#endif
}
void HashTest::hset2()
{
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  hash_set<int, hash<int>, equal_to<int> > s;
  pair<hash_set<int, hash<int>, equal_to<int> >::iterator, bool> p = s.insert(42);
  CPPUNIT_ASSERT( p.second );
  CPPUNIT_ASSERT( *(p.first) == 42 );

  p = s.insert(42);
  CPPUNIT_ASSERT( !p.second );
#endif
}

void HashTest::insert_erase()
{
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  typedef hash_map<string, size_t, hash<string>, equal_to<string> > hmap;
  typedef hmap::value_type val_type;
  {
    hmap values;
    CPPUNIT_ASSERT( values.insert(val_type("foo", 0)).second );
    CPPUNIT_ASSERT( values.insert(val_type("bar", 0)).second );
    CPPUNIT_ASSERT( values.insert(val_type("abc", 0)).second );

    CPPUNIT_ASSERT( values.erase("foo") == 1 );
    CPPUNIT_ASSERT( values.erase("bar") == 1 );
    CPPUNIT_ASSERT( values.erase("abc") == 1 );
  }

  {
    hmap values;
    CPPUNIT_ASSERT( values.insert(val_type("foo", 0)).second );
    CPPUNIT_ASSERT( values.insert(val_type("bar", 0)).second );
    CPPUNIT_ASSERT( values.insert(val_type("abc", 0)).second );

    CPPUNIT_ASSERT( values.erase("abc") == 1 );
    CPPUNIT_ASSERT( values.erase("bar") == 1 );
    CPPUNIT_ASSERT( values.erase("foo") == 1 );
  }
#endif
}

/*
 * Here is the test showing why equality operator on hash containers
 * has no meaning:

struct equality_hash_func {
  size_t operator () (size_t val) const {
    return val % 10;
  }
};

void HashTest::equality()
{
  hash_set<size_t, equality_hash_func, equal_to<size_t> > s1, s2;

  s1.insert(10);
  s1.insert(20);

  s2.insert(20);
  s2.insert(10);

  //s1 and s2 contains both 10 and 20:
  CPPUNIT_ASSERT( s1 == s2 );
}
*/

void HashTest::allocator_with_state()
{
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  char buf1[2048];
  StackAllocator<int> stack1(buf1, buf1 + sizeof(buf1));

  char buf2[2048];
  StackAllocator<int> stack2(buf2, buf2 + sizeof(buf2));

  {
    typedef hash_set<int, hash<int>, equal_to<int>, StackAllocator<int> > HashSetInt;
    HashSetInt hint1(10, hash<int>(), equal_to<int>(), stack1);

    int i;
    for (i = 0; i < 5; ++i)
      hint1.insert(i);
    HashSetInt hint1Cpy(hint1);

    HashSetInt hint2(10, hash<int>(), equal_to<int>(), stack2);
    for (; i < 10; ++i)
      hint2.insert(i);
    HashSetInt hint2Cpy(hint2);

    hint1.swap(hint2);

    CPPUNIT_ASSERT( hint1.get_allocator().swaped() );
    CPPUNIT_ASSERT( hint2.get_allocator().swaped() );

    CPPUNIT_ASSERT( hint1.get_allocator() == stack2 );
    CPPUNIT_ASSERT( hint2.get_allocator() == stack1 );
  }
  CPPUNIT_ASSERT( stack1.ok() );
  CPPUNIT_ASSERT( stack2.ok() );
#endif
}
void HashTest::hash_map_cov1()
	{
	__UHEAP_MARK;
		{
		hash_map <int, int> hm1, hm2;
		hash_map <int, int>::iterator hm1_Iter;
		hash_map <int, int>::const_iterator hm1_cIter;
		typedef pair <int, int> Int_Pair;

		hm1.insert ( Int_Pair ( 1, 10 ) );
		hm1.insert ( Int_Pair ( 2, 20 ) );
		hm1.insert ( Int_Pair ( 3, 30 ) );
		hm2.insert ( Int_Pair ( 30, 300 ) );

		hm1.swap( hm2 );
		hm1_Iter = hm1.begin( ); 
		CPPUNIT_ASSERT( hm1_Iter -> second == 300);
		hm1_Iter = hm1.end( ); 

		hm1_cIter = hm1.end( ); 
		hm1.clear();
		CPPUNIT_ASSERT( hm1.size() == 0 );
		CPPUNIT_ASSERT( hm1.empty() == true );
		}
		{
		hash_map <int, int> hm1, hm2;
		hash_map <int, int>::iterator hm1_Iter;
		hash_map <int, int>::const_iterator hm1_cIter;
		typedef pair <int, int> Int_Pair;

		hm1.insert ( Int_Pair ( 1, 10 ) );
		hm1.insert ( Int_Pair ( 2, 20 ) );
		hm1.insert ( Int_Pair ( 3, 30 ) );
		hm2.insert ( Int_Pair ( 30, 300 ) );

		swap( hm1,hm2 );
		hm1_Iter = hm1.begin( ); 
		CPPUNIT_ASSERT( hm1_Iter -> second == 300);
		hm1_Iter = hm1.end( ); 

		hm1_cIter = hm1.end( ); 
		hm1.clear();
		CPPUNIT_ASSERT( hm1.size() == 0 );
		CPPUNIT_ASSERT( hm1.empty() == true );
		}
		  __UHEAP_MARKEND;
	}
void HashTest::hash_map_cov2()
	{
	__UHEAP_MARK;
		{
		hash_map <int, int> hm1;
		int i,bcount;
		typedef pair <int, int> Int_Pair;

		hm1.insert ( Int_Pair ( 1, 1 ) );
		i = hm1.size( );
		CPPUNIT_ASSERT( i == 1);
		
		i = hm1.max_size(); // for covering the api
		
		hm1.insert ( Int_Pair ( 2, 4 ) );
		i = hm1.size( );
		CPPUNIT_ASSERT( i == 2);
		hm1.resize(10);
		bcount = hm1.bucket_count();
		CPPUNIT_ASSERT( bcount >= 10);
		hm1.elems_in_bucket(1);
		}
		{
		typedef hash_multimap<int, int> mmap;
		mmap m;

		typedef pair <int, int> Int_Pair;
		m.insert ( Int_Pair ( 1, 10 ) );
		m.insert ( Int_Pair ( 2, 20 ) );
		m.insert ( Int_Pair ( 3, 30 ) );
		
		std::pair<mmap::iterator, mmap::iterator> pair1 = m.equal_range(2); 
	    CPPUNIT_ASSERT( pair1.first->first == 2);
	    CPPUNIT_ASSERT( pair1.first->second  == 20);
	    std::pair<mmap::const_iterator, mmap::const_iterator> pair2 = m.equal_range(1); 
	    CPPUNIT_ASSERT( pair2.first->first == 1);
	    CPPUNIT_ASSERT( pair2.first->second  == 10);
		}
		{
		typedef hash_multimap<int, int> mmap;
		mmap m;

		typedef pair <int, int> Int_Pair;
		m.insert ( Int_Pair ( 1, 10 ) );
		m.insert ( Int_Pair ( 2, 20 ) );
		m.insert ( Int_Pair ( 3, 30 ) );
		
		m.erase(m.begin());
		mmap::iterator i1 = m.begin();
		CPPUNIT_ASSERT( i1 -> second == 20);
		CPPUNIT_ASSERT( m.size() == 2 );
		m.erase(m.begin(),m.end());
		CPPUNIT_ASSERT( m.size() == 0 );
		}
		  __UHEAP_MARKEND;
	}
void HashTest::hash_map_cov3()
	{
	__UHEAP_MARK;
		{
		typedef hash_multimap<int, int> mmap;
		mmap m;

		typedef pair <int, int> Int_Pair;
		m.insert ( Int_Pair ( 1, 10 ) );
		m.insert ( Int_Pair ( 2, 20 ) );
		m.insert ( Int_Pair ( 3, 30 ) );
		
		mmap::iterator i1 = m.find(1);
		CPPUNIT_ASSERT( i1 -> second == 10);
		mmap::const_iterator i2 = m.find(3);
		CPPUNIT_ASSERT( i2 -> second == 30);
		
		// negative test case where the element to find is not present in the 
		// hash , so it returns the successive element of the last element.
		mmap::iterator i3 = m.find(4);
		mmap::iterator i4 = m.end();
		CPPUNIT_ASSERT( i3 == i4);
		}
		{
		typedef hash_multimap<int, int> mmap;
		typedef allocator<std::pair<int, int> > Myalloc;
		mmap m;	

		mmap::allocator_type al = m.get_allocator(); 
	    CPPUNIT_ASSERT ((al == Myalloc()) == true);
	    
	    mmap::hasher hfn = m.hash_funct(); // returns the hasher function
		}
		{
		typedef hash_multimap<int, int> mmap;
		mmap m1,m2;

		typedef pair <int, int> Int_Pair;
		m1.insert ( Int_Pair ( 1, 10 ) );
		m1.insert ( Int_Pair ( 2, 20 ) );
		m1.insert ( Int_Pair ( 3, 30 ) );
		
		m2.insert ( m1.begin(),m1.end());
		mmap::iterator i1 = m2.begin();
		CPPUNIT_ASSERT( i1 -> second == 10);
		}
		  __UHEAP_MARKEND;
	}
void HashTest::hash_map_cov4()
	{
	__UHEAP_MARK;
		{
		typedef hash_map<int, int> mmap;
		mmap m;
		typedef pair <int, int> Int_Pair;
		m.insert ( Int_Pair ( 1, 10 ) );
		m.insert ( Int_Pair ( 2, 20 ) );
		m.insert ( Int_Pair ( 3, 30 ) );
		
		std::pair<mmap::iterator, mmap::iterator> pair1 = m.equal_range(2); 
	    CPPUNIT_ASSERT( pair1.first->first == 2);
	    CPPUNIT_ASSERT( pair1.first->second  == 20);
	    std::pair<mmap::const_iterator, mmap::const_iterator> pair2 = m.equal_range(1); 
	    CPPUNIT_ASSERT( pair2.first->first == 1);
	    CPPUNIT_ASSERT( pair2.first->second  == 10);
		}
		{
		typedef hash_map<int, int> mmap;
		mmap m;
		typedef pair <int, int> Int_Pair;
		m.insert ( Int_Pair ( 1, 10 ) );
		m.insert ( Int_Pair ( 2, 20 ) );
		m.insert ( Int_Pair ( 3, 30 ) );
		
		m.erase(m.begin());
		mmap::iterator i1 = m.begin();
		CPPUNIT_ASSERT( i1 -> second == 20);
		CPPUNIT_ASSERT( m.size() == 2 );
		m.erase(m.begin(),m.end());
		CPPUNIT_ASSERT( m.size() == 0 );
		}
		{
		typedef hash_map<int, int> mmap;
		mmap m;

		typedef pair <int, int> Int_Pair;
		m.insert ( Int_Pair ( 1, 10 ) );
		m.insert ( Int_Pair ( 2, 20 ) );
		m.insert ( Int_Pair ( 3, 30 ) );
			
		mmap::iterator i1 = m.find(1);
		CPPUNIT_ASSERT( i1 -> second == 10);
		mmap::const_iterator i2 = m.find(3);
		CPPUNIT_ASSERT( i2 -> second == 30);
		}
		  __UHEAP_MARKEND;
	}
void HashTest::hash_map_cov5()
	{
	__UHEAP_MARK;
		{
		typedef hash_map<int, int> mmap;
		typedef allocator<std::pair<int, int> > Myalloc;
		mmap m;	
		mmap::allocator_type al = m.get_allocator(); 
	    CPPUNIT_ASSERT ((al == Myalloc()) == true);
	    
	    mmap::hasher hfn = m.hash_funct(); // returns the hasher function
		}
		{
		typedef hash_map<int, int> mmap;
		mmap m1,m2;

		typedef pair <int, int> Int_Pair;
		m1.insert ( Int_Pair ( 1, 10 ) );
		m1.insert ( Int_Pair ( 2, 20 ) );
		m1.insert ( Int_Pair ( 3, 30 ) );
		
		m2.insert ( m1.begin(),m1.end());
		mmap::iterator i1 = m2.begin();
		CPPUNIT_ASSERT( i1 -> second == 10);
		}
		{
		typedef hash_map<char, int> mmap; 
		mmap c1; 
		mmap::key_equal cmpfn = c1.key_eq(); 
		CPPUNIT_ASSERT( cmpfn('a','a') == true);
		
		c1.max_bucket_count(); // for covering the api
		}
		{
		typedef hash_map<int, int> mmap;
		mmap m1;
		typedef pair <int, int> Int_Pair;
		m1.insert_noresize ( Int_Pair ( 1, 10 ) );
		m1.insert_noresize ( Int_Pair ( 2, 20 ) );
		m1.insert_noresize ( Int_Pair ( 3, 30 ) );
		
		mmap::iterator i1 = m1.begin();
		CPPUNIT_ASSERT( i1 -> second == 10);
		}
		  __UHEAP_MARKEND;
	}
void HashTest::hash_map_cov6()
	{
	__UHEAP_MARK;
			{
			typedef hash_map<int, int> mmap;
			mmap m1;
			int bcount;
			
			typedef pair <int, int> Int_Pair;
			m1.insert ( Int_Pair ( 1, 10 ) );
			m1.insert ( Int_Pair ( 2, 20 ) );
			m1.insert ( Int_Pair ( 3, 30 ) );
			
			mmap m2(m1);
			mmap::iterator i1 = m2.begin();
			CPPUNIT_ASSERT( i1 -> second == 10);
			
			mmap m3(10);
			bcount = m3.bucket_count();
			CPPUNIT_ASSERT( bcount >= 10);
			
			mmap::hasher hfn = m1.hash_funct();
			mmap m4(20,hfn);
			bcount = m4.bucket_count();
			CPPUNIT_ASSERT( bcount >= 20);
			
			mmap m5(m1.begin(),m2.end());
			CPPUNIT_ASSERT( m5.size() == 3);
			
			mmap m6(m1.begin(),m2.end(),30);
			CPPUNIT_ASSERT( m6.size() == 3);
			bcount = m6.bucket_count();
			CPPUNIT_ASSERT( bcount >= 30);
			
			mmap m7(m1.begin(),m2.end(),30,hfn);
			CPPUNIT_ASSERT( m7.size() == 3);
			bcount = m7.bucket_count();
			CPPUNIT_ASSERT( bcount >= 30);
			
			mmap::key_equal cmpfn;// = c1.key_eq();
			mmap m8(m1.begin(),m2.end(),30,hfn,cmpfn);
			
			mmap m9(30,hfn,cmpfn);
			}
			  __UHEAP_MARKEND;
	}

void HashTest::hash_multimap_cov1()
	{
	__UHEAP_MARK;
		{
		hash_multimap <int, int> hm1, hm2;
		hash_multimap <int, int>::iterator hm1_Iter;
		hash_multimap <int, int>::const_iterator hm1_cIter;
		typedef pair <int, int> Int_Pair;

		hm1.insert ( Int_Pair ( 1, 10 ) );
		hm1.insert ( Int_Pair ( 2, 20 ) );
		hm1.insert ( Int_Pair ( 3, 30 ) );
		hm2.insert ( Int_Pair ( 30, 300 ) );

		hm1.swap( hm2 );
		hm1_Iter = hm1.begin( ); 
		CPPUNIT_ASSERT( hm1_Iter -> second == 300);
		hm1_Iter = hm1.end( ); 
		hm1_cIter = hm1.end( ); 
		hm1.clear();
		CPPUNIT_ASSERT( hm1.size() == 0 );
		CPPUNIT_ASSERT( hm1.empty() == true );
		}
		{
		hash_multimap <int, int> hm1, hm2;
		hash_multimap <int, int>::iterator hm1_Iter;
		hash_multimap <int, int>::const_iterator hm1_cIter;
		typedef pair <int, int> Int_Pair;

		hm1.insert ( Int_Pair ( 1, 10 ) );
		hm1.insert ( Int_Pair ( 2, 20 ) );
		hm1.insert ( Int_Pair ( 3, 30 ) );
		hm2.insert ( Int_Pair ( 30, 300 ) );

		swap( hm1,hm2 );
		hm1_Iter = hm1.begin( ); 
		CPPUNIT_ASSERT( hm1_Iter -> second == 300);
		hm1_Iter = hm1.end( ); 
		hm1_cIter = hm1.end( ); 
		hm1.clear();
		CPPUNIT_ASSERT( hm1.size() == 0 );
		CPPUNIT_ASSERT( hm1.empty() == true );
		}
		  __UHEAP_MARKEND;
	}
void HashTest::hash_multimap_cov2()
	{
	__UHEAP_MARK;
		{
		hash_multimap <int, int> hm1;
		int i,bcount;
		typedef pair <int, int> Int_Pair;

		hm1.insert ( Int_Pair ( 1, 1 ) );
		i = hm1.size( );
		CPPUNIT_ASSERT( i == 1);
		
		i = hm1.max_size(); // for covering the api
		
		hm1.insert ( Int_Pair ( 2, 4 ) );
		i = hm1.size( );
		CPPUNIT_ASSERT( i == 2);
		hm1.resize(10);
		bcount = hm1.bucket_count();
		CPPUNIT_ASSERT( bcount >= 10);
		hm1.elems_in_bucket(1);
		}
		{
		typedef hash_multimap<int, int> mmap;
		mmap m1;

		typedef pair <int, int> Int_Pair;
		m1.insert_noresize ( Int_Pair ( 1, 10 ) );
		m1.insert_noresize ( Int_Pair ( 2, 20 ) );
		m1.insert_noresize ( Int_Pair ( 3, 30 ) );
		
		mmap::iterator i1 = m1.begin();
		CPPUNIT_ASSERT( i1 -> second == 10);
		}
		{
		typedef hash_multimap<char, int> mmap; 
		mmap c1; 
		mmap::key_equal cmpfn = c1.key_eq(); 
		CPPUNIT_ASSERT( cmpfn('a','a') == true);
		
		c1.max_bucket_count(); // for covering the api
		}
		  __UHEAP_MARKEND;
	}
void HashTest::hash_multimap_cov3()
	{
	__UHEAP_MARK;
		{
		typedef hash_multimap<int, int> mmap;
		mmap m1;
		int bcount;
		
		typedef pair <int, int> Int_Pair;
		m1.insert ( Int_Pair ( 1, 10 ) );
		m1.insert ( Int_Pair ( 2, 20 ) );
		m1.insert ( Int_Pair ( 3, 30 ) );
		
		mmap m2(m1);
		mmap::iterator i1 = m2.begin();
		CPPUNIT_ASSERT( i1 -> second == 10);
		
		mmap m3(10);
		bcount = m3.bucket_count();
		CPPUNIT_ASSERT( bcount >= 10);
		
		mmap::hasher hfn = m1.hash_funct();
		mmap m4(20,hfn);
		bcount = m4.bucket_count();
		CPPUNIT_ASSERT( bcount >= 20);
		
		mmap m5(m1.begin(),m2.end());
		CPPUNIT_ASSERT( m5.size() == 3);
		
		mmap m6(m1.begin(),m2.end(),30);
		CPPUNIT_ASSERT( m6.size() == 3);
		bcount = m6.bucket_count();
		CPPUNIT_ASSERT( bcount >= 30);
		
		mmap m7(m1.begin(),m2.end(),30,hfn);
		CPPUNIT_ASSERT( m7.size() == 3);
		bcount = m7.bucket_count();
		CPPUNIT_ASSERT( bcount >= 30);
		
		mmap::key_equal cmpfn;
		mmap m8(m1.begin(),m2.end(),30,hfn,cmpfn);
		
		mmap m9(30,hfn,cmpfn);
		}
		  __UHEAP_MARKEND;
	}

void HashTest::hash_set_cov1()
	{
	__UHEAP_MARK;
		{
		hash_set <int> hm1, hm2;
		hash_set <int>::iterator hm1_Iter;
		hash_set <int>::const_iterator hm1_cIter;

		hm1.insert ( 10 );
		hm1.insert ( 20 );
		hm1.insert ( 30 );
		hm2.insert ( 300 );

		hm1.swap( hm2 );
		hm1_Iter = hm1.begin( ); 
		CPPUNIT_ASSERT( *hm1_Iter == 300);
		hm1_Iter = hm1.end( ); 
		hm1_cIter = hm1.end( ); 
		hm1.clear();
		CPPUNIT_ASSERT( hm1.size() == 0 );
		CPPUNIT_ASSERT( hm1.empty() == true );
		}
		{
		hash_set <int> hm1, hm2;
		hash_set <int>::iterator hm1_Iter;
		hash_set <int>::const_iterator hm1_cIter;

		hm1.insert ( 10 );
		hm1.insert ( 20 );
		hm1.insert ( 30 );
		hm2.insert ( 300 );

		swap( hm1,hm2 );
		hm1_Iter = hm1.begin( ); 
		CPPUNIT_ASSERT( *hm1_Iter == 300);
		hm1_Iter = hm1.end( ); 
		hm1_cIter = hm1.end( ); 
		hm1.clear();
		CPPUNIT_ASSERT( hm1.size() == 0 );
		CPPUNIT_ASSERT( hm1.empty() == true );
		}
		  __UHEAP_MARKEND;
	}
void HashTest::hash_set_cov2()
	{
	__UHEAP_MARK;
		{
		hash_set <int> hm1;
		int i,bcount;

		hm1.insert ( 1 );
		i = hm1.size( );
		CPPUNIT_ASSERT( i == 1);
		
		i = hm1.max_size(); // for covering the api
		
		hm1.insert ( 4 );
		i = hm1.size( );
		CPPUNIT_ASSERT( i == 2);
		hm1.resize(10);
		bcount = hm1.bucket_count();
		CPPUNIT_ASSERT( bcount >= 10);
		hm1.elems_in_bucket(1);
		}
		{
		typedef hash_multiset<int> mmap;
		mmap m;

		m.insert ( 10 );
		m.insert ( 20 );
		m.insert ( 30 );
		
		std::pair<mmap::iterator, mmap::iterator> pair1 = m.equal_range(20); 
	    CPPUNIT_ASSERT( *(pair1.first) == 20);
	    CPPUNIT_ASSERT( *(pair1.second)  == 30);
	    // negative test case for equal_range where the key value is not present.
	    std::pair<mmap::const_iterator, mmap::const_iterator> pair2 = m.equal_range(40); 
	    CPPUNIT_ASSERT( pair2.first == m.end());
	    CPPUNIT_ASSERT( pair2.second  == m.end());
		}
		{
		typedef hash_multiset<int> mmap;
		mmap m;

		m.insert ( 10 );
		m.insert ( 20 );
		m.insert ( 30 );
		
		m.erase(m.begin());
		mmap::iterator i1 = m.begin();
		CPPUNIT_ASSERT( *i1 == 20);
		CPPUNIT_ASSERT( m.size() == 2 );
		m.erase(m.begin(),m.end());
		CPPUNIT_ASSERT( m.size() == 0 );
		}
		  __UHEAP_MARKEND;
	}
void HashTest::hash_set_cov3()
	{
	__UHEAP_MARK;
		{
		typedef hash_multiset<int> mmap;
		mmap m;

		m.insert ( 10 );
		m.insert ( 20 );
		m.insert ( 30 );
		
		mmap::iterator i1 = m.find(10);
		CPPUNIT_ASSERT( *i1 == 10);
		mmap::const_iterator i2 = m.find(30);
		CPPUNIT_ASSERT( *i2 == 30);
		}
		{
		typedef hash_multiset<int> mmap;
		typedef allocator<std::pair<int, int> > Myalloc;
		mmap m;	

		mmap::allocator_type al = m.get_allocator(); 
	    CPPUNIT_ASSERT ((al == Myalloc()) == true);
	    
	    mmap::hasher hfn = m.hash_funct(); // returns the hasher function
		}
		{
		typedef hash_multiset<int> mmap;
		mmap m1,m2;

		m1.insert ( 10 );
		m1.insert ( 20 );
		m1.insert ( 30 );
		
		m2.insert ( m1.begin(),m1.end());
		mmap::iterator i1 = m2.begin();
		CPPUNIT_ASSERT( *i1 == 10);
		}
		
			{
			typedef hash_set<int> mmap;
			mmap m;

			m.insert ( 10  );
			m.insert ( 20  );
			m.insert ( 30  );
			
			std::pair<mmap::iterator, mmap::iterator> pair1 = m.equal_range(20); 
		    CPPUNIT_ASSERT( *(pair1.first) == 20);
		    CPPUNIT_ASSERT( *(pair1.second)  == 30);
		    // negative test case for equal_range where the key value is not present.
		    std::pair<mmap::const_iterator, mmap::const_iterator> pair2 = m.equal_range(40); 
		    CPPUNIT_ASSERT( pair2.first == m.end());
		    CPPUNIT_ASSERT( pair2.second  == m.end());			}
			  __UHEAP_MARKEND;
	}
void HashTest::hash_set_cov4()
	{
	__UHEAP_MARK;
			{
			typedef hash_set<int> mmap;
			mmap m;

			m.insert ( 10 );
			m.insert ( 20 );
			m.insert ( 30 );
			
			m.erase(m.begin());
			mmap::iterator i1 = m.begin();
			CPPUNIT_ASSERT( *i1 == 20);
			CPPUNIT_ASSERT( m.size() == 2 );
			m.erase(m.begin(),m.end());
			CPPUNIT_ASSERT( m.size() == 0 );
			}
			{
			typedef hash_set<int> mmap;
			mmap m;

			m.insert ( 10 );
			m.insert ( 20 );
			m.insert ( 30 );
			
			mmap::iterator i1 = m.find(10);
			CPPUNIT_ASSERT( *i1 == 10);
			mmap::const_iterator i2 = m.find(30);
			CPPUNIT_ASSERT( *i2 == 30);
			}
			{
			typedef hash_set<int> mmap;
			typedef allocator<std::pair<int, int> > Myalloc;
			mmap m;	

			mmap::allocator_type al = m.get_allocator(); 
		    CPPUNIT_ASSERT ((al == Myalloc()) == true);
		    
		    mmap::hasher hfn = m.hash_funct(); // returns the hasher function
			}
			__UHEAP_MARKEND;
	}
void HashTest::hash_set_cov5()
	{
	__UHEAP_MARK;
			{
			typedef hash_set<int> mmap;
			mmap m1,m2;

			m1.insert ( 10 );
			m1.insert ( 20 );
			m1.insert ( 30 );
			
			m2.insert ( m1.begin(),m1.end());
			mmap::iterator i1 = m2.begin();
			CPPUNIT_ASSERT( *i1 == 10);
			}
			{
			typedef hash_set<char> mmap; 
			mmap c1; 
			mmap::key_equal cmpfn = c1.key_eq(); 
			CPPUNIT_ASSERT( cmpfn('a','a') == true);
			
			c1.max_bucket_count(); // for covering the api
			}
			{
			typedef hash_set<int> mmap;
			mmap m1;

			m1.insert_noresize ( 10 );
			m1.insert_noresize ( 20 );
			m1.insert_noresize ( 30 );
			
			mmap::iterator i1 = m1.begin();
			CPPUNIT_ASSERT( *i1 == 10);
			}
			{
			typedef hash_set<int> mmap;
			mmap m1;
			int bcount;
			
			m1.insert ( 10 );
			m1.insert ( 20 );
			m1.insert ( 30 );
			
			mmap m2(m1);
			mmap::iterator i1 = m2.begin();
			CPPUNIT_ASSERT( *i1 == 10);
			
			mmap m3(10);
			bcount = m3.bucket_count();
			CPPUNIT_ASSERT( bcount >= 10);
			
			mmap::hasher hfn = m1.hash_funct();
			mmap m4(20,hfn);
			bcount = m4.bucket_count();
			CPPUNIT_ASSERT( bcount >= 20);
			
			mmap m5(m1.begin(),m2.end());
			CPPUNIT_ASSERT( m5.size() == 3);
			
			mmap m6(m1.begin(),m2.end(),30);
			CPPUNIT_ASSERT( m6.size() == 3);
			bcount = m6.bucket_count();
			CPPUNIT_ASSERT( bcount >= 30);
			
			mmap m7(m1.begin(),m2.end(),30,hfn);
			CPPUNIT_ASSERT( m7.size() == 3);
			bcount = m7.bucket_count();
			CPPUNIT_ASSERT( bcount >= 30);
			
			mmap::key_equal cmpfn;// = c1.key_eq();
			mmap m8(m1.begin(),m2.end(),30,hfn,cmpfn);
			
			mmap m9(30,hfn,cmpfn);
			}
			__UHEAP_MARKEND;
	}
void HashTest::hash_set_cov6()
	{
	__UHEAP_MARK;
			{
			typedef hash_set<int> mmap;
			mmap m1;
			
			m1.insert ( 10 );
			m1.insert ( 20 );
			m1.insert ( 30 );
			
			CPPUNIT_ASSERT( m1.count(10) == 1);	
			CPPUNIT_ASSERT( m1.erase(10) == 1);	
			CPPUNIT_ASSERT( m1.count(10) == 0);	
			}
			{
			hash_set <int> hm1;
			hash_set <int>::iterator hm1_Iter;
			hm1.insert ( 10 );
			hm1.insert ( 20 );
			hm1.insert ( 30 );
			hm1_Iter = hm1.begin( ); 
			CPPUNIT_ASSERT( *hm1_Iter == 10);
			hm1_Iter ++ ;
			CPPUNIT_ASSERT( *hm1_Iter == 20);
			hm1_Iter ++ ;
			CPPUNIT_ASSERT( *hm1_Iter == 30);
			
			}
			__UHEAP_MARKEND;
	}

void HashTest::hash_multiset_cov1()
	{
	__UHEAP_MARK;
		{
		hash_multiset <int> hm1, hm2;
		hash_multiset <int>::iterator hm1_Iter;
		hash_multiset <int>::const_iterator hm1_cIter;

		hm1.insert ( 10 );
		hm1.insert ( 20 );
		hm1.insert ( 30 );
		hm2.insert ( 300 );

		hm1.swap( hm2 );
		hm1_Iter = hm1.begin( ); 
		CPPUNIT_ASSERT( *hm1_Iter== 300);
		hm1_Iter = hm1.end( ); 
		hm1_cIter = hm1.end( ); 
		hm1.clear();
		CPPUNIT_ASSERT( hm1.size() == 0 );
		CPPUNIT_ASSERT( hm1.empty() == true );
		}
		{
		hash_multiset <int> hm1, hm2;
		hash_multiset <int>::iterator hm1_Iter;
		hash_multiset <int>::const_iterator hm1_cIter;

		hm1.insert ( 10 );
		hm1.insert ( 20 );
		hm1.insert ( 30 );
		hm2.insert ( 300 );

		swap( hm1,hm2 );
		hm1_Iter = hm1.begin( ); 
		CPPUNIT_ASSERT( *hm1_Iter== 300);
		hm1_Iter = hm1.end( ); 
		hm1_cIter = hm1.end( ); 
		hm1.clear();
		CPPUNIT_ASSERT( hm1.size() == 0 );
		CPPUNIT_ASSERT( hm1.empty() == true );
		}
		  __UHEAP_MARKEND;
	}
void HashTest::hash_multiset_cov2()
	{
	__UHEAP_MARK;
		{
		hash_multiset <int> hm1;
		int i,bcount;

		hm1.insert ( 1 );
		i = hm1.size( );
		CPPUNIT_ASSERT( i == 1);
		
		i = hm1.max_size(); // for covering the api
		
		hm1.insert ( 4 );
		i = hm1.size( );
		CPPUNIT_ASSERT( i == 2);
		hm1.resize(10);
		bcount = hm1.bucket_count();
		CPPUNIT_ASSERT( bcount >= 10);
		hm1.elems_in_bucket(1);
		}
		{
		typedef hash_multiset<int> mmap;
		mmap m1;

		m1.insert_noresize ( 10 );
		m1.insert_noresize ( 20 );
		m1.insert_noresize ( 30 );
		
		mmap::iterator i1 = m1.begin();
		CPPUNIT_ASSERT( *i1 == 10);
		}
		{
		typedef hash_multiset<char> mmap; 
		mmap c1; 
		mmap::key_equal cmpfn = c1.key_eq(); 
		CPPUNIT_ASSERT( cmpfn('a','a') == true);
		
		c1.max_bucket_count(); // for covering the api
		}
		  __UHEAP_MARKEND;
	}
void HashTest::hash_multiset_cov3()
	{
	__UHEAP_MARK;
		{
		typedef hash_multiset<int> mmap;
		mmap m1;
		int bcount;
		
		m1.insert ( 10 );
		m1.insert ( 20 );
		m1.insert ( 30 );
		
		mmap m2(m1);
		mmap::iterator i1 = m2.begin();
		CPPUNIT_ASSERT( *i1 == 10);
		
		mmap m3(10);
		bcount = m3.bucket_count();
		CPPUNIT_ASSERT( bcount >= 10);
		
		mmap::hasher hfn = m1.hash_funct();
		mmap m4(20,hfn);
		bcount = m4.bucket_count();
		CPPUNIT_ASSERT( bcount >= 20);
		
		mmap m5(m1.begin(),m2.end());
		CPPUNIT_ASSERT( m5.size() == 3);
		
		mmap m6(m1.begin(),m2.end(),30);
		CPPUNIT_ASSERT( m6.size() == 3);
		bcount = m6.bucket_count();
		CPPUNIT_ASSERT( bcount >= 30);
		
		mmap m7(m1.begin(),m2.end(),30,hfn);
		CPPUNIT_ASSERT( m7.size() == 3);
		bcount = m7.bucket_count();
		CPPUNIT_ASSERT( bcount >= 30);
		
		mmap::key_equal cmpfn;// = c1.key_eq();
		mmap m8(m1.begin(),m2.end(),30,hfn,cmpfn);
		
		mmap m9(30,hfn,cmpfn);
		}
		  __UHEAP_MARKEND;
	}
