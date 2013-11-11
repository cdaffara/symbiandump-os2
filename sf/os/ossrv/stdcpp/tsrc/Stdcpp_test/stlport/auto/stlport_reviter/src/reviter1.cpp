 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <vector>
#include <algorithm>

#ifdef MAIN 
#define reviter1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int reviter1_test(int, char**)
{
  int failures=0,i=0;
  cout<<"Results of reviter1_test:"<<endl;
  int array[] = { 1, 5, 2, 3 };
  int arr[4];
  vector<int> v(array, array + 4);
  typedef std::vector<int>::reverse_iterator reviter;
  reviter r(v.end());
  while(r!=v.rend())
  {
    arr[i]=*r;
    cout << *r++ << endl;
    i++;
   }
   
   if(3!=arr[0])
     failures++;
   else if(2!=arr[1])
     failures++;
   else if(5!=arr[2])
     failures++;
   else if(1!=arr[3])
     failures++;
     
     if(failures)
        return 1;
     else 
       return 0;
}
