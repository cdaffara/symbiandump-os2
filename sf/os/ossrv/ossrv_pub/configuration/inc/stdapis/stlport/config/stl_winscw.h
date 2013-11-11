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
//	stl_winscw.h
//	This is a list of settings for STLport specific to the Metrowerks
//  CodeWarrior for Symbian compiler.  Do not include this file directly.
// =============================================================================

#ifndef _STLP_WINSCW
#	define  _STLP_WINSCW

// Uncomment if member template classes are not available
#if defined(__WINS__)
//#  define _STLP_NO_MEMBER_TEMPLATE_CLASSES   1
#endif

// Uncomment if partial specialization is not available
#if defined(__WINS__)
//#  define _STLP_NO_CLASS_PARTIAL_SPECIALIZATION 1
#endif

// -----------------------------------------------------------------------------
//  wstring support
//  This is a bit confusing.
//  * _STLP_NO_WCHAR_T inhibits the standard C wchar functions (Symbian doesn't 
//    have them).
//  * _STLP_HAS_WCHAR_T triggers the typedef of wstring.  It's based on wint_t,
//    which isn't typedef's elsewhere, so we take care of it here.
// -----------------------------------------------------------------------------

#	define _STLP_HAS_WCHAR_T 1
#	define _STLP_WCHAR_T_IS_USHORT 1
#  include <wchar.h>


#define __LIBSTD_CPP_SYMBIAN32_WSD__

//==========================================================
#endif  // _STLP_WINSCW
