
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define unique1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int unique1_test(int, char**)
{
  int failures=0;
  cout<<"Results of unique1_test:"<<endl;
int numbers[8] = { 0, 1, 1, 2, 2, 2, 3, 4 };

  unique((int*)numbers, (int*)numbers + 8);
  for(int i = 0; i < 8; i ++)
    cout << numbers[i] << ' ';
  
  if(0!=numbers[0])
      failures++;
  else if(1!=numbers[1])
     failures++;
  else if(2!=numbers[2])       
     failures++;
  else if(3!=numbers[3])
     failures++;
  else if(4!=numbers[4])       
     failures++;
  else if(2!=numbers[5])
     failures++;
  else if(3!=numbers[6])       
     failures++;      
  else if(4!=numbers[7])       
     failures++;   
  
  cout << endl;
  
  if(failures)
    return 1;
  else  
    return 0;
}

