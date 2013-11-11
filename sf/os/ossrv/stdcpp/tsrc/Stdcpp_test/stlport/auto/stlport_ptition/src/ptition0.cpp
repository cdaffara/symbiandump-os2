  
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define ptition0_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
static int less_10(int a_)
{
  return a_ < 10 ? 1 : 0;
}

int ptition0_test(int, char**)
{
  int failures=0;
  cout<<"Results of ptition0_test:"<<endl;
 int numbers[6] = { 6, 12, 3, 10, 1, 20 };

  partition((int*)numbers, (int*)numbers + 6, less_10);
  for(int i = 0; i < 6; i++)
    cout << numbers[i] << ' ';
  cout << endl;
  
  if(6 !=numbers[0])
    failures++;
  else if(1 !=numbers[1])
    failures++;
  else if(3!=numbers[2])
    failures++;
  else if(10!=numbers[3])
    failures++;
    else if(12!=numbers[4])
    failures++;
  else if(20!=numbers[5])
    failures++;
           
  
  if(failures)
    return 1;
  else 
    return 0;  
}

