
 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <vector>
#include <algorithm>

#ifdef MAIN 
#define vec7_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int vec7_test(int, char**)
{
  int failures=0;
  cout<<"Results of vec7_test:"<<endl;
int array1 [] = { 1, 4, 25 };
int array2 [] = { 9, 16 };

  vector<int> v(array1, array1 + 3);
  v.insert(v.begin(), 0); // Insert before first element.
  v.insert(v.end(), 36); // Insert after last element.
  int i;
  for(i = 0; i < v.size(); i++)
    cout << "v[" << i << "] = " << v[i] << endl;
  cout << endl;
  
  if(0!=v[0])
     failures++;
   else if(1!=v[1]) 
     failures++;
   else if(4!=v[2])
     failures++;
   else if(25!=v[3])
     failures++;
   else if(36!=v[4])
     failures++;
     
  // Insert contents of array2 before fourth element.
  v.insert(v.begin() + 3, array2, array2 + 2);
  for(i = 0; i < v.size(); i++)
    cout << "v[" << i << "] = " << v[i] << endl;
  
  if(0!=v[0])
     failures++;
   else if(1!=v[1]) 
     failures++;
   else if(4!=v[2])
     failures++;
   else if(9!=v[3])
     failures++;
   else if(16!=v[4])
     failures++;
   else if(25!=v[5])
     failures++;
   else if(36!=v[6])
     failures++;
     
  cout << endl;
  
  if(failures)
    return 1;
  else  
    return 0;
}

/*Results of vec7_test:
v[0] = 0
v[1] = 1
v[2] = 4
v[3] = 25
v[4] = 36

v[0] = 0
v[1] = 1
v[2] = 4
v[3] = 9
v[4] = 16
v[5] = 25
v[6] = 36
*/
