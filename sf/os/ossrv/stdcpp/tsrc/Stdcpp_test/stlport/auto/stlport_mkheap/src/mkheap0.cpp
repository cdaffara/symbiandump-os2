 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define mkheap0_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int mkheap0_test(int, char**)
{
  cout<<"Results of mkheap0_test:"<<endl;
int numbers[6] = { 5, 10, 4, 13, 11, 19 };
int array[6];
int failures = 0,a = 0;
  make_heap(numbers, numbers + 6);
  for(int i = 6; i >= 1; i--)
  {
    cout << numbers[0] << endl;
    array[a++] = numbers[0];
    pop_heap(numbers, numbers + i);
  }
  if(array[0] != 19)
  failures++;
  if(array[1] != 13)
  failures++;
  if(array[2] != 11)
  failures++;
  if(array[3] != 10)
  failures++;
  if(array[4] != 5)
  failures++;
  if(array[5] != 4)
  failures++;


  return failures;
}
