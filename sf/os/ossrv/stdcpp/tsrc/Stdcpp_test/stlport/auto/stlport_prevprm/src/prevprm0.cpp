  
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define prevprm0_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int prevprm0_test(int, char**)
{
  int failures=0;
  int arr[3];
  cout<<"Results of prevprm0_test:"<<endl;
  int v1[3] = { 0, 1, 2 };

  prev_permutation(v1, v1 + 3);
  for(int i = 0; i < 3; i++)
    {
      arr[i]=v1[i];
      cout << v1[i] << ' ';
    }
  cout << endl;
   
   if(2 !=arr[0])
    failures++;
   else if(1 !=arr[1])
    failures++;
   else if(0!=arr[2])
     failures++;  
  
  if(failures)
    return 1;
  else
    return 0;  
}


