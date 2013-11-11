/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
 */

#ifndef IOTA_H
#define IOTA_H

#include <numeric>

#ifdef __SYMBIAN32__
using std::iota;
#endif // __SYMBIAN32__

//iota definition used in unit test
template <typename _It, typename _Tp>
void __iota(_It __first, _It __last, _Tp __val) {
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  iota(__first, __last, __val);
#else
  while (__first != __last) {
    *__first++ = __val++;
  }
#endif
}

#endif
