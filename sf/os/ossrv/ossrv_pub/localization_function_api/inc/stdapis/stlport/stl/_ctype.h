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

#ifndef _STLP_INTERNAL_CTYPE_H
#define _STLP_INTERNAL_CTYPE_H

# ifndef _STLP_C_LOCALE_H
#  include <stl/c_locale.h>
# endif
# ifndef _STLP_INTERNAL_LOCALE_H
#  include <stl/_locale.h>
# endif
# ifndef _STLP_INTERNAL_ALGOBASE_H
#  include <stl/_algobase.h>
# endif

#ifdef	__SYMBIAN32__
#include<typeinfo>	
#endif
_STLP_BEGIN_NAMESPACE

class _STLP_CLASS_DECLSPEC ctype_base {
public:
  enum mask {
    space   = _Locale_SPACE,
    print   = _Locale_PRINT,
    cntrl   = _Locale_CNTRL,
    upper   = _Locale_UPPER,
    lower   = _Locale_LOWER,
    alpha   = _Locale_ALPHA,
    digit   = _Locale_DIGIT,
    punct   = _Locale_PUNCT,
    xdigit  = _Locale_XDIGIT,
    alnum   = alpha | digit,
    graph   = alnum | punct
  };
};

// ctype<> template
#ifdef	__SYMBIAN32__

_STLP_DECLSPEC	const ctype_base::mask* _get_classic_table();
_STLP_DECLSPEC	const unsigned char*  _get_S_upper_table();
_STLP_DECLSPEC	const unsigned char*  _get_S_lower_table();
extern locale::id& Ctype_charT_GetFacetLocaleId(const char* type);

_STLP_DECLSPEC _Locale_ctype* __acquire_ctypeE(const char* );
_STLP_DECLSPEC void __release_ctypeE(_Locale_ctype* );
_STLP_DECLSPEC  ctype_base::mask __Locale_ctypeE(_Locale_ctype* , wint_t c, ctype_base::mask);
_STLP_DECLSPEC wint_t __Locale_toupperE(_Locale_ctype* , wint_t );
_STLP_DECLSPEC wint_t __Locale_tolowerE(_Locale_ctype* , wint_t );


template <class charT> 
 class ctype: public locale::facet, public ctype_base {
  friend class _Locale;

public:
	
  typedef charT char_type;

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    static locale::id& GetFacetLocaleId(){return Ctype_charT_GetFacetLocaleId(typeid(charT).name()); }
#else
  static locale::id id;
#endif
# if defined(_STLP_STATIC_CONST_INIT_BUG)
  enum __TableSize { table_size = 256 };
# else
  static const size_t table_size = 256;
# endif


  explicit ctype(size_t __refs = 0) : _BaseFacet(__refs) {
  	}
  bool is(mask __m, char_type __c) const
  	{
		return do_is(__m ,__c);
  	}
  const char_type* is(const char_type* __low, const char_type* __high, mask* __vec) const
 	{
		return do_is(__low, __high, __vec);
  	}
  const char_type* scan_is(mask __m, const char_type* __low, const char_type* __high) const
   	{
		return do_scan_is(__m, __low, __high);
  	}
  const char_type* scan_not(mask __m, const char_type* __low, const char_type* __high) const
   	{
		return do_scan_not(__m, __low, __high);
		
  	}
	//need to function object like _Ctype_w_is_mask
  char_type toupper(char_type __c) const
	{
		return do_toupper(__c);
  	}
  const char_type* toupper(char_type* __low, const char_type* __high) const
   	{
		return do_toupper(__low, __high);
  	}
  char_type tolower(char_type __c) const
   	{
			return do_tolower(__c);
	}
  const char_type* tolower(char_type* __low, const char_type* __high) const
   	{
		return do_tolower(__low, __high);
  	}
	//use api's from c_locale_libc.cpp

  char_type  widen(char __c) const
	 {
		return do_widen(__c);	
  	}
  const char* widen(const char* __low, const char* __high, char_type* __to) const
   	{
		return do_widen(__low, __high, __to);
  	}
  char        narrow(char_type __c, char __dfault) const
   	{
		return do_narrow(__c, __dfault);
  	}
  const char_type* narrow(const char_type* __low, const char_type* __high,
                     char __dfault, char* __to) const
   	{
		return do_narrow( __low, __high, __dfault, __to);
  	}

protected:
  ~ctype();

 virtual bool  do_is(mask __m, char_type __c) const;
 virtual const char_type* do_is(const char_type* __low, const char_type* __high, mask* __vec) const;
 virtual const char_type* do_scan_is(mask __m, const char_type* __low, const char_type* __high) const;
 virtual const char_type* do_scan_not(mask __m, const char_type* __low, const char_type* __high) const;
 virtual char_type  do_toupper(char_type __c) const;
 virtual const char_type* do_toupper(char_type* __low, const char_type* __high) const;
 virtual char_type  do_tolower(char_type __c) const;
 virtual const char_type* do_tolower(char_type* __low, const char_type* __high) const;
 virtual char_type do_widen(char __c) const;
 virtual const char* do_widen(const char* __low, const char* __high, char_type* __to) const;
 virtual char  do_narrow(char_type __c, char __dfault) const;
 virtual const char_type* do_narrow(const char_type* __low, const char_type* __high,
                     char __dfault, char* __to) const;

private:

};

