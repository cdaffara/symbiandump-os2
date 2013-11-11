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

#ifndef _unary_h
#define _unary_h
#include <cmath>
#include <cfloat>
#include <functional>		//*TY 12/26/1998 - added to get unary_function

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using std::unary_function;
#endif

struct odd : public unary_function<int, bool>
{
//  odd() {}
  bool operator()(int n_) const { return(n_ % 2) == 1; }
};
 
struct positive : public unary_function<int, bool>
{
  typedef int argument_type;
  typedef bool result_type;
//  positive() {}
  bool operator()(int n_) const { return n_ >= 0; }
};

struct square_root : public unary_function<double, double>
{
  typedef double argument_type;
  typedef double result_type;
  square_root() {}
  square_root(const square_root &) {}
  double operator()(double x_) const { 
  # if !defined(STLPORT) || defined (__STL_USE_NAMESPACES)
    using namespace std;
  # endif
    return sqrt(x_); 
  }
};
#endif // _unary_h
