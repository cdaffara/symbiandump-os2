// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __STRINGPOOL_INL__
#define __STRINGPOOL_INL__


#include <stringpoolerr.h>

inline RStringTokenBase::RStringTokenBase()
		: iVal(0)
/** Default constructor. */
	{
	}


inline RStringTokenBase RStringTokenBase::operator=(RStringBase aVal)
/** Assignment operator, which makes a string token from a string. 

@param aVal The value of the string to copy.
@return The string token base. */
	{
	iVal = aVal.iVal;
	return *this;
	}


inline TBool RStringTokenBase::IsNull() const
/** Tests if the string is null or not.

@return ETrue if the string is null, otherwise EFalse. */
	{
	return (iVal == 0);
	}


inline TBool RStringToken::operator==(RStringToken aVal) const
/** Equality operator. 

@param aVal The string to compare.
@return ETrue if the strings are the same, otherwise EFalse. */
	{
	return iVal == aVal.iVal;
	}


inline TBool RStringToken::operator!=(RStringToken aVal) const
/** Inequality operator. 

@param aVal The string to compare.
@return ETrue if the strings are different, else EFalse */
	{
	return iVal != aVal.iVal;
	}


inline RStringToken RStringToken::operator=(RString aVal)
/** Assignment operator, which makes a string token from a string. 

@param aVal The string to copy.
@return The string token. */
	{
	iVal = aVal.iVal;
	return *this;
	}


inline TBool RStringTokenF::operator==(RStringTokenF aVal) const
/** Equality operator.

@param aVal The string to compare.
@return ETrue if the strings are the same, otherwise EFalse. */
	{
	return iVal == aVal.iVal;
	}


inline TBool RStringTokenF::operator!=(RStringTokenF aVal) const
/** Inequality operator. 

@param aVal The string to compare.
@return ETrue if any strings are different, else EFalse */
	{
	return iVal != aVal.iVal;
	}


inline RStringTokenF RStringTokenF::operator=(RStringF aVal)
/** Assignment operator that makes a string token from a string. 

@param aVal The string to compare.
@return The string token. */
	{
	iVal = aVal.iVal;
	return *this;
	}


inline RStringBase::RStringBase()
		: iVal(0)
/** Default constructor. */
	{
	}

/** Returns a compact string equivalent to the string  */
inline RStringBase::operator RStringTokenBase() const
	{
	RStringTokenBase s;
	s.iVal = iVal;
	return s;
	}

inline RString RString::Copy()
/** Copies a string. 

Both the original and the copy must be separately closed.

@return The string to copy. */
	{
	RStringBase::Copy();
	return *this;
	}

/** Returns a compact string equivalent to the string (for case-sensitive strings)  */
inline RString::operator RStringToken() const
	{
	RStringToken s;
	s.iVal = iVal;
	return s;
	}

inline RStringF RStringF::Copy()
/** Copies a string. 

Both the original and the copy must be separately closed.

@return The string to copy. */
	{
	RStringBase::Copy();
	return *this;
	}


inline TBool RStringF::operator==(const RStringF& aVal) const
/** Equality operator.

@param aVal The string to compare.
@return ETrue if the strings are equal, else EFalse */
	{
	__ASSERT_DEBUG(iPool.iImplementation==aVal.iPool.iImplementation || aVal.iVal==0 || iVal==0, StringPoolPanic::Panic(StringPoolPanic::EComparisonBetweenTwoStringPoolsNotAllowed));
	return (iVal == aVal.iVal);
	}


inline TBool RStringF::operator!=(const RStringF& aVal) const
/** Inequality operator.

@param aVal The string to compare.
@return ETrue if the strings are not equal, else EFalse */
	{
	__ASSERT_DEBUG(iPool.iImplementation==aVal.iPool.iImplementation || aVal.iVal==0 || iVal==0, StringPoolPanic::Panic(StringPoolPanic::EComparisonBetweenTwoStringPoolsNotAllowed));
	return (iVal != aVal.iVal);
	}


inline RStringF::operator RStringTokenF() const
	{
	RStringTokenF s;
	s.iVal = iVal;
	return s;
	}


inline RStringPool RStringBase::Pool() const 
/** Gets the string pool.

@return The string pool. */
	{
	return iPool;
	}


inline TBool RString::operator==(const RString& aVal) const
/** Equality operator.

@param aVal The string to compare.
@return ETrue if the strings are equal, else EFalse */
	{
	__ASSERT_DEBUG(iPool.iImplementation==aVal.iPool.iImplementation || aVal.iVal==0 || iVal==0, StringPoolPanic::Panic(StringPoolPanic::EComparisonBetweenTwoStringPoolsNotAllowed));
	return (iVal == aVal.iVal);
	}


inline TBool RString::operator!=(const RString& aVal) const
/** Inequality operator.

@param aVal The string to compare.
@return ETrue if the strings are not equal, else EFalse */
	{
	__ASSERT_DEBUG(iPool.iImplementation==aVal.iPool.iImplementation || aVal.iVal==0 || iVal==0, StringPoolPanic::Panic(StringPoolPanic::EComparisonBetweenTwoStringPoolsNotAllowed));
	return (iVal != aVal.iVal);
	}

inline RStringPool::RStringPool()
		: iImplementation(0)
/** Default constructor. */
	{
	}


#endif // __STRINGPOOL_INL__