template <class charT>
class ctype_byname: public ctype<charT> {
public:
  explicit  ctype_byname(const char* __name, size_t __refs = 0);

protected:
~ctype_byname();

virtual bool           do_is(ctype_base::mask __m, charT __c) const;
virtual const charT* do_is(const charT*, const charT*, ctype_base::mask*) const;
virtual const charT* do_scan_is(ctype_base::mask,
                                    const charT*, const charT*) const;
virtual const charT* do_scan_not(ctype_base::mask,
                                     const charT*, const charT*) const;
virtual charT do_toupper(charT c) const;
virtual const charT* do_toupper(charT*, const charT*) const;
virtual charT do_tolower(charT c) const;
virtual const charT* do_tolower(charT*, const charT*) const;

private:
  _Locale_ctype* _M_ctype;
};



#else
template <class charT> class ctype {};
template <class charT> class ctype_byname {};
#endif

//ctype specializations

_STLP_TEMPLATE_NULL
#ifdef __SYMBIAN32__
class ctype <char> : public locale::facet, public ctype_base
#else
class _STLP_CLASS_DECLSPEC ctype<char> :   public locale::facet, public ctype_base
#endif
{

# ifndef _STLP_NO_WCHAR_T
#  ifdef _STLP_MSVC
    typedef ctype<wchar_t> _Wctype;
    friend _Wctype;
#  else
    friend class ctype<wchar_t>;
#  endif
# endif
  friend class _Locale;
public:

  typedef char char_type;

  _STLP_DECLSPEC explicit ctype(const mask* __tab = 0, bool __del = false, size_t __refs = 0);
  bool is(mask __m, char __c) const
    { return ((*(_M_ctype_table+(unsigned char)__c)) & __m) != 0; }

  const char* is(const char* __low, const char* __high, mask* __vec) const {
    for (const char* __p = __low;__p != __high; ++__p, ++__vec) {
      *__vec = _M_ctype_table[(unsigned char)*__p];
    }
    return __high;
  }

  _STLP_DECLSPEC const char* scan_is(mask __m, const char* __low, const char* __high) const;
  _STLP_DECLSPEC const char* scan_not(mask __m, const char* __low, const char* __high) const;

  char        (toupper)(char __c) const { return do_toupper(__c); }
  const char* (toupper)(char* __low, const char* __high) const { 
    return do_toupper(__low, __high); 
  }

  char        (tolower)(char __c) const { return do_tolower(__c); }
  const char* (tolower)(char* __low, const char* __high) const { 
    return do_tolower(__low, __high); 
  }
  
  char        widen(char __c) const { return do_widen(__c); }
  const char* widen(const char* __low, const char* __high, char* __to) const { 
    return do_widen(__low, __high, __to); 
  }

  char        narrow(char __c, char __dfault) const { 
    return do_narrow(__c, __dfault); 
  }
  const char* narrow(const char* __low, const char* __high,
                     char __dfault, char* __to) const { 
    return do_narrow(__low, __high, __dfault, __to); 
  }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
	_STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
#else
	_STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif
# if defined(_STLP_STATIC_CONST_INIT_BUG)
  enum __TableSize { table_size = 256 };
# else
  static const size_t table_size = 256;
# endif

protected:
  const mask* table() const _STLP_NOTHROW {return _M_ctype_table;}
  _STLP_DECLSPEC static const mask*  classic_table() _STLP_NOTHROW;// { return & _S_classic_table [1]; }

  _STLP_DECLSPEC ~ctype();

  _STLP_DECLSPEC virtual char        do_toupper(char __c) const;
  _STLP_DECLSPEC virtual char        do_tolower(char __c) const;
  _STLP_DECLSPEC virtual const char* do_toupper(char* __low, const char* __high) const;
  _STLP_DECLSPEC virtual const char* do_tolower(char* __low, const char* __high) const;
  _STLP_DECLSPEC virtual char        do_widen(char __c) const;
  _STLP_DECLSPEC virtual const char* do_widen(const char* __low, const char* __high,
                               char* __to) const;
  _STLP_DECLSPEC virtual char        do_narrow(char __c, char /* dfault */ ) const;
  _STLP_DECLSPEC virtual const char* do_narrow(const char* __low, const char* __high,
                                char /* dfault */, char* __to) const;
private:
  struct _Is_mask {
    mask __m;
    _Is_mask(mask __x): __m(__x) {}
   bool operator()(char __c) {return (__m & (unsigned char) __c) != 0;}
  };

  static const mask _S_classic_table[257 /* table_size + 1 */];
  const mask* _M_ctype_table;
  bool _M_delete;

  static const unsigned char _S_upper[256 /* table_size */];
  static const unsigned char _S_lower[256 /* table_size */];
};

