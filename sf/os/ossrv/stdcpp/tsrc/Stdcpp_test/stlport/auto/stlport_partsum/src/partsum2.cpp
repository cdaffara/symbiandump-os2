  
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.
#include <iterator>

#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <functional>
#include <numeric>

#ifdef MAIN 
#define partsum2_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int partsum2_test(int, char**)
{

  int failures=0;
  cout<<"Results of partsum2_test:"<<endl;
  vector <int> v1(5);
  iota(v1.begin(), v1.end(), 1);
  vector <int> v2(v1.size());
  partial_sum(v1.begin(), v1.end(), v2.begin(), multiplies<int>());
  ostream_iterator <int> iter(cout, " ");
  copy(v1.begin(), v1.end(), iter);
  cout << endl;
  
  if(1 !=v1[0])
    failures++;
  else if(2 !=v1[1])
    failures++;
  else if(2 !=v1[2])
    failures++;    
  else if(2 !=v1[3])
    failures++;    
  else if(2 !=v1[4])
    failures++; 
  
  copy(v2.begin(), v2.end(), iter);
  cout << endl;
  
   if(1 !=v2[0])
    failures++;
  else if(2 !=v2[1])
    failures++;
  else if(6 !=v2[2])
    failures++;    
  else if(24 !=v2[3])
    failures++;    
  else if(120!=v2[4])
    failures++; 
  
  return 0;
}

