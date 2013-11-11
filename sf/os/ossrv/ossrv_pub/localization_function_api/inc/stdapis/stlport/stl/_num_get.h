/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
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


#ifndef _STLP_INTERNAL_NUM_GET_H
#define _STLP_INTERNAL_NUM_GET_H

#ifndef _STLP_INTERNAL_ISTREAMBUF_ITERATOR_H
# include <stl/_istreambuf_iterator.h>
#endif

# ifndef _STLP_C_LOCALE_H
#  include <stl/c_locale.h>
# endif

#ifndef _STLP_INTERNAL_NUMPUNCT_H
# include <stl/_numpunct.h>
#endif
#ifndef _STLP_INTERNAL_CTYPE_H
# include <stl/_ctype.h>
#endif

_STLP_BEGIN_NAMESPACE

//----------------------------------------------------------------------
// num_get facets

# ifdef _STLP_LIMITED_DEFAULT_TEMPLATES
template <class _CharT, class _InputIter>  
# else
template <class _CharT, class _InputIter = istreambuf_iterator<_CharT> >  
# endif
#ifdef __SYMBIAN32__
class num_get : public locale::facet
#else
class num_get: public locale::facet
#endif
{
  friend class _Locale;
public:
  typedef _CharT     char_type;
  typedef _InputIter iter_type;

  explicit   num_get(size_t __refs = 0): locale::facet(__refs) {}
    
# ifndef _STLP_NO_BOOL
  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                ios_base::iostate& __err, bool& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }
# endif

//# ifdef _STLP_FIX_LIBRARY_ISSUES
  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                ios_base::iostate& __err, short& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }

  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                ios_base::iostate& __err, int& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }
//# endif

  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                ios_base::iostate& __err, long& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }

  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                ios_base::iostate& __err, unsigned short& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }

  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                ios_base::iostate& __err, unsigned int& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }

  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                ios_base::iostate& __err, unsigned long& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }

#ifdef _STLP_LONG_LONG

  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                ios_base::iostate& __err, _STLP_LONG_LONG& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }

  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                ios_base::iostate& __err, unsigned _STLP_LONG_LONG& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }

#endif /* _STLP_LONG_LONG */

  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                 ios_base::iostate& __err, float& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }

  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                ios_base::iostate& __err, double& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }

# ifndef _STLP_NO_LONG_DOUBLE

  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                ios_base::iostate& __err, long double& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }
# endif

  _InputIter get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                ios_base::iostate& __err, void*& __val) const {
    return do_get(__stl_in, __end, __str, __err, __val);
  }

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId();
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId(istreambuf_iterator<wchar_t, char_traits<wchar_t> >* );
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId(const wchar_t**);
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId(istreambuf_iterator<char, char_traits<char> >* );
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id& GetFacetLocaleId(const char**);
    
#else
    _STLP_STATIC_MEMBER_DECLSPEC static locale::id id;
#endif

protected:
  ~num_get() {}

  typedef string               string_type; 
  typedef ctype<_CharT>        _Ctype;
  typedef numpunct<_CharT>     _Numpunct;

# ifndef _STLP_NO_BOOL
  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end,
                           ios_base& __str, ios_base::iostate& __err, bool& __val) const;
# endif

  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                           ios_base::iostate& __err, long& __val) const;
  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                           ios_base::iostate& __err, unsigned short& __val) const;
  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                           ios_base::iostate& __err, unsigned int& __val) const;
  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                           ios_base::iostate& __err, unsigned long& __val) const;
//# ifdef _STLP_FIX_LIBRARY_ISSUES
  // issue 118 : those are actually not supposed to be here
  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                           ios_base::iostate& __err, short& __val) const;
  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                           ios_base::iostate& __err, int& __val) const;
//# endif
  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                          ios_base::iostate& __err, float& __val) const;
  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                           ios_base::iostate& __err, double& __val) const;
  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                           ios_base::iostate& __err,
                           void*& __p) const;

#ifndef _STLP_NO_LONG_DOUBLE
  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                           ios_base::iostate& __err, long double& __val) const;
#endif /* _STLP_NO_LONG_DOUBLE */

#ifdef _STLP_LONG_LONG

  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                            ios_base::iostate& __err, _STLP_LONG_LONG& __val) const;
  virtual _InputIter do_get(_InputIter __stl_in, _InputIter __end, ios_base& __str,
                           ios_base::iostate& __err, unsigned _STLP_LONG_LONG& __val) const;
#endif /* _STLP_LONG_LONG */

};


