
 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.
#include <vector>
#include <numeric>
#include <iterator>
#include <iostream>


#ifdef MAIN 
#define adjdiff2_test main
#endif
static int mult(int a_, int b_)
{
  return a_ * b_;
}

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int adjdiff2_test(int, char**)
{
   cout<<"Results of adjdiff2_test:"<<endl;
  
  //results of test for comparison
  int res_v[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int adj_diff_res[10] = {1, 2, 6, 12, 20, 30, 42, 56, 72,90};
  int failures = 0;  

  vector <int> v(10);
  for(int i = 0; i < v.size(); i++)
    v[i] = i + 1;
  vector <int> rslt(v.size());
  adjacent_difference(v.begin(), v.end(), rslt.begin(), mult);
  ostream_iterator<int> iter(cout, " ");
  copy(v.begin(), v.end(), iter);
  cout << endl;
  copy(rslt.begin(), rslt.end(), iter);
  cout << endl;
  
  //compare the results
  for(int i = 0; i < v.size(); i++)
  {	
  	if(res_v[i] != v[i])
  		failures++;	
  }
  
  for(int i = 0; i < rslt.size(); i++)
  {
  	if(adj_diff_res[i] != rslt[i])	
  		failures++;
  }
  
  if (!failures)
  	  return 0;
  else
  return 1;
}
