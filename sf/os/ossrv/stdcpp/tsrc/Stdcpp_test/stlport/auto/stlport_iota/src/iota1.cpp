 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <numeric>
#include <iostream>

#ifdef MAIN 
#define iota1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int iota1_test(int, char**)
{
  cout<<"Results of iota1_test:"<<endl;
  int numbers[10];
  int failures = 0;
  iota(numbers, numbers + 10, 42);
  for(int i = 0; i < 10; i++)
    cout << numbers[i] << ' ';
  cout << endl;
  if(numbers[0]!=42)
  failures++;
  if(numbers[1]!=43)
  failures++;
  if(numbers[2]!=44)
  failures++;
  if(numbers[3]!=45)
  failures++;
  if(numbers[4]!=46)
  failures++;
  if(numbers[5]!=47)
  failures++;
  if(numbers[6]!=48)
  failures++;
  if(numbers[7]!=49)
  failures++;
  if(numbers[8]!=50)
  failures++;
  if(numbers[9]!=51)
  failures++;
 
  
 return failures;
}
