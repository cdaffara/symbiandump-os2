
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <vector>
#include <algorithm>

#ifdef MAIN
#define binsert2_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int binsert2_test(int, char**)
{

	cout<<"Results of binsert2_test:"<<endl;
	char* array [] = { "laurie", "jennifer", "leisa" };

	vector<char*> names;
	copy(array, array + 3, back_inserter(names));
	std::vector<char*>::iterator i;
	int j;
	int failures = 0;
	for(i = names.begin(), j=0; i != names.end(); i++, j++)
	{
		cout << *i << endl;
		if(*i != array[j])
			failures++;
	}

	if(!failures)
		 return 0;
  	else
  	   return 1;
}
