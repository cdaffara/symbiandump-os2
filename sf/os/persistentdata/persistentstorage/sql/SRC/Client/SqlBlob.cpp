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

#include <s32mem.h>
#include "SqlDb.h"
#include "SqlAssert.h"
#include "SqlDatabaseImpl.h"
#include "IPCBuf.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlBlobTraces.h"
#endif
#include "SqlTraceDef.h"

// The maximum time (100 milliseconds) that a block of data is allowed to take to be read/written by TSqlBlob.
// If the time taken is longer than this value then the next block size used will be less
const TInt KTimeThresholdInMicroSecs = 100000; 
// The largest block size used by TSqlBlob is 32Kb
const TInt KLargestBlockSize = 32 * 1024; 
// The client-side buffer will be used instead if the calculated block size is 2Kb or less 
const TInt KUseClientBufferThreshold = 2 * 1024; 

// Prepare an IPC buffer containing the parameter values
static HBufC8* PrepareIpcParamBufLC(const TDesC& aTableName, const TDesC& aColumnName, TInt64 aRowId, 
								  TBool aReadOnly, const TDesC& aDbName)
	{
	HBufC8* ipcPrmBuf = HBufC8::NewLC(aTableName.Size() + sizeof(TDesC) + 
									  aColumnName.Size() + sizeof(TDesC) +
									  sizeof(aRowId) + sizeof(aReadOnly) + 
									  aDbName.Size() + sizeof(TDesC) + sizeof(TInt32) * 3);
	TPtr8 ipcPrmDes = ipcPrmBuf->Des();
	RDesWriteStream strm(ipcPrmDes);
	
	strm << static_cast <TInt32> (aTableName.Length()) 
	     << aTableName 
	     << static_cast <TInt32> (aColumnName.Length()) 
	     << aColumnName 
	     << aRowId 
	     << static_cast<TInt32>(aReadOnly) 
	     << static_cast <TInt32> (aDbName.Length()) 
	     << aDbName;
	strm.CommitL();
	strm.Close();
	return ipcPrmBuf;
	}

// Create an IPC stream, sending the IPC buffer containing the parameter values
static MStreamBuf* CreateIpcStreamL(RSqlDbSession& aDbSession, const TDesC8& aIpcPrmBuf)
	{
	TIpcArgs ipcArgs;
	ipcArgs.Set(0, aIpcPrmBuf.Length());
	ipcArgs.Set(1, &aIpcPrmBuf);
	HIpcBuf* ipcBuf = HIpcBuf::NewL(aDbSession, ::MakeMsgCode(ESqlSrvDbBlobSource, ESqlSrvNoHandle, 0), ipcArgs);
	return ipcBuf;
	}	
	
// Used by TSqlBlob to calculate the appropriate block size to use for the next transfer of blob data
static TInt CalculateBlockSize(TInt aRemainingDataSize, TInt aPreviousBlockSize, TBool aTimeThresholdBreached)
	{
	TInt blockSize = aPreviousBlockSize;
	if(aTimeThresholdBreached)
		{
		// The time threshold was breached using the previous block size
		// so we will reduce the block size by a factor of 2
		blockSize /= 2;
		if(blockSize <= KUseClientBufferThreshold)
			{
			// Just use the client-side IPC buffer size if things get to this stage
			blockSize = KIpcBufSize; 
			}
		}	
	return aRemainingDataSize <= blockSize ? aRemainingDataSize : blockSize;
	}
		
