/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
 */  
// =============================================================================
//	stl_rvct.h
//	This is a list of settings for STLport specific to the ARM RVCT compiler.
//	Do not include this file directly.
// =============================================================================

# ifndef _STLP_GCCXML_H
#  define  _STLP_GCCXML_H


/*
// Uncomment if member template classes are not available
#if defined(__WINS__)
#  define _STLP_NO_MEMBER_TEMPLATE_CLASSES   1
#endif

// Uncomment if partial specialization is not available
#if defined(__WINS__)
#  define _STLP_NO_CLASS_PARTIAL_SPECIALIZATION 1
#endif
*/

//#warning ********** COMPILER SETTINGS **********
// -----------------------------------------------------------------------------
//  wstring support
//  This is a bit confusing.
//  * _STLP_NO_WCHAR_T inhibits the standard C wchar functions (Symbian doesn't 
//    have them).
//  * _STLP_HAS_WCHAR_T triggers the typedef of wstring.  It's based on wint_t,
//    which isn't typedef's elsewhere, so we take care of it here.
// -----------------------------------------------------------------------------

#define _STLP_HAS_WCHAR_T 1
#  include <wchar.h>

#ifdef _WCHAR_T
#warning ********** _WCHAR_T is defined **********
#endif


#define _STLP_LIBSTD_CPP_NO_STATIC_VAR_
#define _STLP_INIT_AMBIGUITY

#endif // _STLP_RVCT_H
