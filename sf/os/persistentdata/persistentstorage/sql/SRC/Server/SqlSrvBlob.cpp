// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sqlite3.h"
#include "SqlAssert.h"
#include "SqlSrvBlob.h"
#include "SqliteSymbian.h"
#include "SqlSrvUtil.h"
#include "SqlSrvStrings.h"
#include "SqlDb.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlSrvBlobTraces.h"
#endif
#include "SqlTraceDef.h"

#define UNUSED_VAR(var) var = var

/**
Creates a new HBlobBuf instance.

@param aDb 			 The database handle
@param aDbName 		 The database name, zero-terminated. If the blob belongs to one of the attached databases, 
					 then the attached database name should be used as the value of aDbName, otherwise the
					 value should be "main"
@param aTableName    The name of the table to which the blob belongs, zero-terminated.
@param aColumnName   The name of the blob column, zero-terminated.
@param aRowId        The ROWID of the row to which the blob belongs
@param aMode		 Specifies the blob access mode, either HBlobBuf::EReadOnly or HBlobBuf::EReadWrite

@leave KErrNoMemory, An out of memory condition has occurred;
                     Note that the function may also leave with some other system wide errors or 
                     database specific errors categorised as ESqlDbError.

@return A pointer to the created HBlobBuf instance
                     
@panic SqlDb 4 In _DEBUG mode. NULL aDb parameter.
@panic SqlDb 4 In _DEBUG mode. Negative or zero aRowId.
@panic SqlDb 4 In _DEBUG mode. aMode parameter is not HBlobBuf::EReadOnly or HBlobBuf::EReadWrite.
*/
HBlobBuf* HBlobBuf::NewL(sqlite3* aDb, const TDesC8& aDbName, const TDesC8& aTableName, const TDesC8& aColumnName, TInt64 aRowId, TMode aMode)
	{
	__SQLTRACE_BLOBVAR(TBuf<100> des16prnbuf);
	SQL_TRACE_BLOB(OstTraceExt4(TRACE_INTERNALS, HBLOBBUF_NEWL_ENTRY1, "Entry;0;HBlobBuf::NewL;sqlite3*=0x%X;aDbName=%s;aRowId=%lld;aMode=%d", (TUint)aDb, __SQLPRNSTR8(aDbName, des16prnbuf), aRowId, (TInt)aMode));
	SQL_TRACE_BLOB(OstTraceExt1(TRACE_INTERNALS, HBLOBBUF_NEWL_ENTRY2, "Entry;0;HBlobBuf::NewL;aTableName=%s", __SQLPRNSTR8(aTableName, des16prnbuf)));
	SQL_TRACE_BLOB(OstTraceExt1(TRACE_INTERNALS, HBLOBBUF_NEWL_ENTRY3, "Entry;0;HBlobBuf::NewL;aColumnName=%s", __SQLPRNSTR8(aColumnName, des16prnbuf)));
	__ASSERT_DEBUG(aDb != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aRowId > 0, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aMode == HBlobBuf::EReadOnly || aMode == HBlobBuf::EReadWrite, __SQLPANIC2(ESqlPanicBadArgument));
	
	HBlobBuf* self = new (ELeave) HBlobBuf;
	CleanupStack::PushL(self);
	self->ConstructL(aDb, aDbName, aTableName, aColumnName, aRowId, aMode);
	CleanupStack::Pop(self);
	SQL_TRACE_BLOB(OstTraceExt2(TRACE_INTERNALS, HBLOBBUF_NEWL_EXIT, "Exit;0x%X;HBlobBuf::NewL;sqlite3_blob*=0x%X", (TUint)self, (TUint)self->iBlobHandle));
	return self;
	}

/**
Initializes HBlobBuf data members with their default values.
*/
HBlobBuf::HBlobBuf() :
	iBlobHandle(NULL),
	iWrPos(KMinTInt),
	iRdPos(KMinTInt)
	{
	}

