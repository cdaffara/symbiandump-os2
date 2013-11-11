
 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>

#include "fadapter.h"

#ifdef MAIN
#define foreach0_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
static void print(int a_)
{
  cout << a_ << ' ';
}

int foreach0_test(int, char**)
{

	cout<<"Results of foreach0_test:"<<endl;
	int failures = 0;
	
	int numbers[10] = { 1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };

	for_each(numbers, numbers + 10, ptr_proc(print));
		cout << endl;
	if(numbers[0] != 1)
	failures++;
	if(numbers[1] != 1)
	failures++;
	if(numbers[2] != 2)
	failures++;
	if(numbers[3] != 3)
	failures++;
	if(numbers[4] != 5)
	failures++;
	if(numbers[5] != 8)
	failures++;
	if(numbers[6] != 13)
	failures++;
	if(numbers[7] != 21)
	failures++;
	if(numbers[8] != 34)
	failures++;
	if(numbers[9] != 55)
	failures++;
	return failures;
}
