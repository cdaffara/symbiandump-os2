
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <list>

#ifdef MAIN 
#define list3_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int list3_test(int, char**)
{
  cout<<"Results of list3_test:"<<endl;
char array [] = { 'x', 'l', 'x', 't', 's', 's' };
char array1[7];
//char array2[] = {'s','s','t','x','l','x'};
//char array3[] = {'s','s','t','l'};
//char array4[] = {'s','t','l'};
//char array5[] = {'l','s','t'};

int a =0,failures = 0;

  list<char> str(array, array + 6);
  std::list<char>::iterator i;
  cout << "original: ";
  for(i = str.begin(); i != str.end(); i++)
  {
  array1[a++]=*i;
  	 cout << *i;
  }
  
  array1[a] = '\0';
     cout << endl;
     
      if(array1[0] != 'x')
      failures++;
      if(array1[1] != 'l')
      failures++;
      if(array1[2] != 'x')
      failures++;
      if(array1[3] != 't')
      failures++;
      if(array1[4] != 's')
      failures++;
      if(array1[5] != 's')
      failures++;
      
    
   a=0;
  cout << "reversed: ";
  str.reverse();
  for(i = str.begin(); i != str.end(); i++)
   {
   array1[a++]=*i;
  	 cout << *i;
  }
   array1[a] = '\0';
  cout << endl;
   if(array1[0] != 's')
   failures++;
   if(array1[1] != 's')
   failures++;
   if(array1[2] != 't')
   failures++;
   if(array1[3] != 'x')
   failures++;
   if(array1[4] != 'l')
   failures++;
   if(array1[5] != 'x')
   failures++;
   
   a=0;
  cout << "removed: ";
  str.remove('x');
  for(i = str.begin(); i != str.end(); i++)
  {
     array1[a++]=*i;
  	 cout << *i;
  }
  array1[a] = '\0';
  cout << endl;
  if(array1[0] != 's')
   failures++;
   if(array1[1] != 's')
   failures++;
   if(array1[2] != 't')
   failures++;
   if(array1[3] != 'l')
   failures++;
    
   a=0;
  cout << "uniqued: ";
  str.unique();
  for(i = str.begin(); i != str.end(); i++)
  {
   array1[a++]=*i;
  	 cout << *i;
  }
  array1[a] = '\0';
  cout << endl;
   if(array1[0] != 's')
   failures++;
   if(array1[1] != 't')
   failures++;
   if(array1[2] != 'l')
   failures++;
    
   a=0;
  cout << "sorted: ";
  str.sort();
  for(i = str.begin(); i != str.end(); i++)
  {
    array1[a++]=*i;
  	 cout << *i;
  }
  array1[a] = '\0';
  cout << endl;
   if(array1[0] != 'l')
   failures++;
   if(array1[1] != 's')
   failures++;
   if(array1[2] != 't')
   failures++;
    
  return failures;
}
