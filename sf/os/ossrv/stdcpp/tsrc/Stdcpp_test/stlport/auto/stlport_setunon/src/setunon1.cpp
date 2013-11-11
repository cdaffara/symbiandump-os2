 
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
#define setunon1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int setunon1_test(int, char**)
{
  int failures=0;
  cout<<"Results of setunon1_test:"<<endl;
  vector <int> v1(10);
  iota(v1.begin(), v1.end(), 0);
  vector <int> v2(10);
  vector <int> v3(17);
  iota(v2.begin(), v2.end(), 7);
  ostream_iterator <int> iter(cout, " ");
  cout << "v1: ";
  
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
  
  copy(v1.begin(), v1.end(), iter);
  cout << "\nv2: ";
  
   if(7!=v2[0])
     failures++;
   else if(8!=v2[1])
     failures++;
   else if(9!=v2[2])
     failures++;
   else if(10!=v2[3])
     failures++;
   else if(11!=v2[4])
     failures++;
   else if(12!=v2[5])
     failures++;
   else if(13!=v2[6])
     failures++;
   else if(14!=v2[7])
     failures++;
   else if(15!=v2[8])
     failures++;
   else if(16!=v2[9])
     failures++;
  
  copy(v2.begin(), v2.end(), iter);
  cout << endl;
  set_union(v1.begin(), v1.end(), v2.begin(), v2.end(),v3.begin());
  
   if(0!=v3[0])
     failures++;
   else if(1!=v3[1])
     failures++;
   else if(2!=v3[2])
     failures++;
   else if(3!=v3[3])
     failures++;
   else if(4!=v3[4])
     failures++;
   else if(5!=v3[5])
     failures++;
   else if(6!=v3[6])
     failures++;
   else if(7!=v3[7])
     failures++;
   else if(8!=v3[8])
     failures++;
   else if(9!=v3[9])
     failures++;
   else if(10!=v3[10])
     failures++;
   else if(11!=v3[11])
     failures++;
   else if(12!=v3[12])
     failures++;
   else if(13!=v3[13])
     failures++;
   else if(14!=v3[14])
     failures++;
   else if(15!=v3[15])
     failures++;
   else if(16!=v3[16])
     failures++;
     
  copy(v3.begin(),v3.end(),iter);

  cout << endl;
  
  if(failures)
    return 1;
  else  
    return 0;
}
