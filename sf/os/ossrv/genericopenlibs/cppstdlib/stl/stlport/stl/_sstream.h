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


// This header defines classes basic_stringbuf, basic_istringstream,
// basic_ostringstream, and basic_stringstream.  These classes
// represent streamsbufs and streams whose sources or destinations are
// C++ strings.

#ifndef _STLP_INTERNAL_SSTREAM
#define _STLP_INTERNAL_SSTREAM

#ifndef _STLP_INTERNAL_STREAMBUF
#  include <stl/_streambuf.h>
#endif

#ifndef _STLP_INTERNAL_ISTREAM
#  include <stl/_istream.h> // Includes <ostream>, <ios>, <iosfwd>
#endif

#ifndef _STLP_INTERNAL_STRING_H
#  include <stl/_string.h>
#endif

_STLP_BEGIN_NAMESPACE

//----------------------------------------------------------------------
// This version of basic_stringbuf relies on the internal details of
// basic_string.  It relies on the fact that, in this implementation,
// basic_string's iterators are pointers.  It also assumes (as allowed
// by the standard) that _CharT is a POD type.

// We have a very small buffer for the put area, just so that we don't
// have to use append() for every sputc.  Conceptually, the buffer
// immediately follows the end of the underlying string.  We use this
// buffer when appending to write-only streambufs, but we don't use it
// for read-write streambufs.

template <class _CharT, class _Traits, class _Alloc>
class basic_stringbuf : public basic_streambuf<_CharT, _Traits> {
public:                         // Typedefs.
  typedef _CharT                     char_type;
  typedef typename _Traits::int_type int_type;
  typedef typename _Traits::pos_type pos_type;
  typedef typename _Traits::off_type off_type;
  typedef _Traits                    traits_type;

  typedef basic_streambuf<_CharT, _Traits>          _Base;
  typedef basic_stringbuf<_CharT, _Traits, _Alloc>  _Self;
  typedef basic_string<_CharT, _Traits, _Alloc>     _String;

public:                         // Constructors, destructor.
  explicit basic_stringbuf(ios_base::openmode __mode
                                      = ios_base::in | ios_base::out);
  explicit basic_stringbuf(const _String& __s, ios_base::openmode __mode
                                      = ios_base::in | ios_base::out);
  virtual ~basic_stringbuf(){}

public:                         // Get or set the string.
  _String str() const { _M_append_buffer(); return _M_str; }
  void str(const _String& __s);

protected:                      // Overridden virtual member functions.
  
  // Precondition: gptr() >= egptr().  Returns a character, if one is available.
  virtual int_type underflow() {
    return this->gptr() != this->egptr()
      ? _Traits::to_int_type(*this->gptr())
      : _Traits::eof();
  }
  
  virtual int_type uflow() {
      if (this->gptr() != this->egptr()) {
        int_type __c = _Traits::to_int_type(*this->gptr());
        this->gbump(1);
        return __c;
      }
      else
        return _Traits::eof();
  }
  
  virtual int_type pbackfail(int_type __c) {
      if (this->gptr() != this->eback()) {
        if (!_Traits::eq_int_type(__c, _Traits::eof())) {
          if (_Traits::eq(_Traits::to_char_type(__c), this->gptr()[-1])) {
            this->gbump(-1);
            return __c;
          }
          else if (_M_mode & ios_base::out) {
            this->gbump(-1);
            *this->gptr() = _Traits::to_char_type(__c);
            return __c;
          }
          else
            return _Traits::eof();
        }
        else {
          this->gbump(-1);
          return _Traits::not_eof(__c);
        }
      }
      else
        return _Traits::eof();
  }
  
  virtual int_type overflow(int_type __c) {
      // fbp : reverse order of "ifs" to pass Dietmar's test.
      // Apparently, standard allows overflow with eof even for read-only streams.
      if (!_Traits::eq_int_type(__c, _Traits::eof())) {
        if (_M_mode & ios_base::out) {
          if (!(_M_mode & ios_base::in)) {
            // It's a write-only streambuf, so we can use special append buffer.
            if (this->pptr() == this->epptr())
              this->_M_append_buffer();

            if (this->pptr() != this->epptr()) {
              *this->pptr() = _Traits::to_char_type(__c);
              this->pbump(1);
              return __c;
            }
            else
              return _Traits::eof();
          }
          else {
            // We're not using a special append buffer, just the string itself.
            if (this->pptr() == this->epptr()) {
              ptrdiff_t __offset = this->gptr() - this->eback();
              _M_str.push_back(_Traits::to_char_type(__c));

              _CharT* __data_ptr = __CONST_CAST(_CharT*,_M_str.data());
              size_t __data_size = _M_str.size();

              this->setg(__data_ptr, __data_ptr + __offset, __data_ptr+__data_size);
              this->setp(__data_ptr, __data_ptr + __data_size);
              this->pbump((int)__data_size);
              return __c;
            }
            else {
              *this->pptr() = _Traits::to_char_type(__c);
              this->pbump(1);
              return __c;
            }
          }
        }
        else                          // Overflow always fails if it's read-only
          return _Traits::eof();
      }
      else                        // __c is EOF, so we don't have to do anything
        return _Traits::not_eof(__c);
  }

  int_type pbackfail() {return pbackfail(_Traits::eof());}
  int_type overflow() {return overflow(_Traits::eof());}

  
  virtual streamsize xsputn(const char_type* __s, streamsize __n) {
      streamsize __nwritten = 0;

      if ((_M_mode & ios_base::out) && __n > 0) {
        // If the put pointer is somewhere in the middle of the string,
        // then overwrite instead of append.
        if (this->pbase() == _M_str.data() ) {
          ptrdiff_t __avail = _M_str.data() + _M_str.size() - this->pptr();
          if (__avail > __n) {
            _Traits::copy(this->pptr(), __s, __STATIC_CAST(size_t, __n));
            this->pbump((int)__n);
            return __n;
          }
          else {
            _Traits::copy(this->pptr(), __s, __avail);
            __nwritten += __avail;
            __n -= __avail;
            __s += __avail;
            this->setp(_M_Buf, _M_Buf + __STATIC_CAST(int,_S_BufSiz));
          }
        }

        // At this point we know we're appending.
        if (_M_mode & ios_base::in) {
          ptrdiff_t __get_offset = this->gptr() - this->eback();
          _M_str.append(__s, __s + __STATIC_CAST(ptrdiff_t, __n));

          _CharT* __data_ptr = __CONST_CAST(_CharT*, _M_str.data());
          size_t __data_size = _M_str.size();

          this->setg(__data_ptr, __data_ptr + __get_offset, __data_ptr + __data_size);
          this->setp(__data_ptr, __data_ptr + __data_size);
          this->pbump((int)__data_size);
        }
        else {
          _M_append_buffer();
          _M_str.append(__s, __s + __STATIC_CAST(ptrdiff_t, __n));
        }

        __nwritten += __n;
      }

      return __nwritten;
  }
  
  virtual streamsize _M_xsputnc(char_type __c, streamsize __n) {
      streamsize __nwritten = 0;

           if ((_M_mode & ios_base::out) && __n > 0) {
        // If the put pointer is somewhere in the middle of the string,
        // then overwrite instead of append.
        if (this->pbase() == _M_str.data()) {
          ptrdiff_t __avail = _M_str.data() + _M_str.size() - this->pptr();
          if (__avail > __n) {
            _Traits::assign(this->pptr(), __STATIC_CAST(size_t, __n), __c);
            this->pbump(__STATIC_CAST(int, __n));
            return __n;
          }
          else {
            _Traits::assign(this->pptr(), __avail, __c);
            __nwritten += __avail;
            __n -= __avail;
            this->setp(_M_Buf, _M_Buf + __STATIC_CAST(int,_S_BufSiz));
          }
        }


        // At this point we know we're appending.
        size_t __app_size = sizeof(streamsize) > sizeof(size_t) ? __STATIC_CAST(size_t, (min)(__n, __STATIC_CAST(streamsize, _M_str.max_size())))
                                                                : __STATIC_CAST(size_t, __n);
        if (this->_M_mode & ios_base::in) {
          ptrdiff_t __get_offset = this->gptr() - this->eback();
          _M_str.append(__app_size, __c);

          _CharT* __data_ptr = __CONST_CAST(_CharT*,_M_str.data());
          size_t __data_size = _M_str.size();

          this->setg(__data_ptr, __data_ptr + __get_offset, __data_ptr + __data_size);
          this->setp(__data_ptr, __data_ptr + __data_size);
          this->pbump((int)__data_size);
        }
        else {
          _M_append_buffer();
          _M_str.append(__app_size, __c);
        }

        __nwritten += __app_size;
      }

      return __nwritten;
  }
  
