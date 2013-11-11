/*
*
* © Portions copyright (c) 2006 Nokia Corporation.
* All rights reserved.
*
*/

 
#include <string>

#ifdef __SYMBIAN32__
int strass1_test(int, char**)
#else
int main()
#endif
      {
      std::string str1 = "string";
      std::string str2;
      str2.assign(str1.begin(), str1.begin() + 3);
           
      if(str2.compare("str")!=0)
        return 1;
      else
        return 0;
      }
