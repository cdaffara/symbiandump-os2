  
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
#define prevprm1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int prevprm1_test(int, char**)
{
  int failures=0;
  cout<<"Results of prevprm1_test:"<<endl;
  vector <int> v1(3);
  iota(v1.begin(), v1.end(), 0);
  
  ostream_iterator<int> iter(cout, " ");
  copy(v1.begin(), v1.end(), iter);
  cout << endl;
  
   if(0!=v1[0])
     failures++;
   else if(1 !=v1[1])
     failures++;
   else if(2!=v1[2])
     failures++;
         

  for(int i = 0; i < 9; i++)
  {
    prev_permutation(v1.begin(), v1.end());
    copy(v1.begin(), v1.end(), iter);
     switch(i)
     {
       case 0: if(2 !=v1[0])
                 failures++;
               else if(1 !=v1[1])
                 failures++;
               else if(0 !=v1[2])
                 failures++;
                break;                
       case 1: if(2 !=v1[0])
                 failures++;
               else if(0 !=v1[1])
                 failures++;
               else if(1 !=v1[2])
                 failures++;
                     break;        
      case 2: if(1 !=v1[0])
                 failures++;
               else if(2 !=v1[1])
                 failures++;
               else if(0 !=v1[2])
                 failures++;    
                   break;        
     case 3: if(1 !=v1[0])
                 failures++;
               else if(0 !=v1[1])
                 failures++;
               else if(2 !=v1[2])
                 failures++;  
               break;        
   case 4: if(0 !=v1[0])
                 failures++;
               else if(2 !=v1[1])
                 failures++;
               else if(1 !=v1[2])
                 failures++;  
                       break;                 
                 
     case 5: if(0 !=v1[0])
                 failures++;
               else if(1 !=v1[1])
                 failures++;
               else if(2 !=v1[2])
                 failures++;  
                   break;        
    
    case 6: if(2 !=v1[0])
                 failures++;
               else if(1 !=v1[1])
                 failures++;
               else if(0 !=v1[2])
                 failures++;  
                   break;             
    
    case 7: if(2 !=v1[0])
                 failures++;
               else if(0 !=v1[1])
                 failures++;
               else if(1 !=v1[2])
                 failures++;               
                   break;                     
    
     case 8: if(1 !=v1[0])
                 failures++;
               else if(2 !=v1[1])
                 failures++;
               else if(0 !=v1[2])
                 failures++;  
                  break;        
   
   }
     cout << endl;
     }
 
  if(failures)
     return 1;
  else
    return 0;
}

