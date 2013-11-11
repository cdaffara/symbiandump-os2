 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <vector>
#include <iostream>

#ifdef MAIN 
#define replif1_test main
#endif
static bool odd(int a_)
{
  return a_ % 2;
}

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int replif1_test(int, char**)
{
  int failures=0;
  cout<<"Results of replif1_test:"<<endl;

  vector <int> v1(10);
  int i;
  for(i = 0; i < v1.size(); i++)
  {
    v1[i] = i % 5;
    cout << v1[i] << ' ';
  }
  cout << endl;
  
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
 else if(0!=v1[5])
      failures++;
  else if(1!=v1[6])
     failures++;
 else if(2!=v1[7])
      failures++;
  else if(3!=v1[8])
     failures++;
 else if(4!=v1[9])
      failures++;
  

           
  
  replace_if(v1.begin(), v1.end(), odd, 42);
  for(i = 0; i < v1.size(); i++)
    cout << v1[i] << ' ';
  cout << endl;
  
   if(0!=v1[0])
     failures++;
  else if(42!=v1[1])
      failures++;
  else if(2!=v1[2])
     failures++;
 else if(42!=v1[3])
      failures++;
  else if(4!=v1[4])
     failures++;
 else if(0!=v1[5])
      failures++;
  else if(42!=v1[6])
     failures++;
 else if(2!=v1[7])
      failures++;
  else if(42!=v1[8])
     failures++;
 else if(4!=v1[9])
       failures++;
   
   if(failures)
     return 1;
   else
     return 0;
}
