/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Define inline functions supported by TKeyHandleImpl.
*
*/




/**
 @file
 @internalComponent
*/

#ifndef KMSKEXT_INL
#define KMSKEXT_INL

#include "kmskext.h"


inline TKeyHandleImpl::TKeyHandleImpl(TKeyHandle aHandle)
/**
	Construct an implementation key handle from a user key handle.
	
	@param	aHandle			Identifies existing key in store.
 */
	{
	__ASSERT_COMPILE(sizeof(TKeyHandleImpl) == sizeof(TKeyHandle));
	*this = *reinterpret_cast<const TKeyHandleImpl*>(&aHandle);
	}

inline TKeyHandleImpl::TKeyHandleImpl(TCreator aCreator, TUint aIdentifier)
/**
	Construct an implementation key handle from the supplied creator
	and identifier.
	
	@param	aCreator		Identifies this handle as built-in or user-defined.
	@param	aIdentifier		Low 31 bits are used as a unique identifier.
 */
:	iCreator(aCreator),
	iIdentifier(aIdentifier)
	{
	// empty.
	}

inline TBool TKeyHandleImpl::operator==(const TKeyHandleImpl& aRhs) const
/**
	Compare this implementation key with the supplied key.
	
	@param	aRhs			Key to compare this object against.
	@return					Whether the handles describe the same key.
 */
	{
	return iCreator == aRhs.iCreator && iIdentifier == aRhs.iIdentifier;
	}

#endif	// #ifndef KMSKEXT_INL

