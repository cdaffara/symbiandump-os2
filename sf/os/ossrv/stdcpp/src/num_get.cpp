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
#include <stl/_num_get.h>
#include <stl/_istream.h>
#include <stl/_algo.h>

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
#include "libstdcppwsd.h"
# endif

_STLP_BEGIN_NAMESPACE

//----------------------------------------------------------------------
// num_get

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
//initialize the static array
void num_get_array_init()
{
	strcpy(get_num_get_narrow_digits(),"0123456789");
	strcpy(get_num_get_narrow_xdigits(),"aAbBcCdDeEfF");	
}
# else
static char narrow_digits[11]  = "0123456789";
static char narrow_xdigits[13] = "aAbBcCdDeEfF";
# endif

# ifndef _STLP_NO_WCHAR_T 

void  _STLP_CALL
_Initialize_get_digit(wchar_t* digits, wchar_t* xdigits,
                       const ctype<wchar_t>& ct)
{
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
  ct.widen(get_num_get_narrow_digits() + 0,  get_num_get_narrow_digits() + 10,  digits);
  ct.widen(get_num_get_narrow_xdigits() + 0, get_num_get_narrow_xdigits() + 10, xdigits);
#else
  ct.widen(narrow_digits + 0,  narrow_digits + 10,  digits);
  ct.widen(narrow_xdigits + 0, narrow_xdigits + 10, xdigits);
# endif  
}

// Return either the digit corresponding to c, or a negative number if
// if c isn't a digit.  We return -1 if c is the separator character, and
// -2 if it's some other non-digit.
int _STLP_CALL __get_digit(wchar_t c,
                           const wchar_t* digits, const wchar_t* xdigits,
                           wchar_t separator)
{
  // Test if it's the separator.
  if (c == separator)
    return -1;

  const wchar_t* p;

  // Test if it's a decimal digit.
  p = find(digits, digits + 10, c);
  if (p != digits + 10)
    return (int)(p - digits);

  // Test if it's a hex digit.
  p = find(xdigits, xdigits + 12, c);
  if (p != xdigits + 12)
    return (int)(10 + (xdigits - p) / 2);
  else
    return -2;                  // It's not a digit and not the separator.
}

# endif /* _STLP_NO_WCHAR_T */

// __valid_grouping compares two strings, one representing the
// group sizes encountered when reading an integer, and the other
// representing the valid group sizes as returned by the numpunct
// grouping() member function.  Both are interpreted right-to-left.
// The grouping string is treated as if it were extended indefinitely
// with its last value.  For a grouping to be valid, each term in
// the first string must be equal to the corresponding term in the
// second, except for the last, which must be less than or equal.

// boris : this takes reversed first string !
_STLP_EXP_DECLSPEC bool  _STLP_CALL
__valid_grouping(const char * first1, const char * last1, 
                 const char * first2, const char * last2)
{
  if (first1 == last1 || first2 == last2) return true;

  --last1; --last2;

  while (first1 != last1) {
    if (*last1 != *first2)
      return false;
    --last1;
    if (first2 != last2) ++first2;
  }

  return *last1 <= *first2;
}

// this needed for some compilers to make sure sumbols are extern
extern const unsigned char __digit_val_table[];
extern const char __narrow_atoms[];


const unsigned char __digit_val_table[128] = 
{
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,10,11,12,13,14,15,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,10,11,12,13,14,15,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};     

const char __narrow_atoms[5] = {'+', '-', '0', 'x', 'X'};

_STLP_EXP_DECLSPEC unsigned char* _STLP_CALL __get_digit_val_table(void)
{
	return (unsigned char*)__digit_val_table;
}
_STLP_EXP_DECLSPEC char* _STLP_CALL __get_narrow_atoms(void)
{
	return (char*)__narrow_atoms;
}
// index is actually a char

# ifndef _STLP_NO_WCHAR_T


//----------------------------------------------------------------------
// Force instantiation of of num_get<>

#if !defined(_STLP_NO_FORCE_INSTANTIATE)
template class _STLP_CLASS_DECLSPEC  istreambuf_iterator<wchar_t, char_traits<wchar_t> >;
template class num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >;
// template class num_get<wchar_t, const wchar_t*>;
#endif

# endif /* _STLP_NO_WCHAR_T */

//----------------------------------------------------------------------
// Force instantiation of of num_get<>

#if !defined(_STLP_NO_FORCE_INSTANTIATE)
template class _STLP_CLASS_DECLSPEC istreambuf_iterator<char, char_traits<char> >;
// template class num_get<char, const char*>;
template class num_get<char, istreambuf_iterator<char, char_traits<char> > >;
#endif
 
// basic_streambuf<char, char_traits<char> >* _STLP_CALL _M_get_istreambuf(basic_istream<char, char_traits<char> >& ) ;

_STLP_END_NAMESPACE

// Local Variables:
// mode:C++
// End:


