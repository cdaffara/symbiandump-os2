 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.
#include <vector>
#include <algorithm>
#include <iostream>


#ifdef MAIN
#define alg4_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int alg4_test(int, char**)
{
  
  cout<<"Results of alg4_test:"<<endl;
  // for comparison of results
  int res_arr[4] = {1962, 1992, 1999, 2001};
  int failures =0;

  vector<int> years;
  years.push_back(1962);
  years.push_back(1992);
  years.push_back(2001);
  years.push_back(1999);
  sort(years.begin(), years.end());
  vector<int>::const_iterator i;
  int j;
  for(i = years.begin(), j =0; i != years.end(); i++, j++)
  {
  	cout << *i << endl;
  	if( *i != res_arr[j])
  		failures++;
  }

  if (!failures)
  return 0;
  else 
  return 1;
}