# ifdef _STLP_USE_TEMPLATE_EXPORT
_STLP_EXPORT_TEMPLATE_CLASS num_get<char, istreambuf_iterator<char, char_traits<char> > >;
// _STLP_EXPORT_TEMPLATE_CLASS num_get<char, const char*>;
#  ifndef _STLP_NO_WCHAR_T
_STLP_EXPORT_TEMPLATE_CLASS num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >;
// _STLP_EXPORT_TEMPLATE_CLASS num_get<wchar_t, const wchar_t*>;
#  endif /* _STLP_NO_WCHAR_T */
# endif

# if defined (_STLP_EXPOSE_STREAM_IMPLEMENTATION)

_STLP_DECLSPEC extern bool  _STLP_CALL __valid_grouping(const char*, const char*, const char*, const char*);

template <class _InputIter, class _Integer>
bool _STLP_CALL
__get_decimal_integer(_InputIter& __first, _InputIter& __last, _Integer& __val);

inline bool _STLP_CALL __get_fdigit(char& __c, const char*);
inline bool _STLP_CALL __get_fdigit_or_sep(char& __c, char __sep, const char *);
# ifndef _STLP_NO_WCHAR_T
inline bool _STLP_CALL __get_fdigit(wchar_t&, const wchar_t*);
inline bool _STLP_CALL __get_fdigit_or_sep(wchar_t&, wchar_t, const wchar_t*);
# endif

inline void  _STLP_CALL
_Initialize_get_float(const ctype<char>&,
                       char& Plus, char& Minus,
                       char& pow_e, char& pow_E,
                       char*)
{
  Plus = '+';
  Minus = '-';
  pow_e = 'e';
  pow_E = 'E';
}
// Helper functions for _M_do_get_float.

# ifndef _STLP_NO_WCHAR_T

inline void  _STLP_CALL
_Initialize_get_float( const ctype<wchar_t>& ct,
                       wchar_t& Plus, wchar_t& Minus,
                       wchar_t& pow_e, wchar_t& pow_E,
                       wchar_t* digits)
{
  char ndigits[11] = "0123456789";
  Plus  = ct.widen('+');
  Minus = ct.widen('-');
  pow_e = ct.widen('e');
  pow_E = ct.widen('E');
  ct.widen(ndigits + 0, ndigits + 10, digits);
}

# endif /* WCHAR_T */

#ifdef __SYMBIAN32__
template<class _CharT>
inline void  _STLP_CALL
_Initialize_get_float( const ctype<_CharT>& ct,
                       _CharT& Plus, _CharT& Minus,
                       _CharT& pow_e, _CharT& pow_E,
                       _CharT* digits)
{
  char ndigits[11] = "0123456789";
  Plus  = ct.widen('+');
  Minus = ct.widen('-');
  pow_e = ct.widen('e');
  pow_E = ct.widen('E');
  ct.widen(ndigits + 0, ndigits + 10, digits);
}
#endif

_STLP_DECLSPEC int  _STLP_CALL __string_to_float(const string&, float&);
_STLP_DECLSPEC int  _STLP_CALL __string_to_float(const string&, double&);
# ifndef _STLP_NO_LONG_DOUBLE
int  _STLP_CALL __string_to_float(const string&, long double&);
# endif
# endif

# if defined (__BORLANDC__) && defined (_RTLDLL)
inline void _Stl_loc_init_num_get() {  
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
  num_get<char, istreambuf_iterator<char, char_traits<char> > >::GetFacetLocaleId()._M_index = 12;
  num_get<char, const char*>::GetFacetLocaleId()._M_index = 13;
#else
  num_get<char, istreambuf_iterator<char, char_traits<char> > >::id._M_index = 12;
  num_get<char, const char*>::id._M_index = 13;
#endif
# ifndef _STLP_NO_WCHAR_T
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
  num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >::GetFacetLocaleId()._M_index = 31;
  num_get<wchar_t, const wchar_t*>::GetFacetLocaleId()._M_index = 32;
#else
  num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >::id._M_index = 31;
  num_get<wchar_t, const wchar_t*>::id._M_index = 32;
#endif
# endif
} 
# endif

_STLP_DECLSPEC unsigned char* _STLP_CALL __get_digit_val_table(void);
_STLP_DECLSPEC char* _STLP_CALL __get_narrow_atoms(void);
_STLP_END_NAMESPACE

#  if defined (_STLP_EXPOSE_STREAM_IMPLEMENTATION) && ! defined (_STLP_LINK_TIME_INSTANTIATION)
#   include <stl/_num_get.c>
#  endif

#endif /* _STLP_INTERNAL_NUM_GET_H */

// Local Variables:
// mode:C++
// End:

