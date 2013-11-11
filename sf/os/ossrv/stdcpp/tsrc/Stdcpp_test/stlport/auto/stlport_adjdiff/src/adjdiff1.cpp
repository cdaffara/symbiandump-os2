
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <vector>
#include <numeric>
#include <iterator>
#include <iostream>

#ifdef MAIN 
#define adjdiff1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int adjdiff1_test(int, char**)
{
    cout<<"Results of adjdiff1_test:"<<endl;
  
  //declaring the resultant values in an array for comparison
  int compare_v[10] = {0, 1, 4, 9, 16, 25, 36, 49, 64, 81};  
  int compare_res[10] =  {0, 1, 3, 5, 7, 9, 11, 13, 15, 17};
  
  int failures = 0;  
  vector <int> v(10);
  for(int i = 0; i < v.size(); i++)
    v[i] = i * i;
  vector <int> result(v.size());
  adjacent_difference(v.begin(), v.end(), result.begin());
  ostream_iterator<int> iter(cout, " ");
  // vector<int>::iterator iter;
  copy(v.begin(), v.end(), iter);
  cout << endl;
  copy(result.begin(), result.end(), iter);
  cout << endl; 
  
  //compare the results
  for(int i = 0; i < result.size(); i++)
  {
  	if(result[i] != compare_res[i])	
  		failures++;
  }
  
  for(int j = 0; j < v.size(); j++)
  {
  	if(v[j] != compare_v[j])
  		failures++;
  }
  
  if (!failures)
  	  return 0;
  else
   return 1;
}
