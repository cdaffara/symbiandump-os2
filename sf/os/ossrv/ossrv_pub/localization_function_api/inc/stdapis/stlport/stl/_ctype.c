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
#ifndef _STLP_CTYPE_C
#define _STLP_CTYPE_C

#ifndef _STLP_INTERNAL_CTYPE_H
# include <stl/_ctype.h>
#endif


//----------------------------------------------------------------------
// ctype<charT> //any character other han char, wchar_t
//for instantiation of ctype

template<class charT>
 ctype<charT>::~ctype() {}


template<class charT>
bool ctype<charT>::do_is(ctype_base::mask  m, charT c) const
{
	return ((*(_get_classic_table()+(unsigned char)c)) & m) != 0; 
}

template<class charT>
const charT* ctype<charT>::do_is(const charT* low, const charT* high,
                                     ctype_base::mask * vec) const
{
	for (;low != high; ++low, ++vec) {
      *vec = _get_classic_table()[(unsigned char)*low];
    }
    return low;
}

template<class charT>
const charT* 
ctype<charT>::do_scan_is(ctype_base::mask  m,
                           const charT* low, const charT* high) const
{
	while (low!= high && !do_is(m,*low))
    		++low;
	
  	return low;
}


template<class charT>
const charT*
ctype<charT>::do_scan_not(ctype_base::mask  m,
                            const charT* low, const charT* high) const
{
	while (low!= high && do_is(m,*low))
    		++low;
	
  	return low;
}

template<class charT>
charT ctype<charT>::do_toupper(charT c) const
{
	return (charT) _get_S_upper_table()[(unsigned char) c];
}

template<class charT>
const charT* 
ctype<charT>::do_toupper(charT* low, const charT* high) const
{
	for ( ; low < high; ++low)
    		*low = (charT) _get_S_upper_table()[(unsigned char) *low];
  	return high;
}

template<class charT>
charT ctype<charT>::do_tolower(charT c) const
{
	return (charT) _get_S_lower_table()[(unsigned char) c];
}

template<class charT> 
const charT* 
ctype<charT>::do_tolower(charT* low, const charT* high) const
{
	for ( ; low < high; ++low)
    		*low = (charT) _get_S_lower_table()[(unsigned char) *low];
  	return high;
}

template<class charT>
charT ctype<charT>::do_widen(char c) const 
{
  return (charT) c;
}

template<class charT>
const char* 
ctype<charT>::do_widen(const char* low, const char* high,
                         charT* dest) const
{
  while (low != high)
    *dest++ = (charT) *low++;
  return high;
}

template<class charT>
char ctype<charT>::do_narrow(charT c, char dfault) const 
{
  return (char) c == c ? c : dfault;
}

template<class charT>
const charT* ctype<charT>::do_narrow(const charT* low,
                                         const charT* high,
                                         char dfault, char* dest) const
{
  while (low != high) {
    charT c = *low++;
    *dest++ = (char) c == c ? c : dfault;
  }

  return high;
}

// ctype_byname<charT> //any character other han char, wchar_t
template<class charT>
ctype_byname<charT>::ctype_byname(const char* name, size_t refs)
  : ctype<charT>(refs),
    _M_ctype(__acquire_ctypeE(name))
{
  if (!_M_ctype)
    locale::_M_throw_runtime_error();    
}


template<class charT>
ctype_byname<charT>::~ctype_byname() {
	__release_ctypeE(_M_ctype);
}

template<class charT>
bool ctype_byname<charT>::do_is(ctype_base::mask  m, charT c) const
{
  return (m & __Locale_ctypeE(_M_ctype, c, m)) != 0;
}

template<class charT>
const charT*
ctype_byname<charT>::do_is(const charT* low, const charT* high,
                             ctype_base::mask * m) const
{
  ctype_base::mask all_bits = ctype_base::mask(
    ctype_base::space |
    ctype_base::print |
    ctype_base::cntrl |
    ctype_base::upper |
    ctype_base::lower |
    ctype_base::alpha |
    ctype_base::digit |
    ctype_base::punct |
    ctype_base::xdigit);

  for ( ; low < high; ++low, ++m)
    *m = ctype_base::mask (__Locale_ctypeE(_M_ctype, *low, all_bits));
  return high;
}


template<class charT>
const charT*
ctype_byname<charT>
  ::do_scan_is(ctype_base::mask  m, const charT* low, const charT* high) const
{
	while (low!= high && !do_is(m,*low))
    		++low;
	
  	return low;
}

template<class charT>
const charT*
ctype_byname<charT>
  ::do_scan_not(ctype_base::mask  m, const charT* low, const charT* high) const
{
	while (low!= high && do_is(m,*low))
    		++low;
	
  	return low;
}


template<class charT>
charT ctype_byname<charT>::do_toupper(charT c) const 
{
  return __Locale_toupperE(_M_ctype, c);
}


template<class charT>
const charT* 
ctype_byname<charT>::do_toupper(charT* low, const charT* high) const
{
  for ( ; low < high; ++low)
    *low = __Locale_toupperE(_M_ctype, *low);
  return high;
}


template<class charT>
charT ctype_byname<charT>::do_tolower(charT c) const 
{
  return __Locale_tolowerE(_M_ctype, c);
}

template<class charT>
const charT* 
ctype_byname<charT>::do_tolower(charT* low, const charT* high) const
{
  for ( ; low < high; ++low)
    *low = __Locale_tolowerE(_M_ctype, *low);
  return high;
}

#endif //#ifndef _STLP_CTYPE_C
