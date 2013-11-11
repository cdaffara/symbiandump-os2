
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <deque>
#include <algorithm>
#include <string>

#ifdef MAIN 
#define insert2_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

int insert2_test(int, char**)
{
  cout<<"Results of insert2_test:"<<endl;
  char* array1 [] = { "laurie", "jennifer", "leisa" };
  char* array2 [] = { "amanda", "saskia", "carrie" };
	char* array3[6];
	int failures = 0, a = 0;
  deque<char*> names(array1, array1 + 3);
  deque<char*>::iterator i = names.begin() + 2;
  copy(array2, array2 + 3, inserter(names, i));
  std::deque<char*>::iterator j;
  for(j = names.begin(); j != names.end(); j++)
  {
  	    std::cout << *j << std::endl;
  	    	array3[a]=*j;
  	a++;
  }

 if(strcmp(array3[0],"laurie") != 0)
   failures++; 
 if(strcmp(array3[1],"jennifer") != 0)
   failures++; 
 if(strcmp(array3[2],"amanda") != 0)
   failures++; 
 if(strcmp(array3[3],"saskia") != 0)
   failures++; 
 if(strcmp(array3[4],"carrie") != 0)
   failures++; 
 if(strcmp(array3[5],"leisa") != 0)
   failures++;  
 
    
  return failures;
  
}