// Retrieve the data in blocks
static void DoReadInBlocksL(RSqlBlobReadStream& aStrm, TDes8& aDestBuffer, TInt aBlobSize)
	{
	TInt remainingDataSize = aBlobSize;	
	TBool timeThresholdBreached = EFalse;
	TInt blockSize = KLargestBlockSize;
	TPtr8 ptr((TUint8*)aDestBuffer.Ptr(), aDestBuffer.MaxSize());
	
	while(remainingDataSize > 0)
		{	
		// Calculate the block size to use for this iteration, based on 
		// how much data is remaining and how fast the previous read was
		blockSize = CalculateBlockSize(remainingDataSize, blockSize, timeThresholdBreached);
		
		// Read the block of data, timing how long it takes
		TTime t1, t2;
		t1.HomeTime();
		aStrm.ReadL(ptr, blockSize);
		t2.HomeTime();
		TTimeIntervalMicroSeconds readTime = t2.MicroSecondsFrom(t1);
		timeThresholdBreached = (readTime.Int64() > KTimeThresholdInMicroSecs);
	
		// Update how much data is still to be read, and the buffer pointer
		remainingDataSize -= blockSize;
		__ASSERT_DEBUG(remainingDataSize >= 0, __SQLPANIC2(ESqlPanicInternalError));
		ptr.Set((TUint8*)(ptr.Ptr() + blockSize), 0, remainingDataSize);
		}	
	aDestBuffer.SetLength(aBlobSize);
	}
	
// Send the data in blocks
static void DoWriteInBlocksL(RSqlBlobWriteStream& aStrm, const TDesC8& aData, TInt aDataSize)
	{
	TInt remainingDataSize = aDataSize;	
	TBool timeThresholdBreached = EFalse;
	TInt blockSize = KLargestBlockSize;
	
	while(remainingDataSize > 0)
		{	
		// Calculate the block size to use for this iteration, based on 
		// how much data is remaining and how fast the previous write was
		blockSize = CalculateBlockSize(remainingDataSize, blockSize, timeThresholdBreached);

		// Write the block of data, timing how long it takes
		TTime t1, t2;
		t1.HomeTime();
		aStrm.WriteL(aData.Right(remainingDataSize), blockSize);
		t2.HomeTime();
		TTimeIntervalMicroSeconds writeTime = t2.MicroSecondsFrom(t1);
		timeThresholdBreached = (writeTime.Int64() > KTimeThresholdInMicroSecs);
	
		// Update how much data is still to be written
		remainingDataSize -= blockSize;
		__ASSERT_DEBUG(remainingDataSize >= 0, __SQLPANIC2(ESqlPanicInternalError));
		}	
	aStrm.CommitL();	
	}
	
// The data is returned in a UTF-8 buffer that is allocated on the heap
static HBufC8* ReadLC(RSqlDatabase& aDb, 	
				   	  const TDesC& aTableName, 
				   	  const TDesC& aColumnName, 
				   	  TInt64 aRowId,
				   	  const TDesC& aDbName)	
	{
	RSqlBlobReadStream strm;
	CleanupClosePushL(strm);
	strm.OpenL(aDb, aTableName, aColumnName, aRowId, aDbName);		
	TInt blobSize = strm.SizeL(); // size of the blob, in bytes
	HBufC8* buf = HBufC8::NewLC(blobSize);
	TPtr8 ptr = buf->Des();	
	DoReadInBlocksL(strm, ptr, blobSize);
	CleanupStack::Pop(); // buf
	CleanupStack::PopAndDestroy(); // strm
	CleanupStack::PushL(buf);
	return buf;	
	}

// The data is returned in the UTF-8 buffer passed in
static void ReadL(RSqlDatabase& aDb, 	
				  const TDesC& aTableName, 
				  const TDesC& aColumnName, 	
				  TDes8& aBuffer,
				  TInt64 aRowId,
				  const TDesC& aDbName)
	{	
	RSqlBlobReadStream strm;
	CleanupClosePushL(strm);
	strm.OpenL(aDb, aTableName, aColumnName, aRowId, aDbName);		
	TInt blobSize = strm.SizeL(); // size of the blob, in bytes
	if(blobSize > aBuffer.MaxSize())
		{
		__SQLLEAVE2(KErrOverflow);
		}	
	DoReadInBlocksL(strm, aBuffer, blobSize);
	CleanupStack::PopAndDestroy(); // strm
	}
	
