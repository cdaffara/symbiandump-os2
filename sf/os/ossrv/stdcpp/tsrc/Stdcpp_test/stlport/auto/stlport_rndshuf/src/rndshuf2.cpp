 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.
#include <iterator>

#include <algorithm>
#include <vector>
#include <iostream>
#include "rand.h"
#include <iterator>
#include <functional>
#include <numeric>


#ifdef MAIN 
#define rndshuf2_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

int rndshuf2_test(int, char**)
{
  cout<<"Results of rndshuf2_test:"<<endl;
  vector <int> v1(10);
  iota(v1.begin(), v1.end(), 0);
  int failures = 0;
  ostream_iterator <int> iter(cout, " ");
 
  copy(v1.begin(), v1.end(), iter);
 
  cout << endl;
 
  MyRandomGenerator r;
  for(int i = 0; i < 3; i++)
  {
    random_shuffle(v1.begin(), v1.end(), r);
    copy(v1.begin(), v1.end(), iter);
  
    cout << endl;
  
  }
if (v1[0] != 7 && v1[1] != 3 && v1[2] != 1 && v1[3] != 9 && v1[4] != 5 && v1[5] != 2 && v1[6] != 6 && v1[7] != 0 && v1[8] != 8 && v1[9] != 4 )
  failures++;
if (v1[0] != 6 && v1[1] != 9 && v1[2] != 2 && v1[3] != 3 && v1[4] != 7 && v1[5] != 5 && v1[6] != 1 && v1[7] != 8 && v1[8] != 0 && v1[9] != 4 )
  failures++;
if (v1[0] != 8 && v1[1] != 2 && v1[2] != 6 && v1[3] != 5 && v1[4] != 7 && v1[5] != 3 && v1[6] != 0 && v1[7] != 1 && v1[8] != 9 && v1[9] != 4 )
  failures++;

return failures;
}