/**
Initializes a new HBlobBuf instance.

@param aDb 			 The database handle
@param aDbName 		 The database name, zero-terminated. If the blob belongs to one of the attached databases, 
					 then the attached database name should be used as a value of aDbName, otherwise the
					 value should be "main"
@param aTableName    The name of the table to which the blob belongs, zero-terminated.
@param aColumnName   The name of the blob column, zero-terminated.
@param aRowId        The ROWID of the row to which the blob belongs
@param aMode		 Specifies the blob access mode, either HBlobBuf::EReadOnly or HBlobBuf::EReadWrite

@leave KErrNoMemory, 	An out of memory condition has occurred;
                     	Note that the function may also leave with some other system wide errors or 
                    	database specific errors categorised as ESqlDbError.
                     
@panic SqlDb 4 In _DEBUG mode. NULL aDb parameter.
@panic SqlDb 4 In _DEBUG mode. Negative or zero aRowId.
@panic SqlDb 4 In _DEBUG mode. aMode parameter is not HBlobBuf::EReadOnly or HBlobBuf::EReadWrite.
@panic SqlDb 7 In _DEBUG mode. NULL blob handle.
*/
void HBlobBuf::ConstructL(sqlite3* aDb, const TDesC8& aDbName, const TDesC8& aTableName, const TDesC8& aColumnName, TInt64 aRowId, TMode aMode)
	{
	__ASSERT_DEBUG(aDb != NULL, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aRowId > 0, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aMode == HBlobBuf::EReadOnly || aMode == HBlobBuf::EReadWrite, __SQLPANIC(ESqlPanicBadArgument));
	
	(void)sqlite3SymbianLastOsError();//clear last OS error
			
	TInt err = sqlite3_blob_open(aDb, (const char*)aDbName.Ptr(), (const char*)aTableName.Ptr(), (const char*)aColumnName.Ptr(), 
								 aRowId, aMode & HBlobBuf::EReadWrite, &iBlobHandle);
	__SQLLEAVE_IF_ERROR(::Sql2OsErrCode(err, sqlite3SymbianLastOsError()));
	__ASSERT_DEBUG(iBlobHandle != NULL, __SQLPANIC(ESqlPanicInternalError));
	iBlobSize = sqlite3_blob_bytes(iBlobHandle);
	iWrPos = iRdPos = 0;
	}

/**
Closes the blob handle as part of the HBlobBuf object being destroyed.
*/
void HBlobBuf::DoRelease()
	{
	SQL_TRACE_BLOB(OstTraceExt2(TRACE_INTERNALS, HBLOBBUF_DORELEASE_ENTRY, "Entry;0x%X;HBlobBuf::DoRelease;sqlite3_blob*=0x%X", (TUint)this, (TUint)iBlobHandle));
	TRAPD(err, DoSynchL());
	SQL_TRACE_BLOB(OstTraceExt2(TRACE_INTERNALS, HBLOBBUF_DORELEASE_EXIT, "Exit;0x%X;HBlobBuf::DoRelease;err=%d", (TUint)this, err));
    UNUSED_VAR(err);
    }
	
/**
Closes the blob handle.
Any buffered data is delivered to the stream.
*/	
void HBlobBuf::DoSynchL()
	{
	if(iBlobHandle)
		{
		TInt err = sqlite3_blob_close(iBlobHandle);
		SQL_TRACE_BLOB(OstTraceExt2(TRACE_INTERNALS, HBLOBBUF_DOSYNCHL, "0x%X;HBlobBuf::DoSynchL;err=%d", (TUint)this, err));
		iBlobHandle = NULL; // the close is unconditional, even if an error occurs
		__SQLLEAVE_IF_ERROR(::Sql2OsErrCode(err, sqlite3SymbianLastOsError()));
		}	
	}

