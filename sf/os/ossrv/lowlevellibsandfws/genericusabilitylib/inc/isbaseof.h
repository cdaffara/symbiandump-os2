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


#ifndef ISBASEOF_H
#define ISBASEOF_H


// Provisional implementation of a construct that allows to determine
// at compile time whether a class type is derived from another class
// type

// To be redefined using std::tr1:is_base_of when a TR1 implementation
// becomes available for use with Symbian OS.


// Implementation construct - do not use
template<typename B, typename D>
class TIsBaseOf
	{
  private:
	typedef TUint8 TIsDerived;

	class TIsNotDerived
		{
		TUint8 iPadding[2];
		};


	static TIsDerived ReturnType(B*);
	static TIsNotDerived ReturnType(...);

  public:

	enum
	{
		EValue = sizeof(ReturnType(static_cast<D*>(0))) == sizeof(TIsDerived) ? 1 : 0
	};
	};


#define IS_BASE_OF(BaseType, DerivedType) TIsBaseOf<BaseType, DerivedType>::EValue


#endif //! ISBASEOF_H

