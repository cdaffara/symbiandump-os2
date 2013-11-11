// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <vector>
#include <algorithm>

#ifdef MAIN 
#define vec8_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int vec8_test(int, char**)
{
  int failures=0;
  cout<<"Results of vec8_test:"<<endl;
  vector<int> v;
  cout << "capacity = " << v.capacity() << endl;
  
  if(0!=v.capacity())
     failures++;
  
  v.push_back(42);
  cout << "capacity = " << v.capacity() << endl;
  
  if(1!=v.capacity())
     failures++;
  v.reserve(5000);
  cout << "capacity = " << v.capacity() << endl;
  
  if(5000!=v.capacity())
     failures++;
  
  if(failures)
    return 1;
  else  
    return 0;
}
/*Results of vec8_test:
capacity = 0
capacity = 1
capacity = 5000
*/
