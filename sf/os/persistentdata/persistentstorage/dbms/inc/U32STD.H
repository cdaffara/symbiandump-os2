// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef DBMS_INC_U32STD_H
#define DBMS_INC_U32STD_H

#ifndef __E32STD_H__
#include <e32std.h>
#endif
#include "D32Assert.h"

// MSVC++ up to 5.0 has problems with expanding inline functions
// This disables the mad warnings for the whole project
#if defined(NDEBUG) && defined(__VC32__) && _MSC_VER<=1100
#pragma warning(disable : 4710)			// function not expanded. MSVC 5.0 is stupid
#endif

// need a const iterator!
template <class T>
class TSglQueIterC : public TSglQueIterBase
/**
@internalComponent
*/
	{
public:
	inline TSglQueIterC(const TSglQueBase& aQue);
	inline operator const T* ();
	inline const T* operator++(TInt);
	};

template <class T>
inline TSglQueIterC<T>::TSglQueIterC(const TSglQueBase& aQue)
	: TSglQueIterBase(CONST_CAST(TSglQueBase&,aQue))
	{}
template <class T>
inline TSglQueIterC<T>::operator const T* ()
	{return (const T*)DoCurrent();}
template <class T>
inline const T* TSglQueIterC<T>::operator++(TInt)
	{return (const T*)DoPostInc();}

#endif