/**
Reads a data block from the blob with the specified length.

@param aPtr Pointer to the location where the blob data will be copied
@param aMaxLength The length of the data block to be read

@leave KErrNoMemory,  An out of memory condition has occurred;
       KErrEof, 	  An attempt has been made to read beyond the end of the blob;
       KErrBadHandle, NULL blob handle;
                      Note that the function may also leave with some other system 
                      wide errors or database specific errors categorised as ESqlDbError.

@return The number of bytes read.

@panic SqlDb 4 In _DEBUG mode. NULL aPtr parameter.
@panic SqlDb 4 In _DEBUG mode. Negative aMaxLength parameter.
@panic SqlDb 2 In _DEBUG mode. NULL iBlobHandle.
*/
TInt HBlobBuf::DoReadL(TAny* aPtr, TInt aMaxLength)
	{
	__ASSERT_DEBUG(aPtr != NULL, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aMaxLength >= 0, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(iBlobHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	
	SQL_TRACE_BLOB(OstTraceExt5(TRACE_INTERNALS, HBLOBBUF_DOREADL, "0x%X;HBlobBuf::DoReadL;aMaxLength=%d;iBlobSize=%d;iWrPos=%d;iRdPos=%d", (TUint)this, aMaxLength, iBlobSize, iWrPos, iRdPos));
	
	if(aMaxLength <= 0)
		{
		return 0;	
		}
	
	(void)sqlite3SymbianLastOsError();//clear last OS error
	
	if(iRdPos >= iBlobSize)
		{
		__SQLLEAVE(KErrEof);	
		}
	if((aMaxLength + iRdPos) > iBlobSize)
		{
		aMaxLength = iBlobSize - iRdPos;
		}
	TInt err = sqlite3_blob_read(BlobHandleL(), aPtr, aMaxLength, iRdPos);
	__SQLLEAVE_IF_ERROR(::Sql2OsErrCode(err, sqlite3SymbianLastOsError()));
	iRdPos += aMaxLength;
	return aMaxLength;
	}

/**
Writes a data block with the specified length to the blob.

@param aPtr Pointer to the location with the blob data to be written
@param aLength The length of the data block to be written

@leave KErrNoMemory,  An out of memory condition has occurred;
       KErrEof,       An attempt has been made to write beyond the end of the blob;
       KErrBadHandle, NULL blob handle;
                      Note that the function may also leave with some other system 
                      wide errors or database specific errors categorised as ESqlDbError.

@panic SqlDb 4 In _DEBUG mode. NULL aPtr parameter.
@panic SqlDb 4 In _DEBUG mode. Negative aLength parameter.
@panic SqlDb 2 In _DEBUG mode. NULL iBlobHandle.
*/
void HBlobBuf::DoWriteL(const TAny* aPtr, TInt aLength)
	{
	__ASSERT_DEBUG(aPtr != NULL, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aLength >= 0, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(iBlobHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	
	SQL_TRACE_BLOB(OstTraceExt5(TRACE_INTERNALS, HBLOBBUF_DOWRITEL, "0x%X;HBlobBuf::DoWriteL;aLength=%d;iBlobSize=%d;iWrPos=%d;iRdPos=%d", (TUint)this, aLength, iBlobSize, iWrPos, iRdPos));
	
	if(aLength <= 0)
		{
		return;
		}
		
	if((iWrPos + aLength) > iBlobSize)
		{
		__SQLLEAVE(KErrEof);
		}

	(void)sqlite3SymbianLastOsError();//clear last OS error
	TInt err = sqlite3_blob_write(BlobHandleL(), aPtr, aLength, iWrPos);
	err = ::Sql2OsErrCode(err, sqlite3SymbianLastOsError());
	
	__SQLLEAVE_IF_ERROR(err);
	iWrPos += aLength;
	}

/**
Positions the mark(s) indicated by aMark at aOffset from aLocation.

@leave KErrEof,       An attempt has been made to seek beyond the end of the blob;
	   KErrBadHandle, NULL blob handle;
                      Note that the function may also leave with some other system wide errors or 
                      database specific errors categorised as ESqlDbError.

@return The new stream position (read or write)

@panic SqlDb 2 In _DEBUG mode. NULL iBlobHandle.
@panic SqlDb 4 In _DEBUG mode. Negative aOffset parameter.
@panic SqlDb 8 In _DEBUG mode. Invalid aMark parameter.
@panic SqlDb 9 In _DEBUG mode. Invalid aLocation parameter.
*/
TStreamPos HBlobBuf::DoSeekL(MStreamBuf::TMark aMark, TStreamLocation aLocation, TInt aOffset)
	{
	SQL_TRACE_BLOB(OstTraceExt5(TRACE_INTERNALS, HBLOBBUF_DOSEEKL, "0x%X;HBlobBuf::DoSeekL;aMark=%d;aLocation=%d;aOffset=%d;iBlobSize=%d", (TUint)this, (TInt)aMark, (TInt)aLocation, aOffset, iBlobSize));
	__ASSERT_ALWAYS(!(aMark & ~(ERead | EWrite)), __SQLPANIC(ESqlPanicStreamMarkInvalid));
	__ASSERT_DEBUG(aOffset >= 0, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(iBlobHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	
	TInt newPos = 0;
	switch(aLocation)
		{
	case EStreamBeginning:
		newPos = aOffset;
		break;
	case EStreamMark:
		newPos = (aMark & MStreamBuf::EWrite ? iWrPos : iRdPos) + aOffset;
		break;
	case EStreamEnd:
		newPos = iBlobSize + aOffset;
		break;
	default:
		__ASSERT_DEBUG(0, __SQLPANIC(ESqlPanicStreamLocationInvalid));
		newPos = -1;
		break;
		}
	if(newPos < 0 || newPos > iBlobSize)
		{
		__SQLLEAVE(KErrEof);
		}
	if(aMark & MStreamBuf::EWrite)
		{
		iWrPos = newPos;
		}
	else if(aMark & MStreamBuf::ERead)
		{
		iRdPos = newPos;	
		}
	return TStreamPos(newPos);
	}

/**
Returns the blob handle, if it is not NULL.

@leave KErrBadHandle, The blob handle is NULL.

@return The blob handle
*/
sqlite3_blob* HBlobBuf::BlobHandleL()
	{
	if(!iBlobHandle)
		{
		__SQLLEAVE(KErrBadHandle);
		}
	return iBlobHandle;
	}
