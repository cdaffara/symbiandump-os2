/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#ifndef _STLP_NUMPUNCT_C
#define _STLP_NUMPUNCT_C
#ifndef _STLP_INTERNAL_NUMPUNCT_H
# include <stl/_numpunct.h>
#endif


template<class _CharT>
 numpunct<_CharT>::~numpunct() { }

template<class _CharT>
_CharT numpunct<_CharT>::do_decimal_point() const
{
	return (_CharT)'.'; 
}

template<class _CharT>
 _CharT numpunct<_CharT>::do_thousands_sep() const
{
	return (_CharT)','; 
}


template<class _CharT>
string numpunct<_CharT>::do_grouping() const
{
	return _M_grouping;
}

template<class _CharT>
basic_string<_CharT> numpunct<_CharT>::do_truename() const
{
	return _M_truename;
}

template<class _CharT>
basic_string<_CharT> numpunct<_CharT>::do_falsename() const
{
	return _M_falsename;
}


template<class _CharT>
numpunct_byname<_CharT>::numpunct_byname(const char* name, size_t refs)
	: numpunct<_CharT> (refs),
    _M_numeric(__acquire_numericE(name))
{
  if (!_M_numeric)
    locale::_M_throw_runtime_error();

  const char* truename  = _Locale_trueE(_M_numeric);
  const char* falsename = _Locale_falseE(_M_numeric);
	
}

template<class _CharT>
numpunct_byname<_CharT>::~numpunct_byname()
{ 	
	__release_numericE(_M_numeric);
}

template<class _CharT>
_CharT   numpunct_byname<_CharT>::do_decimal_point() const 
{ 
	return (_CharT ) _Locale_decimal_pointE(_M_numeric);
}
template<class _CharT>
_CharT   numpunct_byname<_CharT>::do_thousands_sep() const 
{
	return (_CharT) _Locale_thousands_sepE(_M_numeric); 
}

template<class _CharT>
string numpunct_byname<_CharT>::do_grouping()      const 
{  
		const char * __grouping = _Locale_groupingE(_M_numeric);
  		if (__grouping != NULL && __grouping[0] == CHAR_MAX)
    		__grouping = "";
		
  		return NULL;
}




//----------------------------------------------------------------------

#endif //#ifndef _STLP_NUMPUNCT_C

