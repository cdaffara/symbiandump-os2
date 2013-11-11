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
//

#include <e32math.h>		//Math class
#include <s32mem.h>			//TMemBuf
#include "SqlBufIterator.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////          TSqlBufIterator class               /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@return Represents the content of the current flat buffer field as integer value.
*/
inline TInt TSqlBufRIterator::AsInt() const
	{
	__ASSERT_DEBUG(iCurrent >= iBegin && iCurrent < iEnd, __SQLPANIC(ESqlPanicInternalError));
	return *reinterpret_cast <const TInt32*> (reinterpret_cast <const TUint8*> (iBegin) + iCurrent->iPos);
	}

/**
@return Represents the content of the current flat buffer field as 64 bit integer value.
*/
inline TInt64 TSqlBufRIterator::AsInt64() const
	{
	__ASSERT_DEBUG(iCurrent >= iBegin && iCurrent < iEnd, __SQLPANIC(ESqlPanicInternalError));
	return *reinterpret_cast <const TInt64*> (reinterpret_cast <const TUint8*> (iBegin) + iCurrent->iPos);
	}

/**
@return Represents the content of the current flat buffer field as real value.
*/
inline TReal TSqlBufRIterator::AsReal() const
	{
	__ASSERT_DEBUG(iCurrent >= iBegin && iCurrent < iEnd, __SQLPANIC(ESqlPanicInternalError));
	return *reinterpret_cast <const TReal*> (reinterpret_cast <const TUint8*> (iBegin) + iCurrent->iPos);
	}

/**
@return Represents the content of the current flat buffer field as integer value.
		If the current flat buffer field type does not refer to an integer, then 
		the function will do a data conversion as described in the table which can be found
		in SqlBufIterator.h file.
*/
TInt TSqlBufRIterator::Int() const
	{
	__ASSERT_DEBUG(iCurrent >= iBegin && iCurrent < iEnd, __SQLPANIC(ESqlPanicInternalError));
	switch(Type())
		{
		case ESqlInt64:
			{
			TInt64 val = AsInt64();
			return val == (TInt)val ? (TInt)val : (val < KMinTInt ? KMinTInt : KMaxTInt);
			}
		case ESqlReal:
			{
			TReal roundVal;
			TInt err = Math::Round(roundVal, AsReal(), 0);
			if(err != KErrNone)
				{
				return KMinTInt;
				}
			TRealX val(roundVal);
			return static_cast <TInt> (val);
			}
		case ESqlNull:
		case ESqlText:
		case ESqlBinary:
			return 0;
		case ESqlZeroBlob:
			return AsInt();
		default:
			return AsInt();
		}
	}

/**
@return Represents the content of the current flat buffer field as 64 bit integer value.
		If the current flat buffer field type does not refer to a 64 bit integer, then 
		the function will do a data conversion as described in the table which can be found
		in SqlBufIterator.h file.
*/
TInt64 TSqlBufRIterator::Int64() const
	{
	__ASSERT_DEBUG(iCurrent >= iBegin && iCurrent < iEnd, __SQLPANIC(ESqlPanicInternalError));
	switch(Type())
		{
		case ESqlInt:
		case ESqlZeroBlob:
			return AsInt();
		case ESqlReal:
			{
			TReal roundVal;
			TInt err = Math::Round(roundVal, AsReal(), 0);
			if(err != KErrNone)
				{
				return KMinTInt64;
				}
			TRealX val(roundVal);
			return static_cast <TInt64> (val);
			}
		case ESqlNull:
		case ESqlText:
		case ESqlBinary:
			return 0;
		default:
			return AsInt64();
		}
	}

/**
@return Represents the content of the current flat buffer field as real value.
		If the current flat buffer field type does not refer to a real, then 
		the function will do a data conversion as described in the table which can be found
		in SqlBufIterator.h file.
*/
TReal TSqlBufRIterator::Real() const
	{
	__ASSERT_DEBUG(iCurrent >= iBegin && iCurrent < iEnd, __SQLPANIC(ESqlPanicInternalError));
	switch(Type())
		{
		case ESqlInt:
		case ESqlZeroBlob:
			{
			TRealX val(AsInt());	
			return static_cast <TReal> (val);
			}
		case ESqlInt64:
			{
			TRealX val(AsInt64());	
			return static_cast <TReal> (val);
			}
		case ESqlNull:
		case ESqlText:
		case ESqlBinary:
			return 0.0;
		default:
			return AsReal();
		}
	}

/**
@return Represents the content of the current flat buffer field as binary (8 bit) descriptor.
		If the current flat buffer field type does not refer to a binary block of data, then 
		the function will do a data conversion as described in the table which can be found
		in SqlBufIterator.h file.
*/
TPtrC8 TSqlBufRIterator::Binary() const
	{
	__ASSERT_DEBUG(iCurrent >= iBegin && iCurrent < iEnd, __SQLPANIC(ESqlPanicInternalError));
	TInt size = Size();
	if(Type() != ESqlBinary || size == 0)
		{
		return 	TPtrC8();
		}
	return TPtrC8(reinterpret_cast <const TUint8*> (iBegin) + iCurrent->iPos, size);
	}

/**
@return Represents the content of the current flat buffer field as text (16 bit) descriptor.
		If the current flat buffer field type does not refer to a text block of data, then 
		the function will do a data conversion as described in the table which can be found
		in SqlBufIterator.h file.
*/
TPtrC16 TSqlBufRIterator::Text() const
	{
	__ASSERT_DEBUG(iCurrent >= iBegin && iCurrent < iEnd, __SQLPANIC(ESqlPanicInternalError));
	TInt size = Size();
	if(Type() != ESqlText || size == 0)
		{
		return 	TPtrC16();
		}
	return TPtrC16(reinterpret_cast <const TUint16*> (reinterpret_cast <const TUint8*> (iBegin) + iCurrent->iPos), size);
	}

/**
An instance of the class is used to get a read-only access to the content of a text or binary column
via a stream object.

@internalComponent
*/
class HReadOnlyBuf : public TMemBuf
	{
public:
	static HReadOnlyBuf* NewL(const TUint8* aFrom, TInt aLen)
		{
		__ASSERT_DEBUG(aLen >= 0, __SQLPANIC2(ESqlPanicBadArgument));
   		HReadOnlyBuf* self = new (ELeave) HReadOnlyBuf;
		TUint8* p = const_cast <TUint8*> (aFrom);
		self->Set(p, p + aLen, MStreamBuf::ERead);
		return self;
		}
		
private:
	virtual void DoRelease()
		{
		delete this;	
		}
	
};

/**
@return Represents the content of a text or a binary field as a stream of bytes.

@leave KErrNoMemory, out of memory condition has occured,
	   KErrArgument, the column type is not text, blob or null;
*/	
MStreamBuf* TSqlBufRIterator::StreamL() const
	{
    __ASSERT_DEBUG(iCurrent >= iBegin && iCurrent < iEnd, __SQLPANIC(ESqlPanicInternalError));
	if(!::IsSequenceSqlType(Type()))
		{
		__SQLLEAVE(KErrArgument);
		}
	return HReadOnlyBuf::NewL(reinterpret_cast <const TUint8*> (iBegin) + iCurrent->iPos, iCurrent->Size());
	}
