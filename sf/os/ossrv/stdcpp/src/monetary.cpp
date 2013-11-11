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
# include "stlport_prefix.h"

#include <stl/_monetary.h>
#include <stl/_istream.h>

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
#include "libstdcppwsd.h"
# endif

_STLP_BEGIN_NAMESPACE

static void _Init_monetary_formats(money_base::pattern& pos_format,
				   money_base::pattern& neg_format) {
  pos_format.field[0] = (char) money_base::symbol;
  pos_format.field[1] = (char) money_base::sign;
  pos_format.field[2] = (char) money_base::none;
  pos_format.field[3] = (char) money_base::value;

  neg_format.field[0] = (char) money_base::symbol;
  neg_format.field[1] = (char) money_base::sign;
  neg_format.field[2] = (char) money_base::none;
  neg_format.field[3] = (char) money_base::value;
}

// This is being used throughout the library
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
void monetary_empty_string_init()
{
	get_monetary_S_empty_string().copy("", 1);
# ifndef _STLP_NO_WCHAR_T
	get_monetary_S_empty_wstring().copy(L"", 1);	
# endif //_STLP_NO_WCHAR_T
}
# else
string _S_empty_string("");
# ifndef _STLP_NO_WCHAR_T
wstring _S_empty_wstring(L"");
# endif //_STLP_NO_WCHAR_T
# endif //__LIBSTD_CPP_SYMBIAN32_WSD__

//
// moneypunct<>
//

_STLP_EXP_DECLSPEC moneypunct<char, true>::moneypunct(size_t __refs) : _BaseFacet(__refs) { 
    _Init_monetary_formats(_M_pos_format, _M_neg_format); 
}
 _STLP_EXP_DECLSPEC moneypunct<char, true>::~moneypunct() {}

_STLP_EXP_DECLSPEC char moneypunct<char, true>::do_decimal_point() const {return '.';}
_STLP_EXP_DECLSPEC char moneypunct<char, true>::do_thousands_sep() const {return ',';}

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
_STLP_EXP_DECLSPEC string moneypunct<char, true>::do_grouping() const { return get_monetary_S_empty_string(); }
_STLP_EXP_DECLSPEC string moneypunct<char, true>::do_curr_symbol() const { return get_monetary_S_empty_string(); }
_STLP_EXP_DECLSPEC string moneypunct<char, true>::do_positive_sign() const { return get_monetary_S_empty_string(); }
_STLP_EXP_DECLSPEC string moneypunct<char, true>::do_negative_sign() const { return get_monetary_S_empty_string(); }
# else
_STLP_EXP_DECLSPEC string moneypunct<char, true>::do_grouping() const { return _S_empty_string; }
_STLP_EXP_DECLSPEC string moneypunct<char, true>::do_curr_symbol() const { return _S_empty_string; }
_STLP_EXP_DECLSPEC string moneypunct<char, true>::do_positive_sign() const { return _S_empty_string; }
_STLP_EXP_DECLSPEC string moneypunct<char, true>::do_negative_sign() const { return _S_empty_string; }
# endif //__LIBSTD_CPP_SYMBIAN32_WSD__

_STLP_EXP_DECLSPEC money_base::pattern moneypunct<char, true>::do_pos_format() const  {return _M_pos_format;}
_STLP_EXP_DECLSPEC money_base::pattern moneypunct<char, true>::do_neg_format() const {return _M_neg_format;}
_STLP_EXP_DECLSPEC int moneypunct<char, true>::do_frac_digits() const {return 0;}

_STLP_EXP_DECLSPEC moneypunct<char, false>::moneypunct(size_t __refs) : _BaseFacet(__refs) { 
    _Init_monetary_formats(_M_pos_format, _M_neg_format); 
}
_STLP_EXP_DECLSPEC moneypunct<char, false>::~moneypunct() {}

_STLP_EXP_DECLSPEC char moneypunct<char, false>::do_decimal_point() const {return '.';}
_STLP_EXP_DECLSPEC char moneypunct<char, false>::do_thousands_sep() const {return ',';}

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
_STLP_EXP_DECLSPEC string moneypunct<char, false>::do_grouping() const { return get_monetary_S_empty_string(); }
_STLP_EXP_DECLSPEC string moneypunct<char, false>::do_curr_symbol() const { return get_monetary_S_empty_string(); }
_STLP_EXP_DECLSPEC string moneypunct<char, false>::do_positive_sign() const { return get_monetary_S_empty_string(); }
_STLP_EXP_DECLSPEC string moneypunct<char, false>::do_negative_sign() const { return get_monetary_S_empty_string(); }
# else
_STLP_EXP_DECLSPEC string moneypunct<char, false>::do_grouping() const { return _S_empty_string; }
_STLP_EXP_DECLSPEC string moneypunct<char, false>::do_curr_symbol() const { return _S_empty_string; }
_STLP_EXP_DECLSPEC string moneypunct<char, false>::do_positive_sign() const { return _S_empty_string; }
_STLP_EXP_DECLSPEC string moneypunct<char, false>::do_negative_sign() const { return _S_empty_string; }
# endif //__LIBSTD_CPP_SYMBIAN32_WSD__

_STLP_EXP_DECLSPEC money_base::pattern moneypunct<char, false>::do_pos_format() const {return _M_pos_format;}
_STLP_EXP_DECLSPEC money_base::pattern moneypunct<char, false>::do_neg_format() const {return _M_neg_format;}
_STLP_EXP_DECLSPEC int moneypunct<char, false>::do_frac_digits() const {return 0;}


# ifndef _STLP_NO_WCHAR_T

_STLP_EXP_DECLSPEC moneypunct<wchar_t, true>::moneypunct(size_t __refs) : _BaseFacet(__refs) { 
    _Init_monetary_formats(_M_pos_format, _M_neg_format); 
}
_STLP_EXP_DECLSPEC moneypunct<wchar_t, true>::~moneypunct() {}

_STLP_EXP_DECLSPEC wchar_t moneypunct<wchar_t, true>::do_decimal_point() const {return L'.';}
_STLP_EXP_DECLSPEC wchar_t moneypunct<wchar_t, true>::do_thousands_sep() const {return L',';}

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
_STLP_EXP_DECLSPEC string moneypunct<wchar_t, true>::do_grouping() const {return get_monetary_S_empty_string();}

_STLP_EXP_DECLSPEC wstring moneypunct<wchar_t, true>::do_curr_symbol() const
  {return get_monetary_S_empty_wstring();}
_STLP_EXP_DECLSPEC wstring moneypunct<wchar_t, true>::do_positive_sign() const
  {return get_monetary_S_empty_wstring();}
_STLP_EXP_DECLSPEC wstring moneypunct<wchar_t, true>::do_negative_sign() const
  {return get_monetary_S_empty_wstring();}
# else
_STLP_EXP_DECLSPEC string moneypunct<wchar_t, true>::do_grouping() const {return _S_empty_string;}

_STLP_EXP_DECLSPEC wstring moneypunct<wchar_t, true>::do_curr_symbol() const
  {return _S_empty_wstring;}
_STLP_EXP_DECLSPEC wstring moneypunct<wchar_t, true>::do_positive_sign() const
  {return _S_empty_wstring;}
_STLP_EXP_DECLSPEC wstring moneypunct<wchar_t, true>::do_negative_sign() const
  {return _S_empty_wstring;}
# endif  //__LIBSTD_CPP_SYMBIAN32_WSD__

_STLP_EXP_DECLSPEC int moneypunct<wchar_t, true>::do_frac_digits() const {return 0;}
_STLP_EXP_DECLSPEC money_base::pattern moneypunct<wchar_t, true>::do_pos_format() const
  {return _M_pos_format;}
_STLP_EXP_DECLSPEC money_base::pattern moneypunct<wchar_t, true>::do_neg_format() const
  {return _M_neg_format;}

_STLP_EXP_DECLSPEC moneypunct<wchar_t, false>::moneypunct(size_t __refs) : _BaseFacet(__refs) { 
    _Init_monetary_formats(_M_pos_format, _M_neg_format); 
}
_STLP_EXP_DECLSPEC moneypunct<wchar_t, false>::~moneypunct() {}

_STLP_EXP_DECLSPEC wchar_t moneypunct<wchar_t, false>::do_decimal_point() const {return L'.';}
_STLP_EXP_DECLSPEC wchar_t moneypunct<wchar_t, false>::do_thousands_sep() const {return L',';}

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
_STLP_EXP_DECLSPEC string moneypunct<wchar_t, false>::do_grouping() const { return get_monetary_S_empty_string();}
_STLP_EXP_DECLSPEC wstring moneypunct<wchar_t, false>::do_curr_symbol() const
  {return get_monetary_S_empty_wstring();}
