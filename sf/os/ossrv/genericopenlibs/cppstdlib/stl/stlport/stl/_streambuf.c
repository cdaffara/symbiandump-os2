/*
 * Copyright (c) 1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1999
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */
#ifndef _STLP_STREAMBUF_C
#define _STLP_STREAMBUF_C

#ifndef _STLP_INTERNAL_STREAMBUF
#  include <stl/_streambuf.h>
#endif

_STLP_BEGIN_NAMESPACE
//----------------------------------------------------------------------
// Non-inline basic_streambuf<> member functions.

#if !defined (_STLP_MSVC) || (_STLP_MSVC >= 1300) || !defined (_STLP_USE_STATIC_LIB)
template <class _CharT, class _Traits>
basic_streambuf<_CharT, _Traits>::basic_streambuf()
  : _M_gbegin(0), _M_gnext(0), _M_gend(0),
    _M_pbegin(0), _M_pnext(0), _M_pend(0),
    _M_locale() {
  //  _M_lock._M_initialize();
}
#endif



template <class _CharT, class _Traits>
locale
basic_streambuf<_CharT, _Traits>::pubimbue(const locale& __loc) {
  this->imbue(__loc);
  locale __tmp = _M_locale;
  _M_locale = __loc;
  return __tmp;
}



template <class _CharT, class _Traits>
_STLP_TYPENAME_ON_RETURN_TYPE basic_streambuf<_CharT, _Traits>::int_type
basic_streambuf<_CharT, _Traits>::_M_snextc_aux()
{
  int_type __eof = _Traits::eof();
  if (_M_gend == _M_gnext)
    return _Traits::eq_int_type(this->uflow(), __eof) ? __eof : this->sgetc();
  else {
    _M_gnext = _M_gend;
    return this->underflow();
  }
}



_STLP_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:
