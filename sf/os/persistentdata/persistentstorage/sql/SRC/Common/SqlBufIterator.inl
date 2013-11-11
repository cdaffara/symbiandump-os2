// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// /////////////////////          TSqlBufRIterator class          /////////////////////////////////////////
// 
//

/**
 
 Initializes the iterator.
 After the initialization the iterator points to the first flat buffer field - 1.
 
 @param aBuf A reference to the buffer which will be iterated.
*/
inline void TSqlBufRIterator::Set(const RSqlBufFlat& aBuf)
	{
	iBegin = aBuf.Header();
	iCurrent = iBegin - 1;
	iEnd = iBegin + aBuf.Count();
	}

/**
Moves to the next flat buffer field

@return False if there are no more fields to be iterated.
*/
inline TBool TSqlBufRIterator::Next()
	{
	return ++iCurrent < iEnd;
	}

/**
Moves to the specified field in the flat buffer.

@param aIndex Field index
*/
inline void TSqlBufRIterator::MoveTo(TInt aIndex)
	{
	__ASSERT_DEBUG((iBegin + (TUint)aIndex) < iEnd, __SQLPANIC(ESqlPanicBadArgument));
	iCurrent = iBegin + aIndex;
	}

/**
@return True if the current flat buffer field is "Present"
*/
inline TBool TSqlBufRIterator::IsPresent() const
	{
	__ASSERT_DEBUG(iCurrent >= iBegin && iCurrent < iEnd, __SQLPANIC(ESqlPanicInternalError));
	return iCurrent->iPos > 0;
	}
	
/**
@return Current flat buffer field type. One of TSqlColumnType enum item values or ESqlText8.
*/
inline TInt TSqlBufRIterator::Type() const
	{
	__ASSERT_DEBUG(iCurrent >= iBegin && iCurrent < iEnd, __SQLPANIC(ESqlPanicInternalError));
	return iCurrent->Type();
	}
	
/**
@return Current flat buffer field size
*/
inline TInt TSqlBufRIterator::Size() const
	{
	__ASSERT_DEBUG(iCurrent >= iBegin && iCurrent < iEnd, __SQLPANIC(ESqlPanicInternalError));
	return Type() == ESqlText ? iCurrent->Size() / sizeof(TUint16) : iCurrent->Size();
	}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////                 TSqlBufWIterator class          //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Initializes the iterator.
After the initialization the iterator points to the first flat buffer field - 1.

@param aBuf A reference to the buffer which will be iterated.
*/
inline void TSqlBufWIterator::Set(RSqlBufFlat& aBuf)
	{
	iBuf = &aBuf;
	iIndex = -1;
	}
	
/**
Moves to the next flat buffer field

@return False if there are no more fields to be iterated.
*/
inline TBool TSqlBufWIterator::Next()
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInternalError));
	return ++iIndex < iBuf->Count();	
	}
	
/**
Moves to the specified field in the flat buffer.

@param aIndex Field index
*/
inline void TSqlBufWIterator::MoveTo(TInt aIndex)
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG((TUint)aIndex < iBuf->Count(), __SQLPANIC(ESqlPanicInternalError));
	iIndex = aIndex;
	}

/**
Sets the current flat buffer field to NULL.
*/
inline void TSqlBufWIterator::SetNull()
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG((TUint)iIndex < iBuf->Count(), __SQLPANIC(ESqlPanicInternalError));
	(void)iBuf->SetField(iIndex, ESqlNull, NULL, 0);
	}
	
/**
Sets the current flat buffer field as "Not present".

@param aType Field type. One of TSqlColumnType enum item values or ESqlText8.
@param aLength Field length in bytes
*/
inline void TSqlBufWIterator::SetAsNotPresent(TInt aType, TInt aLength)
	{
	__ASSERT_DEBUG(::IsSequenceSqlType(aType), __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aLength >= 0, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG((TUint)iIndex < iBuf->Count(), __SQLPANIC(ESqlPanicInternalError));
	iBuf->SetField(iIndex, aType, NULL, aType == ESqlText ? aLength * sizeof(TUint16) : aLength);
	}
	
/**
Initializes current flat buffer field with an integer value.

@param aValue An integer value to be set as a field content

@return KErrNone, The operation has completed successfully;
		KErrNoMemory, Out of memory condition has occured.
*/
inline TInt TSqlBufWIterator::SetInt(TInt aValue)
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG((TUint)iIndex < iBuf->Count(), __SQLPANIC(ESqlPanicInternalError));
	return iBuf->SetField(iIndex, ESqlInt, &aValue, sizeof(TInt));
	}
	
/**
Initializes current flat buffer field with an 64 bit integer value.

@param aValue A 64 bit integer value to be set as a field content

@return KErrNone, The operation has completed successfully;
		KErrNoMemory, Out of memory condition has occured.
*/
inline TInt TSqlBufWIterator::SetInt64(TInt64 aValue)
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG((TUint)iIndex < iBuf->Count(), __SQLPANIC(ESqlPanicInternalError));
	return iBuf->SetField(iIndex, ESqlInt64, &aValue, sizeof(TInt64));
	}
	
/**
Initializes current flat buffer field with a real value.

@param aValue A real value to be set as a field content

@return KErrNone, The operation has completed successfully;
		KErrNoMemory, Out of memory condition has occured.
*/
inline TInt TSqlBufWIterator::SetReal(TReal aValue)
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG((TUint)iIndex < iBuf->Count(), __SQLPANIC(ESqlPanicInternalError));
	return iBuf->SetField(iIndex, ESqlReal, &aValue, sizeof(TReal));
	}
	
/**
Initializes current flat buffer field with a block of binary data.

@param aValue An 8 bit descriptor pointing to the block of data to be set as a field content.

@return KErrNone, The operation has completed successfully;
		KErrNoMemory, Out of memory condition has occured.
*/
inline TInt TSqlBufWIterator::SetBinary(const TDesC8& aValue)
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG((TUint)iIndex < iBuf->Count(), __SQLPANIC(ESqlPanicInternalError));
	return iBuf->SetField(iIndex, ESqlBinary, aValue.Ptr(), aValue.Length());
	}
	
/**
Initializes current flat buffer field with a block of 16 bit text.

@param aValue A 16 bit descriptor pointing to the block of text to be set as a field content.

@return KErrNone, The operation has completed successfully;
		KErrNoMemory, Out of memory condition has occured.
*/
inline TInt TSqlBufWIterator::SetText(const TDesC16& aValue)
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG((TUint)iIndex < iBuf->Count(), __SQLPANIC(ESqlPanicInternalError));
	return iBuf->SetField(iIndex, ESqlText, aValue.Ptr(), aValue.Length() * sizeof(TUint16));
	}

	
/**
Initializes current flat buffer field with a zeroblob of the specified size.

@param aSize The size, in bytes, of the zeroblob to be set as the field content.

@return KErrNone, The operation has completed successfully;
		KErrNoMemory, Out of memory condition has occured.
*/
inline TInt TSqlBufWIterator::SetZeroBlob(TInt aSize)
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG((TUint)iIndex < iBuf->Count(), __SQLPANIC(ESqlPanicInternalError));
	return iBuf->SetField(iIndex, ESqlZeroBlob, &aSize, sizeof(TInt));
	}
