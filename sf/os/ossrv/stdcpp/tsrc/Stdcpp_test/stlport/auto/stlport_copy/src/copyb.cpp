
 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>

#ifdef MAIN
#define copyb_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int copyb_test(int, char**)
{
  cout<<"Results of copyb_test:"<<endl;

  int failures = 0;
  vector <int> v1(10);
  for(int i = 0; i < v1.size(); i++)
    v1[i] = i;
  vector <int> v2(v1.size());
  copy_backward(v1.begin(), v1.end(), v2.end());
  for (int i = 0; i < v2.size(); i++)
  {
	if(v2[i] != i)
		failures++;
  }
  ostream_iterator<int> iter(cout, " ");
  copy(v2.begin(), v2.end(), iter);
  cout << endl;
  
  if(!failures)
    return 0;
  else
    return 1;
}
