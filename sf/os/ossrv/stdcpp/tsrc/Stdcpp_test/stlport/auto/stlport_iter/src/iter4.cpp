 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <vector>
#include <algorithm>

#ifdef MAIN 
#define iter4_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int iter4_test(int, char**)
{
  cout<<"Results of iter4_test:"<<endl;
  int failures = 0;
    int array[2];
   vector<int> v; // Empty vector of integers.
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  // Position immediately after last item.
  std::vector<int>::iterator i = v.end(); 
  // Move back one and then access.
   i -= 1;
  cout << "last element is " << *i << endl; 
  array[0] = *i;
  i -= 2; // Jump back two items.
  cout << "first element is " << *i << endl;
  array[1] = *i;
  
  if(array[0] != 3)
  failures++;
  if(array[1] != 1)
  failures++;
  return failures;
}
