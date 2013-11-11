 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define replace0_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int replace0_test(int, char**)
{
 
  int failures=0;
  cout<<"Results of replace0_test:"<<endl;
  int numbers[6] = { 0, 1, 2, 0, 1, 2 };

  replace(numbers, numbers + 6, 2, 42);
  for(int i = 0; i < 6; i++)
    cout << numbers[i] << ' ';
  cout << endl;
  
  if(0!=numbers[0])
      failures++;
  else if(1!=numbers[1])
      failures++;
  else if(42 !=numbers[2])
      failures++;
  else if(0!=numbers[3])
      failures++;
  else if(1!=numbers[4])
      failures++;
  else if(42 !=numbers[5])
      failures++;
          
  if(failures)
     return 1;
  else
     return 0;
}

