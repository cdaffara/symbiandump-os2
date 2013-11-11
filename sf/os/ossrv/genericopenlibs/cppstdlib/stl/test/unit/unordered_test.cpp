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
#include <vector>
#include <algorithm>
#include <string>
#if defined (STLPORT)
#  include <unordered_map>
#  include <unordered_set>
#endif

//#include <iostream>

#include "cppunit/cppunit_proxy.h"

#if !defined (STLPORT) || defined(_STLP_USE_NAMESPACES)
using namespace std;
#endif

//
// TestCase class
//
class UnorderedTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(UnorderedTest);
#if !defined (STLPORT) 
  CPPUNIT_IGNORE;
#endif
  CPPUNIT_TEST(uset);
  CPPUNIT_TEST(umultiset);
#if defined (__DMC__)
  CPPUNIT_IGNORE;
#endif
  CPPUNIT_TEST(umap);
  CPPUNIT_STOP_IGNORE;
  CPPUNIT_TEST(umultimap);
#if defined (__DMC__)
  CPPUNIT_IGNORE;
#endif
  CPPUNIT_TEST(user_case);
  CPPUNIT_STOP_IGNORE;
  CPPUNIT_TEST(hash_policy);
  CPPUNIT_TEST(buckets);
#if defined (__DMC__)
  CPPUNIT_IGNORE;
#endif
  CPPUNIT_TEST(equal_range);
#if !defined (_STLP_USE_CONTAINERS_EXTENSION)
  CPPUNIT_IGNORE;
#endif
  CPPUNIT_TEST(template_methods);
  CPPUNIT_TEST(unordered_set_cov1);
  CPPUNIT_TEST(unordered_set_cov2);
  CPPUNIT_TEST(unordered_set_cov3);
  CPPUNIT_TEST(unordered_map_cov1);
  CPPUNIT_TEST(unordered_map_cov2);
  CPPUNIT_TEST(unordered_map_cov3);
  CPPUNIT_TEST(unordered_map_cov4);
  CPPUNIT_TEST(unordered_map_cov5);
  CPPUNIT_TEST(unordered_multimap_cov1);
  CPPUNIT_TEST(unordered_multimap_cov2);
  CPPUNIT_TEST(unordered_multimap_cov3);
  CPPUNIT_TEST(unordered_multimap_cov4);
  CPPUNIT_TEST(unordered_multimap_cov5);
  CPPUNIT_TEST(unordered_multiset_cov1);
  CPPUNIT_TEST(unordered_multiset_cov2);
  CPPUNIT_TEST(unordered_multiset_cov3);
  CPPUNIT_TEST(unordered_multiset_cov4);
  CPPUNIT_TEST_SUITE_END();

protected:
  void uset();
  void umultiset();
  void umap();
  void umultimap();
  void user_case();
  void hash_policy();
  void buckets();
  void equal_range();
  void template_methods();
  void unordered_set_cov1();
  void unordered_set_cov2();
  void unordered_set_cov3();
  void unordered_map_cov1();
  void unordered_map_cov2();
  void unordered_map_cov3();
  void unordered_map_cov4();
  void unordered_map_cov5();
  void unordered_multimap_cov1();
  void unordered_multimap_cov2();
  void unordered_multimap_cov3();
  void unordered_multimap_cov4();
  void unordered_multimap_cov5();
  void unordered_multiset_cov1();
  void unordered_multiset_cov2();
  void unordered_multiset_cov3();
  void unordered_multiset_cov4();
};

CPPUNIT_TEST_SUITE_REGISTRATION(UnorderedTest);

const int NB_ELEMS = 2000;

//
// tests implementation
//
void UnorderedTest::uset()
{
#if defined (STLPORT)
  typedef unordered_set<int, hash<int>, equal_to<int> > usettype;
  usettype us;

  //Small compilation check of the copy constructor:
  usettype us2(us);
  //And assignment operator
  us = us2;

  int i;
  pair<usettype::iterator, bool> ret;
  for (i = 0; i < NB_ELEMS; ++i) {
    ret = us.insert(i);
    CPPUNIT_ASSERT( ret.second );
    CPPUNIT_ASSERT( *ret.first == i );

    ret = us.insert(i);
    CPPUNIT_ASSERT( !ret.second );
    CPPUNIT_ASSERT( *ret.first == i );
  }

  vector<int> us_val;

  usettype::local_iterator lit, litEnd;
  for (i = 0; i < NB_ELEMS; ++i) {
    lit = us.begin(us.bucket(i));
    litEnd = us.end(us.bucket(i));

    usettype::size_type bucket_pos = us.bucket(*lit);
    for (; lit != litEnd; ++lit) {
      CPPUNIT_ASSERT( us.bucket(*lit) == bucket_pos );
      us_val.push_back(*lit);
    }
  }

  //A compilation time check to uncomment from time to time
  {
    //usettype::iterator it;
    //CPPUNIT_ASSERT( it != lit );
  }

  sort(us_val.begin(), us_val.end());
  for (i = 0; i < NB_ELEMS; ++i) {
    CPPUNIT_ASSERT( us_val[i] == i );
  }
#endif
}

void UnorderedTest::umultiset()
{
#if defined (STLPORT)
  typedef unordered_multiset<int, hash<int>, equal_to<int> > usettype;
  usettype us;

  int i;
  usettype::iterator ret;
  for (i = 0; i < NB_ELEMS; ++i) {
    ret = us.insert(i);
    CPPUNIT_ASSERT( *ret == i );

    ret = us.insert(i);
    CPPUNIT_ASSERT( *ret == i );
  }

  CPPUNIT_ASSERT( us.size() == 2 * NB_ELEMS );
  vector<int> us_val;

  usettype::local_iterator lit, litEnd;
  for (i = 0; i < NB_ELEMS; ++i) {
    lit = us.begin(us.bucket(i));
    litEnd = us.end(us.bucket(i));

    usettype::size_type bucket_pos = us.bucket(*lit);
    for (; lit != litEnd; ++lit) {
      CPPUNIT_ASSERT( us.bucket(*lit) == bucket_pos );
      us_val.push_back(*lit);
    }
  }

  sort(us_val.begin(), us_val.end());
  for (i = 0; i < NB_ELEMS; ++i) {
    CPPUNIT_ASSERT( us_val[2 * i] == i );
    CPPUNIT_ASSERT( us_val[2 * i + 1] == i );
  }
#endif
}

