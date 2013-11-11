
 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <vector>
#include <algorithm>

#ifdef MAIN 
#define vec6_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int vec6_test(int, char**)
{
  int failures=0;
  cout<<"Results of vec6_test:"<<endl;
int array [] = { 1, 4, 9, 16, 25, 36 };

  vector<int> v(array, array + 6);
  int i;
  for(i = 0; i < v.size(); i++)
    cout << "v[" << i << "] = " << v[i] << endl;
  cout << endl;
   
   if(1!=v[0])
     failures++;
   else if(4!=v[1]) 
     failures++;
   else if(9!=v[2])
     failures++;
   else if(16!=v[3])
     failures++;
   else if(25!=v[4])
     failures++;
   else if(36!=v[5])
     failures++;
             
  v.erase(v.begin()); // Erase first element.
  for(i = 0; i < v.size(); i++)
    cout << "v[" << i << "] = " << v[i] << endl;
  cout << endl;
  
  if(4!=v[0])
     failures++;
   else if(9!=v[1]) 
     failures++;
   else if(16!=v[2])
     failures++;
   else if(25!=v[3])
     failures++;
   else if(36!=v[4])
     failures++;
             
  
  v.erase(v.end() - 1); // Erase last element.
  for(i = 0; i < v.size(); i++)
    cout << "v[" << i << "] = " << v[i] << endl;
  cout << endl;
  
  
  if(4!=v[0])
     failures++;
   else if(9!=v[1]) 
     failures++;
   else if(16!=v[2])
     failures++;
   else if(25!=v[3])
     failures++;
       
  v.erase(v.begin() + 1, v.end() - 1); // Erase all but first and last.
  for(i = 0; i < v.size(); i++)
    cout << "v[" << i << "] = " << v[i] << endl;
  cout << endl;
  /*
  v.erase(v.begin(), v.end()); // Erase all.
  */
 
  if(4!=v[0])
     failures++;
  else if(25!=v[1]) 
     failures++;
  
  
  if(failures)
    return 1;
  else   
    return 0;
}

/*Results of vec6_test:
v[0] = 1
v[1] = 4
v[2] = 9
v[3] = 16
v[4] = 25
v[5] = 36

v[0] = 4
v[1] = 9
v[2] = 16
v[3] = 25
v[4] = 36

v[0] = 4
v[1] = 9
v[2] = 16
v[3] = 25

v[0] = 4
v[1] = 25
*/
