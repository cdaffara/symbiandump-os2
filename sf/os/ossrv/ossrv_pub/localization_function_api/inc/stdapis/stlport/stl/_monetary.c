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
#ifndef _STLP_MONETARY_C
#define _STLP_MONETARY_C

# ifndef _STLP_INTERNAL_MONETARY_H
#  include <stl/_monetary.h>
# endif

# if defined (_STLP_EXPOSE_STREAM_IMPLEMENTATION)

#ifndef _STLP_INTERNAL_IOS_H
# include <stl/_ios.h>
#endif

#ifndef _STLP_INTERNAL_NUM_PUT_H
# include <stl/_num_put.h>
#endif

#ifndef _STLP_INTERNAL_NUM_GET_H
# include <stl/_num_get.h>
#endif

_STLP_BEGIN_NAMESPACE

# if ( _STLP_STATIC_TEMPLATE_DATA > 0 )
# if !defined(__LIBSTD_CPP_SYMBIAN32_WSD__) && !defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
template <class _CharT, class _InputIterator>
locale::id money_get<_CharT, _InputIterator>::id;

template <class _CharT, class _OutputIterator>
locale::id money_put<_CharT, _OutputIterator>::id;
#endif
# else /* ( _STLP_STATIC_TEMPLATE_DATA > 0 ) */

typedef money_get<char, const char*> money_get_char;
typedef money_put<char, char*> money_put_char;
typedef money_get<char, istreambuf_iterator<char, char_traits<char> > > money_get_char_2;
typedef money_put<char, ostreambuf_iterator<char, char_traits<char> > > money_put_char_2;

#ifndef __SYMBIAN32__
__DECLARE_INSTANCE(locale::id, money_get_char::id, );
__DECLARE_INSTANCE(locale::id, money_put_char::id, );
__DECLARE_INSTANCE(locale::id, money_get_char_2::id, );
__DECLARE_INSTANCE(locale::id, money_put_char_2::id, );
#endif

# ifndef _STLP_NO_WCHAR_T

typedef money_get<wchar_t, const wchar_t*> money_get_wchar_t;
typedef money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > > money_get_wchar_t_2;
typedef money_put<wchar_t, wchar_t*> money_put_wchar_t;
typedef money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > > money_put_wchar_t_2;

#ifndef __SYMBIAN32__
__DECLARE_INSTANCE(locale::id, money_get_wchar_t::id, );
__DECLARE_INSTANCE(locale::id, money_put_wchar_t::id, );
__DECLARE_INSTANCE(locale::id, money_get_wchar_t_2::id, );
__DECLARE_INSTANCE(locale::id, money_put_wchar_t_2::id, );
#endif

# endif
# endif /* ( _STLP_STATIC_TEMPLATE_DATA > 0 ) */

// money_get facets


// helper functions for do_get
template <class _InIt1, class _InIt2>
pair<_InIt1, bool> __get_string(_InIt1 __first,     _InIt1 __last,
                               _InIt2 __str_first, _InIt2 __str_last) {
  pair<_InIt1, _InIt2> __pr = mismatch(__first, __last, __str_first);
  return make_pair(__pr.first, __pr.second == __str_last);
}