void UnorderedTest::umap()
{
#if defined (STLPORT) && !defined (__DMC__)
  typedef unordered_map<int, int, hash<int>, equal_to<int> > umaptype;
  umaptype us;

  //Compilation check of the [] operator:
  umaptype us2;
  us[0] = us2[0];
  us.clear();

  {
    //An other compilation check
    typedef unordered_map<int, umaptype> uumaptype;
    uumaptype uus;
    umaptype const& uref = uus[0];
    umaptype ucopy = uus[0];
    ucopy = uref;
    //Avoids warning:
    //(void*)&uref;
  }

  int i;
  pair<umaptype::iterator, bool> ret;
  for (i = 0; i < NB_ELEMS; ++i) {
    umaptype::value_type p1(i, i);
    ret = us.insert(p1);
    CPPUNIT_ASSERT( ret.second );
    CPPUNIT_ASSERT( *ret.first == p1 );

    umaptype::value_type p2(i, i + 1);
    ret = us.insert(p2);
    CPPUNIT_ASSERT( !ret.second );
    CPPUNIT_ASSERT( *ret.first == p1 );
  }

  {
    //Lets look for some values to see if everything is normal:
    umaptype::iterator umit;
    for (int j = 0; j < NB_ELEMS; j += NB_ELEMS / 100) {
      umit = us.find(j);

      CPPUNIT_ASSERT( umit != us.end() );
      CPPUNIT_ASSERT( (*umit).second == j );
    }
  }

  CPPUNIT_ASSERT( us.size() == (size_t)NB_ELEMS );
  vector<pair<int, int> > us_val;

  umaptype::local_iterator lit, litEnd;
  for (i = 0; i < NB_ELEMS; ++i) {
    lit = us.begin(us.bucket(i));
    litEnd = us.end(us.bucket(i));

    umaptype::size_type bucket_pos = us.bucket((*lit).first);
    for (; lit != litEnd; ++lit) {
      CPPUNIT_ASSERT( us.bucket((*lit).first) == bucket_pos );
      us_val.push_back(make_pair((*lit).first, (*lit).second));
    }
  }

  sort(us_val.begin(), us_val.end());
  for (i = 0; i < NB_ELEMS; ++i) {
    CPPUNIT_ASSERT( us_val[i] == make_pair(i, i) );
  }
#endif
}

void UnorderedTest::umultimap()
{
#if defined (STLPORT)
  typedef unordered_multimap<int, int, hash<int>, equal_to<int> > umaptype;
  umaptype us;

  int i;
  umaptype::iterator ret;
  for (i = 0; i < NB_ELEMS; ++i) {
    umaptype::value_type p(i, i);
    ret = us.insert(p);
    CPPUNIT_ASSERT( *ret == p );

    ret = us.insert(p);
    CPPUNIT_ASSERT( *ret == p );
  }

  CPPUNIT_ASSERT( us.size() == 2 * NB_ELEMS );
  typedef pair<int, int> ptype;
  vector<ptype> us_val;

  umaptype::local_iterator lit, litEnd;
  for (i = 0; i < NB_ELEMS; ++i) {
    lit = us.begin(us.bucket(i));
    litEnd = us.end(us.bucket(i));

    umaptype::size_type bucket_pos = us.bucket((*lit).first);
    for (; lit != litEnd; ++lit) {
      CPPUNIT_ASSERT( us.bucket((*lit).first) == bucket_pos );
      us_val.push_back(ptype((*lit).first, (*lit).second));
    }
  }

  sort(us_val.begin(), us_val.end());
  for (i = 0; i < NB_ELEMS; ++i) {
    ptype p(i, i);
    CPPUNIT_ASSERT( us_val[i * 2] == p );
    CPPUNIT_ASSERT( us_val[i * 2 + 1] == p );
  }
#endif
}

void UnorderedTest::user_case()
{
#if defined (STLPORT) && !defined (__DMC__)
  typedef unordered_map<int, string> UnorderedMap1;
  typedef unordered_map<int, UnorderedMap1> UnorderedMap2;

  UnorderedMap1 foo;
  UnorderedMap2 bar;

  foo.insert(UnorderedMap1::value_type(1, string("test1")));
  foo.insert(UnorderedMap1::value_type(2, string("test2")));
  foo.insert(UnorderedMap1::value_type(3, string("test3")));
  foo.insert(UnorderedMap1::value_type(4, string("test4")));
  foo.insert(UnorderedMap1::value_type(5, string("test5")));

  bar.insert(UnorderedMap2::value_type(0, foo));
  UnorderedMap2::iterator it = bar.find(0);
  CPPUNIT_ASSERT( it != bar.end() );

  UnorderedMap1 &body = it->second;
  UnorderedMap1::iterator cur = body.find(3);
  CPPUNIT_ASSERT( cur != body.end() );

  body.erase(body.begin(), body.end());
  CPPUNIT_ASSERT( body.empty() );
#endif
}

void UnorderedTest::hash_policy()
{
#if defined (STLPORT)
  unordered_set<int> int_uset;

  CPPUNIT_ASSERT( int_uset.max_load_factor() == 1.0f );
  CPPUNIT_ASSERT( int_uset.load_factor() == 0.0f );

  size_t nbInserts = int_uset.bucket_count() - 1;
  for (int i = 0; (size_t)i < nbInserts; ++i) {
    int_uset.insert(i);
  }
  CPPUNIT_ASSERT( int_uset.size() == nbInserts );

  int_uset.max_load_factor(0.5f);
  int_uset.rehash(0);
  CPPUNIT_ASSERT( int_uset.load_factor() < int_uset.max_load_factor() );

  size_t bucketsHint = int_uset.bucket_count() + 1;
  int_uset.rehash(bucketsHint);
  CPPUNIT_ASSERT( int_uset.bucket_count() >= bucketsHint );

  CPPUNIT_ASSERT( int_uset.key_eq()(10, 10) );
  CPPUNIT_ASSERT( int_uset.hash_function()(10) == 10 );
#endif
}

