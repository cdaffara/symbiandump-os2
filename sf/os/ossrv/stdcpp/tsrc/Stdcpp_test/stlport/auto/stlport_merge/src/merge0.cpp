 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#ifdef MAIN 
#define merge0_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int merge0_test(int, char**)
{
  cout<<"Results of merge0_test:"<<endl;
int numbers1[5] = { 1, 6, 13, 25, 101 };
int numbers2[5] = {-5, 26, 36, 46, 99 };
int failures = 0;
  int result[10];
  merge((int*)numbers1, (int*)numbers1 + 5, (int*)numbers2, (int*)numbers2 + 5, (int*)result);
  for(int i = 0; i < 10; i++)
    cout << result[i] << ' ';
  cout << endl;
  if (result[0] != -5)
  failures++;
  if (result[1] != 1)
  failures++;
  if (result[2] != 6)
  failures++;
  if (result[3] != 13)
  failures++;
  if (result[4] != 25)
  failures++;
  if (result[5] != 26)
  failures++;
  if (result[6] != 36)
  failures++;
  if (result[7] != 46)
  failures++;
  if (result[8] != 99)
  failures++;
  if (result[9] != 101)
  failures++;
 
  return failures;
}
