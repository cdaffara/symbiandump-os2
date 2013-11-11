 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.


#include <iostream>
#include <set>
// #include <algorithm>
#include <functional>


#ifdef MAIN 
#define mset5_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

static bool less_than(int a_, int b_)
{
  return a_ < b_;
}
 
static bool greater_than(int a_, int b_)
{
  return a_ > b_;
}

int mset5_test(int, char**)
{
  cout<<"Results of mset5_test:"<<endl;
  int array [] = { 3, 6, 1, 9 };
  int array1 [4],a =0 , failures = 0;
   typedef pointer_to_binary_function<int, int, bool> fn_type;
  typedef __multiset__<int, fn_type, allocator<int> > mset;
  fn_type f(less_than);

  mset s1(array+0, array + 4 , f );
  mset::const_iterator i = s1.begin();

  cout << "Using less_than: " << endl;
  
  while(i != s1.end())
  {
  	    cout << *i << endl;
  	  array1[a] = *i; 
  	  *i++; 
  	  a++;
  }
  if( array1[0] != 1)
  failures++;
  if( array1[1] != 3)
  failures++;
  if( array1[2] != 6)
  failures++;
  if( array1[3] != 9)
  failures++;
  
  fn_type g(greater_than);
  mset s2(array, array + 4, g);
  i = s2.begin();
  cout << "Using greater_than: " << endl;
  a=0;
  while(i != s2.end())
  {
  	
    cout << *i << endl;
array1[a] = *i;
       *i++;
       a++;
  }
  if( array1[0] != 9)
  failures++;
  if( array1[1] != 6)
  failures++;
  if( array1[2] != 3)
  failures++;
  if( array1[3] != 1)
  failures++;
  
  return failures;
}

