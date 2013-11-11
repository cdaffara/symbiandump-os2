 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>
#include <cstring>
#include <iterator>
#include <functional>

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

#ifdef MAIN 
#define unique2_test main
#endif
static bool str_equal(const char* a_, const char* b_)
{
  return strcmp(a_, b_) == 0 ? 1 : 0;
}

int unique2_test(int, char**)
{
  
  int failures=0;
  cout<<"Results of unique2_test:"<<endl;

  char* labels[] = { "Q","Q","W","W","E","E","R","T","T","Y","Y" };

  char* arr[12];
  const unsigned count = sizeof(labels) / sizeof(labels[0]);
  ostream_iterator <char*> iter(cout);
  
  copy((char**)labels, (char**)labels + count, (char**)arr);
   
   if(str_equal(arr[0],"Q")!=1)
     failures++;
   else if(str_equal(arr[1],"Q")!=1)
      failures++;  
   else if(str_equal(arr[2],"W")!=1)
      failures++;  
   else if(str_equal(arr[3],"W")!=1)
     failures++;  
   else if(str_equal(arr[4],"E")!=1)
      failures++;  
   else if(str_equal(arr[5],"E")!=1)
      failures++;  
   else if(str_equal(arr[6],"R")!=1)
      failures++;  
   else if(str_equal(arr[7],"T")!=1)
      failures++;  
   else if(str_equal(arr[8],"T")!=1)
      failures++;   
   else if(str_equal(arr[9],"Y")!=1)
      failures++;  
   else if(str_equal(arr[10],"Y")!=1)
      failures++;   
  
  copy((char**)arr,(char**)arr +10 ,iter);
  cout << endl;
  unique((char**)labels, (char**)labels + count, str_equal);
  copy((char**)labels, (char**)labels + count, (char**)arr);
  
  if(str_equal(arr[0],"Q")!=1)
     failures++;
   else if(str_equal(arr[1],"W")!=1)
      failures++;  
   else if(str_equal(arr[2],"E")!=1)
      failures++;  
   else if(str_equal(arr[3],"R")!=1)
     failures++;  
   else if(str_equal(arr[4],"T")!=1)
      failures++;  
   else if(str_equal(arr[5],"Y")!=1)
      failures++;  
   else if(str_equal(arr[6],"R")!=1)
      failures++;  
   else if(str_equal(arr[7],"T")!=1)
      failures++;  
   else if(str_equal(arr[8],"T")!=1)
      failures++;   
   else if(str_equal(arr[9],"Y")!=1)
      failures++;  
   else if(str_equal(arr[10],"Y")!=1)
      failures++;   
  
  copy((char**)arr,(char**)arr +10 ,iter);
  
  cout << endl;
  
  if(failures)
    return 1;
  else
    return 0;
}

/*QQWWEERTTYY
QWERTYRTTYY*/
