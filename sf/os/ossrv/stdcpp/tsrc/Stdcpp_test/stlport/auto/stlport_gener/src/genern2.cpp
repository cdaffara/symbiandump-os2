
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.


#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include "fib.h"

#ifdef MAIN
#define genern2_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int genern2_test(int, char**)
{
	cout<<"Results of genern2_test:"<<endl;

	vector <int> v1(10);
	Fibonacci generator;
	generate_n(v1.begin(), v1.size(), generator);
	ostream_iterator<int> iter(cout, " ");
	copy(v1.begin(), v1.end(), iter);
	cout << endl;

    int failures = 0;
	int fib_res[10] = { 1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };
	for (int i = 0; i < 10; i++)
	{
		if(v1[i] != fib_res[i])
		failures++;
	}

	if (!failures)
	return 0;
	else
	return 1;
}
