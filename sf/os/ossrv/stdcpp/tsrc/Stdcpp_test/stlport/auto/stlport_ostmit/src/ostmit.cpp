
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>

#ifdef MAIN 
#define ostmit_test main
#endif
int cmp(char*,char*);

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int ostmit_test(int, char**)
{
  cout<<"Results of ostmit_test:"<<endl;
  ofstream outfile("c:\\ostmit.txt");
int array [] = { 1, 5, 2, 4 };
  char* string = "hello";
  ostream_iterator<char> it1(cout);
  ostream_iterator<char> itfile(outfile);
  copy(string, string + 5, it1);
  copy(string, string + 5, itfile);
  cout << endl;
  outfile << endl;
   
  ostream_iterator<int> it2(cout);
  ostream_iterator<int> itfile1(outfile);
  copy(array, array + 4, it2);
  copy(array, array + 4, itfile1);
  cout << endl;
 
 outfile << endl;
int result =  cmp("c:\\ostmit.txt","c:\\testframework\\tstdcpp\\ostmit_good.txt"); 
  if(result)
     return 0;
  else
  	return 1;
}


int cmp(char*str1,char*str2)
{
  FILE *fp1,*fp2;
  char c1,c2;
  int i=1,j=1;
  fp1=fopen(str1,"r");
 
  if (fp1 == NULL)
   {
 	     printf("Error 1");
       return(0);
    }
  fp2=fopen(str2,"r");
  
  if (fp2 == NULL)
  {
     printf("Error 2");
     return(0);
  }
  c1=getc(fp1);
  c2=getc(fp2);
  
 while(!feof(fp1) && !feof(fp2))
  {
 // i++;
   c1=getc(fp1);
   c2=getc(fp2);
    j++;
   if(c1=='\n' && c2=='\r')
    {
		//for every new line there is '\r' prepended to '\n'
		//so read again to get '\n'
		c2=getc(fp2);
    	  i++;
      j=0;
     }
  
  
   if(c1!=c2)
    {
   	   printf("The Files are different\n");
              printf("c1=%c c2=%c",c1,c2);
       printf("i=%d",i);
       
       return(0);
     }

    }
 if(feof(fp1) && feof(fp2))  
  	  return 1;
 else
   return 0;
}


