 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.
#include <iterator>

#include <vector>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <iostream>
#include <fstream>

#ifdef MAIN 
#define nextprm2_test main
#endif
int cmp(char*,char*);

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int nextprm2_test(int, char**)
{
  cout<<"Results of nextprm2_test:"<<endl;
  ofstream outfile("c:\\nextperm2.txt");
  vector <char> v1(3);
  iota(v1.begin(), v1.end(), 'A');
  ostream_iterator<char> iter(cout);
  ostream_iterator<char> iterfile(outfile);
  
  copy(v1.begin(), v1.end(), iter);
  copy(v1.begin(), v1.end(), iterfile);
  cout << endl;
  outfile << endl;
  for(int i = 0; i < 9; i++)
  {
    next_permutation(v1.begin(), v1.end(), less<char>());
    copy(v1.begin(), v1.end(), iter);
    copy(v1.begin(), v1.end(), iterfile); 
    cout << endl;
    outfile << endl;
  }
  outfile.close();
 int result =  cmp("c:\\nextperm2.txt","c:\\testframework\\tstdcpp\\nextperm2_good.txt"); 
  if(result)
     return 0;
  else
  	return 1;
}
