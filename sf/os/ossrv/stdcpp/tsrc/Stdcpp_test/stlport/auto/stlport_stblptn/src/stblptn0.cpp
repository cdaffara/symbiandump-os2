 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define stblptn0_test main
#endif
static bool less_10(int a_)
{
  return a_ < 10 ? 1 : 0;
}

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int stblptn0_test(int, char**)
{
  
  int failures=0;
  cout<<"Results of stblptn0_test:"<<endl;

int numbers[6] = { 10, 5, 11, 20, 6, -2 };

  stable_partition((int*)numbers, (int*)numbers + 6, less_10);
  for(int i = 0; i < 6; i++)
    cout << numbers[i] << ' ';
  cout << endl;
  if(5!=numbers[0])
    failures++;
  else if(6!=numbers[1])
    failures++;
  else if(-2!=numbers[2])
    failures++;
  else if(10!=numbers[3])
    failures++;
  else if(11!=numbers[4])
    failures++;
  else if(20!=numbers[5])
    failures++;
  cout.flush();
 
  if(failures)
   return 1;
  else 
   return 0; 
}

