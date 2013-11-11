 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define stblsrt1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int stblsrt1_test(int, char**)
{
  int failures=0;
  cout<<"Results of stblsrt1_test:"<<endl;
  int array[6] = { 1, 50, -10, 11, 42, 19 };

  stable_sort(array, array + 6);
  for(int i = 0; i < 6; i++)
    cout << array[i] << ' ';
  cout << endl;
  
  if(-10!=array[0])
    failures++;
  else if(1!=array[1])
   failures++;
  else if(11!=array[2])
   failures++;   
  else if(19!=array[3])
   failures++;
  else if(42!=array[4])
   failures++;   
  else if(50!=array[5])
   failures++;
  
 if(failures)
   return 1;
 else 
   return 0;  
}


