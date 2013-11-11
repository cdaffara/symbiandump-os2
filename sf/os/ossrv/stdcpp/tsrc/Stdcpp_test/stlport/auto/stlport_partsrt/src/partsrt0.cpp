  
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define partsrt0_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int partsrt0_test(int, char**)
{
  int failures=0;
  cout<<"Results of partsrt0_test:"<<endl;
  int numbers[6] = { 5, 2, 4, 3, 1, 6 };

  partial_sort((int*)numbers, (int*)numbers + 3, (int*)numbers + 6);
  for(int i = 0; i < 6; i++)
    cout << numbers[i] << ' ';
   cout << endl;
   
   if(1!=numbers[0])
     failures++;
   else if(2!=numbers[1])
     failures++;
   else if(3!=numbers[2])
     failures++;  
   else if(5!=numbers[3])
      failures++;    
   else if(4!=numbers[4])
     failures++;
   else if(6!=numbers[5])
     failures++;    
  
  if(failures)
    return 1;
  else
    return 0;
}
