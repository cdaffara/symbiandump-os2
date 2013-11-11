  
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define inplmrg1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int inplmrg1_test(int, char**)
{
	  cout<<"Results of inplmrg1_test:"<<endl;
int numbers[6] = { 1, 10, 42, 3, 16, 32 };
int failures = 0;
  int i;
  for(i = 0; i < 6; i++)
    cout << numbers[i] << ' ';
  cout << endl;
  inplace_merge(numbers, numbers + 3, numbers + 6);
  for(i = 0; i < 6; i++)
    cout << numbers[i] << ' ';
  cout << endl;
  if(numbers[0] != 1)
  failures++;
  if(numbers[1] != 3)
  failures++;
  if(numbers[2] != 10)
  failures++;
  if(numbers[3] != 16)
  failures++;
  if(numbers[4] != 32)
  failures++;
  if(numbers[5] != 42)
  failures++;
  
  return failures;
}
