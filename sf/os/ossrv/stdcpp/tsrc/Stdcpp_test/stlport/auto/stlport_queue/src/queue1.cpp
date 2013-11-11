 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <algorithm>
#include <queue>
#include <list>

#ifdef MAIN 
#define queue1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int queue1_test(int, char**)
{
  int failures=0,i=0;
  int arr[3];
  cout<<"Results of queue1_test:"<<endl;
  queue<int, list<int> > q;
  q.push(42);
  q.push(101);
  q.push(69);
  while(!q.empty())
  {
    arr[i]=q.front();
    cout << q.front() << endl;
    q.pop();
    i++;
  }
  if(42!=arr[0])
    failures++;
  else if(101!=arr[1])
    failures++;
  else if(69!=arr[2])
    failures++;
    
  if(failures)
    return 1;
  else         
    return 0;
}

