
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iterator>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>

#ifdef MAIN 
#define parsrtc2_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

static bool str_compare(const char* a_, const char* b_)
{
  return strcmp(a_, b_) < 0 ? 1 : 0;
}

int parsrtc2_test(int, char**)
{
	int failures=0;
	cout<<"Results of parsrtc2_test:"<<endl;

	char* names[] = { "aa", "ff", "dd", "ee", "cc", "bb" };

	const unsigned nameSize = sizeof(names) / sizeof(names[0]);
	vector <char*> v1(nameSize);
	for(int i = 0; i < v1.size(); i++)
		v1[i] = names[i];
	ostream_iterator<char*> iter(cout, " ");
	copy(v1.begin(), v1.end(), iter);

	if(strcmp("aa", v1[0]))
		failures++;
	if(strcmp("ff", v1[1]))
		failures++;
	if(strcmp("dd", v1[2]))
		failures++;
	if(strcmp("ee", v1[3]))
		failures++;
	if(strcmp("cc", v1[4]))
		failures++;
	if(strcmp("bb", v1[5]))
		failures++;

	cout << endl;
	vector <char*> result(5);
	partial_sort_copy(v1.begin(),
			v1.end(),
			result.begin(),
			result.end(),
			str_compare);
	copy(result.begin(),result.end(), iter);

	if(strcmp("aa", result[0]))
		failures++;
	if(strcmp("bb", result[1]))
		failures++;
	if(strcmp("cc", result[2]))
		failures++;
	if(strcmp("dd", result[3]))
		failures++;
	if(strcmp("ee", result[4]))
		failures++;

	cout << endl;

	if(failures)
		return 1;
	else
		return 0;
}
