
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>
#include <iterator>
#include <functional>

#ifdef MAIN 
#define sort2_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int sort2_test(int, char**)
{
  int failures=0;
  cout<<"Results of sort2_test:"<<endl;
int array[] = { 1, 50, -10, 11, 42, 19 };

  int count = sizeof(array) / sizeof(array[0]);
  ostream_iterator <int> iter(cout, " ");
  cout << "before: ";
  copy(array, array + count, iter);
  cout << "\nafter: ";
    if(1!=array[0])
      failures++;
    else if(50 !=array[1])
      failures++;
    else if(-10!=array[2])
      failures++;
    else if(11 !=array[3])
      failures++;
    else if(42!=array[4])
      failures++;
    else if(19 !=array[5])
      failures++;
              
  sort(array, array + count, greater<int>());
     if(50!=array[0])
      failures++;
    else if(42 !=array[1])
      failures++;
    else if(19!=array[2])
      failures++;
    else if(11 !=array[3])
      failures++;
    else if(1!=array[4])
      failures++;
    else if(-10!=array[5])
      failures++;
 
  copy(array, array + count, iter);
  cout << endl;
   
   if(failures)
     return 1;
   else   
    return 0;
  
}

