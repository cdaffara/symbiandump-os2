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


#ifndef _STLP_INTERNAL_MONETARY_H
#define _STLP_INTERNAL_MONETARY_H

#ifndef _STLP_INTERNAL_CTYPE_H
# include <stl/_ctype.h>
#endif

#ifndef _STLP_INTERNAL_OSTREAMBUF_ITERATOR_H
# include <stl/_ostreambuf_iterator.h>
#endif

#ifndef _STLP_INTERNAL_ISTREAMBUF_ITERATOR_H
# include <stl/_istreambuf_iterator.h>
#endif

_STLP_BEGIN_NAMESPACE

class money_base {
public:
  enum part {none, space, symbol, sign, value};
  struct pattern {
    char field[4];
  };
};


#ifdef __SYMBIAN32__

extern locale::id& Moneypunct_charT_GetFacetLocaleId(const char* type);
_STLP_DECLSPEC void _Init_monetary_formatsE(money_base::pattern& pos_format,
				   money_base::pattern& neg_format);

template <class _CharT, __DFL_NON_TYPE_PARAM(bool, _International, false) > 
class moneypunct : public locale::facet , public money_base
{
public:
  typedef _CharT                 		char_type;
  typedef basic_string<_CharT>		string_type;
  explicit moneypunct _STLP_PSPEC2(_CharT, _International) (size_t __refs = 0)
  	{
  		_Init_monetary_formatsE(_M_pos_format, _M_neg_format);
  	}
  _CharT        decimal_point() const { return do_decimal_point(); }
  _CharT         thousands_sep() const { return do_thousands_sep(); }
  string      grouping()      const { return do_grouping(); }
  string_type curr_symbol()   const { return do_curr_symbol(); }
  string_type positive_sign() const { return do_positive_sign(); }
  string_type negative_sign() const { return do_negative_sign(); }
  int         frac_digits()   const { return do_frac_digits(); }
  pattern     pos_format()    const { return do_pos_format(); }
  pattern     neg_format()    const { return do_neg_format(); }
  #if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    static locale::id& GetFacetLocaleId(){return Moneypunct_charT_GetFacetLocaleId(typeid(_CharT).name());  };
#else
   static locale::id id;
#endif
# if defined (_STLP_STATIC_CONST_INIT_BUG)
  enum _IntlVal { intl = _International } ;
# else
  static const bool intl = _International;
# endif

protected:
  pattern _M_pos_format;
  pattern _M_neg_format;
  
  static string_type	 _M_psign, _M_nsign, _M_currSym;
  static string _M_group;
 ~moneypunct _STLP_PSPEC2(char, true) () { };

 virtual _CharT        do_decimal_point() const;
 virtual _CharT        do_thousands_sep() const;
 virtual string      do_grouping()      const;

 virtual basic_string<_CharT>      do_curr_symbol()   const;

 virtual basic_string<_CharT>      do_positive_sign() const;
 virtual basic_string<_CharT>      do_negative_sign() const;
 virtual int         do_frac_digits()   const;
 virtual pattern     do_pos_format()    const;
 virtual pattern     do_neg_format()    const;

  friend class _Locale;

};
template <class _CharT, bool  _International> 
string	moneypunct<_CharT, _International>::_M_group;
template <class _CharT, bool  _International> 
basic_string<_CharT>	moneypunct<_CharT, _International>::_M_psign;
template <class _CharT, bool  _International> 
basic_string<_CharT>	moneypunct<_CharT, _International>::_M_nsign;
template <class _CharT, bool  _International> 
basic_string<_CharT>	moneypunct<_CharT, _International>::_M_currSym;


#else
// moneypunct facets: forward declaration
template <class _charT, __DFL_NON_TYPE_PARAM(bool, _International, false) > class moneypunct {};
#endif

// money_get facets

template <class _CharT, __DFL_TMPL_PARAM(_InputIter , istreambuf_iterator<_CharT>) >
class money_get : public locale::facet 
{
  friend class _Locale;
public:
  typedef _CharT               char_type;
  typedef _InputIter           iter_type;
  typedef basic_string<_CharT, char_traits<_CharT>, allocator<_CharT> > string_type;