// The data in the client-specified UTF-8 buffer is written
static void WriteL(RSqlDatabase& aDb, 	
				   const TDesC& aTableName, 
				   const TDesC& aColumnName,
				   const TDesC8& aData,	
				   TInt64 aRowId,
				   const TDesC& aDbName)
	{
	RSqlBlobWriteStream strm;
	CleanupClosePushL(strm);
	strm.OpenL(aDb, aTableName, aColumnName, aRowId, aDbName);
	TInt dataSize = aData.Size(); // size of the data, in bytes
	TInt blobSize = strm.SizeL(); // size of the blob, in bytes
	if(dataSize > blobSize)
		{
		__SQLLEAVE2(KErrEof);
		}	
	if(dataSize > 0)
		{
		DoWriteInBlocksL(strm, aData, dataSize);
		}	
	CleanupStack::PopAndDestroy(); // strm
	}

////////////////////////////////////////////////////////////////////////////////	
			
/**
Gives access to a blob as a read-only stream of bytes.

@param aDb        	  A connection to the database that contains the blob
@param aTableName     The name of the table that contains the blob
@param aColumnName    The name of the column that contains the blob
@param aRowId         The ROWID of the record that contains the blob,
					  or KSqlLastInsertedRowId if the last inserted ROWID 
					  of the specified database connection is to be used
@param aDbName  	  The name of the attached database if the blob is 
					  contained in an attached database

@leave KSqlErrGeneral, 		 Invalid database connection or table name or column name 
							 or column type or ROWID or database name;							 
	   KErrNoMemory, 		 An out of memory condition occurred;	   
	   KErrArgument, 		 The ROWID is zero or negative or a UTF-16 to UTF-8 string conversion failed;
  	   KErrBadName,  		 The table name, column name or database name has an invalid length;
	   KErrPermissionDenied, The client does not have the required security capabilites for this operation; 						 
       						 Note that the function may also leave with some other system wide errors or 
                     		 database specific errors categorised as ESqlDbError.
	     
@panic SqlDb 2 The database object is not yet created
@panic SqlDb 3 Server failed to create a handle to the blob
@panic SqlDb 4 In _DEBUG mode. Bad parameter value
@panic SqlDb 7 In _DEBUG mode. NULL blob handle

@capability None, if the aDb parameter represents a handle which operates on a non-secure database;
            RSqlSecurityPolicy::EReadPolicy or 
            RSqlSecurityPolicy::ESchemaPolicy database policy type, if the blob belongs to a secure database; 
*/		
EXPORT_C void RSqlBlobReadStream::OpenL(RSqlDatabase& aDb, const TDesC& aTableName, const TDesC& aColumnName, 
										TInt64 aRowId, const TDesC& aDbName)
	{
	SQL_TRACE_BORDER(OstTraceExt5(TRACE_BORDER, RSQLBLOBREADSTREAM_OPENL_ENTRY, "Entry;0;RSqlBlobReadStream::OpenL;aDb=0x%X;aTableName=%S;aColumnName=%S;aDbName=%S;aRowId=%lld", (TUint)&aDb, __SQLPRNSTR(aTableName), __SQLPRNSTR(aColumnName), __SQLPRNSTR(aDbName), aRowId));
	HBufC8* ipcPrmBuf = ::PrepareIpcParamBufLC(aTableName, aColumnName, aRowId, ETrue, aDbName);
	MStreamBuf* strm = ::CreateIpcStreamL(aDb.Impl().Session(), ipcPrmBuf->Des());
	Attach(strm);
	CleanupStack::PopAndDestroy(ipcPrmBuf);	
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, RSQLBLOBREADSTREAM_OPENL_EXIT, "Exit;0x%x;RSqlBlobReadStream::OpenL;strm=0x%X", (TUint)this, (TUint)strm));
	}
	
/**
Returns the size of the blob object, in bytes.

@return The size of the blob object, in bytes

@leave One of the system-wide error codes

@panic SqlDb 2 The stream buffer is NULL
	
@capability None
*/
EXPORT_C TInt RSqlBlobReadStream::SizeL()
	{
    SQL_TRACE_BORDER(OstTrace1(TRACE_BORDER, RSQLBLOBREADSTREAM_SIZEL_ENTRY, "Entry;0x%X;RSqlBlobReadStream::SizeL;", (TUint)this));
	MStreamBuf* src = Source();
	__ASSERT_ALWAYS(src != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	TInt size = src->SizeL();
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, RSQLBLOBREADSTREAM_SIZEL_EXIT, "Exit;0x%X;RSqlBlobReadStream::SizeL;size=%d", (TUint)this, size));
	return size;
	}

/**
Gives access to a blob as a writeable stream of bytes.

@param aDb        	  A connection to the database that contains the blob
@param aTableName     The name of the table that contains the blob
@param aColumnName    The name of the column that contains the blob
@param aRowId         The ROWID of the record that contains the blob,
					  or KSqlLastInsertedRowId if the last inserted ROWID 
					  of the specified database connection is to be used
@param aDbName  	  The name of the attached database if the blob is 
					  contained in an attached database

@leave KSqlErrGeneral, 		 Invalid database connection or table name or column name or column 
							 type or ROWID or database name, or the specified column is indexed;
	   KErrNoMemory, 		 An out of memory condition occurred;	   
	   KErrArgument, 		 The ROWID is zero or negative or a UTF-16 to UTF-8 string conversion failed;
  	   KErrBadName,  		 The table name, column name or database name has an invalid length;
	   KErrPermissionDenied, The client does not have the required security capabilites for this operation; 						 
       						 Note that the function may also leave with some other system wide errors or 
                     		 database specific errors categorised as ESqlDbError.
	     
@panic SqlDb 2 The database object is not yet created
@panic SqlDb 3 Server failed to create a handle to the blob
@panic SqlDb 4 In _DEBUG mode. Bad parameter value
@panic SqlDb 7 In _DEBUG mode. NULL blob handle

@capability None, if the aDb parameter represents a handle which operates on a non-secure database;
            RSqlSecurityPolicy::EWritePolicy or 
            RSqlSecurityPolicy::ESchemaPolicy database policy type, if the blob belongs to a secure database; 
*/							
EXPORT_C void RSqlBlobWriteStream::OpenL(RSqlDatabase& aDb, const TDesC& aTableName, const TDesC& aColumnName, 
										 TInt64 aRowId, const TDesC& aDbName)
	{
    SQL_TRACE_BORDER(OstTraceExt5(TRACE_BORDER, RSQLBLOBWRITESTREAM_OPENL_ENTRY, "Entry;0;RSqlBlobWriteStream::OpenL;aDb=0x%X;aTableName=%S;aColumnName=%S;aDbName=%S;aRowId=%lld", (TUint)&aDb, __SQLPRNSTR(aTableName), __SQLPRNSTR(aColumnName), __SQLPRNSTR(aDbName), aRowId));
	HBufC8* ipcPrmBuf = ::PrepareIpcParamBufLC(aTableName, aColumnName, aRowId, EFalse, aDbName);
	MStreamBuf* strm = ::CreateIpcStreamL(aDb.Impl().Session(), ipcPrmBuf->Des());
	Attach(strm);
	CleanupStack::PopAndDestroy(ipcPrmBuf);		
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, RSQLBLOBWRITESTREAM_OPENL_EXIT, "Exit;0x%x;RSqlBlobWriteStream::OpenL;strm=0x%X", (TUint)this, (TUint)strm));
	}

/**
Returns the size of the blob object, in bytes.

@return The size of the blob object, in bytes

@leave One of the system-wide error codes

@panic SqlDb 2 The stream buffer is NULL
	
@capability None
*/
EXPORT_C TInt RSqlBlobWriteStream::SizeL()
	{
    SQL_TRACE_BORDER(OstTrace1(TRACE_BORDER, RSQLBLOBWRITESTREAM_SIZEL_ENTRY, "Entry;0x%X;RSqlBlobWriteStream::SizeL;", (TUint)this));
	MStreamBuf* sink = Sink();
	__ASSERT_ALWAYS(sink != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	TInt size =  sink->SizeL();
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, RSQLBLOBWRITESTREAM_SIZEL_EXIT, "Exit;0x%X;RSqlBlobWriteStream::SizeL;size=%d", (TUint)this, size));
    return size;
	}

/**
Retrieves the entire content of a blob and returns it to the client 
in a heap allocated buffer which has been placed on the cleanup stack. 
It is the responsibility of the client to destroy the returned buffer.

@param aDb        	  A connection to the database that contains the blob
@param aTableName     The name of the table that contains the blob
@param aColumnName    The name of the column that contains the blob
@param aRowId         The ROWID of the record that contains the blob,
					  or KSqlLastInsertedRowId if the last inserted ROWID 
					  of the specified database connection is to be used
@param aDbName  	  The name of the attached database if the blob is 
					  contained in an attached database

@leave KSqlErrGeneral, 		 Invalid database connection or table name or column name 
							 or column type or ROWID or database name;							 
	   KErrNoMemory, 		 An out of memory condition occurred;	   
	   KErrArgument, 		 The ROWID is zero or negative or a UTF-16 to UTF-8 string conversion failed;
  	   KErrBadName,  		 The table name, column name or database name has an invalid length;
	   KErrPermissionDenied, The client does not have the required security capabilites for this operation; 						 
       						 Note that the function may also leave with some other system wide errors or 
                     		 database specific errors categorised as ESqlDbError.
	     
@panic SqlDb 2 The database object is not yet created
@panic SqlDb 3 Server failed to create a handle to the blob
@panic SqlDb 4 In _DEBUG mode. Bad parameter value
@panic SqlDb 7 In _DEBUG mode. NULL blob handle or miscalculation of remaining data to be read

@capability None, if the aDb parameter represents a handle which operates on a non-secure database;
            RSqlSecurityPolicy::EReadPolicy or 
            RSqlSecurityPolicy::ESchemaPolicy database policy type, if the blob belongs to a secure database; 
*/
EXPORT_C HBufC8* TSqlBlob::GetLC(RSqlDatabase& aDb, 	
					     	     const TDesC& aTableName, 
					     		 const TDesC& aColumnName, 	
					     		 TInt64 aRowId,
					     		 const TDesC& aDbName)
	{
    SQL_TRACE_BORDER(OstTraceExt5(TRACE_BORDER, TSQLBLOB_GETLC_ENTRY, "Entry;0;TSqlBlob::GetLC;aDb=0x%X;aTableName=%S;aColumnName=%S;aDbName=%S;aRowId=%lld", (TUint)&aDb, __SQLPRNSTR(aTableName), __SQLPRNSTR(aColumnName), __SQLPRNSTR(aDbName), aRowId));
	HBufC8* res = ReadLC(aDb, aTableName, aColumnName, aRowId, aDbName);
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLBLOB_GETLC_EXIT, "Exit;0;TSqlBlob::GetLC;res=0x%X;res->Size()=%d", (TUint)res, res->Des().Size()));
	return res;
	}
	
/**
Retrieves the entire content of a blob into a client specified buffer.

@param aDb        	  A connection to the database that contains the blob
@param aTableName     The name of the table that contains the blob
@param aColumnName    The name of the column that contains the blob
@param aRowId         The ROWID of the record that contains the blob,
					  or KSqlLastInsertedRowId if the last inserted ROWID 
					  of the specified database connection is to be used
@param aDbName  	  The name of the attached database if the blob is 
					  contained in an attached database

@return KSqlErrGeneral, 	  Invalid database connection or table name or column name 
							  or column type or ROWID or database name;							 
	    KErrNoMemory, 		  An out of memory condition occurred;	   
	    KErrArgument, 		  The ROWID is zero or negative or a UTF-16 to UTF-8 string conversion failed;
 	    KErrOverflow, 		  The specified buffer is not big enough to hold the entire blob;
  	    KErrBadName,  		  The table name, column name or database name has an invalid length;
	    KErrPermissionDenied, The client does not have the required security capabilites for this operation; 						 
       						  Note that the function may also leave with some other system wide errors or 
                     		  database specific errors categorised as ESqlDbError.                    		 
	     
@panic SqlDb 2 The database object is not yet created
@panic SqlDb 3 Server failed to create a handle to the blob
@panic SqlDb 4 In _DEBUG mode. Bad parameter value
@panic SqlDb 7 In _DEBUG mode. NULL blob handle or miscalculation of remaining data to be read

@capability None, if the aDb parameter represents a handle which operates on a non-secure database;
            RSqlSecurityPolicy::EReadPolicy or 
            RSqlSecurityPolicy::ESchemaPolicy database policy type, if the blob belongs to a secure database; 
*/
EXPORT_C TInt TSqlBlob::Get(RSqlDatabase& aDb, 	
					 		const TDesC& aTableName, 
					 	    const TDesC& aColumnName, 	
					 	    TDes8& aBuffer,
					 		TInt64 aRowId,
					 	    const TDesC& aDbName)
	{
    SQL_TRACE_BORDER(OstTraceExt5(TRACE_BORDER, TSQLBLOB_GET_ENTRY, "Entry;0;TSqlBlob::Get;aDb=0x%X;aTableName=%S;aColumnName=%S;aDbName=%S;aRowId=%lld", (TUint)&aDb, __SQLPRNSTR(aTableName), __SQLPRNSTR(aColumnName), __SQLPRNSTR(aDbName), aRowId));
	TRAPD(err, ReadL(aDb, aTableName, aColumnName, aBuffer, aRowId, aDbName));
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLBLOB_GET_EXIT, "Exit;0;TSqlBlob::Get;aBuffer.Ptr()=0x%X;aBuffer.Size()=%d", (TUint)aBuffer.Ptr(), aBuffer.Size()));
	return err;	
	}

