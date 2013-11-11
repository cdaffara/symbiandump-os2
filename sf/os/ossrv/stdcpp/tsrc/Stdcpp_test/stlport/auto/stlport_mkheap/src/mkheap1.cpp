 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>
#include <functional>

#ifdef MAIN 
#define mkheap1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int mkheap1_test(int, char**)
{
  cout<<"Results of mkheap1_test:"<<endl;
int numbers[6] = { 5, 10, 4, 13, 11, 19 };
int array[6];
int failures = 0,a = 0;
  make_heap(numbers, numbers + 6, greater<int>());
  for(int i = 6; i >= 1; i--)
  {
    cout << numbers[0] << endl;
    array[a++] = numbers[0];
    pop_heap(numbers, numbers + i, greater<int>());
  }
   if(array[0] != 4)
  failures++;
  if(array[1] != 5)
  failures++;
  if(array[2] != 10)
  failures++;
  if(array[3] != 11)
  failures++;
  if(array[4] != 13)
  failures++;
  if(array[5] != 19)
  failures++;

  return failures;
}
