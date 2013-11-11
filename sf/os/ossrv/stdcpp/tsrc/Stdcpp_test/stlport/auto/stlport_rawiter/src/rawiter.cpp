 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <algorithm>
#include <iterator>
#include <memory>

#ifdef MAIN 
#define rawiter_test main
#endif

#include "rawiter.hpp"

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

int rawiter_test(int, char**)
{
  cout<<"Results of rawiter_test:"<<endl;

  allocator<X> a;
  typedef X* x_pointer;
  x_pointer save_p, p;
  int array[5];
  int failures = 0;
  p = a.allocate(5); 
  save_p=p;
  raw_storage_iterator<X*, X> r(p);
  int i;
  for(i = 0; i < 5; i++)
    *r++ = X(i);
  for(i = 0; i < 5; i++)
  {  	
    cout << *p << endl;
    array[i] = *p;
    *p++;
  }
# ifdef __STLPORT_VERSION
  a.deallocate(save_p,5);
# else
  a.deallocate(save_p);
# endif

if(array[0] != 0 )
failures++;
if(array[1] != 1)
failures++;
if(array[2] != 2)
failures++;
if(array[3] != 3)
failures++;
if(array[4] != 4)
failures++;

  return failures;
}
