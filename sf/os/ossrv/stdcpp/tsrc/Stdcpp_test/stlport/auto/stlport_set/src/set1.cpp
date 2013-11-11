 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <algorithm>
#include <set>

#ifdef MAIN 
#define set1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int set1_test(int, char**)
{
  int failures=0;
  cout<<"Results of set1_test:"<<endl;
  set<int, less<int> > s;
  cout << "count(42) = " << s.count(42) << endl;
   
  if(0!=s.count(42))
    failures++;
  
  s.insert(42);
  cout << "count(42) = " << s.count(42) << endl;
  
  if(1!=s.count(42))
    failures++;
  
  s.insert(42);
  cout << "count(42) = " << s.count(42) << endl;
  
  if(1!=s.count(42))
    failures++;
  
  int count = s.erase(42);
  cout << count << " elements erased" << endl;
  
  if(1!=count)
    failures++;
    
  if(failures)
    return 1;
  else 
    return 0;
}
