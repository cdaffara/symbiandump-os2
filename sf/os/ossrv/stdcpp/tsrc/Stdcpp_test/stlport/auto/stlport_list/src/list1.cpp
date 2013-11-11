 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <list>

#ifdef MAIN 
#define list1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int list1_test(int, char**)
{
  cout<<"Results of list1_test:"<<endl;
int array1 [] = { 9, 16, 36 };
int array2 [] = { 1, 4 };
int array3[5];
int failures = 0;int a=0;
  list<int> l1(array1, array1 + 3);
  list<int> l2(array2, array2 + 2);
  std::list<int>::iterator i1 = l1.begin();
  l1.splice(i1, l2);
  std::list<int>::iterator i2 = l1.begin();
  while(i2 != l1.end())
  {
  	array3[a] = *i2;
  	cout << *i2++ << endl;
  	a++;
  
  }
    
    if(array3[0] != 1)
    failures++;
    if(array3[1] != 4)
    failures++;
    if(array3[2] != 9)
    failures++;
    if(array3[3] != 16)
    failures++;
     if(array3[4] != 36)
    failures++;
  return failures;
}