/**
Writes the data in a client specified buffer to a blob.

@param aDb        	  A connection to the database that contains the blob
@param aTableName     The name of the table that contains the blob
@param aColumnName    The name of the column that contains the blob
@param aRowId         The ROWID of the record that contains the blob,
					  or KSqlLastInsertedRowId if the last inserted ROWID 
					  of the specified database connection is to be used
@param aDbName  	  The name of the attached database if the blob is 
					  contained in an attached database

@leave KSqlErrGeneral, 		 Invalid database connection or table name or column name or column 
							 type or ROWID or database name, or the specified column is indexed;
	   KErrNoMemory, 		 An out of memory condition occurred;	   
	   KErrArgument, 		 The ROWID is zero or negative or a UTF-16 to UTF-8 string conversion failed;
	   KErrEof,	     		 The data in the specified buffer is larger in size than the blob object;
  	   KErrBadName,  		 The table name, column name or database name has an invalid length;
	   KErrPermissionDenied, The client does not have the required security capabilites for this operation; 						 
       						 Note that the function may also leave with some other system wide errors or 
                     		 database specific errors categorised as ESqlDbError.
	     
@panic SqlDb 2 The database object is not yet created
@panic SqlDb 3 Server failed to create a handle to the blob
@panic SqlDb 4 In _DEBUG mode. Bad parameter value
@panic SqlDb 7 In _DEBUG mode. NULL blob handle or miscalculation of remaining data to be written

@capability None, if the aDb parameter represents a handle which operates on a non-secure database;
            RSqlSecurityPolicy::EWritePolicy or 
            RSqlSecurityPolicy::ESchemaPolicy database policy type, if the blob belongs to a secure database; 
*/
EXPORT_C void TSqlBlob::SetL(RSqlDatabase& aDb, 	
					  		 const TDesC& aTableName, 
					  		 const TDesC& aColumnName,
					  		 const TDesC8& aData,	
					  		 TInt64 aRowId,
					  		 const TDesC& aDbName)
	{
    SQL_TRACE_BORDER(OstTraceExt5(TRACE_BORDER, TSQLBLOB_SET_ENTRY, "Entry;0;TSqlBlob::Set;aDb=0x%X;aTableName=%S;aColumnName=%S;aDbName=%S;aRowId=%lld", (TUint)&aDb, __SQLPRNSTR(aTableName), __SQLPRNSTR(aColumnName), __SQLPRNSTR(aDbName), aRowId));
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLBLOB_SET_ENTRYEXT, "EntryExt;0;TSqlBlob::Set;aData.Ptr=0x%X;aData.Size()=%d", (TUint)aData.Ptr(), aData.Size()));
	WriteL(aDb, aTableName, aColumnName, aData,	aRowId, aDbName);
    SQL_TRACE_BORDER(OstTrace0(TRACE_BORDER, TSQLBLOB_SET_EXIT, "Exit;0;TSqlBlob::Set"));
	}

