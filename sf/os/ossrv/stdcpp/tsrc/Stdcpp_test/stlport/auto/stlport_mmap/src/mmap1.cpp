 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <map>
#include <iostream>

#ifdef MAIN 
#define mmap1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int mmap1_test(int, char**)
{
  cout<<"Results of mmap1_test:"<<endl;
  typedef multimap<char, int, less<char> > mmap;
  mmap m;
  int failures = 0;
  
  cout << "count('X') = " << m.count('X') << endl;
  if(m.count('X') != 0)
  failures++;
  
  m.insert(pair<const char, int>('X', 10)); // Standard way.
  cout << "count('X') = " << m.count('X') << endl;
  //  m.insert('X', 20); // Non-standard, but very convenient!
  if(m.count('X') != 1)
  failures++;
  
  m.insert(pair<const char, int>('X', 20));	// jbuck: standard way
  cout << "count('X') = " << m.count('X') << endl;
//  m.insert('Y', 32);
if(m.count('X') != 2)
  failures++;
  m.insert(pair<const char, int>('Y', 32));	// jbuck: standard way
  mmap::iterator i = m.find('X'); // Find first match.
  while(i != m.end()) // Loop until end is reached.
  {
    cout <<(*i).first << " -> " <<(*i).second << endl;
    i++;
  }
  int count = m.erase('X');
  cout << "Erased " << count << " items" << endl;
  if (count != 2)
   failures++;
  
  return failures;
}
