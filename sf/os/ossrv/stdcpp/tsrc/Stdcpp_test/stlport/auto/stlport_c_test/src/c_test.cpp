
#include <string>
#include <iostream>
#include <fstream>
#include <list>


#ifndef STLPORT
//#error You Did not get the STLport include files!
#endif

using namespace std;

int c_test (int/* argc*/, char **/*argv[]*/)
{

	#ifdef STLPORT
	string s2 = "STLport included";
	cout << s2 << endl;
	#else
	string s2 = "STLport NOT included!";
	cout << s2 << endl;
	#endif

	int failures = 0;

	string s = "hello";
	cout << s << endl;
	ofstream fstr("testfile");
	fstr << s << endl;

	list<int> L;
	for (int i = 0; i < 10; ++i) {
	L.push_back(i);
	}
	//for comparing results
	int i = 9;
	for (list<int>::const_reverse_iterator cri = L.rbegin();
	   cri != (list<int>::const_reverse_iterator) L.rend(); ++cri) {
	cout << *cri << endl;
	if(*cri != i)
		failures++;
	i--;
	}
#ifndef __SYMBIAN32__

	#ifdef STLPORT
	string s3 = "PASSED";
	if (strstr(argv[0], "nostlport"))
	s3 = "FAILED";
	cout << s3 << endl;
	#else
	string s3 = "PASSED";
	if (!strstr(argv[0], "nostlport"))
	s3 = "FAILED";
	cout << s3 << endl;
	#endif

#endif
	if (!failures)
    return 0;
  else
    return 1;

}










