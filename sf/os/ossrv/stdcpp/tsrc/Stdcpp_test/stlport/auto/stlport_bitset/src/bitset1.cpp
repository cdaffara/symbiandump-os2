 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.


#include <stdexcept>
#include <iostream>
#include <bitset>
#include <algorithm>



#if defined (_STLP_MSVC) && (_MSC_VER < 1300)
# define _STLP_NON_TYPE_TMPL_PARAM_BUG
# define _STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS
#endif

# ifdef _STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS
# define CHART
# else
# define CHART <char, char_traits<char>, allocator<char> >
# endif

#ifdef MAIN
#define bitset1_test main
#endif

#if !defined (STLPORT) || defined(_STLP_USE_NAMESPACES)
using namespace std;
#endif

int bitset1_test(int, char**)
{
  cout<<"Results of bitset1_test:"<<endl;
  int failures = 0;

# if ! (defined (_STLP_MSVC) && (_MSC_VER < 1300)) && ! (defined (__SUNPRO_CC) && (__SUNPRO_CC < 0x500))
  bitset<13U> b1(0xFFFF);
  bitset<13U> b2(0x1111);
  // Boris : MSVC just cannot take it right
  cout << "b1 size = " << b1.size() << endl;
  //check for failure
  if (b1.size() != 13)
  	failures++;
# if !defined (_STLP_NON_TYPE_TMPL_PARAM_BUG)
  cout << "b1 = "<<b1<<endl;
# else
  cout << "b1 = "<<b1.to_string CHART ()<<endl;
# endif
  if (b1 != 0xFFFF)
  	failures++;

  cout << "b2 size = " << b2.size() << endl;
  if (b2.size() != 13)
  	failures++;
# if defined (_STLP_NON_TYPE_TMPL_PARAM_BUG)
  cout << "b2 = "<<b2.to_string CHART ()<<endl;
# else
  cout << "b2 = "<<b2<<endl;
# endif
  if (b2 != 0x1111)
  	failures++;

  b1 = b1^(b2<<2);
# ifdef _STLP_NON_TYPE_TMPL_PARAM_BUG
  cout << "b1 = "<<b1.to_string CHART ()<<endl;
# else
  cout << "b1 = "<<b1<<endl;
# endif
  if(b1 != 0x1BBB)
  	failures++;
# endif /* MSVC */

  if (!failures)
    return 0;
  else
    return 1;
}
