 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define trnsfrm1_test main
#endif
static int negate_int(int a_)
{
  return -a_;
}

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int trnsfrm1_test(int, char**)
{ 
  int failures=0;
  cout<<"Results of trnsfrm1_test:"<<endl;

 int numbers[6] = { -5, -1, 0, 1, 6, 11 };

  int result[6];
  transform((int*)numbers, (int*)numbers + 6, (int*)result, negate_int);
  for(int i = 0; i < 6; i++)
    cout << result[i] << ' ';
  cout << endl;
  
  if(5!=result[0])
    failures++;
  else if(1!=result[1])
    failures++;
  else if(0!=result[2])
    failures++;
  else if(-1!=result[3])
    failures++;
  else if(-6!=result[4])
    failures++;
  else if(-11!=result[5])
    failures++;  
         
      
  if(failures)
    return 1;
  else
    return 0;  
}

