  
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <functional>

#ifdef MAIN 
#define ptition1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int ptition1_test(int, char**)
{
  int failures=0;
  int arr[10]={2,4,1,7,6,5,3,8,9,0};
  cout<<"Results of ptition1_test:"<<endl;
  vector <int> v1(10);
  for(int i = 0; i < v1.size(); i++)
    v1[i] = arr[i];
  ostream_iterator <int> iter(cout, " ");
  copy(v1.begin(), v1.end(), iter);
  cout << endl;
  
  if(2!=v1[0])
    failures++;
  else if(4!=v1[1])
    failures++;  
  else if(1!=v1[2])
    failures++;    
  else if(7!=v1[3])
    failures++;  
  else if(6!=v1[4])
    failures++;  
  else if(5!=v1[5])
    failures++;  
  else if(3!=v1[6])
    failures++;  
   else if(8!=v1[7])
    failures++;  
  else if(9!=v1[8])
    failures++;  
  else if(0!=v1[9])
    failures++;      
              
  partition(v1.begin(), v1.end(), bind2nd(less<int>(), 11));
  copy(v1.begin(), v1.end(), iter);
  cout << endl;
  
   if(2!=v1[0])
    failures++;
  else if(4!=v1[1])
    failures++;  
  else if(1!=v1[2])
    failures++;    
  else if(7!=v1[3])
    failures++;  
  else if(6!=v1[4])
    failures++;  
  else if(5!=v1[5])
    failures++;  
  else if(3!=v1[6])
    failures++;  
   else if(8!=v1[7])
    failures++;  
  else if(9!=v1[8])
    failures++;  
  else if(0!=v1[9])
      failures++; 
      
   if(failures)
      return 1;
   else       
      return 0;
}