_STLP_EXP_DECLSPEC wstring moneypunct<wchar_t, false>::do_positive_sign() const
  {return get_monetary_S_empty_wstring();}
_STLP_EXP_DECLSPEC wstring moneypunct<wchar_t, false>::do_negative_sign() const
  {return get_monetary_S_empty_wstring();}
# else
_STLP_EXP_DECLSPEC string moneypunct<wchar_t, false>::do_grouping() const { return _S_empty_string;}
_STLP_EXP_DECLSPEC wstring moneypunct<wchar_t, false>::do_curr_symbol() const
  {return _S_empty_wstring;}
_STLP_EXP_DECLSPEC wstring moneypunct<wchar_t, false>::do_positive_sign() const
  {return _S_empty_wstring;}
_STLP_EXP_DECLSPEC wstring moneypunct<wchar_t, false>::do_negative_sign() const
  {return _S_empty_wstring;}
# endif //__LIBSTD_CPP_SYMBIAN32_WSD__

_STLP_EXP_DECLSPEC int moneypunct<wchar_t, false>::do_frac_digits() const {return 0;}

_STLP_EXP_DECLSPEC money_base::pattern moneypunct<wchar_t, false>::do_pos_format() const
  {return _M_pos_format;}
_STLP_EXP_DECLSPEC money_base::pattern moneypunct<wchar_t, false>::do_neg_format() const
  {return _M_neg_format;}

# endif /* WCHAR_T */

//
// Instantiations
//

# if !defined(_STLP_NO_FORCE_INSTANTIATE)

template class money_get<char, istreambuf_iterator<char, char_traits<char> > >;
template class money_put<char, ostreambuf_iterator<char, char_traits<char> > >;
// template class money_put<char, char*>;

#ifndef _STLP_NO_WCHAR_T
template class money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >;
template class money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >;
// template class money_put<wchar_t, wchar_t*>;
// template class money_get<wchar_t, const wchar_t*>;
#endif

# endif

#ifdef __SYMBIAN32__
_STLP_EXP_DECLSPEC void _Init_monetary_formatsE(money_base::pattern& pos_format,
				   money_base::pattern& neg_format)
{
	_Init_monetary_formats(pos_format, neg_format);
}

_Locale_monetary* __acquire_monetary(const char* name); 
void __release_monetary(_Locale_monetary* cat);

_STLP_EXP_DECLSPEC _Locale_monetary* __acquire_monetaryE(const char*  name)
{
	return __acquire_monetary(name);
}
_STLP_EXP_DECLSPEC void __release_monetaryE (_Locale_monetary* _M_monetary)
{
	__release_monetary(_M_monetary);
}

_STLP_EXP_DECLSPEC char _Locale_mon_decimal_pointE(_Locale_monetary* _M_monetary)
{
	return _Locale_mon_decimal_point(_M_monetary);
}

_STLP_EXP_DECLSPEC char _Locale_mon_thousands_sepE(_Locale_monetary* _M_monetary)
{
	return _Locale_mon_thousands_sep(_M_monetary);
}

_STLP_EXP_DECLSPEC string _Locale_mon_groupingE(_Locale_monetary* _M_monetary)
{
	return _Locale_mon_grouping(_M_monetary);
}


_STLP_EXP_DECLSPEC string _Locale_int_curr_symbolE(_Locale_monetary* _M_monetary)
{
	return _Locale_int_curr_symbol(_M_monetary);
}

_STLP_EXP_DECLSPEC string _Locale_positive_signE(_Locale_monetary* _M_monetary)
{
	return _Locale_positive_sign(_M_monetary);
}

_STLP_EXP_DECLSPEC string _Locale_negative_signE(_Locale_monetary* _M_monetary)
{
	return _Locale_negative_sign(_M_monetary);
}


_STLP_EXP_DECLSPEC int _Locale_int_frac_digitsE(_Locale_monetary* _M_monetary)
{
	return _Locale_int_frac_digits(_M_monetary);
}


#endif
_STLP_END_NAMESPACE  

// Local Variables:
// mode:C++
// End:
