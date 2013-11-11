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
# include <stl/_numpunct.h>

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
#include "libstdcppwsd.h"
# endif

_STLP_BEGIN_NAMESPACE

//----------------------------------------------------------------------
// numpunct<char>
# if !defined(__LIBSTD_CPP_SYMBIAN32_WSD__) && !defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
_STLP_STATIC_MEMBER_EXP_DECLSPEC  string numpunct<char>::_M_truename("true");
_STLP_STATIC_MEMBER_EXP_DECLSPEC  string  numpunct<char>::_M_falsename("false");
_STLP_STATIC_MEMBER_EXP_DECLSPEC  string numpunct<char>::_M_grouping("");
# endif

_STLP_EXP_DECLSPEC char   numpunct<char>::do_decimal_point() const {return '.';}
_STLP_EXP_DECLSPEC char   numpunct<char>::do_thousands_sep() const { return ','; }
_STLP_EXP_DECLSPEC string numpunct<char>::do_grouping()  const { return string();}

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
_STLP_EXP_DECLSPEC  string numpunct<char>::do_truename()  const { return GetNumPunct_M_truename();}
_STLP_EXP_DECLSPEC  string numpunct<char>::do_falsename() const { return GetNumPunct_M_falsename(); }
# else
string numpunct<char>::do_truename()  const { return _M_truename;}
string numpunct<char>::do_falsename() const { return _M_falsename; }
# endif

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
void numpunct_name_init()
{	
	numpunct<char>::GetNumPunct_M_truename() = "true";	
	numpunct<char>::GetNumPunct_M_falsename() = "false";
# ifndef _STLP_NO_WCHAR_T	
	numpunct<wchar_t>::GetNumPunct_M_Wchar_truename() = L"true";
	numpunct<wchar_t>::GetNumPunct_M_Wchar_falsename() = L"false";
# endif	
}
# endif


# ifndef _STLP_NO_WCHAR_T

_STLP_EXP_DECLSPEC  wchar_t numpunct<wchar_t>::do_decimal_point() const { return L'.'; }

_STLP_EXP_DECLSPEC wchar_t numpunct<wchar_t>::do_thousands_sep() const
{
  return L',';
}

_STLP_EXP_DECLSPEC string numpunct<wchar_t>::do_grouping() const
{
  return string();
}

_STLP_EXP_DECLSPEC wstring numpunct<wchar_t>::do_truename() const
{
  return GetNumPunct_M_Wchar_truename();
}
 
_STLP_EXP_DECLSPEC wstring numpunct<wchar_t>::do_falsename() const
{
  return GetNumPunct_M_Wchar_falsename();
} 

# if !defined(__LIBSTD_CPP_SYMBIAN32_WSD__) && !defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
_STLP_STATIC_MEMBER_EXP_DECLSPEC  wstring numpunct<wchar_t>::_M_truename(L"true");
_STLP_STATIC_MEMBER_EXP_DECLSPEC  wstring numpunct<wchar_t>::_M_falsename(L"false");
_STLP_STATIC_MEMBER_EXP_DECLSPEC  string numpunct<wchar_t>::_M_grouping("");
#endif

# endif

#ifdef	__SYMBIAN32__

_Locale_numeric* __acquire_numeric(const char* name); 
void __release_numeric(_Locale_numeric* cat);

_STLP_EXP_DECLSPEC _Locale_numeric* __acquire_numericE(const char* name){return __acquire_numeric(name); }
_STLP_EXP_DECLSPEC void __release_numericE(_Locale_numeric* _M_numeric){ __release_numeric(_M_numeric);}
_STLP_EXP_DECLSPEC const char* _Locale_trueE(_Locale_numeric* _M_numeric){ return _Locale_true(_M_numeric);}
_STLP_EXP_DECLSPEC const char* _Locale_falseE(_Locale_numeric* _M_numeric){ return _Locale_false(_M_numeric); }
_STLP_EXP_DECLSPEC char _Locale_decimal_pointE(_Locale_numeric* _M_numeric){return _Locale_decimal_point(_M_numeric); }
_STLP_EXP_DECLSPEC char _Locale_thousands_sepE(_Locale_numeric* _M_numeric){return _Locale_thousands_sep(_M_numeric); }
_STLP_EXP_DECLSPEC const char*_Locale_groupingE(_Locale_numeric* _M_numeric){return _Locale_grouping(_M_numeric); }


#endif//__SYMBIAN32
_STLP_END_NAMESPACE

// Local Variables:
// mode:C++
// End:

