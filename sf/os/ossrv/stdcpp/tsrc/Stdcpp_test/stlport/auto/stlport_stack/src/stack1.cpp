 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <algorithm>
#include <queue>
#include <deque>
#include <stack>


#ifdef MAIN 
#define stack1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int stack1_test(int, char**)
{
  int failures=0,i=0;
  int arr[3];
  cout<<"Results of stack1_test:"<<endl;
  stack<int, deque<int> > s;
  s.push(42);
  s.push(101);
  s.push(69);
  while(!s.empty())
  {
    arr[i]=s.top();
    cout << s.top() << endl;
    s.pop();
    i++;
  }
  
  if(69!=arr[0])
    failures++;
  else if(101!=arr[1])
    failures++;
  else if(42!=arr[2])
    failures++;
    
  if(failures)
    return 1;
  else
    return 0;
}