void UnorderedTest::buckets()
{
#if defined (STLPORT) 
  unordered_set<int> int_uset;

  CPPUNIT_ASSERT( int_uset.bucket_count() < int_uset.max_bucket_count() );

  int i;
  size_t nbBuckets = int_uset.bucket_count();
  size_t nbInserts = int_uset.bucket_count() - 1;
  for (i = 0; (size_t)i < nbInserts; ++i) {
    int_uset.insert(i);
  }
  CPPUNIT_ASSERT( nbBuckets == int_uset.bucket_count() );

  size_t bucketSizes = 0;
  for (i = 0; (size_t)i < nbBuckets; ++i) {
    bucketSizes += int_uset.bucket_size(i);
  }
  CPPUNIT_ASSERT( bucketSizes == int_uset.size() );
#endif
}

void UnorderedTest::equal_range()
{
#if defined (STLPORT) && !defined (__DMC__)
  typedef unordered_multiset<size_t> umset;
  {
    //General test
    umset iumset;
    iumset.max_load_factor(10.0f);

    size_t nbBuckets = iumset.bucket_count();

    for (size_t i = 0; i < nbBuckets; ++i) {
      iumset.insert(i);
      iumset.insert(i + nbBuckets);
      iumset.insert(i + 2 * nbBuckets);
      iumset.insert(i + 3 * nbBuckets);
      iumset.insert(i + 4 * nbBuckets);
    }

    CPPUNIT_ASSERT( nbBuckets == iumset.bucket_count() );
    CPPUNIT_ASSERT( iumset.size() == 5 * nbBuckets );

    pair<umset::iterator, umset::iterator> p = iumset.equal_range(1);
    CPPUNIT_ASSERT( p.first != p.second );

    size_t nbElems = iumset.size();
    nbElems -= distance(p.first, p.second);
    for (umset::iterator j = p.first; j != p.second;) {
      iumset.erase(j++);
    }

    CPPUNIT_ASSERT( nbElems == iumset.size() );

    p = iumset.equal_range(2);
    CPPUNIT_ASSERT( p.first != p.second );
    nbElems -= distance(p.first, p.second);
    iumset.erase(p.first, p.second);
    CPPUNIT_ASSERT( nbElems == iumset.size() );
  }

  {
    //More specific test that tries to put many values in the same bucket
    umset iumset;

    size_t i;
    const size_t nbBuckets = iumset.bucket_count();
    const size_t targetedBucket = nbBuckets / 2;

    //Lets put 10 values in the targeted bucket:
    for (i = 0; i < 10; ++i) {
      iumset.insert(targetedBucket + (i * nbBuckets));
    }

    //We put again 10 values in the targeted bucket and in reverse order:
    for (i = 9; i <= 10; --i) {
      iumset.insert(targetedBucket + (i * nbBuckets));
    }

    //Now we put some more elements until hash container is resized:
    i = 0;
    while (iumset.bucket_count() == nbBuckets) {
      iumset.insert(i++);
    }

    //CPPUNIT_ASSERT( iumset.bucket_size(targetedBucket) == 21 );

    pair<umset::iterator, umset::iterator> p = iumset.equal_range(targetedBucket);
    CPPUNIT_ASSERT( p.first != p.second );
    CPPUNIT_ASSERT( distance(p.first, p.second) == 3 );
  }

  {
    srand(0);
    for (int runs = 0; runs < 2; ++runs) {
      size_t magic = rand();
      umset hum;
      size_t c = 0;
      for (int i = 0; i < 10000; ++i) {
        if ((rand() % 500) == 0) {
          hum.insert(magic);
          ++c;
        }
        else {
          size_t r;
          while ((r = rand()) == magic);
          hum.insert(r);
        }

        /*
        if ((float)(hum.size() + 1) / (float)hum.bucket_count() > hum.max_load_factor()) {
          cout << "Hash container dump: Nb elems: " << hum.size() << ", Nb buckets: " << hum.bucket_count() << "\n";
          for (size_t b = 0; b < hum.bucket_count(); ++b) {
            if (hum.bucket_size(b) != 0) {
              umset::local_iterator litBegin(hum.begin(b)), litEnd(hum.end(b));
              cout << "B" << b << ": ";
              for (umset::local_iterator lit = litBegin; lit != litEnd; ++lit) {
                if (lit != litBegin) {
                  cout << " - ";
                }
                cout << *lit;
              }
              cout << "\n";
            }
          }
          cout << endl;
        }
        */
      }
      CPPUNIT_ASSERT( hum.count(magic) == c );
    }
  }
#endif
}

struct Key
{
  Key() : m_data(0) {}
  explicit Key(int data) : m_data(data) {}

  int m_data;
};

struct KeyHash
{
  size_t operator () (Key key) const
  { return (size_t)key.m_data; }

  size_t operator () (int data) const
  { return (size_t)data; }
};

struct KeyEqual
{
  bool operator () (Key lhs, Key rhs) const
  { return lhs.m_data == rhs.m_data; }

  bool operator () (Key lhs, int rhs) const
  { return lhs.m_data == rhs; }

  bool operator () (int lhs, Key rhs) const
  { return lhs == rhs.m_data; }
};

struct KeyHashPtr
{
  size_t operator () (Key const volatile *key) const
  { return (size_t)key->m_data; }

  size_t operator () (int data) const
  { return (size_t)data; }
};

