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

#include "SqlUtil.h"
#include "SqlBufFlat.h"
#include "OstTraceDefinitions.h"

/**
Sets the flat buffer pointer to NULL
*/
RSqlBufFlat::RSqlBufFlat() :
	iBuf(NULL),
	iMaxSize(0),
	iBufPtr(NULL, 0)
	{
	}

/**
"Resource acquisiton" method.
Sets the elements count of a new or already existing flat buffer.

The occupied memory won't be freed (in case of buffer resizing).
The buffer content is not preserved (in case of buffer resizing).

All elements set to have:
- invalid type;
- zero length;
- zero data position;

@param aCount Desired flat buffer elements count

@return KErrNone, The operation has completed successfully;
		KErrNoMemory, Out of memory condition has occured.
*/
TInt RSqlBufFlat::SetCount(TInt aCount)
	{
	__ASSERT_DEBUG(aCount >= 0, __SQLPANIC(ESqlPanicBadArgument));
	TInt headerSize = sizeof(RSqlBufFlat::TCell) * aCount;
	TInt newSize = headerSize + sizeof(RSqlBufFlat::TBufFlat);
	if(DoReAlloc(newSize) != KErrNone)
		{
		return KErrNoMemory;	
		}
	TBufFlat& buf = *iBuf;
	buf.iCount = aCount;
	buf.iHeaderSize = headerSize;
	buf.iSize = newSize;
	buf.iReserved = 0;
	DoInit();
	SQLFLATBUF_INVARIANT();
	return KErrNone;	
	}

/**
Reallocates the amount of the occupied by the flat buffer memory.
The operation preserves the content of the flat buffer.
Note: if the new size is less or equal to the max size of the buffer,
	  no memory will be allocated.

@param aSize Desired flat buffer size in bytes

@return KErrNone, The operation has completed successfully;
		KErrNoMemory, Out of memory condition has occured.
*/
TInt RSqlBufFlat::ReAlloc(TInt aSize)
	{
	SQLFLATBUF_INVARIANT();
	TInt err = DoReAlloc(aSize);
	SQLFLATBUF_INVARIANT();
	return err;
	}

/**
Cleans up the flat buffer content and 
frees the occupied memory if the memory is above KBufLimit.
The count of elements is preserved.

If the buffer size is bigger than KBufLimit,
the buffer will be reallocated and the buffer content - not preserved.

If the buffer size is less or equal to KBufLimit,
no memory will be reallocated and the buffer preserves its content.

It is guaranteed that the reallocated buffer will have the same address in the heap 
as the original one.
*/	
void RSqlBufFlat::ResetAndMinimize()
	{
	SQLFLATBUF_INVARIANT();
	Reset();
#ifdef _DEBUG	
	const TInt KBufLimit = Max((TInt)RSqlBufFlat::EExpandSize, SysDataSize());
	const TBufFlat* oldBuf = iBuf;
#else
	const TInt KBufLimit = Max((8 * 1024), SysDataSize());
#endif
	if(iMaxSize > KBufLimit)
		{
		iMaxSize = KBufLimit - 1;  //to force the reallocation
		(void)DoReAlloc(KBufLimit);//User::ReAlloc() does not fail if the new requested size is less than the current block size
		}
	__ASSERT_DEBUG(oldBuf == iBuf, __SQLPANIC(ESqlPanicInternalError));
	SQLFLATBUF_INVARIANT();
	}

/**
Cleans up the flat buffer content but does not free the occupied memory.
The count of elements is preserved.

All elements set to have:
- invalid type;
- zero length;
- zero data position;

The element count is preserved.
*/
void RSqlBufFlat::Reset()
	{
	SQLFLATBUF_INVARIANT();
	iBuf->iSize = SysDataSize();
	DoInit();
	SQLFLATBUF_INVARIANT();
	}

/**
Closes the flat bufer and frees the allocated memory.
*/
void RSqlBufFlat::Close()
	{
	User::Free(iBuf);
	iBuf = NULL;	
	}

