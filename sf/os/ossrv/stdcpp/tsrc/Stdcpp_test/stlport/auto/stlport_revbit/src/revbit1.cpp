 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <algorithm>
#include <list>

#ifdef MAIN 
#define revbit1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int revbit1_test(int, char**)
{
  int failures=0,i=0;
  cout<<"Results of revbit1_test:"<<endl;
int array[]={ 1, 5, 2, 3 };
int ar[4];
  list<int> v(array, array + 4);
  std::list<int>::reverse_iterator r(v.rbegin());
  while(!(r == v.rend()))
     {
       ar[i]=*r; 
       cout << *r++ << endl;
       i++;
      }
   
   if(3!=ar[0])
     failures++;
   else if(2!=ar[1])
     failures++;
   else if(5!=ar[2])
     failures++;
   else if(1!=ar[3])
     failures++;    
         
  if(failures)
     return 1;
   else
      return 0;
}
