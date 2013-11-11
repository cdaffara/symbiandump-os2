
 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <vector>
#include <algorithm>

#ifdef MAIN 
#define vec4_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int vec4_test(int, char**)
{
  int failures=0;
  cout<<"Results of vec4_test:"<<endl;
  vector<int> v(4);
  v[0] = 1;
  v[1] = 4;
  v[2] = 9;
  v[3] = 16;
  cout << "front = " << v.front() << endl;
  
  if(1!=v.front() )
     failures++;
  cout << "back = " << v.back() << ", size = " << v.size() << endl;
  
  if(16!=v.back())
     failures++;
  else if(4!=v.size())
    failures++;   
  
  v.push_back(25);
  cout << "back = " << v.back() << ", size = " << v.size() << endl;
  
  if(25!=v.back())
     failures++;
  else if(5!=v.size())
    failures++;  
  
  v.pop_back();
  cout << "back = " << v.back() << ", size = " << v.size() << endl;
  if(16!=v.back())
     failures++;
  else if(4!=v.size())
    failures++;  
  if(failures)
    return 1;
  else   
    return 0;
}

/*Results of vec4_test:
front = 1
back = 16, size = 4
back = 25, size = 5
back = 16, size = 4
*/
