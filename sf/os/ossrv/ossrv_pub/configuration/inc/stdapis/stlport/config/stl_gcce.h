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

  
// =============================================================================
//	stl_gcce.h
//	This is a list of settings for STLport specific to the GCCE compiler.
//	Do not include this file directly.
// =============================================================================

# ifndef _STLP_GCCE_H
#  define  _STLP_GCCE_H


#ifndef __PLACEMENT_NEW_INLINE
#ifndef __E32STD_H__
#ifdef __cplusplus
inline void * operator new(unsigned int, void *_P) throw() { return (_P); }
inline void operator delete(void* /*aPtr*/, void* /*aBase*/) __NO_THROW {}
#endif
#define __PLACEMENT_NEW_INLINE
#endif // E32STD_H
#endif

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

#endif // _STLP_GCCE_H
