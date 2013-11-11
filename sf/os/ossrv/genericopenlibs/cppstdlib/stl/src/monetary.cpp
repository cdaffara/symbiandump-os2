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
#include "stlport_prefix.h"

#include <locale>
#include <istream>

#if defined(__SYMBIAN32__WSD__)
#include "libstdcppwsd.h"

void monetary_empty_string_init()
{
	get_monetary_S_empty_string().copy("", 1);
# ifndef _STLP_NO_WCHAR_T
	get_monetary_S_empty_wstring().copy(L"", 1);	
# endif //_STLP_NO_WCHAR_T
}
#define _S_empty_string		get_monetary_S_empty_string()
#define _S_empty_wstring 	get_monetary_S_empty_wstring()

#endif

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
#if !defined(__SYMBIAN32__WSD__)
static const string _S_empty_string;
#ifndef _STLP_NO_WCHAR_T
static const wstring _S_empty_wstring;
#endif
#endif  //__SYMBIAN32__WSD__
//
// moneypunct<>
//

_STLP_DECLSPEC moneypunct<char, true>::moneypunct(size_t __refs) : locale::facet(__refs)
{ _Init_monetary_formats(_M_pos_format, _M_neg_format); }
_STLP_DECLSPEC moneypunct<char, true>::~moneypunct() {}

_STLP_DECLSPEC char moneypunct<char, true>::do_decimal_point() const {return ' ';}
_STLP_DECLSPEC char moneypunct<char, true>::do_thousands_sep() const {return ' ';}
_STLP_DECLSPEC string moneypunct<char, true>::do_grouping() const { return _S_empty_string; }
_STLP_DECLSPEC string moneypunct<char, true>::do_curr_symbol() const { return _S_empty_string; }
_STLP_DECLSPEC string moneypunct<char, true>::do_positive_sign() const { return _S_empty_string; }
_STLP_DECLSPEC string moneypunct<char, true>::do_negative_sign() const { return _S_empty_string; }
_STLP_DECLSPEC money_base::pattern moneypunct<char, true>::do_pos_format() const  {return _M_pos_format;}
_STLP_DECLSPEC money_base::pattern moneypunct<char, true>::do_neg_format() const {return _M_neg_format;}
_STLP_DECLSPEC int moneypunct<char, true>::do_frac_digits() const {return 0;}

_STLP_DECLSPEC moneypunct<char, false>::moneypunct(size_t __refs) : locale::facet(__refs)
{ _Init_monetary_formats(_M_pos_format, _M_neg_format); }
_STLP_DECLSPEC moneypunct<char, false>::~moneypunct() {}

_STLP_DECLSPEC char moneypunct<char, false>::do_decimal_point() const {return ' ';}
_STLP_DECLSPEC char moneypunct<char, false>::do_thousands_sep() const {return ' ';}

_STLP_DECLSPEC string moneypunct<char, false>::do_grouping() const { return _S_empty_string; }
_STLP_DECLSPEC string moneypunct<char, false>::do_curr_symbol() const { return _S_empty_string; }
_STLP_DECLSPEC string moneypunct<char, false>::do_positive_sign() const { return _S_empty_string; }
_STLP_DECLSPEC string moneypunct<char, false>::do_negative_sign() const { return _S_empty_string; }
_STLP_DECLSPEC money_base::pattern moneypunct<char, false>::do_pos_format() const {return _M_pos_format;}
_STLP_DECLSPEC money_base::pattern moneypunct<char, false>::do_neg_format() const {return _M_neg_format;}
_STLP_DECLSPEC int moneypunct<char, false>::do_frac_digits() const {return 0;}

#ifndef _STLP_NO_WCHAR_T
_STLP_DECLSPEC moneypunct<wchar_t, true>::moneypunct(size_t __refs) : locale::facet(__refs)
{ _Init_monetary_formats(_M_pos_format, _M_neg_format); }
_STLP_DECLSPEC moneypunct<wchar_t, true>::~moneypunct() {}

_STLP_DECLSPEC wchar_t moneypunct<wchar_t, true>::do_decimal_point() const {return L' ';}
_STLP_DECLSPEC wchar_t moneypunct<wchar_t, true>::do_thousands_sep() const {return L' ';}
_STLP_DECLSPEC string moneypunct<wchar_t, true>::do_grouping() const {return _S_empty_string;}

