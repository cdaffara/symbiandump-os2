
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <deque>
#include <algorithm>

#ifdef MAIN
#define finsert2_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int finsert2_test(int, char**)
{
	cout<<"Results of finsert2_test:"<<endl;
	char* array [] = { "laurie", "jennifer", "leisa" };
	char* res_array[] = { "leisa", "jennifer", "laurie" };
	int failures = 0;

	deque<char*> names;
	copy(array, array + 3, front_inserter(names));
	deque<char*>::iterator i;
	int j;
	for(i = names.begin(), j = 0; i != names.end(); i++, j++)
	{
		cout << *i << endl;
		if((*i) !=  res_array[j])
			failures++;
	}

	if(!failures)
	return 0;
	else
	return 1;
}
