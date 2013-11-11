  
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>

#ifdef MAIN 
#define parsrt1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int parsrt1_test(int, char**)
{

  int failures=0;
  cout<<"Results of parsrt1_test:"<<endl;
  vector <int> v1(10);
  //for(int i = 0; i < v1.size(); i++)
    //v1[i] = i;
  int arr[]={1,4,5,2,6,7,0,9,8,3};  
    
   for(int i=0;i<v1.size();i++)
     v1[i]=arr[i]; 
    
  ostream_iterator<int> iter(cout, " ");
  copy(v1.begin(), v1.end(), iter);
  cout << endl;
  
  if(1!=v1[0])
    failures++;
  else if( 4 !=v1[1])  
    failures++;
  else if( 5 !=v1[2])  
    failures++;
  else if( 2 !=v1[3])  
    failures++;
  else if( 6 !=v1[4])  
    failures++;
  else if( 7 !=v1[5])  
    failures++;
  else if( 0 !=v1[6])  
    failures++;
  else if( 9 !=v1[7])  
    failures++;
  else if( 8 !=v1[8])  
    failures++;
  else if( 3 !=v1[9])  
    failures++;
    
  partial_sort(v1.begin(),
                v1.begin() + v1.size() / 2,
                v1.end());
  copy(v1.begin(), v1.end(), iter);

   cout << endl;

  if(0!=v1[0])    
    failures++;
  else if( 1 !=v1[1])  
    failures++;
  else if( 2 !=v1[2])  
    failures++;
  else if( 3 !=v1[3])  
    failures++;
  else if( 4 !=v1[4])  
    failures++;
  else if( 7 !=v1[5])  
    failures++;
  else if( 6 !=v1[6])  
    failures++;
  else if( 9 !=v1[7])  
    failures++;
  else if( 8 !=v1[8])  
    failures++;
  else if( 5 !=v1[9])  
    failures++;
 
   if(failures)
     return 1;
   else   
     return 0;
}
