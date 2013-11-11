 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.
#include <iostream>
#include <deque>
#include <algorithm>
#include <string>

#ifdef MAIN 
#define insert1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

int insert1_test(int, char**)
{
  cout<<"Results of insert1_test:"<<endl;
  char* array1 [] = { "laurie", "jennifer", "leisa" };
  char* array2 [] = { "amanda", "saskia", "carrie" };
  char* array3 [6];
int failures = 0;
  deque<char*> names(array1, array1 + 3);
  std::deque<char*>::iterator i = names.begin() + 2;
  copy(array2, array2 + 3, insert_iterator<deque <char*> >(names, i));
  std::deque<char*>::iterator j;
  int a=0;
  for(j = names.begin(); j != names.end(); j++)
  {
  	cout << *j << endl;
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
