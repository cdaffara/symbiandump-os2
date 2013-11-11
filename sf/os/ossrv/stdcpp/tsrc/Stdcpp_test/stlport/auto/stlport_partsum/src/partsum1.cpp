  
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.
#include <iterator>

#include <vector>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <iterator>

#ifdef MAIN 
#define partsum1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int partsum1_test(int, char**)
{

  int failures=0;
  cout<<"Results of partsum1_test:"<<endl;
  vector <int> v1(10);
  iota(v1.begin(), v1.end(), 0);
  vector <int> v2(v1.size());
  partial_sum(v1.begin(), v1.end(), v2.begin());
  ostream_iterator <int> iter(cout, " ");
  copy(v1.begin(), v1.end(), iter);
  cout << endl;
   
  if(0!=v1[0])
    failures++;
  else if(1 !=v1[1])
    failures++;
  else if(2 !=v1[2])
    failures++;
  else if(3 !=v1[3])
    failures++;      
  else if(4 !=v1[4])
    failures++;
  else if(5 !=v1[5])
    failures++;
  else if(6 !=v1[6])
    failures++;
  else if(7 !=v1[7])
    failures++;      
  else if(8 !=v1[8])
    failures++;
  else if(9 !=v1[9])
    failures++;  
  
  copy(v2.begin(), v2.end(), iter);
  cout << endl;
   
   if(0!=v2[0])
    failures++;
  else if(1 !=v2[1])
    failures++;
  else if(3 !=v2[2])
    failures++;
  else if(6 !=v2[3])
    failures++;      
  else if(10 !=v2[4])
    failures++;
  else if(15 !=v2[5])
    failures++;
  else if(21 !=v2[6])
    failures++;
  else if(28 !=v2[7])
    failures++;      
  else if(36 !=v2[8])
    failures++;
  else if(45 !=v2[9])
    failures++;   
 
 
 if(failures)
   return 1;
 else
   return 0;
}


