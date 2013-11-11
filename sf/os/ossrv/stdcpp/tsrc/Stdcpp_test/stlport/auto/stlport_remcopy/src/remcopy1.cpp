 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define remcopy1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int remcopy1_test(int, char**)
{
  
  int failures=0;
  cout<<"Results of remcopy1_test:"<<endl;
  int numbers[6] = { 1, 2, 3, 1, 2, 3 };
  int result[6] = { 0, 0, 0, 0, 0, 0 };

  remove_copy((int*)numbers, (int*)numbers + 6, (int*)result, 2);
  for(int i = 0; i < 6; i++)
    cout << result[i] << ' ';
  cout << endl;
 
  if(1!=result[0])
     failures++;
  else if(3!=result[1])
     failures++; 
  else if(1!=result[2])
     failures++;        
  else if(3!=result[3])
     failures++; 
  else if(0!=result[4])
     failures++; 
  else if(0!=result[5])
     failures++;       
  
  if(failures)
     return 1;
  else 
     return 0;   
}

