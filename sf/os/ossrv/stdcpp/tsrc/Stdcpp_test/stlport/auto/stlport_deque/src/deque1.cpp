
 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <deque>
#include <algorithm>
#include <iostream>


#ifdef MAIN
#define deque1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

int deque1_test(int, char**)
{
	cout<<"Results of deque1_test:"<<endl;

	int failures = 0;
	int res_arr[4] = {1, 4, 9, 16};
	deque<int> d;
	d.push_back(4); // Add after end.
	d.push_back(9);
	d.push_back(16);
	d.push_front(1); // Insert at beginning.
	int i;
	for(i = 0; i < d.size(); i++)
	{
		cout << "d[" << i << "] = " << d[i] << endl;
		if (d[i] != res_arr[i])
			failures++;
	}
	cout << endl;

	int res_arr2[] = {4, 9, 25};

	d.pop_front(); // Erase first element.

	d[2] = 25; // Replace last element.
	for(i = 0; i < d.size(); i++)
	{
		cout << "d[" << i << "] = " << d[i] << endl;
		if (d[i] != res_arr2[i])
			failures++;
	}

	if(!failures)
    return 0;
  else
    return 1;

}




