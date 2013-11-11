
 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>

#ifdef MAIN
#define copy2_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int copy2_test(int, char**)
{
	cout<<"Results of copy2_test:"<<endl;
	vector <int> v(10);
	int failure = 0;
	for(int i = 0; i < v.size(); i++)
		v[i] = i;
	ostream_iterator<int> iter(cout, " ");
	copy(v.begin(), v.end(), iter);
	cout << endl;
if(v[0] != 0)
failure++;
if(v[1] != 1)
failure++;
if(v[2] != 2)
failure++;
if(v[3] != 3)
failure++;
if(v[4] != 4)
failure++;
if(v[5] != 5)
failure++;
if(v[6] != 6)
failure++;
if(v[7] != 7)
failure++;
if(v[8] != 8)
failure++;
if(v[9] != 9)
failure++;
	return failure;
}
