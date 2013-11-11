 
#include <algorithm>
#include <cstring> 
#include <string>
#include <deque>
#include <iterator>
#include <iostream>
#include <functional>

typedef std::string Str;
typedef std::deque<Str> Dq;
#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int insert1_test(int, char**)
{
  Str array1 [] = { "laurie", "jennifer", "leisa" };
  Dq nam(array1, array1 + 3);
  Dq::iterator i = nam.begin() + 2;
  Str a;
  
   a=*i;
   cout<<*i;
   
  if(a.compare("leisa")!=0)  
   return 1;
  else   
  return 0;
}