  money_get(size_t __refs = 0) : _BaseFacet(__refs) {}
# ifndef _STLP_NO_LONG_DOUBLE
  iter_type get(iter_type __s, iter_type  __end, bool __intl,
                ios_base&  __str, ios_base::iostate&  __err,
                long double& __units) const
    { return do_get(__s,  __end, __intl,  __str,  __err, __units); }
# endif  
  iter_type get(iter_type __s, iter_type  __end, bool __intl,
                ios_base&  __str, ios_base::iostate& __err,
                string_type& __digits) const
    { return do_get(__s,  __end, __intl,  __str,  __err, __digits); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
	_STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId(istreambuf_iterator<wchar_t, char_traits<wchar_t> >* );
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId(const wchar_t**);
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId(istreambuf_iterator<char, char_traits<char> >* );
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId(const char **);
#else
  _STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif

protected:
  ~money_get() {}
# ifndef _STLP_NO_LONG_DOUBLE
  virtual iter_type do_get(iter_type __s, iter_type  __end, bool  __intl,
                           ios_base&  __str, ios_base::iostate& __err,
                           long double& __units) const;
# endif
  virtual iter_type do_get(iter_type __s, iter_type __end, bool __intl,
                           ios_base&  __str, ios_base::iostate& __err,
                           string_type& __digits) const;
};


// moneypunct facets: definition of specializations

_STLP_TEMPLATE_NULL
#ifdef __SYMBIAN32__
class moneypunct<char, true> : public locale::facet, public money_base
#else
class _STLP_CLASS_DECLSPEC moneypunct<char, true> : public locale::facet, public money_base
#endif
{

public:
  typedef char                 char_type;
  typedef string               string_type;
_STLP_DECLSPEC   explicit moneypunct _STLP_PSPEC2(char, true) (size_t __refs = 0);

  char        decimal_point() const { return do_decimal_point(); }
  char        thousands_sep() const { return do_thousands_sep(); }
  string      grouping()      const { return do_grouping(); }
  string_type curr_symbol()   const { return do_curr_symbol(); }
  string_type positive_sign() const { return do_positive_sign(); }
  string_type negative_sign() const { return do_negative_sign(); }
  int         frac_digits()   const { return do_frac_digits(); }
  pattern     pos_format()    const { return do_pos_format(); }
  pattern     neg_format()    const { return do_neg_format(); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
#else
  _STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif
# if defined (_STLP_STATIC_CONST_INIT_BUG)
  enum _IntlVal { intl = 1 } ;
# else
  static const bool intl = true;
# endif

protected:
  pattern _M_pos_format;
  pattern _M_neg_format;

_STLP_DECLSPEC   ~moneypunct _STLP_PSPEC2(char, true) ();

_STLP_DECLSPEC   virtual char        do_decimal_point() const;
  _STLP_DECLSPEC virtual char        do_thousands_sep() const;
_STLP_DECLSPEC   virtual string      do_grouping()      const;

_STLP_DECLSPEC   virtual string      do_curr_symbol()   const;

_STLP_DECLSPEC   virtual string      do_positive_sign() const;
_STLP_DECLSPEC   virtual string      do_negative_sign() const;
_STLP_DECLSPEC   virtual int         do_frac_digits()   const;
_STLP_DECLSPEC   virtual pattern     do_pos_format()    const;
_STLP_DECLSPEC   virtual pattern     do_neg_format()    const;

  friend class _Locale;

};

_STLP_TEMPLATE_NULL
#ifdef __SYMBIAN32__
class moneypunct<char, false> : public locale::facet, public money_base
#else
class _STLP_CLASS_DECLSPEC moneypunct<char, false> : public locale::facet, public money_base
#endif
{
public:
  typedef char                 char_type;
  typedef string               string_type;

_STLP_DECLSPEC   explicit moneypunct _STLP_PSPEC2(char, false) (size_t __refs = 0);

  char        decimal_point() const { return do_decimal_point(); }
  char        thousands_sep() const { return do_thousands_sep(); }
  string      grouping()      const { return do_grouping(); }
  string_type curr_symbol()   const { return do_curr_symbol(); }
  string_type positive_sign() const { return do_positive_sign(); }
  string_type negative_sign() const { return do_negative_sign(); }
  int         frac_digits()   const { return do_frac_digits(); }
  pattern     pos_format()    const { return do_pos_format(); }
  pattern     neg_format()    const { return do_neg_format(); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
#else
  _STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif
# if defined (_STLP_STATIC_CONST_INIT_BUG)
  enum _IntlVal { intl = 0 } ;
# else
  static const bool intl = false;
# endif

protected:
  pattern _M_pos_format;
  pattern _M_neg_format;

_STLP_DECLSPEC   ~moneypunct _STLP_PSPEC2(char, false) ();

_STLP_DECLSPEC   virtual char        do_decimal_point() const;
  _STLP_DECLSPEC virtual char        do_thousands_sep() const;
_STLP_DECLSPEC   virtual string      do_grouping()      const;

_STLP_DECLSPEC   virtual string      do_curr_symbol()   const;

_STLP_DECLSPEC   virtual string      do_positive_sign() const;
_STLP_DECLSPEC   virtual string      do_negative_sign() const;
_STLP_DECLSPEC   virtual int         do_frac_digits()   const;
_STLP_DECLSPEC   virtual pattern     do_pos_format()    const;
_STLP_DECLSPEC   virtual pattern     do_neg_format()    const;

  friend class _Locale;
};


# ifndef _STLP_NO_WCHAR_T

_STLP_TEMPLATE_NULL
#ifdef __SYMBIAN32__
class moneypunct<wchar_t, true> : public locale::facet, public money_base
#else
class _STLP_CLASS_DECLSPEC moneypunct<wchar_t, true> : public locale::facet, public money_base
#endif
{
  friend class _Locale;
public:
  typedef wchar_t                 char_type;
  typedef wstring                 string_type;
_STLP_DECLSPEC   explicit moneypunct _STLP_PSPEC2(wchar_t, true) (size_t __refs = 0);
  wchar_t     decimal_point() const { return do_decimal_point(); }
  wchar_t     thousands_sep() const { return do_thousands_sep(); }
  string      grouping()      const { return do_grouping(); }
  string_type curr_symbol()   const { return do_curr_symbol(); }
  string_type positive_sign() const { return do_positive_sign(); }
  string_type negative_sign() const { return do_negative_sign(); }
  int         frac_digits()   const { return do_frac_digits(); }
  pattern     pos_format()    const { return do_pos_format(); }
  pattern     neg_format()    const { return do_neg_format(); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
	_STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
#else
  _STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif
# if defined (_STLP_STATIC_CONST_INIT_BUG)
  enum _IntlVal { intl = 1 } ;
# else
  static const bool intl = true;
# endif

protected:
  pattern _M_pos_format;
  pattern _M_neg_format;

_STLP_DECLSPEC   ~moneypunct _STLP_PSPEC2(wchar_t, true) ();

_STLP_DECLSPEC   virtual wchar_t     do_decimal_point() const;
  _STLP_DECLSPEC virtual wchar_t     do_thousands_sep() const;
_STLP_DECLSPEC   virtual string      do_grouping()      const;

_STLP_DECLSPEC   virtual string_type do_curr_symbol()   const;

_STLP_DECLSPEC   virtual string_type do_positive_sign() const;
_STLP_DECLSPEC   virtual string_type do_negative_sign() const;
_STLP_DECLSPEC   virtual int         do_frac_digits()   const;
_STLP_DECLSPEC   virtual pattern     do_pos_format()    const;
_STLP_DECLSPEC   virtual pattern     do_neg_format()    const;
};


_STLP_TEMPLATE_NULL
#ifdef __SYMBIAN32__
class moneypunct<wchar_t, false> : public locale::facet, public money_base
#else
class _STLP_CLASS_DECLSPEC moneypunct<wchar_t, false> : public locale::facet, public money_base
#endif
{
  friend class _Locale;
public:
  typedef wchar_t                 char_type;
  typedef wstring                 string_type;
_STLP_DECLSPEC   explicit moneypunct _STLP_PSPEC2(wchar_t, false) (size_t __refs = 0);
  wchar_t     decimal_point() const { return do_decimal_point(); }
  wchar_t     thousands_sep() const { return do_thousands_sep(); }
  string      grouping()      const { return do_grouping(); }
  string_type curr_symbol()   const { return do_curr_symbol(); }
  string_type positive_sign() const { return do_positive_sign(); }
  string_type negative_sign() const { return do_negative_sign(); }
  int         frac_digits()   const { return do_frac_digits(); }
  pattern     pos_format()    const { return do_pos_format(); }
  pattern     neg_format()    const { return do_neg_format(); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
	_STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
#else
	_STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif
# if defined (_STLP_STATIC_CONST_INIT_BUG)
  enum _IntlVal { intl = 0 } ;
# else
  static const bool intl = false;
# endif

protected:
  pattern _M_pos_format;
  pattern _M_neg_format;

_STLP_DECLSPEC   ~moneypunct _STLP_PSPEC2(wchar_t, false) ();

_STLP_DECLSPEC   virtual wchar_t     do_decimal_point() const;
  _STLP_DECLSPEC virtual wchar_t     do_thousands_sep() const;
_STLP_DECLSPEC   virtual string      do_grouping()      const;

_STLP_DECLSPEC   virtual string_type do_curr_symbol()   const;

_STLP_DECLSPEC   virtual string_type do_positive_sign() const;
_STLP_DECLSPEC   virtual string_type do_negative_sign() const;
_STLP_DECLSPEC   virtual int         do_frac_digits()   const;
_STLP_DECLSPEC   virtual pattern     do_pos_format()    const;
_STLP_DECLSPEC   virtual pattern     do_neg_format()    const;
};

# endif


#ifdef __SYMBIAN32__
_STLP_DECLSPEC  _Locale_monetary* __acquire_monetaryE(const char* );
_STLP_DECLSPEC void __release_monetaryE (_Locale_monetary* );


_STLP_DECLSPEC char _Locale_mon_decimal_pointE(_Locale_monetary* _M_monetary);

_STLP_DECLSPEC char _Locale_mon_thousands_sepE(_Locale_monetary* _M_monetary);

_STLP_DECLSPEC string _Locale_mon_groupingE(_Locale_monetary* _M_monetary);

_STLP_DECLSPEC string _Locale_int_curr_symbolE(_Locale_monetary* _M_monetary);


_STLP_DECLSPEC string _Locale_positive_signE(_Locale_monetary* _M_monetary);

_STLP_DECLSPEC string _Locale_negative_signE(_Locale_monetary* _M_monetary);

_STLP_DECLSPEC int _Locale_int_frac_digitsE(_Locale_monetary* _M_monetary);


template <class _CharT, __DFL_NON_TYPE_PARAM(bool , _International , false) > 
class moneypunct_byname: public  moneypunct<_CharT, _International>
{
public:
  typedef money_base::pattern   pattern;
  typedef char                  char_type;
  typedef basic_string<_CharT>                string_type;

  explicit moneypunct_byname _STLP_PSPEC2(char, _International) (const char * __name, size_t __refs = 0);
  
protected:
  _Locale_monetary* _M_monetary;
  ~moneypunct_byname _STLP_PSPEC2(_CharT, _International) ();
  virtual _CharT        do_decimal_point() const;
  virtual _CharT        do_thousands_sep() const;
  virtual string      do_grouping()      const;

  virtual string_type do_curr_symbol()   const;

  virtual string_type do_positive_sign() const;
 virtual string_type do_negative_sign() const;
 virtual int         do_frac_digits()   const;
 private: 
 void Convert_string2_string_chart(basic_string<_CharT> &dst, string src);
 	
};

#else

template <class _charT, __DFL_NON_TYPE_PARAM(bool , _International , false) > class moneypunct_byname {};

#endif

_STLP_TEMPLATE_NULL
class _STLP_CLASS_DECLSPEC moneypunct_byname<char, true> : public moneypunct<char, true> 
{
public:
  typedef money_base::pattern   pattern;
  typedef char                  char_type;
  typedef string                string_type;

  explicit _STLP_DECLSPEC moneypunct_byname _STLP_PSPEC2(char, true) (const char * __name, size_t __refs = 0);

protected:
  _Locale_monetary* _M_monetary;
_STLP_DECLSPEC   ~moneypunct_byname _STLP_PSPEC2(char, true) ();
_STLP_DECLSPEC   virtual char        do_decimal_point() const;
  _STLP_DECLSPEC virtual char        do_thousands_sep() const;
_STLP_DECLSPEC   virtual string      do_grouping()      const;

_STLP_DECLSPEC   virtual string_type do_curr_symbol()   const;

_STLP_DECLSPEC   virtual string_type do_positive_sign() const;
_STLP_DECLSPEC   virtual string_type do_negative_sign() const;
_STLP_DECLSPEC   virtual int         do_frac_digits()   const;
};

_STLP_TEMPLATE_NULL
class _STLP_CLASS_DECLSPEC moneypunct_byname<char, false> : public moneypunct<char, false> 
{
public:
  typedef money_base::pattern   pattern;
  typedef char                  char_type;
  typedef string                string_type;

  explicit _STLP_DECLSPEC moneypunct_byname _STLP_PSPEC2(char, false) (const char * __name, size_t __refs = 0);

protected:
  _Locale_monetary* _M_monetary;
_STLP_DECLSPEC   ~moneypunct_byname _STLP_PSPEC2(char, false) ();
_STLP_DECLSPEC   virtual char        do_decimal_point() const;
  _STLP_DECLSPEC virtual char        do_thousands_sep() const;
_STLP_DECLSPEC   virtual string      do_grouping()      const;

_STLP_DECLSPEC   virtual string_type do_curr_symbol()   const;

_STLP_DECLSPEC   virtual string_type do_positive_sign() const;
_STLP_DECLSPEC   virtual string_type do_negative_sign() const;
_STLP_DECLSPEC   virtual int         do_frac_digits()   const;
};

# ifndef _STLP_NO_WCHAR_T
_STLP_TEMPLATE_NULL
class _STLP_CLASS_DECLSPEC moneypunct_byname<wchar_t, true> : public moneypunct<wchar_t, true> 
{
public:
  typedef money_base::pattern   pattern;
  typedef wchar_t               char_type;
  typedef wstring               string_type;

  explicit _STLP_DECLSPEC moneypunct_byname _STLP_PSPEC2(wchar_t, true) (const char * __name, size_t __refs = 0);

protected:
  _Locale_monetary* _M_monetary;
_STLP_DECLSPEC   ~moneypunct_byname _STLP_PSPEC2(wchar_t, true) ();
_STLP_DECLSPEC   virtual wchar_t     do_decimal_point() const;
  _STLP_DECLSPEC virtual wchar_t     do_thousands_sep() const;
_STLP_DECLSPEC   virtual string      do_grouping()      const;

_STLP_DECLSPEC   virtual string_type do_curr_symbol()   const;

_STLP_DECLSPEC   virtual string_type do_positive_sign() const;
_STLP_DECLSPEC   virtual string_type do_negative_sign() const;
_STLP_DECLSPEC   virtual int         do_frac_digits()   const;
};

_STLP_TEMPLATE_NULL
class _STLP_CLASS_DECLSPEC moneypunct_byname<wchar_t, false> : public moneypunct<wchar_t, false> 
{
public:
  typedef money_base::pattern   pattern;
  typedef wchar_t               char_type;
  typedef wstring               string_type;

  explicit _STLP_DECLSPEC moneypunct_byname _STLP_PSPEC2(wchar_t, false) (const char * __name, size_t __refs = 0);

protected:
  _Locale_monetary* _M_monetary;
_STLP_DECLSPEC   ~moneypunct_byname _STLP_PSPEC2(wchar_t, false) ();
_STLP_DECLSPEC   virtual wchar_t     do_decimal_point() const;
  _STLP_DECLSPEC virtual wchar_t     do_thousands_sep() const;
_STLP_DECLSPEC   virtual string      do_grouping()      const;

_STLP_DECLSPEC   virtual string_type do_curr_symbol()   const;

_STLP_DECLSPEC   virtual string_type do_positive_sign() const;
_STLP_DECLSPEC   virtual string_type do_negative_sign() const;
_STLP_DECLSPEC   virtual int         do_frac_digits()   const;
};
# endif

//===== methods ======


// money_put facets

template <class _CharT, __DFL_TMPL_PARAM( _OutputIter , ostreambuf_iterator<_CharT>) >
class money_put : public locale::facet {
  friend class _Locale;

public:
  typedef _CharT               char_type;
  typedef _OutputIter          iter_type;
  typedef basic_string<_CharT, char_traits<_CharT>, allocator<_CharT> > string_type;

   money_put(size_t __refs = 0) : _BaseFacet(__refs) {}
# ifndef _STLP_NO_LONG_DOUBLE
  iter_type put(iter_type __s, bool __intl, ios_base& __str,
                char_type  __fill, long double __units) const
    { return do_put(__s, __intl, __str, __fill, __units); }
# endif
  iter_type put(iter_type __s, bool __intl, ios_base& __str,
                char_type  __fill, 
                const string_type& __digits) const
    { return do_put(__s, __intl, __str, __fill, __digits); }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
	_STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& 
                GetFacetLocaleId(ostreambuf_iterator<wchar_t, char_traits<wchar_t> > *);
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& 
                GetFacetLocaleId(wchar_t**);
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& 
                GetFacetLocaleId(ostreambuf_iterator<char, char_traits<char> > *);
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId(char**);
#else
  _STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif

protected:
  ~money_put() {}
# ifndef _STLP_NO_LONG_DOUBLE
  virtual iter_type do_put(iter_type __s, bool  __intl, ios_base&  __str,
                           char_type __fill, long double /*  __units */ ) const {

    locale __loc = __str.getloc();
    _CharT  __buf[64];
    return do_put(__s, __intl, __str, __fill, __buf + 0);
  }
# endif    
  virtual iter_type do_put(iter_type __s, bool  __intl, ios_base&  __str,
                           char_type __fill,
                           const string_type& __digits) const;
};

# if defined (_STLP_USE_TEMPLATE_EXPORT)
_STLP_EXPORT_TEMPLATE_CLASS money_get<char, istreambuf_iterator<char, char_traits<char> > >;
_STLP_EXPORT_TEMPLATE_CLASS money_put<char, ostreambuf_iterator<char, char_traits<char> > >;
// _STLP_EXPORT_TEMPLATE_CLASS money_get<char, const char* >;
// _STLP_EXPORT_TEMPLATE_CLASS money_put<char, char* >;
#  if ! defined (_STLP_NO_WCHAR_T)
_STLP_EXPORT_TEMPLATE_CLASS money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >;
_STLP_EXPORT_TEMPLATE_CLASS money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >;
// _STLP_EXPORT_TEMPLATE_CLASS money_get<wchar_t, const wchar_t* >;
// _STLP_EXPORT_TEMPLATE_CLASS money_put<wchar_t, wchar_t* >;
#  endif
# endif /* _STLP_USE_TEMPLATE_EXPORT */

# if defined (__BORLANDC__) && defined (_RTLDLL)
inline void _Stl_loc_init_monetary() {
  money_get<char, istreambuf_iterator<char, char_traits<char> > >::id._M_index                     = 8;
  money_get<char, const char*>::id._M_index        = 9;
  money_put<char, ostreambuf_iterator<char, char_traits<char> > >::id._M_index                     = 10;
  money_put<char, char*>::id._M_index              = 11;
# ifndef _STLP_NO_WCHAR_T
  money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >::id._M_index                  = 27;
  money_get<wchar_t, const wchar_t*>::id._M_index  = 28;
  money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >::id._M_index                  = 29;
  money_put<wchar_t, wchar_t*>::id._M_index        = 30;
# endif  
}
#endif

_STLP_END_NAMESPACE

# if defined (_STLP_EXPOSE_STREAM_IMPLEMENTATION) && !defined (_STLP_LINK_TIME_INSTANTIATION)
#  include <stl/_monetary.c>
# endif

#endif /* _STLP_INTERNAL_MONETARY_H */

// Local Variables:
// mode:C++
// End:


