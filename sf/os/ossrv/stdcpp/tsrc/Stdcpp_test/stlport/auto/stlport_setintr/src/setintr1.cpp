
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.
#include <iterator>

#include <algorithm>
#include <vector>
#include <iostream>
#include <iterator>
// #include <functional>
#include <numeric>

#ifdef MAIN 
#define setintr1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int setintr1_test(int, char**)
{
  int failures=0;
  cout<<"Results of setintr1_test:"<<endl;
  vector <int> v1(10);
  iota(v1.begin(), v1.end(), 0);
  vector <int> v2(10);
  vector <int> v3(3);
  iota(v2.begin(), v2.end(), 7);
  ostream_iterator <int> iter(cout, " ");
  cout << "v1: ";
  copy(v1.begin(), v1.end(), iter);
   
   if(0!=v1[0])
     failures++;
   else if(1!=v1[1])
     failures++;
   else if(2!=v1[2])
     failures++;  
   else if(3!=v1[3])
     failures++;
   else if(4!=v1[4])
     failures++;  
   else if(5!=v1[5])
     failures++;
   else if(6!=v1[6])
     failures++;  
   else if(7!=v1[7])
     failures++;
   else if(8!=v1[8])
     failures++; 
   else if(9!=v1[9])
     failures++;   
     
  cout << "\nv2: ";
  copy(v2.begin(), v2.end(), iter);
  cout << endl;
  
  if(7!=v2[0])
     failures++;
   else if(8!=v2[1])
     failures++;
   else if(9!=v2[2])
     failures++;  
   else if(10!=v2[3])
     failures++;
   else if(11!=v2[4])
     failures++;  
   else if(12!=v2[5])
     failures++;
   else if(13!=v2[6])
     failures++;  
   else if(14!=v2[7])
     failures++;
   else if(15!=v2[8])
     failures++; 
   else if(16!=v2[9])
     failures++;   
  
  set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(),v3.begin());
   copy(v3.begin(),v3.end(),iter);
  cout << endl;
  
  if(failures)
     return 1;
  else
     return 0;
}

