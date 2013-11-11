  
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <fstream>

#ifdef MAIN 
#define nthelem1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int nthelem1_test(int, char**)
{
ofstream Outfile("c:\\nthelem1.txt");
  cout<<"Results of nthelem1_test:"<<endl;
  vector <int> v1(10) ;
  int failures = 0;
	v1[0] = 23;
	v1[1] = 6;
	v1[2] = 7;
	v1[3] = 5;
	v1[4] = 3;
	v1[5] = 5;
	v1[6] = 6;
	v1[7] = 2;
	v1[8] = 9;
	v1[9] = 1;
   
  /*for(int i = 0; i < v1.size(); i++)
    v1[i] = rand() % 10;*/
    
  ostream_iterator<int> iter(cout, " ");
   ostream_iterator<int> iterfile(Outfile, " ");
  copy(v1.begin(), v1.end(), iter);
   copy(v1.begin(), v1.end(), iterfile);
  cout << endl;
  Outfile << endl;
  
          
  nth_element(v1.begin(),
               v1.begin() + v1.size() / 2,
               v1.end());
  copy(v1.begin(), v1.end(), iter);
  copy(v1.begin(), v1.end(), iterfile);
  cout << endl;
   Outfile << endl;
    Outfile <<v1[0]<<endl;
  Outfile <<v1[1]<<endl;
Outfile <<v1[2]<<endl;
Outfile <<v1[3]<<endl;
Outfile <<v1[4]<<endl;
Outfile <<v1[5]<<endl;
Outfile <<v1[6]<<endl;
Outfile <<v1[7]<<endl;
Outfile <<v1[8]<<endl;
Outfile <<v1[9]<<endl;
	if(v1[0] != 1)
		failures++;
	if(v1[1] != 2)
		failures++;
	if(v1[2] != 5)
		failures++;
	if(v1[3] != 3)
		failures++;
	if(v1[4] != 5)
		failures++;
	if(v1[5] != 6)
		failures++;
	if(v1[6] != 6)
		failures++;
	if(v1[7] != 7)
		failures++;
	if(v1[8] != 9)
		failures++;
	if(v1[9] != 23)
		failures++;

   Outfile.close();
  return failures;
}
