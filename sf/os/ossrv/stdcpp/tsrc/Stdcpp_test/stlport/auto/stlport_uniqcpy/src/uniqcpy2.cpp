
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

#ifdef MAIN 
#define uniqcpy2_test main
#endif
static bool str_equal(const char* a_, const char* b_)
{
  return strcmp(a_, b_) == 0 ? 1 : 0;
}

int uniqcpy2_test(int, char**)
{
  int failures=0;
  cout<<"Results of uniqcpy2_test:"<<endl;

  char* labels[] = { "Q","Q","W","W","E","E","R","T","T","Y","Y" };

  const unsigned count = sizeof(labels) / sizeof(labels[0]);
  ostream_iterator <char*> iter(cout);
  char* arr1[12];
  char* arr2[7];
  
  
  copy((char**)labels, (char**)labels + count, (char**)arr1);
  arr1[11]='\0'; 
 

   if(strcmp("Q",arr1[0])!=0)
     failures++;
   else if(strcmp(arr1[1],"Q")!=0)
      failures++;  
   else if(strcmp(arr1[2],"W")!=0)
      failures++;  
   else if(strcmp(arr1[3],"W")!=0)
     failures++;  
   else if(strcmp(arr1[4],"E")!=0)
      failures++;  
   else if(strcmp(arr1[5],"E")!=0)
      failures++;  
   else if(strcmp(arr1[6],"R")!=0)
      failures++;  
   else if(strcmp(arr1[7],"T")!=0)
      failures++;  
   else if(strcmp(arr1[8],"T")!=0)
      failures++;   
   else if(strcmp(arr1[9],"Y")!=0)
      failures++;  
   else if(strcmp(arr1[10],"Y")!=0)
      failures++;   

  
 
  copy((char**)arr1,(char**)arr1 +10 ,iter);
  
  cout << endl;
 
  char* uCopy[count];
  fill((char**)uCopy, (char**)uCopy + count, (char*)"");
  unique_copy((char**)labels, (char**)labels + count, (char**)uCopy, str_equal);
  
  copy((char**)labels, (char**)labels + count,(char**)arr1);
   arr1[11]='\0'; 
   
  if(strcmp(arr1[0],"Q")!=0)
     failures++;
   else if(strcmp(arr1[1],"Q")!=0)
      failures++;  
   else if(strcmp(arr1[2],"W")!=0)
      failures++;  
   else if(strcmp(arr1[3],"W")!=0)
     failures++;  
   else if(strcmp(arr1[4],"E")!=0)
      failures++;  
   else if(strcmp(arr1[5],"E")!=0)
      failures++;  
   else if(strcmp(arr1[6],"R")!=0)
      failures++;  
   else if(strcmp(arr1[7],"T")!=0)
      failures++;  
   else if(strcmp(arr1[8],"T")!=0)
      failures++;   
   else if(strcmp(arr1[9],"Y")!=0)
      failures++;  
   else if(strcmp(arr1[10],"Y")!=0)
      failures++;   

 
 
 
  copy((char**)arr1 ,(char**)arr1 + count,iter);
  cout << endl;
 
  copy((char**)uCopy, (char**)uCopy + count, (char**)arr2);
  arr2[6]='\0';
 
   if(strcmp(arr2[0],"Q")!=0)
     failures++;
   else if(strcmp(arr2[1],"W")!=0)
      failures++;  
   else if(strcmp(arr2[2],"E")!=0)
      failures++;  
   else if(strcmp(arr2[3],"R")!=0)
      failures++;  
   else if(strcmp(arr2[4],"T")!=0)
      failures++;   
   else if(strcmp(arr2[5],"Y")!=0)
      failures++;   

  
  copy((char**)arr2 ,(char**)arr2 +6,iter);
  cout << endl;
  
  if(failures)
      return 1;
  else 
     return 0;    
}


