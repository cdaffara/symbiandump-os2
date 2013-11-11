 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <algorithm>
#include <functional>

#ifdef MAIN 
#define nequal_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int nequal_test(int, char**)
{
  cout<<"Results of nequal_test:"<<endl;
int input1 [4] = { 1, 7, 2, 2 };
int input2 [4] = { 1, 6, 2, 3 };
int failure = 0;
  int output [4];
  transform((int*)input1, (int*)input1 + 4, (int*)input2, (int*)output, not_equal_to<int>());
  for(int i = 0; i < 4; i++)
    cout << output[i] << endl;
    if(output[0] != 0)
    failure++;
    if(output[1] != 1)
    failure++;
    if(output[2] != 0)
    failure++;
    if(output[3] != 1)
    failure++;
  return failure;
}