_STLP_TEMPLATE_NULL
class _STLP_CLASS_DECLSPEC ctype_byname<char>: public ctype<char> {
public:
  explicit _STLP_DECLSPEC ctype_byname(const char*, size_t = 0);
_STLP_DECLSPEC  ~ctype_byname();

_STLP_DECLSPEC  virtual char        do_toupper(char __c) const;
_STLP_DECLSPEC  virtual char        do_tolower(char __c) const;

_STLP_DECLSPEC  virtual const char* do_toupper(char*, const char*) const;
_STLP_DECLSPEC  virtual const char* do_tolower(char*, const char*) const;

private:
  mask _M_byname_table[table_size + 1];
  _Locale_ctype* _M_ctype;
};


# ifndef _STLP_NO_WCHAR_T
_STLP_TEMPLATE_NULL
#ifdef __SYMBIAN32__
class ctype <wchar_t> : public locale::facet, public ctype_base
#else
class _STLP_CLASS_DECLSPEC ctype<wchar_t> : public locale::facet, public ctype_base
#endif
{
  friend class _Locale;
public:
  typedef wchar_t char_type;

  explicit ctype(size_t __refs = 0) : _BaseFacet(__refs) {}

  bool is(mask __m, wchar_t __c) const
    { return do_is(__m, __c); }

  const wchar_t* is(const wchar_t* __low, const wchar_t* __high,
                    mask* __vec) const
    { return do_is(__low, __high, __vec); }

  const wchar_t* scan_is(mask __m, 
                         const wchar_t* __low, const wchar_t* __high) const
    { return do_scan_is(__m, __low, __high); }

  const wchar_t* scan_not (mask __m, 
                           const wchar_t* __low, const wchar_t* __high) const
    { return do_scan_not(__m, __low, __high); }

  wchar_t (toupper)(wchar_t __c) const { return do_toupper(__c); }
  const wchar_t* (toupper)(wchar_t* __low, const wchar_t* __high) const
    { return do_toupper(__low, __high); }

  wchar_t (tolower)(wchar_t __c) const { return do_tolower(__c); }
  const wchar_t* (tolower)(wchar_t* __low, const wchar_t* __high) const
    { return do_tolower(__low, __high); }

  wchar_t widen(char __c) const { return do_widen(__c); }
  const char* widen(const char* __low, const char* __high,
                    wchar_t* __to) const
    { return do_widen(__low, __high, __to); }

  char narrow(wchar_t __c, char __dfault) const
    { return do_narrow(__c, __dfault); }
  const wchar_t* narrow(const wchar_t* __low, const wchar_t* __high,
                        char __dfault, char* __to) const
    { return do_narrow(__low, __high, __dfault, __to); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
#else
  _STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif

protected:
  _STLP_DECLSPEC ~ctype();

  _STLP_DECLSPEC virtual bool           do_is(mask __m, wchar_t __c) const;
  _STLP_DECLSPEC virtual const wchar_t* do_is(const wchar_t*, const wchar_t*, mask*) const;
  _STLP_DECLSPEC virtual const wchar_t* do_scan_is(mask,
                                    const wchar_t*, const wchar_t*) const;
  _STLP_DECLSPEC virtual const wchar_t* do_scan_not(mask,
                                     const wchar_t*, const wchar_t*) const;
  _STLP_DECLSPEC virtual wchar_t do_toupper(wchar_t __c) const;
  _STLP_DECLSPEC virtual const wchar_t* do_toupper(wchar_t*, const wchar_t*) const;
  _STLP_DECLSPEC virtual wchar_t do_tolower(wchar_t c) const;
  _STLP_DECLSPEC virtual const wchar_t* do_tolower(wchar_t*, const wchar_t*) const;
  _STLP_DECLSPEC virtual wchar_t do_widen(char c) const;
  _STLP_DECLSPEC virtual const char* do_widen(const char*, const char*, wchar_t*) const;
  _STLP_DECLSPEC virtual char  do_narrow(wchar_t __c, char __dfault) const;
  _STLP_DECLSPEC virtual const wchar_t* do_narrow(const wchar_t*, const wchar_t*,
                                   char, char*) const;
};

_STLP_TEMPLATE_NULL
class _STLP_CLASS_DECLSPEC ctype_byname<wchar_t>: public ctype<wchar_t> {
public:
  explicit _STLP_DECLSPEC  ctype_byname(const char* __name, size_t __refs = 0);

protected:
 _STLP_DECLSPEC ~ctype_byname();

_STLP_DECLSPEC  virtual bool           do_is(mask __m, wchar_t __c) const;
_STLP_DECLSPEC  virtual const wchar_t* do_is(const wchar_t*, const wchar_t*, mask*) const;
_STLP_DECLSPEC  virtual const wchar_t* do_scan_is(mask,
                                    const wchar_t*, const wchar_t*) const;
_STLP_DECLSPEC  virtual const wchar_t* do_scan_not(mask,
                                     const wchar_t*, const wchar_t*) const;
_STLP_DECLSPEC  virtual wchar_t do_toupper(wchar_t __c) const;
_STLP_DECLSPEC  virtual const wchar_t* do_toupper(wchar_t*, const wchar_t*) const;
_STLP_DECLSPEC  virtual wchar_t do_tolower(wchar_t c) const;
_STLP_DECLSPEC  virtual const wchar_t* do_tolower(wchar_t*, const wchar_t*) const;

private:
  _Locale_ctype* _M_ctype;
};

# endif /* WCHAR_T */
#ifdef	__SYMBIAN32__
#include <stl/_ctype.c>
#endif

_STLP_END_NAMESPACE

#endif /* _STLP_INTERNAL_CTYPE_H */

// Local Variables:
// mode:C++
// End:

