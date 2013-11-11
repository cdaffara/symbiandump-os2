
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define uniqcpy1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int uniqcpy1_test(int, char**)
{
  int failures=0;
  cout<<"Results of uniqcpy1_test:"<<endl;
int numbers[8] = { 0, 1, 1, 2, 2, 2, 3, 4 };
int result[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  unique_copy((int*)numbers, (int*)numbers + 8, (int*)result);
  for(int i = 0; i < 8; i++)
    cout << result[i] << ' ';
  cout << endl;
  
  if(0!=result[0])
    failures++;
  else if(1!=result[1])
    failures++;
  else if(2!=result[2])
    failures++;    
  else if(3!=result[3])
    failures++;
  else if(4!=result[4])
    failures++;    
  else if(0!=result[5])
    failures++;
  else if(0!=result[6])
    failures++;    
  else if(0!=result[7])
    failures++;
     

  return 0;
}

