// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef ISSAME_H
#define ISSAME_H


// Provisional implementation of a construct that allows to determine
// at compile time whether a type is the same as another type

// To be redefined using std::tr1:is_same when a TR1 implementation
// becomes available for use with Symbian OS.


// Implementation construct - do not use
template<typename T, typename U>
struct TIsSame
	{
	enum
	{
		EValue = 0
	};
	};

// Implementation construct - do not use
template<typename T>
struct TIsSame<T, T>
	{
	enum
	{
		EValue = 1
	};
	};


#define IS_SAME(Type1, Type2) TIsSame<Type1, Type2>::EValue


#endif //! ISSAME_H

