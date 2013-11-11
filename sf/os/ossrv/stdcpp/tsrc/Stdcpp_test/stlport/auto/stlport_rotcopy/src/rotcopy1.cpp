 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.
#include <iterator>

#include <algorithm>
#include <vector>
#include <iostream>
#include <iterator>
// #include <functional>
#include <numeric>

#ifdef MAIN 
#define rotcopy1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int rotcopy1_test(int, char**)
{
  int failures=0;
  cout<<"Results of rotcopy1_test:"<<endl;
  vector <int> v1(10);
  iota(v1.begin(), v1.end(), 0);
  ostream_iterator <int> iter(cout, " ");
  copy(v1.begin(), v1.end(), iter);
  cout << endl;
 
  if(0!=v1[0])
     failures++;
  else if(1!=v1[1])
     failures++;
  else if(2!=v1[2]) 
    failures++;
  else if(3!=v1[3])
    failures++;
  else if(4!=v1[4])
    failures++;          
  else if(5!=v1[5])
     failures++;
  else if(6!=v1[6])
    failures++;
  else if(7!=v1[7])
    failures++;
  else if(8!=v1[8])
    failures++;
  else if(9!=v1[9])
    failures++;
    
  vector <int> v2(v1.size());
  for(int i = 0; i < v1.size(); i++)
  {
    rotate_copy(v1.begin(),
                 v1.begin() + i,
                 v1.end(),
                 v2.begin());
    ostream_iterator <int> iter(cout, " ");
    copy(v2.begin(), v2.end(), iter);
      
    switch(i)
    {
      case 0: if(0!=v1[0])
                failures++;
			  else if(1!=v1[1])
			     failures++;
			  else if(2!=v1[2])
			    failures++;
			  else if(3!=v1[3])
			    failures++;
			  else if(4!=v1[4])  // 0 1 2 3 4 5 6 7 8 9
			    failures++;          
			  else if(5!=v1[5])
			     failures++;
			  else if(6!=v1[6])
			    failures++;
			  else if(7!=v1[7])
			    failures++;
			  else if(8!=v1[8])
			    failures++;
			  else if(9!=v1[9])
			    failures++;
                 break;
       case 1: if(1!=v1[0])
                failures++;
			  else if(2!=v1[1])
			     failures++;
			  else if(3!=v1[2])
			    failures++;
			  else if(4!=v1[3])
			    failures++;
			  else if(5!=v1[4])  // 1 2 3 4 5 6 7 8 9 0
			    failures++;          
			  else if(6!=v1[5])
			     failures++;
			  else if(7!=v1[6])
			    failures++;
			  else if(8!=v1[7])
			    failures++;
			  else if(9!=v1[8])
			    failures++;
			  else if(0!=v1[9])
			    failures++;
                 break;
      case 2: if(2!=v1[0])
                failures++;
			  else if(3!=v1[1])
			     failures++;      // 2 3 4 5 6 7 8 9 0 1
			  else if(4!=v1[2])
			    failures++;
			  else if(5!=v1[3])
			    failures++;
			  else if(6!=v1[4])
			    failures++;          
			  else if(7!=v1[5])
			     failures++;
			  else if(8!=v1[6])
			    failures++;
			  else if(9!=v1[7])
			    failures++;
			  else if(0!=v1[8])
			    failures++;
			  else if(1!=v1[9])
			    failures++;
			     break;
    
      case 3: if(3!=v1[0])
                failures++;
			  else if(4!=v1[1])
			     failures++;
			  else if(5!=v1[2])
			    failures++;
			  else if(6!=v1[3])
			    failures++;
			  else if(7!=v1[4])
			    failures++;        
			  else if(8!=v1[5]) // 3 4 5 6 7 8 9 0 1 2
			     failures++;
			  else if(9!=v1[6])
			    failures++;
			  else if(0!=v1[7])
			    failures++;
			  else if(1!=v1[8])
			    failures++;
			  else if(2!=v1[9])
			    failures++;
			     break;
      
      case 4: if(4!=v1[0])
                failures++;
			  else if(5!=v1[1])
			     failures++;
			  else if(6!=v1[2])
			    failures++;
			  else if(7!=v1[3])
			    failures++;
			  else if(8!=v1[4])
			    failures++;         // 4 5 6 7 8 9 0 1 2 3
			  else if(9!=v1[5])
			     failures++;
			  else if(0!=v1[6])
			    failures++;
			  else if(1!=v1[7])
			    failures++;
			  else if(2!=v1[8])
			    failures++;
			  else if(3!=v1[9])
			    failures++;
			     break;
      case 5: if(5!=v1[0])
                failures++;
			  else if(6!=v1[1])
			     failures++;
			  else if(7!=v1[2])
			    failures++;
			  else if(8!=v1[3])
			    failures++;
			  else if(9!=v1[4]) // 5 6 7 8 9 0 1 2 3 4
			    failures++;          
			  else if(0!=v1[5])
			     failures++;
			  else if(1!=v1[6])
			    failures++;
			  else if(2!=v1[7])
			    failures++;
			  else if(3!=v1[8])
			    failures++;
			  else if(4!=v1[9])
			    failures++;
			      break;
      case 6: if(6!=v1[0])
                failures++;
			  else if(7!=v1[1])
			     failures++;
			  else if(8!=v1[2])
			    failures++;
			  else if(9!=v1[3])
			    failures++;     // 6 7 8 9 0 1 2 3 4 5
			  else if(9!=v1[4])
			    failures++;          
			  else if(1!=v1[5])
			     failures++;
			  else if(2!=v1[6])
			    failures++;
			  else if(3!=v1[7])
			    failures++;
			  else if(4!=v1[8])
			    failures++;
			  else if(5!=v1[9])
			    failures++;
			      break;
      case 7: if(7!=v1[0])
                failures++;
			  else if(8!=v1[1])
			     failures++;
			  else if(9!=v1[2])
			    failures++;
			  else if(0!=v1[3])
			    failures++;        //7 8 9 0 1 2 3 4 5 6
			  else if(1!=v1[4])
			    failures++;          
			  else if(2!=v1[5])
			     failures++;
			  else if(3!=v1[6])
			    failures++;
			  else if(4!=v1[7])
			    failures++;
			  else if(5!=v1[8])
			    failures++;
			  else if(6!=v1[9])
			    failures++;
			      break;
      			      
      case 8: if(8!=v1[0])
                failures++;    //8 9 0 1 2 3 4 5 6 7
			  else if(9!=v1[1])
			     failures++;
			  else if(0!=v1[2])
			    failures++;
			  else if(1!=v1[3])
			    failures++;
			  else if(2!=v1[4])
			    failures++;          
			  else if(3!=v1[5])
			     failures++;
			  else if(4!=v1[6])
			    failures++;
			  else if(5!=v1[7])
			    failures++;
			  else if(6!=v1[8])
			    failures++;
			  else if(7!=v1[9])
			    failures++;
			     break;
      case 9: if(9!=v1[0])
                failures++;
			  else if(0!=v1[1])
			     failures++;
			  else if(1!=v1[2])  //9 0 1 2 3 4 5 6 7 8
			    failures++;
			  else if(2!=v1[3])
			    failures++;
			  else if(3!=v1[4])
			    failures++;          
			  else if(4!=v1[5])
			     failures++;
			  else if(5!=v1[6])
			    failures++;
			  else if(6!=v1[7])
			    failures++;
			  else if(7!=v1[8])
			    failures++;
			  else if(8!=v1[9])
			    failures++;
                 break;
    
    }
    
    cout << endl;
  }
  cout << endl;
  return 0;
}