template <class _InIt, class _OuIt, class _CharT>
bool
__get_monetary_value(_InIt& __first, _InIt __last, _OuIt __stl_out,
                     const ctype<_CharT>& _c_type,
                     _CharT   __point,
                     int      __frac_digits,
                     _CharT __sep,
                     const string& __grouping,
                     bool&         __syntax_ok)
{

	size_t __digits = 0;

  if (__first == __last || !_c_type.is(ctype_base::digit, *__first))
    return false;

  char __group_sizes[128];
  char* __group_sizes_end = __grouping.size() == 0 ? 0 : __group_sizes;
  char   __current_group_size = 0;

  while (__first != __last) {
    if (_c_type.is(ctype_base::digit, *__first)) {
      ++__current_group_size;
      *__stl_out++ = *__first++;
	  __digits++;
    }
#ifdef __SYMBIAN32__
else if ( (__current_group_size)&&(__group_sizes_end) ){
#else
    else if (__group_sizes_end) {
#endif		
      if (*__first == __sep) {
	*__group_sizes_end++ = __current_group_size; 
	__current_group_size = 0;
	++__first;
      }
      else break;
    }
    else
      break;
  }

  if (__grouping.size() == 0)
    __syntax_ok = true;
  else {
    if (__group_sizes_end != __group_sizes)
     *__group_sizes_end++ = __current_group_size; 
    
    __syntax_ok = __valid_grouping(__group_sizes, __group_sizes_end,
                                   __grouping.data(), __grouping.data()+ __grouping.size());  
    
    if (__first == __last || *__first != __point) {
      for (int __digits = 0; __digits != __frac_digits; ++__digits)
        *__stl_out++ = _CharT('0');
      return true; // OK not to have decimal point
    }
  }
//bug fix testcase_22427 - commented
//  ++__first; 


  //bug fix testcase_22427 - added if

  if (__first != __last && *__first == __point && __frac_digits)
  {  	
	//bug fix testcase_22427 - commented
	++__first; 
	  while (__first != __last && _c_type.is(ctype_base::digit, *__first)) {
	      *__stl_out++ = *__first++;
	     ++__digits;
	  }
  }

  //bug fix testcase_22427 - condition changed
//  __syntax_ok = __syntax_ok && (__digits == __frac_digits);
  
 if(__digits == 0)
  	__syntax_ok = false;

  return true;
}

# ifndef _STLP_NO_LONG_DOUBLE

//===== methods ======
template <class _CharT, class _InputIter>
_InputIter 
money_get<_CharT, _InputIter>::do_get(_InputIter __s, _InputIter  __end, bool  __intl,
				      ios_base&  __str, ios_base::iostate& __err,
				      long double& __units) const {
  string_type __buf;
  __s = do_get(__s, __end, __intl, __str, __err, __buf);

  if (__err == ios_base::goodbit || __err == ios_base::eofbit) {
    __buf.push_back(0);
    typename string_type::iterator __b = __buf.begin(), __e = __buf.end();
    // Can't use atold, since it might be wchar_t. Don't get confused by name below :
    // it's perfectly capable of reading long double.
    __get_decimal_integer(__b, __e, __units);
  }
  if (__s == __end)
    __err |= ios_base::eofbit;
  return __s;
}
# endif

template <class _CharT, class _InputIter>
_InputIter 
money_get<_CharT, _InputIter>::do_get(iter_type __s, 
				      iter_type  __end, bool  __intl,
				      ios_base&  __str, ios_base::iostate&  __err,
				      string_type& __digits) const {
	__err = 0;
  if (__s == __end) {
    __err |= ios_base::eofbit;
    return __s;
  }

  typedef moneypunct<_CharT, false> _Punct;
  typedef moneypunct<_CharT, true>  _Punct_intl;
  typedef ctype<_CharT>             _Ctype;
#ifdef __SYMBIAN32__			
	bool		__isSignSympresent = false;
#endif			

 bool	__testvalid = true;
  locale __loc = __str.getloc();
  const _Punct&      __punct      = use_facet<_Punct>(__loc) ;
  const _Punct_intl& __punct_intl = use_facet<_Punct_intl>(__loc) ;
  const _Ctype&      __c_type      = use_facet<_Ctype>(__loc) ;
                   
  money_base::pattern __format = __intl ? __punct_intl.neg_format()
                                        : __punct.neg_format();
  string_type __ns = __intl ? __punct_intl.negative_sign()
                            : __punct.negative_sign();
  string_type __ps = __intl ? __punct_intl.positive_sign()
                            : __punct.positive_sign();
  string_type __sign  = __ps;
  int __i;
  bool __is_positive = true;
  bool __symbol_required = (__str.flags() & ios_base::showbase) !=0;
  string_type __buf;
  back_insert_iterator<string_type> __stl_out(__buf);
//  pair<iter_type, bool> __result;

  for (__i = 0; __i < 4; ++__i) {
    switch (__format.field[__i]) {
    case (char) money_base::none:
#ifndef	__SYMBIAN32__
	  case (char) money_base::space:
#endif	  	

      if (__i == 3) {
        //if (__c_type.is(ctype_base::space, *__s)) { //gnu buf fix, 22/12/06
        if((__s != __end) && (!__c_type.is(ctype_base::space, *__s)) && (__ps.size()<=1) && (__ns.size()<=1) ){
          __err = ios_base::failbit;
          return __s;
        }
       // break; //gnu bug fix 2/1/07
      }
      while (__s != __end && __c_type.is(ctype_base::space, *__s))
        ++__s;
      break;
#ifdef	__SYMBIAN32__	  
    case (char) money_base::space:
      if (!__c_type.is(ctype_base::space, *__s)) {
#ifdef __SYMBIAN32__      
      	if(!__isSignSympresent) //if no sign symbol present, space is not mandatory.
      		break;
#endif      	
        __err = ios_base::failbit;
        return __s;
      }
      ++__s;
      while (__s != __end && __c_type.is(ctype_base::space, *__s))
        ++__s;
      break;
#endif	  
    case money_base::symbol: {
      string_type __curs = __intl ? __punct_intl.curr_symbol()
                                : __punct.curr_symbol();
      pair<iter_type, bool>
      __result  = __get_string(__s, __end, __curs.begin(), __curs.end());
      if (!__result.second && __symbol_required)
        __err = ios_base::failbit;
      __s = __result.first;
      break;
    }
    case money_base::sign: {
      if (__s == __end) {
        if (__ps.size() == 0)
          break;
        if (__ns.size() == 0) {
          __is_positive = false;
	__sign  = __ns;
          break;
        }
        __err = ios_base::failbit;
        return __s;
      }
      else {
        if (__ps.size() == 0) {
          if (__ns.size() == 0)
            break;
          if (*__s == __ns[0]) {
            ++__s;
            __is_positive = false;
			__sign  = __ns;
#ifdef __SYMBIAN32__			
			__isSignSympresent = true;
#endif			

            break;
          }
        //  __err = ios_base::failbit; //if ps !=0 and ns!=0, and no negative sign mean it is positive, not fail.
	  //          return __s;
	  break;
        } 
        else {
          if (*__s == __ps[0]) {
            ++__s;
#ifdef __SYMBIAN32__			
			__isSignSympresent = true;
#endif			

            break;
          }
#ifdef	__SYMBIAN32__
	    if (__ns.size() == 0) //here positive have symbol,negative have no symbol
	    {
	    	__is_positive = false; // in this case if symbol not present means it is negative
	    	__sign = __ns;
	    	break;
	    }
            
#else          
          if (__ns.size() == 0)
            break;
#endif          
          if (*__s == __ns[0]) {
            ++__s;
            __is_positive = false;
			__sign  = __ns;
#ifdef __SYMBIAN32__			
			__isSignSympresent = true;
#endif			
            break;
          }
          __err = ios_base::failbit;
	  //          return __s;
        }
      }
      return __s;
      //      break;
    }
    case money_base::value: {
      _CharT __point = __intl ? __punct_intl.decimal_point()
                              : __punct.decimal_point();
      int __frac_digits = __intl ? __punct_intl.frac_digits()
                                 : __punct.frac_digits();
      string __grouping = __intl ? __punct_intl.grouping()
                                 : __punct.grouping();
      bool __syntax_ok = true;

      bool __result;

      _CharT __sep = __grouping.size() == 0 ? _CharT() : 
	__intl ? __punct_intl.thousands_sep() : __punct.thousands_sep();

      __result = __get_monetary_value(__s, __end, __stl_out, __c_type,
                                      __point, __frac_digits,
                                      __sep,
                                      __grouping, __syntax_ok);      

      if (!__syntax_ok)
        __err |= ios_base::failbit;
      if (!__result) {
        __err = ios_base::failbit;
        return __s;
      }
      break;
      
    }                           // Close money_base::value case


    }                           // Close switch statement
  }                             // Close for loop

#ifndef __SYMBIAN32__ //find the given number pos or neg, required for cases where sign is present at string starting and ending
	do{
	bool type = true;

	int si = __ps.size();
	for (int i = 1;i<__ps.size();i++)
		{
			if(__s[i-1]!=__ps[i])
			{
				type = false;
				break;
			}
		}
	
	if ((si!=0) && (type ==true))
	{
		__sign = __ps;
            break;
	}  
          if (__ns.size() == 0)
            break;
	type = true;
	for (int i =1;i<__ns.size();i++)
		{
			if(__s[i-1]!=__ns[i])
			{
				type = false;
				break;
			}
		}
          if (type ==true) {
            __is_positive = false;
		__sign = __ns;
            break;
          }
	}while(0);

#endif


#ifdef __SYMBIAN32__
// Need to get the rest of the sign characters, if they exist.
      if (__sign.size() > 1)
        {
          int __len = __sign.size();
          int __i = 1;
	  char_type __c = *__s;
	  char_type __eol = '\0';
          for (; __c != __eol && __i < __len; __i++)
          	{
            	if (__s != __end)
              	__c = *(++__s);
            	
          }
		  //checking sign completely extracted successfully
		 
          if (__i != __len)
            __testvalid = false;
        }

#endif

  if (__is_positive) {
    if (__ps.size() > 1) {
#ifndef __SYMBIAN32__		
      pair<_InputIter, bool>
        __result = __get_string(__s, __end, __ps.begin() + 1, __ps.end());
      __s = __result.first;
      if (!__result.second)
	__err |= ios::failbit;
#endif
	if(!__testvalid)
		__err |= ios::failbit;
    }
    if (!(__err & ios_base::failbit))
      __digits = __buf;
  }
  else {
    if (__ns.size() > 1) {
#ifndef __SYMBIAN32__		
      pair<_InputIter, bool>
        __result = __get_string(__s, __end, __ns.begin() + 1, __ns.end());
      __s = __result.first;
      if (!__result.second)
	__err |= ios::failbit;
#endif
	if(!__testvalid)
		__err |= ios::failbit;
    }
    if (!(__err & ios::failbit) && (__ns == __sign)) {
      __buf.insert(__buf.begin(),__c_type.widen('-'));      
    }
#ifdef __SYMBIAN32__	
  if (!(__err & ios_base::failbit))
      __digits = __buf;	
#else  
   __digits = __buf;	
#endif
  }
  if (__s == __end)
    __err |= ios::eofbit;

  return __s;
}

// money_put facets

template <class _CharT, class _OutputIter>
_OutputIter
money_put<_CharT, _OutputIter>
 ::do_put(_OutputIter __s, bool __intl, ios_base& __str,
          char_type __fill,
          const string_type& __digits) const { 
  typedef ctype<_CharT>             _Ctype;
  typedef moneypunct<_CharT, false> _Punct;
  typedef moneypunct<_CharT, true>  _Punct_intl;

  locale __loc = __str.getloc();
  const _Ctype&      __c_type      = use_facet<_Ctype>(__loc) ;
  const _Punct&      __punct      = use_facet<_Punct>(__loc) ;
  const _Punct_intl& __punct_intl = use_facet<_Punct_intl>(__loc) ;

  // some special characters

  char_type __minus = __c_type.widen('-');
  char_type __plus  = __c_type.widen('+');
  char_type __space = __c_type.widen(' ');
  char_type __zero  = __c_type.widen('0');
  char_type __point = __intl ? __c_type.widen(__punct_intl.decimal_point())
			     : __c_type.widen(__punct.decimal_point());

  char_type __sep = __intl ? __punct_intl.thousands_sep()
			   : __punct     .thousands_sep();

  string __grouping = __intl ? __punct_intl.grouping()
		             : __punct     .grouping();
				
  int __frac_digits      = __intl ? __punct_intl.frac_digits() 
                                  : __punct.frac_digits();

  string_type __curr_sym = __intl ? __punct_intl.curr_symbol() 
                                  : __punct.curr_symbol();

    // if there are no digits we are going to return __s.  If there
    // are digits, but not enough to fill the frac_digits, we are
    // going to add zeros.  I don't know whether this is right or
    // not.

  if (__digits.size() == 0) 
    return __s;

  typename string_type::const_iterator __digits_first = __digits.begin();
  typename string_type::const_iterator __digits_last  = __digits.end();

  bool __is_negative = *__digits_first == __minus;
  if (__is_negative)
    ++__digits_first;

  string_type __sign = __intl ?
			 __is_negative ? __punct_intl.negative_sign()
				       : __punct_intl.positive_sign()
			      :
			 __is_negative ? __punct.negative_sign()
				       : __punct.positive_sign();
  typename string_type::const_iterator __cp = __digits_first;
  while (__cp != __digits_last && __c_type.is(ctype_base::digit, *__cp))
    ++__cp;
  if (__cp == __digits_first)
    return __s;
  __digits_last = __cp;

  // If grouping is required, we make a copy of __digits and
  // insert the grouping.

  // To handle the fractional digits, we augment the first group
  // by frac_digits.  If there is only one group, we need first
  // to duplicate it.

  string_type __new_digits(__digits_first, __digits_last);
#ifdef	__SYMBIAN32__
	int __numberofseperators = 0;
	if (__grouping.size()>0)
	__numberofseperators = (__new_digits.size()/__grouping[0])+1;
	else
		__numberofseperators = 0;
  __new_digits.resize(__new_digits.size()+__numberofseperators);
#endif  
  if (__grouping.size() != 0) {
    if (__grouping.size() == 1)
      __grouping.push_back(__grouping[0]);
    __grouping[0] += __frac_digits;
    _CharT* __data_ptr = __CONST_CAST(_CharT*,__new_digits.data());
    _CharT* __data_end = __data_ptr + __new_digits.size();
    
    
    ptrdiff_t __value_length = __insert_grouping(__data_ptr,
#ifdef	__SYMBIAN32__	    
	  				         __data_end-__numberofseperators,
#else
							__data_end,
#endif	  				         
					         __grouping,
					         __sep,
					         __plus, __minus, 0);
    __digits_first = __new_digits.begin();
    __digits_last  = __digits_first + __value_length;
  }

  // Determine the amount of padding required, if any.  
#ifdef	__SYMBIAN32__
	  int __width        = __str.width(); //width returns signed value.
#else    
  size_t __width        = __str.width();
#endif

#if defined(_STLP_DEBUG) && (defined(__HP_aCC) || (__HP_aCC <= 1))
  size_t __value_length = operator -(__digits_last, __digits_first);
#else
  size_t __value_length = __digits_last - __digits_first;
#endif

  size_t __length       = __value_length;
      
  __length += __sign.size();
  if (__frac_digits != 0)
    ++__length;

  bool __generate_curr = (__str.flags() & ios_base::showbase) !=0;
  if (__generate_curr)
    __length += __curr_sym.size();
  money_base::pattern __format =
    __intl ? (__is_negative ? __punct_intl.neg_format() 
                            : __punct_intl.pos_format())
           : (__is_negative ? __punct.neg_format() 
                            : __punct.pos_format());
  {
    for (int __i = 0; __i < 4; ++__i)
      if (__format.field[__i] == (char) money_base::space)
        ++__length;
  }

  size_t __fill_amt = (int)__length < __width ? __width - __length : 0;

  ios_base::fmtflags __fill_pos = __str.flags() & ios_base::adjustfield;

  if (__fill_amt != 0 &&
      !(__fill_pos & (ios_base::left | ios_base::internal)))
    __s = fill_n(__s, __fill_amt, __fill);
    
  for (int __i = 0; __i < 4; ++__i) {
    char __ffield = __format.field[__i];
    if (__ffield == money_base::none) {
      if (__fill_amt != 0 && __fill_pos == ios_base::internal)
        __s = fill_n(__s, __fill_amt, __fill);
    }
    else if (__ffield == money_base::space) {
#ifdef __SYMBIAN32__
	if(__fill != __space)
		*__s++ = __fill;
	else
#endif		
      *__s++ = __space;
      if (__fill_amt != 0 && __fill_pos == ios_base::internal)
        __s = fill_n(__s, __fill_amt, __fill);
    }
    else if (__ffield == money_base::symbol) {
      if (__generate_curr)
        __s = copy(__curr_sym.begin(), __curr_sym.end(), __s);
    }
    else if (__ffield == money_base::sign) {
      if (__sign.size() != 0)
        *__s++ = __sign[0];
    }
    else if (__ffield == money_base::value) {
      if (__frac_digits == 0)
        __s = copy(__digits_first, __digits_last, __s);
      else {
        if ((int)__value_length <= __frac_digits) {
          *__s++ = __point;
          __s = copy(__digits_first, __digits_last, __s);
          __s =  fill_n(__s, __frac_digits - __value_length, __zero);
        }
        else {
#ifdef	__SYMBIAN32__
	  if (__frac_digits>0)
	  	{
#endif	  	
          __s = copy(__digits_first, __digits_last - __frac_digits, __s);
          if (__frac_digits != 0) {
            *__s++ = __point;
            __s = copy(__digits_last - __frac_digits, __digits_last, __s);
#ifdef	__SYMBIAN32__
	          }
#endif			  
          }
        }
      }
    }
  } // Close for loop

  // Ouput rest of sign if necessary.

  if (__sign.size() > 1)
    __s = copy(__sign.begin() + 1, __sign.end(), __s);
  if (!(__fill_pos & (ios_base::right | ios_base::internal)))
    __s = fill_n(__s, __fill_amt, __fill);
  
#ifdef __SYMBIAN32__
	__str.width(0);
#endif
  return __s;
}

#ifdef	__SYMBIAN32__
/*
template<class _CharT, bool _International>//__DFL_NON_TYPE_PARAM(bool, _International, false)>
moneypunct<_CharT, _International>::moneypunct(size_t __refs = 0):_BaseFacet(__refs)
{ 
	
	
}*/

template<class _CharT, bool _International>//__DFL_NON_TYPE_PARAM(bool, _International, false)>
_CharT        moneypunct<_CharT,_International>::do_decimal_point() const
{
	return _CharT('.');
}

template<class _CharT, bool _International>//__DFL_NON_TYPE_PARAM(bool, _International, false)>
_CharT        moneypunct<_CharT,_International>::do_thousands_sep() const
{
	return _CharT(',');
}
template<class _CharT, bool _International>//__DFL_NON_TYPE_PARAM(bool, _International, false)>
basic_string<_CharT>      moneypunct<_CharT,_International>::do_curr_symbol()   const
{
	return _M_currSym;
}

template<class _CharT, bool _International>//__DFL_NON_TYPE_PARAM(bool, _International, false)>
basic_string<_CharT>      moneypunct<_CharT,_International>::do_positive_sign()   const
{
	return _M_psign;
}

template<class _CharT, bool _International>//__DFL_NON_TYPE_PARAM(bool, _International, false)>
basic_string<_CharT>      moneypunct<_CharT,_International>::do_negative_sign()   const
{
	return _M_nsign;
}

template<class _CharT, bool _International>//__DFL_NON_TYPE_PARAM(bool, _International, false)>
string      moneypunct<_CharT,_International>::do_grouping()   const
{
	return _M_group;
}

template<class _CharT, bool _International>//__DFL_NON_TYPE_PARAM(bool, _International, false)>
money_base::pattern        moneypunct<_CharT,_International>::do_pos_format()   const
{
	return _M_pos_format;
}

template<class _CharT, bool _International>//__DFL_NON_TYPE_PARAM(bool, _International, false)>
money_base::pattern        moneypunct<_CharT,_International>::do_neg_format()   const
{
	return _M_neg_format;
}

template<class _CharT, bool _International>
int       moneypunct<_CharT,_International>::do_frac_digits()   const
{
	return 0;
}


//monetary_byname<any>

template<class _CharT, bool _International>
_CharT moneypunct_byname<_CharT, _International>::do_decimal_point() const 
  {return _Locale_mon_decimal_pointE(_M_monetary);}

template<class _CharT, bool _International>
_CharT moneypunct_byname<_CharT, _International>::do_thousands_sep() const
  {return _Locale_mon_thousands_sepE(_M_monetary);}

template<class _CharT, bool _International>
string  moneypunct_byname<_CharT, _International>::do_grouping() const
  {return moneypunct<_CharT,_International>::_M_group;}

template<class _CharT, bool _International>
basic_string<_CharT> moneypunct_byname<_CharT, _International>::do_curr_symbol() const
  {return moneypunct<_CharT,_International>::_M_currSym;}

template<class _CharT, bool _International>
basic_string<_CharT> moneypunct_byname<_CharT, _International>::do_positive_sign() const
  {return moneypunct<_CharT,_International>::_M_psign;}

template<class _CharT, bool _International>
basic_string<_CharT> moneypunct_byname<_CharT, _International>::do_negative_sign() const
  {return moneypunct<_CharT,_International>::_M_nsign;}

template<class _CharT, bool _International>
int moneypunct_byname<_CharT, _International>::do_frac_digits() const 
  {return _Locale_int_frac_digitsE(_M_monetary);}

template<class _CharT, bool _International>
void moneypunct_byname<_CharT, _International>::Convert_string2_string_chart(basic_string<_CharT> &dst, string src)
	{
		int length = src.length();
		const char* str = src.c_str();
		for(int i = 0; i<length;i++)
		{
			dst.append(1, (_CharT)*str++);
			
		}
			
	}

template<class _CharT, bool _International>
moneypunct_byname<_CharT, _International>::moneypunct_byname(const char * name,
						  size_t refs):
  moneypunct<_CharT, _International>(refs),
  _M_monetary(__acquire_monetaryE(name))
{
  if (!_M_monetary)
    locale::_M_throw_runtime_error();
  
  		moneypunct<_CharT,_International>::_M_group = _Locale_mon_groupingE(_M_monetary);
		Convert_string2_string_chart(moneypunct<_CharT,_International>::_M_psign, _Locale_positive_signE(_M_monetary));
		Convert_string2_string_chart(moneypunct<_CharT,_International>::_M_nsign, _Locale_negative_signE(_M_monetary));
		Convert_string2_string_chart(moneypunct<_CharT,_International>::_M_currSym, _Locale_int_curr_symbolE(_M_monetary));
}

template<class _CharT, bool _International>
moneypunct_byname<_CharT, _International>::~moneypunct_byname()
{
  __release_monetaryE(_M_monetary);
}


#endif
_STLP_END_NAMESPACE

# endif /* EXPOSE */

#endif /* _STLP_MONETARY_C */
