
 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <vector>
#include <algorithm>
#include <iostream>


#ifdef MAIN 
#define advance_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int advance_test(int, char**)
{
 	
  cout<<"Results of advance_test:"<<endl;
  int failures = 0;
  
  typedef vector <int> IntVector;
  IntVector v(10);
  for(int i = 0; i < v.size(); i++)
    v[i] = i;
  
  IntVector::iterator location = v.begin();
  cout << "At Beginning: " << *location << endl;
  if(*location != 0)
  	failures++;  
  
  advance(location, 5);
  cout << "At Beginning + 5: " << *location << endl;
  if(*location != 5)
  	failures++;  
   
  if (!failures)
  	   return 0;
  else
  return 1;
}
