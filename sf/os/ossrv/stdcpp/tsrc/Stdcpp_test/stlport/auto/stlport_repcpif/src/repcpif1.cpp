 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <vector>
#include <iostream>
#include <iterator>
#include <functional>

#ifdef MAIN 
#define repcpif1_test main
#endif
static bool odd(int a_)
{
  return a_ % 2;
}

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int repcpif1_test(int, char**)
{
  int failures=0;
  cout<<"Results of repcpif1_test:"<<endl;

  vector <int> v1(10);
  for(int i = 0; i < v1.size(); i++)
    v1[i] = i % 5;
  ostream_iterator <int> iter(cout, " ");
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
  else if(0!=v1[5])
    failures++;
  else if(1!=v1[6])
    failures++;
  else if(2!=v1[7])
    failures++;
  else if(3!=v1[8])
    failures++;
  else if(4!=v1[9])
    failures++;
   
    
  vector <int> v2(v1.size());
  replace_copy_if(v1.begin(), v1.end(), v2.begin(), odd, 42);
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
  else if(0!=v1[5])
    failures++;
  else if(1!=v1[6])
    failures++;
  else if(2!=v1[7])
    failures++;
  else if(3!=v1[8])
    failures++;
  else if(4!=v1[9])
    failures++;
 
  copy(v2.begin(), v2.end(), iter);
  cout << endl;
 
 if(0!=v2[0])
    failures++;
 else if(42!=v2[1])
    failures++;
 else if(2!=v2[2])
    failures++;
 else if(42!=v2[3])
    failures++;
 else if(4!=v2[4])
    failures++;
 else if(0!=v2[5])
    failures++;
 else if(42!=v2[6])
    failures++;
 else if(2!=v2[7])
    failures++;
 else if(42!=v2[8])
    failures++;
 else if(4!=v2[9])
    failures++; 
  
  
  if(failures)
     return 1;
  else 
    return 0;   
}
