/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>

#ifdef MAIN 
#define istmit1_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int istmit1_test(int, char**)
{
  cout<<"Results of istmit1_test:"<<endl;
  char buffer [100];
  int i = 0;
  int failures = 0;
  cin.unsetf(ios::skipws); // Disable white-space skipping.
  cout << "Please enter a string: ";
#ifndef _STLP_LIMITED_DEFAULT_TEMPLATES
  istream_iterator<char> s(cin),meos;		//*TY 01/10/1999 - added eos()
#else
  istream_iterator<char, ptrdiff_t> s(cin),meos;		//*TY 01/10/1999 - added eos()
#endif
  while(!(s == meos)  &&		
	//*TY 01/10/1999 - added end of stream check 
	// NOTE operator!= should not be used here ifndef __STL_FUNCTION_TMPL_PARTIAL_ORDER
  		*s != '\n' &&
  		(i < sizeof(buffer)/sizeof(buffer[0])) )		//*TY 07/28/98 - added index check
    buffer[i++] = *s++;
  buffer[i] = '\0'; // Null terminate buffer.
  cout << "read " << buffer << endl;
   while(!(s == meos)  &&  *s != '\n' && (i < sizeof(buffer)/sizeof(buffer[0])))
   {
   	  if(buffer[i++] != *s++)
   	  failures++;
   }
  return failures;
}
