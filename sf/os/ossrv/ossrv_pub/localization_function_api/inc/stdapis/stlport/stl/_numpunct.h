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


#ifndef _STLP_INTERNAL_NUMPUNCT_H
#define _STLP_INTERNAL_NUMPUNCT_H

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

//----------------------------------------------------------------------
// numpunct facets

#ifdef	__SYMBIAN32__
extern locale::id& Numpunct_charT_GetFacetLocaleId(const char* type);

template <class _CharT> class numpunct: public locale::facet
{
	friend class _Locale;
public:
 	typedef char               		char_type;
  	typedef basic_string<_CharT>             string_type;
	explicit numpunct(size_t __refs = 0) : _BaseFacet(__refs) {
	_M_truename.append(1, (_CharT)'t');
	_M_truename.append(1, (_CharT)'r');
	_M_truename.append(1, (_CharT)'u');
	_M_truename.append(1, (_CharT)'e');

	_M_falsename.append(1, (_CharT)'f');
	_M_falsename.append(1, (_CharT)'a');
	_M_falsename.append(1, (_CharT)'l');
	_M_falsename.append(1, (_CharT)'s');
	_M_falsename.append(1, (_CharT)'e');
	}
	
  	_CharT decimal_point() const { return do_decimal_point(); }
  	_CharT thousands_sep() const { return do_thousands_sep(); }
  	string grouping() const { return do_grouping(); }
  	string_type truename() const { return do_truename(); }
  	string_type falsename() const { return do_falsename(); }
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
	static locale::id& GetFacetLocaleId(){return Numpunct_charT_GetFacetLocaleId(typeid(_CharT).name()); }
#else
  	 static locale::id id;
#endif

# ifndef _STLP_NO_FRIEND_TEMPLATES
protected:
# endif
	~numpunct();

protected:
  	 static string_type  _M_truename;
  	static string_type  _M_falsename;
  	static string  _M_grouping;
protected:

  	virtual _CharT do_decimal_point() const;
  	virtual _CharT do_thousands_sep() const;
  	virtual string do_grouping() const; 
  	virtual string_type do_truename() const;
  	virtual string_type do_falsename()  const;
};

template <class _CharT>
basic_string<_CharT> numpunct<_CharT>::_M_truename;
template <class _CharT>
basic_string<_CharT> numpunct<_CharT>::_M_falsename;
template <class _CharT>
string numpunct<_CharT>::_M_grouping ;

_STLP_DECLSPEC _Locale_numeric* __acquire_numericE(const char* );
_STLP_DECLSPEC void __release_numericE(_Locale_numeric* );
_STLP_DECLSPEC const char* _Locale_trueE(_Locale_numeric*);
_STLP_DECLSPEC const char* _Locale_falseE(_Locale_numeric*);
_STLP_DECLSPEC char _Locale_decimal_pointE(_Locale_numeric*);
_STLP_DECLSPEC char _Locale_thousands_sepE(_Locale_numeric*);
_STLP_DECLSPEC const char*_Locale_groupingE(_Locale_numeric*);

template <class _CharT>
class numpunct_byname : public numpunct<_CharT>{
public:
	typedef _CharT                char_type;
  	typedef basic_string<_CharT>              string_type;

  	explicit  numpunct_byname(const char* name, size_t refs = 0);	
protected:

	   ~numpunct_byname();

	 virtual _CharT   do_decimal_point() const;
  	 virtual _CharT   do_thousands_sep() const;
	  virtual string do_grouping()      const;

private:
  	_Locale_numeric* _M_numeric;
};

#else
template <class _CharT> class numpunct {};
template <class _CharT> class numpunct_byname {};
#endif
template <class _Ch, class _InIt> class num_get;

