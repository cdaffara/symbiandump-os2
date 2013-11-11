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

#ifndef _fib_h
#define _fib_h
class Fibonacci
{
  public:
    Fibonacci() : v1(0), v2(1) {}
    inline int operator()();
  private:
    int v1;
    int v2;
};

inline int
Fibonacci::operator()()
{
  int r = v1 + v2;
  v1 = v2;
  v2 = r;
  return v1;
}
#endif // _fib_h
