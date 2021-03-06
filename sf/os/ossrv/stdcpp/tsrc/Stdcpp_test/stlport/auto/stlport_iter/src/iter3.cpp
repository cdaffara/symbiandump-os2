 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <vector>
#include <algorithm>

#ifdef MAIN 
#define iter3_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

int iter3_test(int, char**)
{
  cout<<"Results of iter3_test:"<<endl;
  typedef vector<const char*> Vec;
    const char* array[2];
  int j =0 ;
  int failures = 0;
  Vec v; // Vector of character strings.
  v.push_back((char*) "zippy"); // First element.
  v.push_back((char*) "motorboy"); // Second element.
  Vec::reverse_iterator it;
  for(it = v.rbegin(); it != v.rend(); it++)
  {
  	    cout << *it << endl; // Display item.
  	    array[j++] = *it;
  }
      
  
  if(array[0] != "motorboy")
  failures++;
   if(array[1] != "zippy")
  failures++;
  
  return failures;
}
