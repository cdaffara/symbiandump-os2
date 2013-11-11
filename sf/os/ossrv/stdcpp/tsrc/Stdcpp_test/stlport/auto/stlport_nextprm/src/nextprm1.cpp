 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <vector>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <iostream>
#include <fstream>
#ifdef MAIN 
#define nextprm1_test main
#endif
int cmp(char*,char*);

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int nextprm1_test(int, char**)
{
  cout<<"Results of nextprm1_test:"<<endl;
  ofstream outfile("c:\\nextperm1.txt");
  vector <int> v1(3);
  iota(v1.begin(), v1.end(), 0);
  ostream_iterator<int> iter(cout, " ");
  ostream_iterator<int> iterfile(outfile, " ");
  
  copy(v1.begin(), v1.end(), iter);
  copy(v1.begin(), v1.end(), iterfile);
  cout << endl;
  outfile << endl;
  for(int i = 0; i < 9; i++)
  {
    next_permutation(v1.begin(), v1.end());
    copy(v1.begin(), v1.end(), iter);
    copy(v1.begin(), v1.end(), iterfile); 
    cout << endl;
    outfile << endl;
  }
  outfile.close();
  int result =  cmp("c:\\nextperm1.txt","c:\\testframework\\tstdcpp\\nextperm1_good.txt"); 
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