_STLP_TEMPLATE_NULL
#ifdef __SYMBIAN32__
class numpunct <char> : public locale::facet
#else
class _STLP_CLASS_DECLSPEC numpunct<char> : public locale::facet
#endif
{
  friend class _Locale;
# ifndef _STLP_NO_FRIEND_TEMPLATES
  template <class _Ch, class _InIt> friend class num_get;
# endif
public:
  typedef char               char_type;
  typedef string             string_type;

  explicit numpunct(size_t __refs = 0) : _BaseFacet(__refs) {}

  char decimal_point() const { return do_decimal_point(); }
  char thousands_sep() const { return do_thousands_sep(); }
  string grouping() const { return do_grouping(); }
  string truename() const { return do_truename(); }
  string falsename() const { return do_falsename(); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
	_STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
#else
  	_STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif

# ifndef _STLP_NO_FRIEND_TEMPLATES
protected:
# endif
  ~numpunct(){};

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
public:
  _STLP_STATIC_MEMBER_DECLSPEC static string&  GetNumPunct_M_truename();
  _STLP_STATIC_MEMBER_DECLSPEC static string&  GetNumPunct_M_falsename();
  _STLP_STATIC_MEMBER_DECLSPEC static string&  GetNumPunct_M_grouping();
#else
protected:
  _STLP_STATIC_MEMBER_DECLSPEC static string  _M_truename;
  _STLP_STATIC_MEMBER_DECLSPEC static string  _M_falsename;
  _STLP_STATIC_MEMBER_DECLSPEC static string  _M_grouping;
#endif

protected:

  _STLP_DECLSPEC virtual char do_decimal_point() const;
  _STLP_DECLSPEC virtual char do_thousands_sep() const;
  _STLP_DECLSPEC virtual string do_grouping() const;
  _STLP_DECLSPEC virtual string do_truename() const;
  _STLP_DECLSPEC virtual string do_falsename()  const;
};

# if ! defined (_STLP_NO_WCHAR_T)

_STLP_TEMPLATE_NULL
#ifdef __SYMBIAN32__
class numpunct<wchar_t> : public locale::facet
#else
class _STLP_CLASS_DECLSPEC numpunct<wchar_t> : public locale::facet
#endif
{
  friend class _Locale;
public:
  typedef wchar_t               char_type;
  typedef wstring               string_type;

  explicit numpunct(size_t __refs = 0) : _BaseFacet(__refs) {}

  wchar_t decimal_point() const { return do_decimal_point(); }
  wchar_t thousands_sep() const { return do_thousands_sep(); }
  string grouping() const { return do_grouping(); }
  wstring truename() const { return do_truename(); }
  wstring falsename() const { return do_falsename(); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
#else
 	_STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
public:
  _STLP_STATIC_MEMBER_DECLSPEC static wstring&  GetNumPunct_M_Wchar_truename();
  _STLP_STATIC_MEMBER_DECLSPEC static wstring&  GetNumPunct_M_Wchar_falsename();
  _STLP_STATIC_MEMBER_DECLSPEC static string&  GetNumPunct_M_Wchar_grouping();
#else
protected:
  _STLP_STATIC_MEMBER_DECLSPEC static wstring _M_truename;
  _STLP_STATIC_MEMBER_DECLSPEC static wstring _M_falsename;
  _STLP_STATIC_MEMBER_DECLSPEC static string _M_grouping;
#endif
protected:
  ~numpunct() {}

  _STLP_DECLSPEC virtual wchar_t do_decimal_point() const;
  _STLP_DECLSPEC virtual wchar_t do_thousands_sep() const;
  _STLP_DECLSPEC virtual string do_grouping() const;
  _STLP_DECLSPEC virtual wstring do_truename() const;
  _STLP_DECLSPEC virtual wstring do_falsename()  const;
};

# endif /* WCHAR_T */

_STLP_TEMPLATE_NULL
class _STLP_CLASS_DECLSPEC numpunct_byname<char> : public numpunct<char> {
public:
  typedef char                char_type;
  typedef string              string_type;

  explicit _STLP_DECLSPEC numpunct_byname(const char* __name, size_t __refs = 0);

protected:

_STLP_DECLSPEC   ~numpunct_byname();

_STLP_DECLSPEC   virtual char   do_decimal_point() const;
  _STLP_DECLSPEC virtual char   do_thousands_sep() const;
_STLP_DECLSPEC   virtual string do_grouping()      const;

private:
  _Locale_numeric* _M_numeric;
};

# ifndef _STLP_NO_WCHAR_T
_STLP_TEMPLATE_NULL
class _STLP_CLASS_DECLSPEC numpunct_byname<wchar_t>: public numpunct<wchar_t> {
public:
  typedef wchar_t               char_type;
  typedef wstring               string_type;

  explicit _STLP_DECLSPEC numpunct_byname(const char* __name, size_t __refs = 0);

protected:

_STLP_DECLSPEC   ~numpunct_byname();

_STLP_DECLSPEC   virtual wchar_t   do_decimal_point() const;
  _STLP_DECLSPEC virtual wchar_t   do_thousands_sep() const;
_STLP_DECLSPEC   virtual string do_grouping() const;

private:
  _Locale_numeric* _M_numeric;
};

# endif /* WCHAR_T */

#ifdef	__SYMBIAN32__
#include<stl/_numpunct.c>
#endif
_STLP_END_NAMESPACE

#endif /* _STLP_NUMPUNCT_H */

// Local Variables:
// mode:C++
// End:

