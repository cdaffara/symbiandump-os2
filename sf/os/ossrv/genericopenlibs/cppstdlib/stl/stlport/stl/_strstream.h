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
#ifndef _STLP_INTERNAL_STRSTREAM
#define _STLP_INTERNAL_STRSTREAM

#ifndef _STLP_INTERNAL_STREAMBUF
#  include <stl/_streambuf.h>
#endif

#ifndef _STLP_INTERNAL_ISTREAM
#  include <stl/_istream.h>              // Includes <ostream>, <ios>, <iosfwd>
#endif

#ifndef _STLP_INTERNAL_STRING_H
#  include <stl/_string.h>
#endif

_STLP_BEGIN_NAMESPACE

#ifndef _STLP_USE_NAMESPACES
#  define strstream _STLP_strstream
#  define ostrstream _STLP_ostrstream
#  define istrstream _STLP_istrstream
#  define strstreambuf _STLP_strstreambuf
#endif

//----------------------------------------------------------------------
// Class strstreambuf, a streambuf class that manages an array of char.
// Note that this class is not a template.

class _STLP_CLASS_DECLSPEC strstreambuf : public basic_streambuf<char, char_traits<char> > {
public:                         // Types.
  typedef char_traits<char>              _Traits;
  typedef basic_streambuf<char, char_traits<char> > _Base;
  typedef void* (*__alloc_fn)(size_t);
  typedef void (*__free_fn)(void*);
public:                         // Constructor, destructor

  _STLP_DECLSPEC explicit strstreambuf(streamsize _Initial_capacity = 0);

  _STLP_DECLSPEC strstreambuf(__alloc_fn, __free_fn);

  _STLP_DECLSPEC strstreambuf(char* __get, streamsize __n, char* __put = 0);
  _STLP_DECLSPEC strstreambuf(signed char* __get, streamsize __n, signed char* __put = 0);
  _STLP_DECLSPEC strstreambuf(unsigned char* __get, streamsize __n, unsigned char* __put=0);

  _STLP_DECLSPEC strstreambuf(const char* __get, streamsize __n);
  _STLP_DECLSPEC strstreambuf(const signed char* __get, streamsize __n);
  _STLP_DECLSPEC strstreambuf(const unsigned char* __get, streamsize __n);

  _STLP_DECLSPEC virtual ~strstreambuf();

public:                         // strstreambuf operations.
  _STLP_DECLSPEC void freeze(bool = true);
  _STLP_DECLSPEC char* str();
  _STLP_DECLSPEC int pcount() const;

protected:                      // Overridden virtual member functions.
  _STLP_DECLSPEC virtual int_type overflow(int_type __c  = _Traits::eof());
  _STLP_DECLSPEC virtual int_type pbackfail(int_type __c = _Traits::eof());
  _STLP_DECLSPEC virtual int_type underflow();
  _STLP_DECLSPEC virtual _Base* setbuf(char* __buf, streamsize __n);
  _STLP_DECLSPEC virtual pos_type seekoff(off_type __off, ios_base::seekdir __dir,
                           ios_base::openmode __mode
                                      = ios_base::in | ios_base::out);
  _STLP_DECLSPEC virtual pos_type seekpos(pos_type __pos, ios_base::openmode __mode
                                      = ios_base::in | ios_base::out);

private:                        // Helper functions.
  // Dynamic allocation, possibly using _M_alloc_fun and _M_free_fun.
  char* _M_alloc(size_t);
  void  _M_free(char*);

  // Helper function used in constructors.
  void _M_setup(char* __get, char* __put, streamsize __n);
private:                        // Data members.
  __alloc_fn _M_alloc_fun;
  __free_fn  _M_free_fun;
  bool _M_dynamic  : 1;
  bool _M_frozen   : 1;
  bool _M_constant : 1;
};

//----------------------------------------------------------------------
// Class istrstream, an istream that manages a strstreambuf.

class _STLP_CLASS_DECLSPEC istrstream : public basic_istream<char, char_traits<char> > {
public:
  _STLP_DECLSPEC explicit istrstream(char*);
  _STLP_DECLSPEC explicit istrstream(const char*);
  _STLP_DECLSPEC istrstream(char* , streamsize);
  _STLP_DECLSPEC istrstream(const char*, streamsize);
  _STLP_DECLSPEC virtual ~istrstream();

  _STLP_DECLSPEC strstreambuf* rdbuf() const;
  _STLP_DECLSPEC char* str();

private:
  strstreambuf _M_buf;
};

//----------------------------------------------------------------------
// Class ostrstream

class _STLP_CLASS_DECLSPEC ostrstream : public basic_ostream<char, char_traits<char> >
{
public:
  _STLP_DECLSPEC ostrstream();
  _STLP_DECLSPEC ostrstream(char*, int, ios_base::openmode = ios_base::out);
  _STLP_DECLSPEC virtual ~ostrstream();

  _STLP_DECLSPEC strstreambuf* rdbuf() const;
  _STLP_DECLSPEC void freeze(bool = true);
  _STLP_DECLSPEC char* str();
  _STLP_DECLSPEC int pcount() const;

private:
  strstreambuf _M_buf;
};

//----------------------------------------------------------------------
// Class strstream

class _STLP_CLASS_DECLSPEC strstream : public basic_iostream<char, char_traits<char> > {
public:
  typedef char                        char_type;
  typedef char_traits<char>::int_type int_type;
  typedef char_traits<char>::pos_type pos_type;
  typedef char_traits<char>::off_type off_type;

  _STLP_DECLSPEC strstream();
  _STLP_DECLSPEC strstream(char*, int, ios_base::openmode = ios_base::in | ios_base::out);
  _STLP_DECLSPEC virtual ~strstream();

  _STLP_DECLSPEC strstreambuf* rdbuf() const;
  _STLP_DECLSPEC void freeze(bool = true);
  _STLP_DECLSPEC int pcount() const;
  _STLP_DECLSPEC char* str();

private:
  strstreambuf _M_buf;

  //explicitely defined as private to avoid warnings:
  strstream(strstream const&);
  strstream& operator = (strstream const&);
};

_STLP_END_NAMESPACE

#endif /* _STLP_INTERNAL_STRSTREAM */
