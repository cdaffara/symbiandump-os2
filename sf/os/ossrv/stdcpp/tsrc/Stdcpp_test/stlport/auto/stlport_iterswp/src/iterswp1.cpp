 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iterator>

#ifdef MAIN 
#define iterswp1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int iterswp1_test(int, char**)
{

  cout<<"Results of iterswp1_test:"<<endl;
  int failures=0;
  vector <int> v1(6);
  iota(v1.begin(), v1.end(), 0);
  iter_swap(v1.begin(), v1.begin() + 3);
  ostream_iterator <int> iter(cout, " ");
  
  copy(v1.begin(), v1.end(), iter);
  
  cout << endl;
  if(v1[0] != 3)
  failures++;
   if(v1[1] != 1)
  failures++;
   if(v1[2] != 2)
  failures++;
   if(v1[3] !=0 )
  failures++;
   if(v1[4] != 4)
  failures++;
   if(v1[5] != 5)
  failures++;
  return failures;
}