struct KeyEqualPtr
{
  bool operator () (Key const volatile *lhs, Key const volatile *rhs) const
  { return lhs->m_data == rhs->m_data; }

  bool operator () (Key const volatile *lhs, int rhs) const
  { return lhs->m_data == rhs; }

  bool operator () (int lhs, Key const volatile *rhs) const
  { return lhs == rhs->m_data; }
};

void UnorderedTest::template_methods()
{
#if defined (STLPORT) && defined (_STLP_USE_CONTAINERS_EXTENSION)
  {
    typedef unordered_set<Key, KeyHash, KeyEqual> Container;
    Container cont;
    cont.insert(Key(1));
    cont.insert(Key(2));
    cont.insert(Key(3));
    cont.insert(Key(4));

    CPPUNIT_ASSERT( cont.count(Key(1)) == 1 );
    CPPUNIT_ASSERT( cont.count(1) == 1 );
    CPPUNIT_ASSERT( cont.count(5) == 0 );

    CPPUNIT_ASSERT( cont.find(2) != cont.end() );
    CPPUNIT_ASSERT( cont.equal_range(2) != make_pair(cont.begin(), cont.end()) );

    Container const& ccont = cont;
    CPPUNIT_ASSERT( ccont.find(2) != ccont.end() );
    CPPUNIT_ASSERT( ccont.bucket(2) == ccont.bucket(2) );
    CPPUNIT_ASSERT( ccont.equal_range(2) != make_pair(ccont.begin(), ccont.end()) );
  }

  {
    typedef unordered_set<Key*, KeyHashPtr, KeyEqualPtr> Container;
    Container cont;
    Key key1(1), key2(2), key3(3), key4(4);
    cont.insert(&key1);
    cont.insert(&key2);
    cont.insert(&key3);
    cont.insert(&key4);

    CPPUNIT_ASSERT( cont.count(1) == 1 );
    CPPUNIT_ASSERT( cont.count(5) == 0 );

    CPPUNIT_ASSERT( cont.find(2) != cont.end() );
    CPPUNIT_ASSERT( cont.equal_range(2) != make_pair(cont.begin(), cont.end()) );

    Container const& ccont = cont;
    CPPUNIT_ASSERT( ccont.find(2) != ccont.end() );
    CPPUNIT_ASSERT( ccont.bucket(2) == ccont.bucket(2) );
    CPPUNIT_ASSERT( ccont.equal_range(2) != make_pair(ccont.begin(), ccont.end()) );
  }
  {
    typedef unordered_multiset<Key, KeyHash, KeyEqual> Container;
    Container cont;
    cont.insert(Key(1));
    cont.insert(Key(2));
    cont.insert(Key(1));
    cont.insert(Key(2));

    CPPUNIT_ASSERT( cont.count(Key(1)) == 2 );
    CPPUNIT_ASSERT( cont.count(1) == 2 );
    CPPUNIT_ASSERT( cont.count(5) == 0 );

    CPPUNIT_ASSERT( cont.find(2) != cont.end() );
    CPPUNIT_ASSERT( cont.equal_range(1) != make_pair(cont.end(), cont.end()) );

    Container const& ccont = cont;
    CPPUNIT_ASSERT( ccont.find(2) != ccont.end() );
    CPPUNIT_ASSERT( ccont.bucket(2) == ccont.bucket(2) );
    CPPUNIT_ASSERT( ccont.equal_range(2) != make_pair(ccont.end(), ccont.end()) );
  }

  {
    typedef unordered_multiset<Key const volatile*, KeyHashPtr, KeyEqualPtr> Container;
    Container cont;
    Key key1(1), key2(2), key3(3), key4(4);
    cont.insert(&key1);
    cont.insert(&key2);
    cont.insert(&key3);
    cont.insert(&key4);

    CPPUNIT_ASSERT( cont.count(1) == 1 );
    CPPUNIT_ASSERT( cont.count(5) == 0 );

    CPPUNIT_ASSERT( cont.find(2) != cont.end() );
    CPPUNIT_ASSERT( cont.equal_range(2) != make_pair(cont.begin(), cont.end()) );

    Container const& ccont = cont;
    CPPUNIT_ASSERT( ccont.find(2) != ccont.end() );
    CPPUNIT_ASSERT( ccont.bucket(2) == ccont.bucket(2) );
    CPPUNIT_ASSERT( ccont.equal_range(2) != make_pair(ccont.begin(), ccont.end()) );
  }
#endif
}
void UnorderedTest::unordered_set_cov1()
	{
	__UHEAP_MARK;		
		{
		typedef unordered_set<char> Myset;	
		Myset c1,c2; 
		
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c'); 

	    c2.insert('d'); 
	    c2.insert('e'); 
	    c2.insert('f'); 
	 
	    c1.swap(c2); 
	    Myset::const_iterator it1 = c1.begin();
	    Myset::const_iterator it2 = c2.begin(); 
	    CPPUNIT_ASSERT( *it1 == 'd' );
	    CPPUNIT_ASSERT( *it2 == 'a' );
	    it1++;it2++;
	    CPPUNIT_ASSERT( *it1 == 'e' );
	    CPPUNIT_ASSERT( *it2 == 'b' );
	    it1++;it2++;
	    CPPUNIT_ASSERT( *it1 == 'f' );
	    CPPUNIT_ASSERT( *it2 == 'c' );
		}
		{
		typedef unordered_set<char> Myset;	
		Myset c1,c2; 
		
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c'); 

	    c2.insert('d'); 
	    c2.insert('e'); 
	    c2.insert('f'); 
	 
	    swap(c1,c2); 
		Myset::const_iterator it1 = c1.begin();
		Myset::const_iterator it2 = c2.begin(); 
	    CPPUNIT_ASSERT( *it1 == 'd' );
	    CPPUNIT_ASSERT( *it2 == 'a' );
	    it1++;it2++;
	    CPPUNIT_ASSERT( *it1 == 'e' );
	    CPPUNIT_ASSERT( *it2 == 'b' );
	    it1++;it2++;
	    CPPUNIT_ASSERT( *it1 == 'f' );
	    CPPUNIT_ASSERT( *it2 == 'c' );
		}
		{
		typedef unordered_set<char> Myset;	
		Myset c1;
		
		c1.max_size();
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_set_cov2()
	{
	__UHEAP_MARK;
		{
		typedef unordered_set<char> Myset; 
		Myset c1; 
		 
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c'); 
	    
	    c1.clear(); 
		CPPUNIT_ASSERT(c1.size() == 0 );
		CPPUNIT_ASSERT(c1.empty() );
		}
		{
		typedef unordered_set<char> Myset; 
		Myset c1; 
		 
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c'); 
	    
	    Myset::const_iterator it = c1.end();
	    //it--;
	    //CPPUNIT_ASSERT(*it == 'a' );
	    
	    c1.max_size();
		}
		{
		typedef unordered_set<char> Myset; 
		Myset c1; 
		 
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c'); 
	    
	    c1.erase(c1.begin()); 
	    Myset::iterator it2 = c1.begin();
	    CPPUNIT_ASSERT(*it2 == 'b' );
	    
	    c1.erase(c1.begin(), c1.end()); 
	    CPPUNIT_ASSERT(c1.size() == 0 );
		}
		{
		typedef unordered_set<char> Myset; 
		Myset c1; 
		 
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c');
		CPPUNIT_ASSERT( c1.erase('b')  == 1);	
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_set_cov3()
	{
	__UHEAP_MARK;
		{
		typedef unordered_set<char> Mymap; 
		typedef allocator<std::pair<const char, int> > Myalloc;
		Mymap c1; 
		 
	    Mymap::allocator_type al = c1.get_allocator(); 
	    CPPUNIT_ASSERT ((al == Myalloc()) == true);

		}
		__UHEAP_MARKEND;
    }

void UnorderedTest::unordered_map_cov1()
	{
	__UHEAP_MARK;
		{
		typedef unordered_map<char, int> Mymap;
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 
	    
	    Mymap::const_iterator it = c1.begin(); 
	    CPPUNIT_ASSERT( it->first == 'a');
	    CPPUNIT_ASSERT( it->second  == 1);

	    Mymap::const_iterator it1 = c1.end();
	    
	    Mymap::const_local_iterator lit = c1.begin(c1.bucket('a')); 
	    CPPUNIT_ASSERT( lit->first == 'a');
	    CPPUNIT_ASSERT( lit->second  == 1);
	    
	    Mymap::const_local_iterator lit1 = c1.end(c1.bucket('a')); 
		}
		{
		typedef unordered_map<char, int> Mymap; 
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 
	    c1.bucket_count();
	    
	    Mymap::size_type bs = c1.bucket('a'); 
	    CPPUNIT_ASSERT( c1.bucket_size(bs)  == 1);
	    
	    CPPUNIT_ASSERT( c1.count('A')  == 0);
	    CPPUNIT_ASSERT( c1.count('b')  == 1);
	    CPPUNIT_ASSERT( c1.count('C')  == 0);
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_map_cov2()
	{
	__UHEAP_MARK;
		{
		typedef unordered_map<char, int> Mymap; 
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 

	    std::pair<Mymap::iterator, Mymap::iterator> pair1 = c1.equal_range('b'); 
	    CPPUNIT_ASSERT( pair1.first->first == 'b');
	    CPPUNIT_ASSERT( pair1.first->second  == 2);
	    
	    std::pair<Mymap::iterator, Mymap::const_iterator> pair2 = c1.equal_range('c'); 
	    CPPUNIT_ASSERT( pair2.first->first == 'c');
	    CPPUNIT_ASSERT( pair2.first->second  == 3);
		}
		{
		typedef unordered_map<char, int> Mymap; 
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 
	
	    c1.erase(c1.begin()); 
	    Mymap::iterator it2 = c1.begin();
	    CPPUNIT_ASSERT( it2->first == 'b');
	    CPPUNIT_ASSERT( it2->second  == 2);
	    
	    CPPUNIT_ASSERT( c1.erase('b')  == 1);
		}
		{
		typedef unordered_map<char, int> Mymap;	
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 

	    Mymap::iterator it = c1.find('b'); 
	    CPPUNIT_ASSERT( it->first == 'b');
	    CPPUNIT_ASSERT( it->second  == 2);
	    
	    // negative test case where the element to find is not present in the 
	    // unordered_map.
	    Mymap::iterator it1 = c1.find('d');
	    Mymap::iterator it2 = c1.end();
	    CPPUNIT_ASSERT( it1 == it2);
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_map_cov3()
	{
	__UHEAP_MARK;
		{
		typedef unordered_map<char, int> Mymap; 
		typedef allocator<std::pair<const char, int> > Myalloc;
		Mymap c1; 
		 
	    Mymap::allocator_type al = c1.get_allocator(); 
	    CPPUNIT_ASSERT ((al == Myalloc()) == true);
	    
	    Mymap::hasher hfn = c1.hash_function(); 
	    hfn('a');// returns the hash function object
		}
		{
		typedef unordered_map<char, int> Mymap; 
		Mymap c1,c2; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 
	    
	    Mymap::iterator it2;
	    
	    c2.insert(c1.begin(), c1.end()); 
	    it2 = c2.begin();
	    CPPUNIT_ASSERT( it2->first == 'a');
	    CPPUNIT_ASSERT( it2->second  == 1);
		}		
		{
		typedef unordered_map<char, int> Mymap; 
		Mymap c1; 
		Mymap::key_equal cmpfn = c1.key_eq(); 
	    CPPUNIT_ASSERT( cmpfn('a','a') == true);
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_map_cov4()
	{
	__UHEAP_MARK;
		{
		typedef unordered_map<char, int> Mymap; 
		Mymap c1,c2; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 
	    
	    c2.insert(Mymap::value_type('d', 4)); 
	    c2.insert(Mymap::value_type('e', 5)); 
	    c2.insert(Mymap::value_type('f', 6)); 

	    c1.swap(c2); 
	    
	    Mymap::iterator it1 = c1.begin();
	    CPPUNIT_ASSERT( it1->first == 'd');
	    CPPUNIT_ASSERT( it1->second  == 4);
	    it1++;
	    CPPUNIT_ASSERT( it1->first == 'e');
	    CPPUNIT_ASSERT( it1->second  == 5);
	    it1++;
	    CPPUNIT_ASSERT( it1->first == 'f');
	    CPPUNIT_ASSERT( it1->second  == 6);
		}
		{
		typedef unordered_map<char, int> Mymap; 
		Mymap c1,c2; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 
	    
	    c2.insert(Mymap::value_type('d', 4)); 
	    c2.insert(Mymap::value_type('e', 5)); 
	    c2.insert(Mymap::value_type('f', 6)); 

	    swap(c1,c2); 
	    
	    Mymap::iterator it1 = c1.begin();
	    CPPUNIT_ASSERT( it1->first == 'd');
	    CPPUNIT_ASSERT( it1->second  == 4);
	    it1++;
	    CPPUNIT_ASSERT( it1->first == 'e');
	    CPPUNIT_ASSERT( it1->second  == 5);
	    it1++;
	    CPPUNIT_ASSERT( it1->first == 'f');
	    CPPUNIT_ASSERT( it1->second  == 6);
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_map_cov5()
	{
	__UHEAP_MARK;
		{
		typedef unordered_map<char, int> Mymap; 
		Mymap c1; 
		float a,b; 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 
	    a = (float)( (float)c1.size()/(float)c1.bucket_count() )  ;
	    b = (float)c1.load_factor();
	    CPPUNIT_ASSERT( a == b );
	    
	    c1.max_load_factor(0.10f); 
	    CPPUNIT_ASSERT( c1.max_load_factor() == 0.1f );
	    
	    CPPUNIT_ASSERT( c1.max_bucket_count() >= c1.bucket_count() );
	    c1.rehash(100); 
		}
		{
		typedef unordered_map<char, int> Mymap;
		Mymap c1;
		c1.max_size();
		}
		{
		typedef unordered_map<char, int> Mymap;	
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 

	    Mymap c3(c1.begin(),c1.end(),8,hash<char>(),equal_to<char>(),allocator<std::pair<const char, int> >()); 
	    Mymap::iterator it1 = c3.begin();
	    CPPUNIT_ASSERT( it1->first == 'a');
	    CPPUNIT_ASSERT( it1->second  == 1);
	    it1++;
	    CPPUNIT_ASSERT( it1->first == 'b');
	    CPPUNIT_ASSERT( it1->second  == 2);
	    it1++;
	    CPPUNIT_ASSERT( it1->first == 'c');
	    CPPUNIT_ASSERT( it1->second  == 3);

		}
		__UHEAP_MARKEND;
	}

void UnorderedTest::unordered_multimap_cov1()
	{
	__UHEAP_MARK;
		{
		typedef unordered_multimap<char, int> Mymap;
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 

	    Mymap::const_iterator it = c1.begin(); 
	    Mymap::iterator it2 = c1.begin(); 
	    
	    CPPUNIT_ASSERT( it->first == it2->first);
	    CPPUNIT_ASSERT( it->second  == it2->second);
	    
	    Mymap::const_local_iterator lit = c1.begin(c1.bucket('a')); 
	    CPPUNIT_ASSERT( lit->first == 'a');
	    CPPUNIT_ASSERT( lit->second  == 1);
		}
		{
		typedef unordered_multimap<char, int> Mymap; 
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 
	    c1.bucket_count();
	    
	    Mymap::size_type bs = c1.bucket('a'); 
	    CPPUNIT_ASSERT( c1.bucket_size(bs)  == 1);
	    
	    CPPUNIT_ASSERT( c1.count('A')  == 0);
	    CPPUNIT_ASSERT( c1.count('b')  == 1);
	    CPPUNIT_ASSERT( c1.count('C')  == 0);
		}
		{
		typedef unordered_multimap<char, int> Mymap; 
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 
	    
	    c1.clear(); 
	    CPPUNIT_ASSERT( c1.size() == 0);
	    CPPUNIT_ASSERT( c1.empty() == true);
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_multimap_cov2()
	{
	__UHEAP_MARK;
		{
		typedef unordered_multimap<char, int> Mymap; 
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	
	    Mymap::const_local_iterator lit = c1.end(c1.bucket('a')); 
		}
		{
		typedef unordered_multimap<char, int> Mymap; 
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 

	    std::pair<Mymap::iterator, Mymap::iterator> pair1 = c1.equal_range('b'); 
	    CPPUNIT_ASSERT( pair1.first->first == 'b');
	    CPPUNIT_ASSERT( pair1.first->second  == 2);
	    
	    std::pair<Mymap::iterator, Mymap::const_iterator> pair2 = c1.equal_range('c'); 
	    CPPUNIT_ASSERT( pair2.first->first == 'c');
	    CPPUNIT_ASSERT( pair2.first->second  == 3);
		}
		{
		typedef unordered_multimap<char, int> Mymap; 
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 
	
	    c1.erase(c1.begin()); 
	    Mymap::iterator it2 = c1.begin();
	    CPPUNIT_ASSERT( it2->first == 'b');
	    CPPUNIT_ASSERT( it2->second  == 2);
	    
	    CPPUNIT_ASSERT( c1.erase('b')  == 1);
	    
	    c1.erase(c1.begin(), c1.end()); 
	    CPPUNIT_ASSERT( c1.empty() );
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_multimap_cov3()
	{
	__UHEAP_MARK;
		{
		typedef unordered_map<char, int> Mymap;	
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 

	    Mymap::iterator it = c1.find('b'); 
	    CPPUNIT_ASSERT( it->first == 'b');
	    CPPUNIT_ASSERT( it->second  == 2);
	    
	    Mymap::const_iterator it1 = c1.find('c'); 
	    CPPUNIT_ASSERT( it1->first == 'c');
	    CPPUNIT_ASSERT( it1->second  == 3);
		}
		{
		typedef unordered_multimap<char, int> Mymap; 
		typedef allocator<std::pair<const char, int> > Myalloc;
		Mymap c1; 
		 
	    Mymap::allocator_type al = c1.get_allocator(); 
	    CPPUNIT_ASSERT ((al == Myalloc()) == true);
	    
	    Mymap::hasher hfn = c1.hash_function(); 
	    hfn('a');// returns the hash function object
		}
		{
		typedef unordered_multimap<char, int> Mymap; 
		Mymap c1,c2; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 
	    
	    Mymap::iterator it2;
	    
	    c2.insert(c1.begin(), c1.end()); 
	    it2 = c2.begin();
	    CPPUNIT_ASSERT( it2->first == 'a');
	    CPPUNIT_ASSERT( it2->second  == 1);
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_multimap_cov4()
	{
	__UHEAP_MARK;
		{
		typedef unordered_multimap<char, int> Mymap; 
		Mymap c1; 
		Mymap::key_equal cmpfn = c1.key_eq(); 
		CPPUNIT_ASSERT( cmpfn('a','a') == true);
		}
		{
		typedef unordered_multimap<char, int> Mymap; 
		Mymap c1,c2; 
			 
		c1.insert(Mymap::value_type('a', 1)); 
		c1.insert(Mymap::value_type('b', 2)); 
		c1.insert(Mymap::value_type('c', 3)); 
		    
		c2.insert(Mymap::value_type('d', 4)); 
		c2.insert(Mymap::value_type('e', 5)); 
		c2.insert(Mymap::value_type('f', 6)); 

		c1.swap(c2); 
		    
		Mymap::iterator it1 = c1.begin();
		CPPUNIT_ASSERT( it1->first == 'd');
		CPPUNIT_ASSERT( it1->second  == 4);
		it1++;
		CPPUNIT_ASSERT( it1->first == 'e');
		CPPUNIT_ASSERT( it1->second  == 5);
		it1++;
		CPPUNIT_ASSERT( it1->first == 'f');
		CPPUNIT_ASSERT( it1->second  == 6);
		}
		{
		typedef unordered_multimap<char, int> Mymap; 
		Mymap c1,c2; 
			 
		c1.insert(Mymap::value_type('a', 1)); 
		c1.insert(Mymap::value_type('b', 2)); 
		c1.insert(Mymap::value_type('c', 3)); 
		    
		c2.insert(Mymap::value_type('d', 4)); 
		c2.insert(Mymap::value_type('e', 5)); 
		c2.insert(Mymap::value_type('f', 6)); 

		swap(c1,c2); 
		    
		Mymap::iterator it1 = c1.begin();
		CPPUNIT_ASSERT( it1->first == 'd');
		CPPUNIT_ASSERT( it1->second  == 4);
		it1++;
		CPPUNIT_ASSERT( it1->first == 'e');
		CPPUNIT_ASSERT( it1->second  == 5);
		it1++;
		CPPUNIT_ASSERT( it1->first == 'f');
		CPPUNIT_ASSERT( it1->second  == 6);
		}
		{
		typedef unordered_multimap<char, int> Mymap; 
		Mymap c1; 
		float a,b; 
		
		c1.insert(Mymap::value_type('a', 1)); 
		c1.insert(Mymap::value_type('b', 2)); 
		c1.insert(Mymap::value_type('c', 3)); 
		    
		a = (float)( (float)c1.size()/(float)c1.bucket_count() )  ;
	    b = (float)c1.load_factor();
	    CPPUNIT_ASSERT( a == b );
	    
		c1.max_load_factor(0.10f); 
		CPPUNIT_ASSERT( c1.max_load_factor() == 0.1f );
		    
		CPPUNIT_ASSERT( c1.max_bucket_count() >= c1.bucket_count() );
		c1.rehash(100); 
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_multimap_cov5()
	{
	__UHEAP_MARK;
		{
		typedef unordered_multimap<char, int> Mymap;
		Mymap c1;
		c1.max_size();
		}		
		{
		typedef unordered_multimap<char, int> Mymap; 
		Mymap c1,c2; 
		 
		c1.insert(Mymap::value_type('a', 1)); 
		c1.insert(Mymap::value_type('b', 2)); 
		c1.insert(Mymap::value_type('c', 3)); 
		    
		c2 = c1;
		Mymap::iterator it2;
	    
	    it2 = c2.begin();
	    CPPUNIT_ASSERT( it2->first == 'a');
	    CPPUNIT_ASSERT( it2->second  == 1);
		}
		{
		typedef unordered_multimap<char, int> Mymap;	
		Mymap c1; 
		 
	    c1.insert(Mymap::value_type('a', 1)); 
	    c1.insert(Mymap::value_type('b', 2)); 
	    c1.insert(Mymap::value_type('c', 3)); 

	    Mymap c3(c1.begin(),c1.end(),8,hash<char>(),equal_to<char>(),allocator<std::pair<const char, int> >()); 
	    Mymap::iterator it1 = c3.begin();
	    CPPUNIT_ASSERT( it1->first == 'a');
	    CPPUNIT_ASSERT( it1->second  == 1);
	    it1++;
	    CPPUNIT_ASSERT( it1->first == 'b');
	    CPPUNIT_ASSERT( it1->second  == 2);
	    it1++;
	    CPPUNIT_ASSERT( it1->first == 'c');
	    CPPUNIT_ASSERT( it1->second  == 3);

		}
		__UHEAP_MARKEND;
}
void UnorderedTest::unordered_multiset_cov1()
	{
	__UHEAP_MARK;
		{
		#if defined (STLPORT) 
		unordered_multiset<int> int_uset;

		CPPUNIT_ASSERT( int_uset.bucket_count() < int_uset.max_bucket_count() );

		int i;
		size_t nbBuckets = int_uset.bucket_count();
		size_t nbInserts = int_uset.bucket_count() - 1;
		for (i = 0; (size_t)i < nbInserts; ++i) {
			int_uset.insert(i);
			}
		CPPUNIT_ASSERT( nbBuckets == int_uset.bucket_count() );

		size_t bucketSizes = 0;
		for (i = 0; (size_t)i < nbBuckets; ++i) {
			bucketSizes += int_uset.bucket_size(i);
			}
		CPPUNIT_ASSERT( bucketSizes == int_uset.size() );
		#endif
		}
		{
		#if defined (STLPORT)
		  unordered_multiset<int> int_uset;

		  CPPUNIT_ASSERT( int_uset.max_load_factor() == 1.0f );
		  CPPUNIT_ASSERT( int_uset.load_factor() == 0.0f );

		  size_t nbInserts = int_uset.bucket_count() - 1;
		  for (int i = 0; (size_t)i < nbInserts; ++i) {
		    int_uset.insert(i);
		  }
		  CPPUNIT_ASSERT( int_uset.size() == nbInserts );

		  int_uset.max_load_factor(0.5f);
		  int_uset.rehash(0);
		  CPPUNIT_ASSERT( int_uset.load_factor() < int_uset.max_load_factor() );

		  size_t bucketsHint = int_uset.bucket_count() + 1;
		  int_uset.rehash(bucketsHint);
		  CPPUNIT_ASSERT( int_uset.bucket_count() >= bucketsHint );

		  CPPUNIT_ASSERT( int_uset.key_eq()(10, 10) );
		  CPPUNIT_ASSERT( int_uset.hash_function()(10) == 10 );
		#endif			
		}
		{
		typedef unordered_multiset<char> Myset; 
		Myset c1; 
		 
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c'); 
	    Myset::const_iterator it = c1.begin(); 
		CPPUNIT_ASSERT(*it == 'a');
		
		Myset::const_iterator it1 = c1.end();
		
		c1.max_size();
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_multiset_cov2()
	{
	__UHEAP_MARK;
		{
		typedef unordered_multiset<char> Myset; 
		Myset c1; 
		 
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c'); 
	    
	    c1.clear(); 
		CPPUNIT_ASSERT(c1.size() == 0 );
		}
		{
		typedef unordered_multiset<char> Myset; 
		Myset c1; 
		 
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c');
		CPPUNIT_ASSERT( c1.erase('b')  == 1);	
		}
		{
		typedef unordered_multimap<char, int> Mymap; 
		typedef allocator<std::pair<const char, int> > Myalloc;
		Mymap c1; 
		 
	    Mymap::allocator_type al = c1.get_allocator(); 
	    CPPUNIT_ASSERT ((al == Myalloc()) == true);
		}
		{
		typedef unordered_multiset<char> Myset; 
		Myset c1,c2; 
		 
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c'); 
	    Myset::iterator it2 = c1.insert(c1.begin(), 'd'); 
	    CPPUNIT_ASSERT(c1.size() == 4 );
	    
	    c2.insert(c1.begin(),c1.end());
	    CPPUNIT_ASSERT(c2.size() == 4 );
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_multiset_cov3()
	{
	__UHEAP_MARK;
		{
		typedef unordered_multiset<char> Myset;	
		Myset c1,c2; 
		
		
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c'); 

	    c2.insert('d'); 
	    c2.insert('e'); 
	    c2.insert('f'); 
	 
	    c1.swap(c2); 
	    Myset::const_iterator it1 = c1.begin(); 
		Myset::const_iterator it2 = c2.begin();
	    CPPUNIT_ASSERT( *it1 == 'd' );
	    CPPUNIT_ASSERT( *it2 == 'a' );
	    it1++;it2++;
	    CPPUNIT_ASSERT( *it1 == 'e' );
	    CPPUNIT_ASSERT( *it2 == 'b' );
	    it1++;it2++;
	    CPPUNIT_ASSERT( *it1 == 'f' );
	    CPPUNIT_ASSERT( *it2 == 'c' );
		}
		{
		typedef unordered_multiset<char> Myset;	
		Myset c1,c2; 
		
		
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c'); 

	    c2.insert('d'); 
	    c2.insert('e'); 
	    c2.insert('f'); 
	 
	    swap(c1,c2); 
	    Myset::const_iterator it1 = c1.begin(); 
		Myset::const_iterator it2 = c2.begin();
	    CPPUNIT_ASSERT( *it1 == 'd' );
	    CPPUNIT_ASSERT( *it2 == 'a' );
	    it1++;it2++;
	    CPPUNIT_ASSERT( *it1 == 'e' );
	    CPPUNIT_ASSERT( *it2 == 'b' );
	    it1++;it2++;
	    CPPUNIT_ASSERT( *it1 == 'f' );
	    CPPUNIT_ASSERT( *it2 == 'c' );
		}
		__UHEAP_MARKEND;
	}
void UnorderedTest::unordered_multiset_cov4()
	{
	__UHEAP_MARK;
		{
		typedef unordered_multiset<char> Myset;	
		Myset c1,c2; 	
		c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c'); 
	    
	    c2 = c1;
	    CPPUNIT_ASSERT(c2.size() == 3 );
		}
		{
		typedef unordered_multiset<char> Myset; 
		Myset c1; 
		 
	    c1.insert('a'); 
	    c1.insert('b'); 
	    c1.insert('c'); 
	    Myset c3(c1.begin(),c1.end(),8,hash<char>(),equal_to<char>(),allocator<pair<const char, int> >()); 
	    
	    Myset::iterator it1 = c3.begin();
	    CPPUNIT_ASSERT( *it1 == 'a');
	    it1++;
	    CPPUNIT_ASSERT( *it1 == 'b');
	    it1++;
	    CPPUNIT_ASSERT( *it1 == 'c');
		}
		__UHEAP_MARKEND;
}