_STLP_DECLSPEC wstring moneypunct<wchar_t, true>::do_curr_symbol() const
{return _S_empty_wstring;}
_STLP_DECLSPEC wstring moneypunct<wchar_t, true>::do_positive_sign() const
{return _S_empty_wstring;}
_STLP_DECLSPEC wstring moneypunct<wchar_t, true>::do_negative_sign() const
{return _S_empty_wstring;}
_STLP_DECLSPEC int moneypunct<wchar_t, true>::do_frac_digits() const {return 0;}
_STLP_DECLSPEC money_base::pattern moneypunct<wchar_t, true>::do_pos_format() const
{return _M_pos_format;}
_STLP_DECLSPEC money_base::pattern moneypunct<wchar_t, true>::do_neg_format() const
{return _M_neg_format;}

_STLP_DECLSPEC moneypunct<wchar_t, false>::moneypunct(size_t __refs) : locale::facet(__refs)
{ _Init_monetary_formats(_M_pos_format, _M_neg_format); }
_STLP_DECLSPEC moneypunct<wchar_t, false>::~moneypunct() {}

_STLP_DECLSPEC wchar_t moneypunct<wchar_t, false>::do_decimal_point() const {return L' ';}
_STLP_DECLSPEC wchar_t moneypunct<wchar_t, false>::do_thousands_sep() const {return L' ';}
_STLP_DECLSPEC string moneypunct<wchar_t, false>::do_grouping() const { return _S_empty_string;}
_STLP_DECLSPEC wstring moneypunct<wchar_t, false>::do_curr_symbol() const
{return _S_empty_wstring;}
_STLP_DECLSPEC wstring moneypunct<wchar_t, false>::do_positive_sign() const
{return _S_empty_wstring;}
_STLP_DECLSPEC wstring moneypunct<wchar_t, false>::do_negative_sign() const
{return _S_empty_wstring;}
_STLP_DECLSPEC int moneypunct<wchar_t, false>::do_frac_digits() const {return 0;}

_STLP_DECLSPEC money_base::pattern moneypunct<wchar_t, false>::do_pos_format() const
{return _M_pos_format;}
_STLP_DECLSPEC money_base::pattern moneypunct<wchar_t, false>::do_neg_format() const
{return _M_neg_format;}

#endif /* WCHAR_T */

#if defined (__SYMBIAN32__WSD__)
template <>
_STLP_DECLSPEC locale::id& money_get<char, istreambuf_iterator<char, char_traits<char> > >::GetFacetLocaleId()
    {
	return get_libcpp_wsd().money_get_char_istreambuf_iterator_id;
    }
template <>
_STLP_DECLSPEC locale::id& money_put<char, ostreambuf_iterator<char, char_traits<char> > >::GetFacetLocaleId()
    {
	return get_libcpp_wsd().money_put_char_ostreambuf_iterator_id;
    }
#  ifndef _STLP_NO_WCHAR_T
template <>
_STLP_DECLSPEC locale::id& money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >::GetFacetLocaleId()
	{
	return get_libcpp_wsd().money_get_wchar_istreambuf_iterator_id;
	}
template <>
_STLP_DECLSPEC locale::id& money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >::GetFacetLocaleId()
	{
	return get_libcpp_wsd().money_put_wchar_ostreambuf_iterator_id;
	}
#  endif /* _STLP_NO_WCHAR_T */
#endif /* __SYMBIAN32__WSD__ */

//
// Instantiations
//

#if !defined(_STLP_NO_FORCE_INSTANTIATE)

template class _STLP_CLASS_DECLSPEC money_get<char, istreambuf_iterator<char, char_traits<char> > >;
template class _STLP_CLASS_DECLSPEC money_put<char, ostreambuf_iterator<char, char_traits<char> > >;
// template class money_put<char, char*>;

#  ifndef _STLP_NO_WCHAR_T
template class _STLP_CLASS_DECLSPEC money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >;
template class _STLP_CLASS_DECLSPEC money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >;
// template class money_put<wchar_t, wchar_t*>;
// template class money_get<wchar_t, const wchar_t*>;
#  endif

#endif

#if defined (__EPOC32__)
template <>
locale::id money_get<char, istreambuf_iterator<char, char_traits<char> > >::id={8};

template <>
locale::id money_put<char, ostreambuf_iterator<char, char_traits<char> > >::id={10};
template <>
locale::id money_put<char, char*>::id;

# if !defined (_STLP_NO_WCHAR_T)
template <>
locale::id money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >::id={27};
template <>
locale::id money_get<wchar_t, const wchar_t*>::id;

template <>
locale::id money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >::id={29};
# endif
#endif // __EPOC32__


_STLP_END_NAMESPACE

// Local Variables:
// mode:C++
// End:
