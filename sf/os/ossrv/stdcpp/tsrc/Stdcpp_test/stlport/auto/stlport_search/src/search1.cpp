 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.
#include <iterator>

#include <algorithm>
#include <vector>
#include <iostream>
#include <iterator>
// #include <functional>
#include <numeric>

#ifdef MAIN 
#define search1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int search1_test(int, char**)
{
  int failures=0;
  cout<<"Results of search1_test:"<<endl;
  typedef vector <int> IntVec;
  IntVec v1(10);
  iota(v1.begin(), v1.end(), 0);
  IntVec v2(3);
  iota(v2.begin(), v2.end(), 50);
  ostream_iterator <int> iter(cout, " ");

  cout << "v1: ";
  copy(v1.begin(), v1.end(), iter);
  cout << endl;
   
  if(0!=v1[0])
   failures++;             
  else if(1!=v1[1])
    failures++;
  else if(2!=v1[2])
    failures++;
  else if(3!=v1[3])
    failures++;                                         
  else if(4!=v1[4])
    failures++;
  else if(5!=v1[5])
    failures++;
  else if(6!=v1[6])
    failures++;                                         
  else if(7!=v1[7])
    failures++;
  else if(8!=v1[8])
    failures++;
  else if(9!=v1[9])
    failures++;                                         
  
  cout << "v2: ";
  copy(v2.begin(), v2.end(), iter);
  cout << endl;

  if(50!=v2[0])
   failures++;             
  else if(51!=v2[1])
    failures++;
  else if(52!=v2[2])
    failures++;
   
  IntVec::iterator location;
  location = search(v1.begin(), v1.end(), v2.begin(), v2.end());

  if(location == v1.end())
    cout << "v2 not contained in v1" << endl;
  else
    cout << "Found v2 in v1 at offset: " << location - v1.begin() << endl;
   
  if(location !=v1.end())
     failures++; 
  
  iota(v2.begin(), v2.end(), 4);
  cout << "v1: ";
  copy(v1.begin(), v1.end(), iter);
  cout << endl;
  
  if(0!=v1[0])
   failures++;             
  else if(1!=v1[1])
    failures++;
  else if(2!=v1[2])
    failures++;
  else if(3!=v1[3])
    failures++;                                         
  else if(4!=v1[4])
    failures++;
  else if(5!=v1[5])
    failures++;
  else if(6!=v1[6])
    failures++;                                         
  else if(7!=v1[7])
    failures++;
  else if(8!=v1[8])
    failures++;
  else if(9!=v1[9])
    failures++;    
  
  cout << "v2: ";
  copy(v2.begin(), v2.end(), iter);
  cout << endl;
    
    if(4!=v2[0])
   failures++;             
  else if(5!=v2[1])
    failures++;
  else if(6!=v2[2])
    failures++; 
   
  location = search(v1.begin(), v1.end(), v2.begin(), v2.end());

  if(location == v1.end())
    cout << "v2 not contained in v1" << endl;
  else
    cout << "Found v2 in v1 at offset: " << location - v1.begin() << endl;
   
   if(location ==v1.end())
      failures++;
    
   if(failures)
     return 1;
   else 
      return 0;
}

