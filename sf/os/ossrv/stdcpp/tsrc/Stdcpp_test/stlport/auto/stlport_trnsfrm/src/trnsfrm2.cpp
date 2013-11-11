 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>
#include <cstring>
#include <iterator>
#include <functional>

#ifdef MAIN 
#define trnsfrm2_test main
#endif
static char map_char(char a_, int b_)
{
  return char(a_ + b_);
}

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int trnsfrm2_test(int, char**)
{
  cout<<"Results of trnsfrm2_test:"<<endl;

#if defined (__MVS__)
int trans[] = {-11, 4, -6, -6, -18, 0, 18, -14, 6, 0, -1, -59};
#else
int trans[] = {-4, 4, -6, -6, -10, 0, 10, -6, 6, 0, -1, -77};
#endif
  char n[] = "Larry Mullen";
  char arr[13];
  const unsigned count = ::strlen(n);
  ostream_iterator <char> iter(cout);
  transform(n, n + count, trans, arr, map_char);
  cout << endl;
  arr[12]='\0';
  if(strcmp(arr,"Hello World!"))
    return 1;
  else
    return 0;
}