  virtual _Base* setbuf(_CharT*, streamsize __n) {
      if (__n > 0) {
        bool __do_get_area = false;
        bool __do_put_area = false;
        ptrdiff_t __offg = 0;
        ptrdiff_t __offp = 0;

        if (this->pbase() == _M_str.data()) {
          __do_put_area = true;
          __offp = this->pptr() - this->pbase();
        }

        if (this->eback() == _M_str.data()) {
          __do_get_area = true;
          __offg = this->gptr() - this->eback();
        }

        if ((_M_mode & ios_base::out) && !(_M_mode & ios_base::in))
          _M_append_buffer();

        _M_str.reserve(sizeof(streamsize) > sizeof(size_t) ? __STATIC_CAST(size_t, (min)(__n, __STATIC_CAST(streamsize, _M_str.max_size())))
                                                           : __STATIC_CAST(size_t, __n));

        _CharT* __data_ptr = __CONST_CAST(_CharT*, _M_str.data());
        size_t __data_size = _M_str.size();

        if (__do_get_area) {
          this->setg(__data_ptr, __data_ptr + __offg, __data_ptr + __data_size);
        }

        if (__do_put_area) {
          this->setp(__data_ptr, __data_ptr + __data_size);
          this->pbump((int)__offp);
        }
      }

      return this;
  }
  virtual pos_type seekoff(off_type __off, ios_base::seekdir __dir,
                             ios_base::openmode __mode
                                        = ios_base::in | ios_base::out) {
      __mode &= _M_mode;

      bool __imode  = (__mode & ios_base::in) != 0;
      bool __omode = (__mode & ios_base::out) != 0;

      if ( !(__imode || __omode) )
        return pos_type(off_type(-1));

      if ( (__imode && (this->gptr() == 0)) || (__omode && (this->pptr() == 0)) )
        return pos_type(off_type(-1));

    #ifndef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
      if ((_M_mode & ios_base::out) && !(_M_mode & ios_base::in))
        _M_append_buffer();
    #endif

      streamoff __newoff;
      switch(__dir) {
      case ios_base::beg:
        __newoff = 0;
        break;
      case ios_base::end:
        __newoff = _M_str.size();
        break;
      case ios_base::cur:
        __newoff = __imode ? this->gptr() - this->eback() : this->pptr() - this->pbase();
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
          if ( __off == 0 ) {
            return pos_type(__newoff);
          }
    #endif
        break;
      default:
        return pos_type(off_type(-1));
      }

      __off += __newoff;
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
      _CharT* __data_ptr = __CONST_CAST(_CharT*, _M_str.data());
      size_t __data_size = _M_str.size();
    #endif

      if (__imode) {
        ptrdiff_t __n = this->egptr() - this->eback();

        if (__off < 0 || __off > __n)
          return pos_type(off_type(-1));
        this->setg(this->eback(), this->eback() + __STATIC_CAST(ptrdiff_t, __off),
                                  this->eback() + __STATIC_CAST(ptrdiff_t, __n));
      }

      if (__omode) {
        ptrdiff_t __n = this->epptr() - this->pbase();

        if (__off < 0 || __off > __n)
          return pos_type(off_type(-1));
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
     this->setp(__data_ptr, __data_ptr+__data_size);
    #else
        this->setp(this->pbase(), this->pbase() + __n);
    #endif
        this->pbump((int)__off);
      }

      return pos_type(__off);
  }
  
    virtual pos_type seekpos(pos_type __pos, ios_base::openmode __mode
                                        = ios_base::in | ios_base::out) {
        __mode &= _M_mode;

        bool __imode  = (__mode & ios_base::in) != 0;
        bool __omode = (__mode & ios_base::out) != 0;

        if ( !(__imode || __omode) )
          return pos_type(off_type(-1));

        if ( (__imode && (this->gptr() == 0)) || (__omode && (this->pptr() == 0)) )
          return pos_type(off_type(-1));

        const off_type __n = __pos - pos_type(off_type(0));
        if ((_M_mode & ios_base::out) && !(_M_mode & ios_base::in))
          _M_append_buffer();

        if (__imode) {
          if (__n < 0 || __n > this->egptr() - this->eback())
            return pos_type(off_type(-1));
          this->setg(this->eback(), this->eback() + __STATIC_CAST(ptrdiff_t, __n), this->egptr());
        }

        if (__omode) {
          if (__n < 0 || size_t(__n) > _M_str.size())
            return pos_type(off_type(-1));

          _CharT* __data_ptr = __CONST_CAST(_CharT*,_M_str.data());
          size_t __data_size = _M_str.size();

          this->setp(__data_ptr, __data_ptr+__data_size);
          this->pbump((int)__n);
        }

        return __pos;
    }
  
                                                                            
private:                        // Helper functions.
  // Append the internal buffer to the string if necessary.
  void _M_append_buffer() const;
  void _M_set_ptrs();

private:
  ios_base::openmode _M_mode;
  mutable basic_string<_CharT, _Traits, _Alloc> _M_str;

  enum _JustName { _S_BufSiz = 8 };
  _CharT _M_Buf[ 8 /* _S_BufSiz */];
};

#if defined (_STLP_USE_TEMPLATE_EXPORT)
_STLP_EXPORT_TEMPLATE_CLASS basic_stringbuf<char, char_traits<char>, allocator<char> >;
#  if !defined (_STLP_NO_WCHAR_T)
_STLP_EXPORT_TEMPLATE_CLASS basic_stringbuf<wchar_t, char_traits<wchar_t>, allocator<wchar_t>  >;
#  endif
#endif /* _STLP_USE_TEMPLATE_EXPORT */

//----------------------------------------------------------------------
// Class basic_istringstream, an input stream that uses a stringbuf.

template <class _CharT, class _Traits, class _Alloc>
class basic_istringstream : public basic_istream<_CharT, _Traits> {
public:                         // Typedefs
  typedef typename _Traits::char_type   char_type;
  typedef typename _Traits::int_type    int_type;
  typedef typename _Traits::pos_type    pos_type;
  typedef typename _Traits::off_type    off_type;
  typedef _Traits traits_type;

  typedef basic_ios<_CharT, _Traits>                _Basic_ios;
  typedef basic_istream<_CharT, _Traits>            _Base;
  typedef basic_string<_CharT, _Traits, _Alloc>     _String;
  typedef basic_stringbuf<_CharT, _Traits, _Alloc>  _Buf;

public:                         // Constructors, destructor.
  basic_istringstream(ios_base::openmode __mode = ios_base::in);
  basic_istringstream(const _String& __str,
                      ios_base::openmode __mode = ios_base::in);
  ~basic_istringstream(){}

public:                         // Member functions

  basic_stringbuf<_CharT, _Traits, _Alloc>* rdbuf() const
    { return __CONST_CAST(_Buf*,&_M_buf); }

  _String str() const { return _M_buf.str(); }
  void str(const _String& __s) { _M_buf.str(__s); }

private:
  basic_stringbuf<_CharT, _Traits, _Alloc> _M_buf;

#if defined (_STLP_MSVC) && (_STLP_MSVC >= 1300 && _STLP_MSVC <= 1310)
  typedef basic_istringstream<_CharT, _Traits> _Self;
  //explicitely defined as private to avoid warnings:
  basic_istringstream(_Self const&);
  _Self& operator = (_Self const&);
#endif
};


//----------------------------------------------------------------------
// Class basic_ostringstream, an output stream that uses a stringbuf.

template <class _CharT, class _Traits, class _Alloc>
class basic_ostringstream : public basic_ostream<_CharT, _Traits> {
public:                         // Typedefs
  typedef typename _Traits::char_type   char_type;
  typedef typename _Traits::int_type    int_type;
  typedef typename _Traits::pos_type    pos_type;
  typedef typename _Traits::off_type    off_type;
  typedef _Traits traits_type;

