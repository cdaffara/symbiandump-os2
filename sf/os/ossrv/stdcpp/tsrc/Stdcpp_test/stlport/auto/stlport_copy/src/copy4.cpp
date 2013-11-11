
 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>

#ifdef MAIN
#define copy4_test main
#endif


#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int copy4_test(int, char**)
{
  cout<<"Results of copy4_test:"<<endl;
  typedef vector<int> IVec;
  vector<int> v1(10);
  for(int loc = 0; loc < v1.size(); loc++)
    v1[loc] = loc;
  vector<int> v2;
  insert_iterator<IVec> i(v2, v2.begin());
  copy(v1.begin(), v1.end(), i);
  
  int failures = 0;
  for (int i = 0; i < v1.size(); i++)
  {
	if(v2[i] != i)
		failures++;
  }
  ostream_iterator<int> outIter(cout, " ");
  copy(v2.begin(), v2.end(), outIter);
  cout << endl;

  if(!failures)
    return 0;
  else
    return 1;

}
