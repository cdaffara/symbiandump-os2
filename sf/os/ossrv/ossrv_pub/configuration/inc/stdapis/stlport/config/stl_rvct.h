/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.

* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this 
* list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
* this list of conditions and the following disclaimer in the documentation 
* and/or other materials provided with the distribution.
* Neither the name of Nokia Corporation nor the names of its contributors 
* may be used to endorse or promote products derived from this software 
* without specific prior written permission.

* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Description:
*
*/
  
// =============================================================================
//	stl_rvct.h
//	This is a list of settings for STLport specific to the ARM RVCT compiler.
//	Do not include this file directly.
// =============================================================================

# ifndef _STLP_RVCT_H
#  define  _STLP_RVCT_H


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

#endif // _STLP_RVCT_H