  typedef basic_ios<_CharT, _Traits>                _Basic_ios;
  typedef basic_ostream<_CharT, _Traits>            _Base;
  typedef basic_string<_CharT, _Traits, _Alloc>     _String;
  typedef basic_stringbuf<_CharT, _Traits, _Alloc>  _Buf;

public:                         // Constructors, destructor.
  basic_ostringstream(ios_base::openmode __mode = ios_base::out);
  basic_ostringstream(const _String& __str,
                      ios_base::openmode __mode = ios_base::out);
  ~basic_ostringstream(){}

public:                         // Member functions.

  basic_stringbuf<_CharT, _Traits, _Alloc>* rdbuf() const
    { return __CONST_CAST(_Buf*,&_M_buf); }

  _String str() const { return _M_buf.str(); }
    void str(const _String& __s) { _M_buf.str(__s); } // dwa 02/07/00 - BUG STOMPER DAVE


private:
  basic_stringbuf<_CharT, _Traits, _Alloc> _M_buf;

#if defined (_STLP_MSVC) && (_STLP_MSVC >= 1300 && _STLP_MSVC <= 1310)
  typedef basic_ostringstream<_CharT, _Traits> _Self;
  //explicitely defined as private to avoid warnings:
  basic_ostringstream(_Self const&);
  _Self& operator = (_Self const&);
#endif
};


//----------------------------------------------------------------------
// Class basic_stringstream, a bidirectional stream that uses a stringbuf.

template <class _CharT, class _Traits, class _Alloc>
class basic_stringstream : public basic_iostream<_CharT, _Traits> {
public:                         // Typedefs
  typedef typename _Traits::char_type char_type;
  typedef typename _Traits::int_type  int_type;
  typedef typename _Traits::pos_type  pos_type;
  typedef typename _Traits::off_type  off_type;
  typedef _Traits  traits_type;

  typedef basic_ios<_CharT, _Traits>                 _Basic_ios;
  typedef basic_iostream<_CharT, _Traits>            _Base;
  typedef basic_string<_CharT, _Traits, _Alloc>      _String;
  typedef basic_stringbuf<_CharT, _Traits, _Alloc>  _Buf;

  typedef ios_base::openmode openmode;

public:                         // Constructors, destructor.
  basic_stringstream(openmode __mod = ios_base::in | ios_base::out);
  basic_stringstream(const _String& __str,
                     openmode __mod = ios_base::in | ios_base::out);
  ~basic_stringstream(){}

public:                         // Member functions.

  basic_stringbuf<_CharT, _Traits, _Alloc>* rdbuf() const
    { return __CONST_CAST(_Buf*,&_M_buf); }

  _String str() const { return _M_buf.str(); }
    void str(const _String& __s) { _M_buf.str(__s); }

private:
  basic_stringbuf<_CharT, _Traits, _Alloc> _M_buf;

#if defined (_STLP_MSVC) && (_STLP_MSVC >= 1300 && _STLP_MSVC <= 1310)
  typedef basic_stringstream<_CharT, _Traits> _Self;
  //explicitely defined as private to avoid warnings:
  basic_stringstream(_Self const&);
  _Self& operator = (_Self const&);
#endif
};


#if defined (_STLP_USE_TEMPLATE_EXPORT)
_STLP_EXPORT_TEMPLATE_CLASS basic_istringstream<char, char_traits<char>, allocator<char> >;
_STLP_EXPORT_TEMPLATE_CLASS basic_ostringstream<char, char_traits<char>, allocator<char> >;
_STLP_EXPORT_TEMPLATE_CLASS basic_stringstream<char, char_traits<char>, allocator<char> >;
#  if !defined (_STLP_NO_WCHAR_T)
_STLP_EXPORT_TEMPLATE_CLASS basic_istringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t>  >;
_STLP_EXPORT_TEMPLATE_CLASS basic_ostringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t>  >;
_STLP_EXPORT_TEMPLATE_CLASS basic_stringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t>  >;
#  endif
#endif /* _STLP_USE_TEMPLATE_EXPORT */

_STLP_END_NAMESPACE

#if defined (_STLP_EXPOSE_STREAM_IMPLEMENTATION) && !defined (_STLP_LINK_TIME_INSTANTIATION)
#  include <stl/_sstream.c>
#endif

#endif /* _STLP_INTERNAL_SSTREAM */

// Local Variables:
// mode:C++
// End:
