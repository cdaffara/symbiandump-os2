
 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>


#ifdef MAIN
#define alg1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int alg1_test(int, char**)
{
  
  cout<<"Results of alg1_test:"<<endl;
  int failures = 0;

  int i = min(4, 7);
  cout << "min(4, 7) = " << i << endl;
  if (i != 4)
  	failures++;
  char c = max('a', 'z');
  cout << "max('a', 'z') = " << c << endl;
  if (c != 'z')
  	failures++;

  if (!failures)
    return 0;
  else 
  return 1;
}
