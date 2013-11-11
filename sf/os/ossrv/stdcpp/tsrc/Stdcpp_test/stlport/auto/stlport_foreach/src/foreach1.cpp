
 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "fadapter.h"

#define LOG_FILENAME_LINE __FILE__, __LINE__
extern void std_log(const char *filename,const int lineno,const char* aformat,...);

#ifdef MAIN
#define foreach1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

int array[10],j=0;
static void print_sqr(int a_)
{

  cout << a_ * a_ << " ";
	array[j++] = a_ * a_;	
}

int foreach1_test(int, char**)
{
  
  vector <int> v1(10);
  int failures = 0;
  
  for(int i = 0; i < v1.size(); i++)
      v1[i] = i;
  for_each(v1.begin(), v1.end(), ptr_proc(print_sqr) );
   cout << endl;
 
  
  	if(array[0] != 0)
	failures++;
	if(array[1] != 1)
	failures++;
	if(array[2] != 4)
	failures++;
	if(array[3] != 9)
	failures++;
	if(array[4] != 16)
	failures++;
	if(array[5] != 25)
	failures++;
	if(array[6] != 36)
	failures++;
	if(array[7] != 49)
	failures++;
	if(array[8] != 64)
	failures++;
	if(array[9] != 81)
	failures++;

	return failures;
}
