
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <numeric>
#include <iostream>



#ifdef MAIN 
#define adjdiff0_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

int adjdiff0_test(int, char**)
{
 
  
  cout<<"Results of adjdiff0_test:"<<endl;  
  int numbers[5] = { 1, 2, 4, 8, 16 };
  
  // To compare the results
  int failures = 0;
  int arr[5];
  int k;
  for(k =0; k < 5; k++)
  	arr[k] = numbers[k];	
  
  int difference[5];
  adjacent_difference(numbers, numbers + 5, (int*)difference);
  int i;
  for(i = 0; i < 5; i++)
  {
  	cout << numbers[i] << ' ';
  	//check the number in array with the initial arr
  	if(numbers[i] != arr[i])
  		failures++;
  }    
  cout << endl;
  
  for(i = 0; i < 5; i++)
    cout << difference[i] << ' ';
  cout << endl;
  
  //check the difference array
  if((difference[0] != 1) || (difference[1] != 1) ||
     (difference[2] != 2) || (difference[3] != 4) ||
     (difference[4] != 8))
     failures++;
  
  if (!failures)
  	   return 0;
  else
  return 1;
}