/**
Sets the content of a field.

@param aIndex Field index
@param aType Field type
@param aData Field data, may be NULL
@param aDataLength Field data length, may be 0

@return KErrNone, The operation has completed successfully;
		KErrNoMemory, Out of memory condition has occured.
*/
TInt RSqlBufFlat::SetField(TInt aIndex, TInt aType, const void* aData, TInt aDataLength)
	{
	SQLFLATBUF_INVARIANT();
	__ASSERT_DEBUG((TUint)aIndex < iBuf->iCount, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG((TUint)aType < RSqlBufFlat::EMaxType, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG((TUint)aDataLength < RSqlBufFlat::EMaxLength, __SQLPANIC(ESqlPanicBadArgument));
	if(aData)						//field value "present"
		{
		__ASSERT_DEBUG(aDataLength >= 0, __SQLPANIC(ESqlPanicBadArgument));
		if(aDataLength > 0)
			{
			if(Reserve(aDataLength) != KErrNone)
				{
				return KErrNoMemory;	
				}
			}
		DoSet(aIndex, aType, aData, aDataLength);
		}
	else if(aDataLength == 0)		//data is NULL, length is 0 - "null" field
		{
		DoSet(aIndex, aType, NULL, 0);
		}
	else							//field value "not present"
		{
		RSqlBufFlat::TCell& cell = *(Header() + aIndex);
		cell.iBits = (TUint)(((TUint)aType << RSqlBufFlat::EWidthLen) | (TUint)aDataLength);
		cell.iPos = 0;
		}
	SQLFLATBUF_INVARIANT();
	return KErrNone;
	}
	
/**
Initialzies the flat buffer header.
All field set:
- invalid type;
- zero length;
- "Not present";
*/	
void RSqlBufFlat::DoInit()
	{
	TBufFlat& buf = *iBuf;
	__ASSERT_DEBUG(buf.iCount >= 0, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(buf.iSize <= iMaxSize, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(buf.iHeaderSize == sizeof(RSqlBufFlat::TCell) * buf.iCount, __SQLPANIC(ESqlPanicInternalError));
	if(buf.iHeaderSize > 0)
		{
		Mem::FillZ(Header(), buf.iHeaderSize);
		}
	}

/**
Reallocates the amount of the occupied by the flat buffer memory
(only in case the requested size is bigger than the buffer size or the buffer does not exist).
The operation preserves the content of the flat buffer.

@param aSize Desired flat buffer size in bytes.

@return KErrNone, The operation has completed successfully;
		KErrNoMemory, Out of memory condition has occured.
*/
TInt RSqlBufFlat::DoReAlloc(TInt aSize)
	{
	if(!iBuf || iMaxSize < aSize)
		{
		//Calculate buffer new size (sometimes allocates more, for example, if
		//aSize % RSqlBufFlat::EExpandSize == 0, then one more RSqlBufFlat::EExpandSize page is allocated).
		TInt newSize = (aSize / RSqlBufFlat::EExpandSize + 1) * RSqlBufFlat::EExpandSize;
		RSqlBufFlat::TBufFlat* newBuf = static_cast <RSqlBufFlat::TBufFlat*> (User::ReAlloc(iBuf, newSize));
		if(!newBuf)
			{
			return KErrNoMemory;	
			}
		iBuf = newBuf;
		iMaxSize = newSize;
		}
	return KErrNone;
	}

/**
Initialzes a flat buffer field.
A memory for the field data has to be allocated before the call.

@param aIndex Field index
@param aType Field type
@param aData Field data, may be NULL
@param aDataLength Field data length, may be 0
*/
void RSqlBufFlat::DoSet(TInt aIndex, TInt aType, const void* aData, TInt aDataLength)
	{
	TBufFlat& buf = *iBuf;
	__ASSERT_DEBUG((TUint)aDataLength < RSqlBufFlat::EMaxLength, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aDataLength > 0 ? aData != NULL : ETrue, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aDataLength <= (iMaxSize - buf.iSize), __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(::IsAligned8(buf.iSize), __SQLPANIC(ESqlPanicInternalError));
	RSqlBufFlat::TCell& cell = *(Header() + aIndex);
	cell.iBits = (TUint)(((TUint)aType << RSqlBufFlat::EWidthLen) | (TUint)aDataLength);
	cell.iPos = 1;	//not 0, because 0 means "not present"
	if(aDataLength > 0)	//for fields with length > 0 set the data and reinitalize cell.iPos
		{
#ifdef _DEBUG
		Mem::Copy(reinterpret_cast <TUint8*> (iBuf) + buf.iSize, &KSqlBufFlatMagicValue, sizeof(KSqlBufFlatMagicValue));
		buf.iSize += sizeof(KSqlBufFlatMagicValue);
#endif		
		cell.iPos = buf.iSize - sizeof(RSqlBufFlat::TBufFlat);
		Mem::Copy(reinterpret_cast <TUint8*> (iBuf) + buf.iSize, reinterpret_cast <const TUint8*> (aData), aDataLength);
		buf.iSize += ::AlignedLen8(aDataLength);	//align the next field start position
				                                  	//it is guaranteed that this "+" operation will not make iSize bigger than
				                                   	//iMaxSize, because the memory allocations are 8-byte aligned 
				                                   	//(even RSqlBufFlat::EExpandSize aligned)
		}
	}

/**
Makes sure that the flat buffer has enough free space for a block of data with "aLength" length.
The function may reallocated the buffer if there is not enough space.

@param aLength The requested free memory length.

@return KErrNone, The operation has completed successfully;
		KErrNoMemory, Out of memory condition has occured.
*/
TInt RSqlBufFlat::Reserve(TInt aLength)
	{
#ifdef _DEBUG
	TInt diff = aLength + sizeof(KSqlBufFlatMagicValue) - Available();
#else
	TInt diff = aLength - Available();
#endif		
	return diff > 0 ? DoReAlloc(iMaxSize + diff) : KErrNone;
	}

#ifdef _DEBUG
/**
Panics in _DEBUG mode if the flat buffer content is inconsistent.
*/
void RSqlBufFlat::Invariant() const
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInternalError));
	const TBufFlat& buf = *iBuf;
	__ASSERT_DEBUG(buf.iCount >= 0, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(buf.iHeaderSize == sizeof(RSqlBufFlat::TCell) * buf.iCount, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(::IsAligned8(buf.iSize), __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(buf.iSize >= buf.iHeaderSize + sizeof(RSqlBufFlat::TBufFlat), __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(buf.iSize <= iMaxSize, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(buf.iSize <= User::AllocLen(iBuf), __SQLPANIC(ESqlPanicInternalError));	
	for(TInt i=0;i<(TInt)buf.iCount;++i)
		{
		const RSqlBufFlat::TCell& cell = *((reinterpret_cast <const RSqlBufFlat::TCell*> (iBuf + 1)) + i);
		__ASSERT_DEBUG(cell.Type() < RSqlBufFlat::EMaxType, __SQLPANIC(ESqlPanicInternalError));
		if(cell.Size() > 0 && cell.iPos >= buf.iHeaderSize) //only for present fields with length > 0
			{
			__ASSERT_DEBUG((TUint)cell.Size() <= buf.iSize, __SQLPANIC(ESqlPanicInternalError));
			__ASSERT_DEBUG(cell.iPos < (buf.iSize - sizeof(RSqlBufFlat::TBufFlat)), __SQLPANIC(ESqlPanicInternalError));
			TUint64 val = *(TUint64*)(reinterpret_cast <TUint8*> (iBuf) + cell.iPos + sizeof(RSqlBufFlat::TBufFlat) - sizeof(KSqlBufFlatMagicValue));
			__ASSERT_DEBUG(val == KSqlBufFlatMagicValue, __SQLPANIC(ESqlPanicInternalError));
			}
		}
	}
#endif//_DEBUG
