/*
 * Portions Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 *
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

// Implementation of the classes in header <strstream>.
// WARNING: The classes defined in <strstream> are DEPRECATED.  This
// header is defined in section D.7.1 of the C++ standard, and it
// MAY BE REMOVED in a future standard revision.  You should use the
// header <sstream> instead.

#include "stlport_prefix.h"

#include <strstream>
#include <algorithm>
#include <limits>

_STLP_BEGIN_NAMESPACE

// strstreambuf constructor, destructor.
_STLP_DECLSPEC strstreambuf::strstreambuf(streamsize initial_capacity)
   : _M_alloc_fun(0), _M_free_fun(0),
     _M_dynamic(true), _M_frozen(false), _M_constant(false) {
  size_t n = (sizeof(streamsize) > sizeof(size_t)) ? __STATIC_CAST(size_t, (min)(__STATIC_CAST(streamsize, (numeric_limits<size_t>::max)()),
                                                                                 (max)(initial_capacity, streamsize(16))))
                                                   : __STATIC_CAST(size_t, (max)(initial_capacity, streamsize(16)));

  char* buf = _M_alloc(n);
  if (buf) {
    setp(buf, buf + n);
    setg(buf, buf, buf);
  }
}

_STLP_DECLSPEC strstreambuf::strstreambuf(__alloc_fn alloc_f, __free_fn free_f)
  : _M_alloc_fun(alloc_f), _M_free_fun(free_f),
    _M_dynamic(true), _M_frozen(false), _M_constant(false) {
  size_t n = 16;

  char* buf = _M_alloc(n);
  if (buf) {
    setp(buf, buf + n);
    setg(buf, buf, buf);
  }
}

_STLP_DECLSPEC strstreambuf::strstreambuf(char* get, streamsize n, char* put)
  : _M_alloc_fun(0), _M_free_fun(0),
    _M_dynamic(false), _M_frozen(false), _M_constant(false) {
  _M_setup(get, put, n);
}

_STLP_DECLSPEC strstreambuf::strstreambuf(signed char* get, streamsize n, signed char* put)
  : _M_alloc_fun(0), _M_free_fun(0),
    _M_dynamic(false), _M_frozen(false), _M_constant(false) {
  _M_setup(__REINTERPRET_CAST(char*,get), __REINTERPRET_CAST(char*,put), n);
}

_STLP_DECLSPEC strstreambuf::strstreambuf(unsigned char* get, streamsize n,
                           unsigned char* put)
  : _M_alloc_fun(0), _M_free_fun(0),
    _M_dynamic(false), _M_frozen(false), _M_constant(false) {
  _M_setup(__REINTERPRET_CAST(char*,get), __REINTERPRET_CAST(char*,put), n);
}

_STLP_DECLSPEC strstreambuf::strstreambuf(const char* get, streamsize n)
  : _M_alloc_fun(0), _M_free_fun(0),
    _M_dynamic(false), _M_frozen(false), _M_constant(true) {
  _M_setup(__CONST_CAST(char*,get), 0, n);
}

_STLP_DECLSPEC strstreambuf::strstreambuf(const signed char* get, streamsize n)
  : _M_alloc_fun(0), _M_free_fun(0),
    _M_dynamic(false), _M_frozen(false), _M_constant(true) {
  _M_setup(__REINTERPRET_CAST(char*, __CONST_CAST(signed char*,get)), 0, n);
}

_STLP_DECLSPEC strstreambuf::strstreambuf(const unsigned char* get, streamsize n)
  : _M_alloc_fun(0), _M_free_fun(0),
    _M_dynamic(false), _M_frozen(false), _M_constant(true) {
  _M_setup(__REINTERPRET_CAST(char*, __CONST_CAST(unsigned char*,get)), 0, n);
}

_STLP_DECLSPEC strstreambuf::~strstreambuf() {
  if (_M_dynamic && !_M_frozen)
    _M_free(eback());
}

_STLP_DECLSPEC void strstreambuf::freeze(bool frozenflag) {
  if (_M_dynamic)
    _M_frozen = frozenflag;
}

_STLP_DECLSPEC char* strstreambuf::str() {
  freeze(true);
  return eback();
}

_STLP_DECLSPEC int strstreambuf::pcount() const {
  return int(pptr() ? pptr() - pbase() : 0);
}

_STLP_DECLSPEC strstreambuf::int_type strstreambuf::overflow(int_type c) {
  if (c == traits_type::eof())
    return traits_type::not_eof(c);

  // Try to expand the buffer.
  if (pptr() == epptr() && _M_dynamic && !_M_frozen && !_M_constant) {
    ptrdiff_t old_size = epptr() - pbase();
    ptrdiff_t new_size = (max)(2 * old_size, ptrdiff_t(1));

    char* buf = _M_alloc(new_size);
    if (buf) {
      memcpy(buf, pbase(), old_size);

      char* old_buffer = pbase();
      bool reposition_get = false;
      ptrdiff_t old_get_offset;
      if (gptr() != 0) {
        reposition_get = true;
        old_get_offset = gptr() - eback();
      }

      setp(buf, buf + new_size);
      pbump((int)old_size);

      if (reposition_get)
        setg(buf, buf + old_get_offset, buf + (max)(old_get_offset, old_size));

      _M_free(old_buffer);
    }
  }

  if (pptr() != epptr()) {
    *pptr() = traits_type::to_char_type(c);
    pbump(1);
    return c;
  }
  else
    return traits_type::eof();
}

_STLP_DECLSPEC strstreambuf::int_type strstreambuf::pbackfail(int_type c) {
  if (gptr() != eback()) {
    if (c == traits_type::eof()) {
      gbump(-1);
      return traits_type::not_eof(c);
    }
    else if (c == gptr()[-1]) {
      gbump(-1);
      return c;
    }
    else if (!_M_constant) {
      gbump(-1);
      *gptr() = traits_type::to_char_type(c);
      return c;
    }
  }

  return traits_type::eof();
}

_STLP_DECLSPEC strstreambuf::int_type strstreambuf::underflow() {
  if (gptr() == egptr() && pptr() && pptr() > egptr())
    setg(eback(), gptr(), pptr());

  if (gptr() != egptr())
    return (unsigned char) *gptr();
  else
    return _Traits::eof();
}

_STLP_DECLSPEC basic_streambuf<char, char_traits<char> >*
strstreambuf::setbuf(char*, streamsize) {
  return this;
}

_STLP_DECLSPEC strstreambuf::pos_type
strstreambuf::seekoff(off_type off,
                      ios_base::seekdir dir, ios_base::openmode mode) {
  bool do_get = false;
  bool do_put = false;

  if ((mode & (ios_base::in | ios_base::out)) ==
          (ios_base::in | ios_base::out) &&
      (dir == ios_base::beg || dir == ios_base::end))
    do_get = do_put = true;
  else if (mode & ios_base::in)
    do_get = true;
  else if (mode & ios_base::out)
    do_put = true;

  // !gptr() is here because, according to D.7.1 paragraph 4, the seekable
  // area is undefined if there is no get area.
  if ((!do_get && !do_put) || (do_put && !pptr()) || !gptr())
    return pos_type(off_type(-1));

  char* seeklow  = eback();
  char* seekhigh = epptr() ? epptr() : egptr();

  off_type newoff;
  switch(dir) {
  case ios_base::beg:
    newoff = 0;
    break;
  case ios_base::end:
    newoff = seekhigh - seeklow;
    break;
  case ios_base::cur:
    newoff = do_put ? pptr() - seeklow : gptr() - seeklow;
    break;
  default:
    return pos_type(off_type(-1));
  }

  off += newoff;
  if (off < 0 || off > seekhigh - seeklow)
    return pos_type(off_type(-1));

  if (do_put) {
    if (seeklow + __STATIC_CAST(ptrdiff_t, off) < pbase()) {
      setp(seeklow, epptr());
      pbump((int)off);
    }
    else {
      setp(pbase(), epptr());
      pbump((int)(off - (pbase() - seeklow)));
    }
  }
  if (do_get) {
    if (off <= egptr() - seeklow)
      setg(seeklow, seeklow + __STATIC_CAST(ptrdiff_t, off), egptr());
    else if (off <= pptr() - seeklow)
      setg(seeklow, seeklow + __STATIC_CAST(ptrdiff_t, off), pptr());
    else
      setg(seeklow, seeklow + __STATIC_CAST(ptrdiff_t, off), epptr());
  }

  return pos_type(newoff);
}

_STLP_DECLSPEC strstreambuf::pos_type
strstreambuf::seekpos(pos_type pos, ios_base::openmode mode) {
  return seekoff(pos - pos_type(off_type(0)), ios_base::beg, mode);
}


char* strstreambuf::_M_alloc(size_t n) {
  if (_M_alloc_fun)
    return __STATIC_CAST(char*,_M_alloc_fun(n));
  else
    return new char[n];
}

void strstreambuf::_M_free(char* p) {
  if (p)
    if (_M_free_fun)
      _M_free_fun(p);
    else
      delete[] p;
}

void strstreambuf::_M_setup(char* get, char* put, streamsize n) {
  if (get) {
    size_t N = n > 0 ? size_t(n) : n == 0 ? strlen(get) : size_t(INT_MAX);

    if (put) {
      setg(get, get, get + N);
      setp(put, put + N);
    }
    else {
      setg(get, get, get + N);
    }
  }
}

//----------------------------------------------------------------------
// Class istrstream

_STLP_DECLSPEC istrstream::istrstream(char* s)
  : basic_istream<char, char_traits<char> >(0), _M_buf(s, 0) {
  this->init(&_M_buf);
}

_STLP_DECLSPEC istrstream::istrstream(const char* s)
  : basic_istream<char, char_traits<char> >(0), _M_buf(s, 0) {
  this->init(&_M_buf);
}

_STLP_DECLSPEC istrstream::istrstream(char* s, streamsize n)
  : basic_istream<char, char_traits<char> >(0), _M_buf(s, n) {
  this->init(&_M_buf);
}

_STLP_DECLSPEC istrstream::istrstream(const char* s, streamsize n)
  : basic_istream<char, char_traits<char> >(0), _M_buf(s, n) {
  this->init(&_M_buf);
}

_STLP_DECLSPEC istrstream::~istrstream() {}

_STLP_DECLSPEC strstreambuf* istrstream::rdbuf() const {
  return __CONST_CAST(strstreambuf*,&_M_buf);
}

_STLP_DECLSPEC char* istrstream::str() { return _M_buf.str(); }

//----------------------------------------------------------------------
// Class ostrstream

_STLP_DECLSPEC ostrstream::ostrstream()
  : basic_ostream<char, char_traits<char> >(0), _M_buf() {
  basic_ios<char, char_traits<char> >::init(&_M_buf);
}

_STLP_DECLSPEC ostrstream::ostrstream(char* s, int n, ios_base::openmode mode)
  : basic_ostream<char, char_traits<char> >(0),
    _M_buf(s, n, mode & ios_base::app ? s + strlen(s) : s) {
  basic_ios<char, char_traits<char> >::init(&_M_buf);
}

_STLP_DECLSPEC ostrstream::~ostrstream() {}

_STLP_DECLSPEC strstreambuf* ostrstream::rdbuf() const {
  return __CONST_CAST(strstreambuf*,&_M_buf);
}

_STLP_DECLSPEC void ostrstream::freeze(bool freezeflag) {
  _M_buf.freeze(freezeflag);
}

_STLP_DECLSPEC char* ostrstream::str() {
  return _M_buf.str();
}

_STLP_DECLSPEC int ostrstream::pcount() const {
  return _M_buf.pcount();
}


//----------------------------------------------------------------------
// Class strstream

_STLP_DECLSPEC strstream::strstream()
  : basic_iostream<char, char_traits<char> >(0), _M_buf() {
  basic_ios<char, char_traits<char> >::init(&_M_buf);
}

_STLP_DECLSPEC strstream::strstream(char* s, int n, ios_base::openmode mode)
  : basic_iostream<char, char_traits<char> >(0),
    _M_buf(s, n, mode & ios_base::app ? s + strlen(s) : s) {
  basic_ios<char, char_traits<char> >::init(&_M_buf);
}

_STLP_DECLSPEC strstream::~strstream() {}

_STLP_DECLSPEC strstreambuf* strstream::rdbuf() const {
  return __CONST_CAST(strstreambuf*,&_M_buf);
}

_STLP_DECLSPEC void strstream::freeze(bool freezeflag) {
  _M_buf.freeze(freezeflag);
}

_STLP_DECLSPEC int strstream::pcount() const {
  return _M_buf.pcount();
}

_STLP_DECLSPEC char* strstream::str() {
  return _M_buf.str();
}

_STLP_END_NAMESPACE

// Local Variables:
// mode:C++
// End:
