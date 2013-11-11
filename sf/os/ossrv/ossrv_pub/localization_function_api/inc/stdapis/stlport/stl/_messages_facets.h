/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
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

// WARNING: This is an internal header file, included by other C++
// standard library headers.  You should not attempt to use this header
// file directly.


#ifndef _STLP_INTERNAL_MESSAGES_H
#define _STLP_INTERNAL_MESSAGES_H

#ifndef _STLP_IOS_BASE_H
# include <stl/_ios_base.h>
#endif

# ifndef _STLP_C_LOCALE_H
#  include <stl/c_locale.h>
# endif

#ifndef _STLP_STRING_H
# include <stl/_string.h>
#endif

_STLP_BEGIN_NAMESPACE

// messages facets

class messages_base {
public:
  typedef int catalog;
};

template <class _CharT> class messages {};

class _Messages;

_STLP_TEMPLATE_NULL
#ifdef __SYMBIAN32__
class messages<char> : public locale::facet, public messages_base
#else
class _STLP_CLASS_DECLSPEC messages<char> : public locale::facet, public messages_base
#endif
{
  friend class _Locale;
public:
  typedef messages_base::catalog catalog;
  typedef char                   char_type;
  typedef string    string_type;

 _STLP_DECLSPEC explicit messages(size_t __refs = 0);

 catalog open(const string& __fn, const locale& __loc) const
    { return do_open(__fn, __loc); }
  string_type get(catalog __c, int __set, int __msgid,
		  const string_type& __dfault) const
    { return do_get(__c, __set, __msgid, __dfault); }
  inline void close(catalog __c) const
    { do_close(__c); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
#else
	_STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif

  _STLP_DECLSPEC  messages(_Messages*);

protected:
 _STLP_DECLSPEC  messages(size_t, _Locale_messages*);
_STLP_DECLSPEC   ~messages();

_STLP_DECLSPEC   virtual catalog     do_open(const string& __fn, const locale& __loc) const;
_STLP_DECLSPEC   virtual string_type do_get(catalog __c, int __set, int __msgid,
                             const string_type& __dfault) const;
_STLP_DECLSPEC   virtual void        do_close(catalog __c) const;

  void _M_initialize(const char* __name);

private:
  _Messages* _M_impl;
};

# if !defined (_STLP_NO_WCHAR_T)

_STLP_TEMPLATE_NULL
#ifdef __SYMBIAN32__
class messages<wchar_t> : public locale::facet, public messages_base
#else
class _STLP_CLASS_DECLSPEC messages<wchar_t> : public locale::facet, public messages_base
#endif
{
  friend class _Locale;
public:
  typedef messages_base::catalog catalog;
  typedef wchar_t                char_type;
  typedef wstring  string_type;

_STLP_DECLSPEC   explicit messages(size_t __refs = 0);
  
  inline catalog open(const string& __fn, const locale& __loc) const
    { return do_open(__fn, __loc); }
  inline string_type get(catalog __c, int __set, int __msgid,
                         const string_type& __dfault) const
    { return do_get(__c, __set, __msgid, __dfault); }
  inline void close(catalog __c) const
    { do_close(__c); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
#else
  _STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif

  _STLP_DECLSPEC  messages(_Messages*);

protected:

_STLP_DECLSPEC   messages(size_t, _Locale_messages*);
_STLP_DECLSPEC   ~messages();

_STLP_DECLSPEC   virtual catalog     do_open(const string& __fn, const locale& __loc) const;
_STLP_DECLSPEC   virtual string_type do_get(catalog __c, int __set, int __msgid,
                             const string_type& __dfault) const;
_STLP_DECLSPEC   virtual void        do_close(catalog __c) const;

  void _M_initialize(const char* __name);

private:
  _Messages* _M_impl;
};

# endif /* WCHAR_T */

template <class _CharT> class messages_byname {};

_STLP_TEMPLATE_NULL
class _STLP_CLASS_DECLSPEC messages_byname<char> : public messages<char> {
public:
  typedef messages_base::catalog catalog;
  typedef string     string_type;

 _STLP_DECLSPEC  explicit messages_byname(const char* __name, size_t __refs = 0);

protected:
_STLP_DECLSPEC   ~messages_byname();
};

# ifndef _STLP_NO_WCHAR_T
_STLP_TEMPLATE_NULL
class _STLP_CLASS_DECLSPEC messages_byname<wchar_t> : public messages<wchar_t> {
public:
  typedef messages_base::catalog catalog;
  typedef wstring                string_type;

_STLP_DECLSPEC   explicit messages_byname(const char* __name, size_t __refs = 0);

protected:
_STLP_DECLSPEC   ~messages_byname();
};
# endif /* WCHAR_T */

_STLP_END_NAMESPACE

#endif /* _STLP_INTERNAL_MESSAGES_H */

// Local Variables:
// mode:C++
// End:

