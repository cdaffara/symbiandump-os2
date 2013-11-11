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

#ifndef _STLP_INTERNAL_COLLATE_H
#define _STLP_INTERNAL_COLLATE_H

#ifndef _STLP_C_LOCALE_H
# include <stl/c_locale.h>
#endif

#ifndef _STLP_INTERNAL_LOCALE_H
# include <stl/_locale.h>
#endif

#ifndef _STLP_STRING_H
# include <stl/_string.h>
#endif

_STLP_BEGIN_NAMESPACE


template <class _CharT> class collate {};
template <class _CharT> class collate_byname {};

_STLP_TEMPLATE_NULL
#ifdef __SYMBIAN32__
class  collate<char> : public locale::facet
#else
class _STLP_CLASS_DECLSPEC collate<char> : public locale::facet
#endif
{
  friend class _Locale;
public:
  typedef char   char_type;
  typedef string string_type;

  explicit collate(size_t __refs = 0) : _BaseFacet(__refs) {}

  int compare(const char* __low1, const char* __high1,
              const char* __low2, const char* __high2) const {
    return do_compare( __low1, __high1, __low2, __high2);
  }

  string_type transform(const char* __low, const char* __high) const {
    return do_transform(__low, __high);
  }

  long hash(const char* __low, const char* __high) const
    { return do_hash(__low, __high); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
#else
	_STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif

protected:
_STLP_DECLSPEC  ~collate();

_STLP_DECLSPEC   virtual int do_compare(const char*, const char*,
                         const char*, const char*) const;
_STLP_DECLSPEC   virtual string_type do_transform(const char*, const char*) const;
_STLP_DECLSPEC   virtual long do_hash(const char*, const char*) const;
private:
  collate(const collate<char>&);
  collate<char>& operator =(const collate<char>&);  
};

# ifndef _STLP_NO_WCHAR_T

_STLP_TEMPLATE_NULL
#ifdef __SYMBIAN32__
class  collate<wchar_t> : public locale::facet
#else
class _STLP_CLASS_DECLSPEC collate<wchar_t> : public locale::facet
#endif
{
  friend class _Locale;
public:
  typedef wchar_t char_type;
  typedef wstring string_type;

  explicit collate(size_t __refs = 0) : _BaseFacet(__refs) {}

  int compare(const wchar_t* __low1, const wchar_t* __high1,
              const wchar_t* __low2, const wchar_t* __high2) const {
    return do_compare( __low1, __high1, __low2, __high2);
  }

  string_type transform(const wchar_t* __low, const wchar_t* __high) const {
    return do_transform(__low, __high);
  }

  long hash(const wchar_t* __low, const wchar_t* __high) const
    { return do_hash(__low, __high); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
#else
	_STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif

protected:
_STLP_DECLSPEC   ~collate();

_STLP_DECLSPEC   virtual int do_compare(const wchar_t*, const wchar_t*,
                         const wchar_t*, const wchar_t*) const;
_STLP_DECLSPEC   virtual string_type do_transform(const wchar_t*, const wchar_t*) const;
_STLP_DECLSPEC   virtual long do_hash(const wchar_t* __low, const wchar_t* __high) const;
private:
  collate(const collate<wchar_t>&);
  collate<wchar_t>& operator = (const collate<wchar_t>&);  
};

# endif /* NO_WCHAR_T */

_STLP_TEMPLATE_NULL
class _STLP_CLASS_DECLSPEC collate_byname<char>: public collate<char> 
{
public:
  explicit _STLP_DECLSPEC collate_byname(const char* __name, size_t __refs = 0);

protected:
_STLP_DECLSPEC   ~collate_byname();

_STLP_DECLSPEC   virtual int do_compare(const char*, const char*,
                         const char*, const char*) const;
_STLP_DECLSPEC   virtual string_type do_transform(const char*, const char*) const;

private:
  _Locale_collate* _M_collate;
  collate_byname(const collate_byname<char>&);
  collate_byname<char>& operator =(const collate_byname<char>&);  
};

# ifndef _STLP_NO_WCHAR_T

_STLP_TEMPLATE_NULL
class _STLP_CLASS_DECLSPEC collate_byname<wchar_t>: public collate<wchar_t> 
{
public:
  explicit _STLP_DECLSPEC  collate_byname(const char * __name, size_t __refs = 0);

protected:
_STLP_DECLSPEC   ~collate_byname();

_STLP_DECLSPEC   virtual int do_compare(const wchar_t*, const wchar_t*,
                         const wchar_t*, const wchar_t*) const;
_STLP_DECLSPEC   virtual string_type do_transform(const wchar_t*, const wchar_t*) const;

private:
  _Locale_collate* _M_collate;
  collate_byname(const collate_byname<wchar_t>&);
  collate_byname<wchar_t>& operator =(const collate_byname<wchar_t>&);  
};

# endif /* NO_WCHAR_T */


template <class _CharT, class _Traits, class _Alloc>
bool 
__locale_do_operator_call (const locale* __that, 
                           const basic_string<_CharT, _Traits, _Alloc >& __x,
                           const basic_string<_CharT, _Traits, _Alloc >& __y) 
{
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
  collate<_CharT>* __f = (collate<_CharT>*)__that->_M_get_facet(collate<_CharT>::GetFacetLocaleId());
#else
  collate<_CharT>* __f = (collate<_CharT>*)__that->_M_get_facet(collate<_CharT>::id);
#endif
  if (!__f)
    __that->_M_throw_runtime_error();
  return __f->compare(__x.data(), __x.data() + __x.size(),
                      __y.data(), __y.data() + __y.size()) < 0;
  
}

_STLP_END_NAMESPACE

#endif /* _STLP_INTERNAL_COLLATE_H */

// Local Variables:
// mode:C++
// End:
