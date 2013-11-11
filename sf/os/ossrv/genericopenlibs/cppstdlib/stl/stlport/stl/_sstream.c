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

#ifndef _STLP_SSTREAM_C
#define _STLP_SSTREAM_C

#ifndef _STLP_INTERNAL_SSTREAM
#  include <stl/_sstream.h>
#endif

#if defined ( _STLP_NESTED_TYPE_PARAM_BUG )
// no wint_t is supported for this mode
#  define __BSB_int_type__ int
#  define __BSB_pos_type__ streampos
#else
#  define __BSB_int_type__ _STLP_TYPENAME_ON_RETURN_TYPE basic_stringbuf<_CharT, _Traits, _Alloc>::int_type
#  define __BSB_pos_type__ _STLP_TYPENAME_ON_RETURN_TYPE basic_stringbuf<_CharT, _Traits, _Alloc>::pos_type
#endif

_STLP_BEGIN_NAMESPACE

//----------------------------------------------------------------------
// Non-inline stringbuf member functions.

// Constructors.  Note that the base class constructor sets all of the
// get and area pointers to null.

template <class _CharT, class _Traits, class _Alloc>
basic_stringbuf<_CharT, _Traits, _Alloc>
  ::basic_stringbuf(ios_base::openmode __mode)
    : basic_streambuf<_CharT, _Traits>(), _M_mode(__mode), _M_str()
{}

template <class _CharT, class _Traits, class _Alloc>
basic_stringbuf<_CharT, _Traits, _Alloc>
  ::basic_stringbuf(const basic_string<_CharT, _Traits, _Alloc>& __s, ios_base::openmode __mode)
    : basic_streambuf<_CharT, _Traits>(), _M_mode(__mode), _M_str(__s)
{
  _M_set_ptrs();
}



// Set the underlying string to a new value.
template <class _CharT, class _Traits, class _Alloc>
void
basic_stringbuf<_CharT, _Traits, _Alloc>::str(const basic_string<_CharT, _Traits, _Alloc>& __s)
{
  _M_str = __s;
  _M_set_ptrs();
}

template <class _CharT, class _Traits, class _Alloc>
void
basic_stringbuf<_CharT, _Traits, _Alloc>::_M_set_ptrs() {
  _CharT* __data_ptr = __CONST_CAST(_CharT*,_M_str.data());
  _CharT* __data_end = __data_ptr + _M_str.size();
  // The initial read position is the beginning of the string.
  if (_M_mode & ios_base::in) {
    if (_M_mode & ios_base::ate)
      this->setg(__data_ptr, __data_end, __data_end);
    else
      this->setg(__data_ptr, __data_ptr, __data_end);
  }

  // The initial write position is the beginning of the string.
  if (_M_mode & ios_base::out) {
    if (_M_mode & (ios_base::app | ios_base::ate))
      this->setp(__data_end, __data_end);
    else
      this->setp(__data_ptr, __data_end);
  }
}




// This is declared as a const member function because it is
// called by basic_stringbuf<>::str().  Precondition: this is a
// write-only stringbuf.  We can't use an output buffer for read-
// write stringbufs.  Postcondition: pptr is reset to the beginning
// of the buffer.
template <class _CharT, class _Traits, class _Alloc>
void basic_stringbuf<_CharT, _Traits, _Alloc>::_M_append_buffer() const {
  // Do we have a buffer to append?
  if (this->pbase() == this->_M_Buf && this->pptr() != this->_M_Buf) {
    basic_stringbuf<_CharT, _Traits, _Alloc>* __this = __CONST_CAST(_Self*,this);
    __this->_M_str.append((const _CharT*)this->pbase(), (const _CharT*)this->pptr());
#ifndef __MWERKS__
    __this->setp(__CONST_CAST(_CharT*,_M_Buf),
                 __CONST_CAST(_CharT*,_M_Buf + __STATIC_CAST(int,_S_BufSiz)));
#else // CodeWarrior treat const char * and const char [8] as different types
    __this->setp((_CharT*)_M_Buf,
                 (_CharT*)(_M_Buf + __STATIC_CAST(int,_S_BufSiz)));
#endif
  }

  // Have we run off the end of the string?
  else if (this->pptr() == this->epptr()) {
    basic_stringbuf<_CharT, _Traits, _Alloc>* __this = __CONST_CAST(_Self*,this);
#ifndef __MWERKS__
    __this->setp(__CONST_CAST(_CharT*,_M_Buf),
                 __CONST_CAST(_CharT*,_M_Buf + __STATIC_CAST(int,_S_BufSiz)));
#else // CodeWarrior treat const char * and const char [8] as different types
    __this->setp((_CharT*)_M_Buf,
                 (_CharT*)(_M_Buf + __STATIC_CAST(int,_S_BufSiz)));
#endif
  }
}

//----------------------------------------------------------------------
// Non-inline istringstream member functions.

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(ios_base::openmode __mode)
    : basic_istream<_CharT, _Traits>(0),
      _M_buf(__mode | ios_base::in) {
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(const _String& __str,ios_base::openmode __mode)
    : basic_istream<_CharT, _Traits>(0),
      _M_buf(__str, __mode | ios_base::in) {
  this->init(&_M_buf);
}



//----------------------------------------------------------------------
// Non-inline ostringstream member functions.

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(ios_base::openmode __mode)
    : basic_ostream<_CharT, _Traits>(0),
      _M_buf(__mode | ios_base::out) {
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(const _String& __str, ios_base::openmode __mode)
    : basic_ostream<_CharT, _Traits>(0),
      _M_buf(__str, __mode | ios_base::out) {
  this->init(&_M_buf);
}



//----------------------------------------------------------------------
// Non-inline stringstream member functions.

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(ios_base::openmode __mode)
    : basic_iostream<_CharT, _Traits>(0), _M_buf(__mode) {
   this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(const _String& __str, ios_base::openmode __mode)
    : basic_iostream<_CharT, _Traits>(0), _M_buf(__str, __mode) {
  this->init(&_M_buf);
}






_STLP_END_NAMESPACE

# undef __BSB_int_type__
# undef __BSB_pos_type__

#endif /* _STLP_SSTREAM_C */

// Local Variables:
// mode:C++
// End:
