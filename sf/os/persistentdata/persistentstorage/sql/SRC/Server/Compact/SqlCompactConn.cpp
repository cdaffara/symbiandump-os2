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

#include <f32file.h>
#include <sqldb.h>
#include "SqlAssert.h"
#include "sqlite3.h"
#include "SqliteSymbian.h"		//TSqlFreePageCallback
#include "SqlSrvUtil.h"
#include "SqlSrvStatementUtil.h"
#include "SqlCompactConn.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlCompactConnTraces.h"
#endif
#include "SqlTraceDef.h"

/**
Creates a new CSqlCompactConn instance.

@param aFullName The full database name, including the path.
@param aFreePageCallback A reference to an object containing the free pages threshold and the callback
					     that needs to be called when the free page count reaches or is above the threshold.
						 aFreePageCallback.iThreshold must be set to be in Kb. 	
						 If the function call completes successfully and the free pages space is above the threshold,
						 the aFreePageCallback.iThreshold will be set to contain the free pages count.
						 Otherwise aFreePageCallback.iThreshold will be initialized with zero.

@return A pointer to the created CSqlCompactConn instance

@leave KErrNoMemory, an out of memory condition has occurred,
	   KErrArgument, invalid data in the aFreePageCallback object;
                     Note that the function may also leave with some other database specific 
                     errors categorised as ESqlDbError, and other system-wide error codes.

@panic SqlDb 4 In _DEBUG mode. Too short or too long database name (aFullName parameter)
@panic SqlDb 4 In _DEBUG mode. Invalid aFreePageCallback content.
*/
CSqlCompactConn* CSqlCompactConn::NewL(const TDesC& aFullName, TSqlFreePageCallback& aFreePageCallback)
	{
	SQL_TRACE_COMPACT(OstTraceExt1(TRACE_INTERNALS, CSQLCOMPACTCONN_NEWLC_ENTRY, "Entry;0;CSqlCompactConn::NewL;aFullName=%S", __SQLPRNSTR(aFullName)));
	__ASSERT_DEBUG(aFullName.Length() > 0 && aFullName.Length() <= KMaxFileName, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aFreePageCallback.IsValid(), __SQLPANIC2(ESqlPanicBadArgument));
	CSqlCompactConn* self = new (ELeave) CSqlCompactConn;
	CleanupStack::PushL(self);
	self->ConstructL(aFullName, aFreePageCallback);
	CleanupStack::Pop(self);
	SQL_TRACE_COMPACT(OstTraceExt2(TRACE_INTERNALS, CSQLCOMPACTCONN_NEWLC_EXIT, "Exit;0x%X;CSqlCompactConn::NewL;aFullName=%S", (TUint)self, __SQLPRNSTR(aFullName)));
	return self;
	}

/**
Destroys the CSqlCompactConn instance.
*/
CSqlCompactConn::~CSqlCompactConn()
	{
	SQL_TRACE_COMPACT(OstTrace1(TRACE_INTERNALS, CSQLCOMPACTCONN_CSQLCOMPACTCONN2, "0x%X;CSqlCompactConn::~CSqlCompactConn", (TUint)this));
	::CloseDbHandle(iHandle);
 	}

/**
Implements MSqlCompactConn::Release().
Destroys the CSqlCompactConn instance.
*/
void CSqlCompactConn::Release()
	{
	SQLCOMPACTCONN_INVARIANT();
	delete this;	
	}

/**
Implements MSqlCompactConn::Compact().

Compacts the database making an attempt to remove the specified amount of free pages.

@param aPageCount 			The amount of free pages to be removed.
@param aProcessedPageCount	Output parameter. The actual count of the removed free pages will be stored there.
@param aLength 				Desired length of the compaction in milliseconds.
@return KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also return some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.
                      
@panic SqlDb 4 In _DEBUG mode. Negative aPageCount value.
@panic SqlDb 4 In _DEBUG mode. Negative aLength value.
*/
TInt CSqlCompactConn::Compact(TInt aPageCount, TInt& aProcessedPageCount, TInt aLength)
	{
	__ASSERT_DEBUG(aPageCount >= 0, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aLength >= 0, __SQLPANIC(ESqlPanicBadArgument));
	SQLCOMPACTCONN_INVARIANT();
	TInt err = ::DbCompact(iHandle, KNullDesC, aPageCount, aProcessedPageCount, aLength);
	SQLCOMPACTCONN_INVARIANT();
	return err;
	}

/**
Initializes the CSqlCompactConn instance establishing a connection with the database to be compacted.
Registers the passed "free page" callback.

Note: The free page threshold data member of the callback is in Kb. 
	  The function implementation will convert that threshold to pages and pass it to the OS porting layer.

@param aFullName The full database name, including the path.
@param aFreePageCallback Input/Output parameter. A reference to an object containing the free pages threshold and 
						 the callback that needs to be called when the free page count reaches or is above the threshold.
						 aFreePageCallback.iThreshold must be set to be in Kb. 	
						 If the function call completes successfully and the free pages space is above the threshold,
						 the aFreePageCallback.iThreshold will be set to contain the free pages count.
						 Otherwise aFreePageCallback.iThreshold will be initialized with zero.

@leave KErrNoMemory, an out of memory condition has occurred,
	   KErrArgument, invalid data in the aFreePageCallback object;
                     Note that the function may also leave with some other database specific 
                     errors categorised as ESqlDbError, and other system-wide error codes.

@panic SqlDb 4 In _DEBUG mode. Too short or too long database name (aFullName parameter)
@panic SqlDb 4 In _DEBUG mode. Invalid aFreePageCallback content.
@panic SqlDb 7 In _DEBUG mode. The database connection has been already established (not null db handle).
*/
void CSqlCompactConn::ConstructL(const TDesC& aFullName, TSqlFreePageCallback& aFreePageCallback)
	{
	__ASSERT_DEBUG(aFullName.Length() > 0 && aFullName.Length() <= KMaxFileName, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aFreePageCallback.IsValid(), __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(!iHandle, __SQLPANIC(ESqlPanicInternalError));
	
	TBuf8<KMaxFileName + 1> fname;
	(void)::UTF16ToUTF8Z(aFullName, fname);//The file is first open by the main connection. 
										   //The conversion here should always succeed.
										   //Even in a case of a conversion failure, the next line will report a failure.
	__SQLLEAVE_IF_ERROR(::CreateDbHandle8(fname, iHandle));
	
	TInt pageSize = PageSizeL();
	TInt64 freePageThresholdKb = aFreePageCallback.iThreshold;//"TInt64" because the calculation of the pages may cause an overflow on the next line
	aFreePageCallback.iThreshold = (freePageThresholdKb * 1024) / pageSize;//the threshold can be 0
	
	TBuf8<sizeof(KMainDb8) + 1> dbName;
	dbName.Copy(KMainDb8);
	TInt err = sqlite3_file_control(iHandle, (const char *)dbName.PtrZ(), KSqlFcntlRegisterFreePageCallback, &aFreePageCallback);
	__SQLLEAVE_IF_ERROR(::Sql2OsErrCode(err, sqlite3SymbianLastOsError()));

	TInt64 freePageCount = FreePageCountL();//"TInt64" because the calculation of the free space may cause an overflow on the next line
	TInt freePageSpaceKb = (freePageCount * pageSize) / 1024;
	//Set iThreshold with the free pages count, if right now the database has free space above the threshold.
	aFreePageCallback.iThreshold = freePageSpaceKb >= freePageThresholdKb ? freePageCount : 0;
	}

/**
Retrieves the database free pages count.

@leave KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also return some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.
                      
@return Free pages count
*/
TInt CSqlCompactConn::FreePageCountL()
	{
	SQLCOMPACTCONN_INVARIANT();
	TInt freePageCount = 0;
	__SQLLEAVE_IF_ERROR(::DbFreePageCount(iHandle, KNullDesC, freePageCount));
	SQLCOMPACTCONN_INVARIANT();
	return freePageCount;
	}

/**
Retrieves the database page size in bytes.

@leave KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also return some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.
                      
@return Page size
*/
TInt CSqlCompactConn::PageSizeL()
	{
	SQLCOMPACTCONN_INVARIANT();
	TInt pageSize = 0;
	__SQLLEAVE_IF_ERROR(::DbPageSize(iHandle, KNullDesC, pageSize));
	SQLCOMPACTCONN_INVARIANT();
	return pageSize;
	}

#ifdef _DEBUG
/**
CSqlCompactConn invariant.
*/
void CSqlCompactConn::Invariant() const
	{
	__ASSERT_ALWAYS(iHandle != NULL, __SQLPANIC(ESqlPanicInternalError));
	}
#endif//_DEBUG

/**
A factory function for CSqlCompactConn.

@param aFullName The full name of the database to be compacted (including the path).
@param aFreePageCallback A reference to an object containing the free pages threshold and the callback
					     that needs to be called when the free page count reaches or is above the threshold.
						 aFreePageCallback.iThreshold must be set to be in Kb. 	
						 If the function call completes successfully and the free pages space is above the threshold,
						 the aFreePageCallback.iThreshold will be set to contain the free pages count.
						 Otherwise aFreePageCallback.iThreshold will be initialized with zero.
					  
@return A pointer to the created MSqlCompactConn interface.

@leave KErrNoMemory, an out of memory condition has occurred,
	   KErrArgument, invalid data in the aFreePageCallback object;
                     Note that the function may also leave with some other database specific 
                     errors categorised as ESqlDbError, and other system-wide error codes.

@see MSqlCompactConn
@see CSqlCompactConn

@internalComponent

@panic SqlDb 4 In _DEBUG mode. Too short or too long database name (aFullName parameter)
@panic SqlDb 4 In _DEBUG mode. Invalid aFreePageCallback content.
*/
MSqlCompactConn* SqlCreateCompactConnL(const TDesC& aFullName, TSqlFreePageCallback& aFreePageCallback)
	{
	__ASSERT_DEBUG(aFullName.Length() > 0 && aFullName.Length() <= KMaxFileName, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aFreePageCallback.IsValid(), __SQLPANIC2(ESqlPanicBadArgument));
	return CSqlCompactConn::NewL(aFullName, aFreePageCallback);
	}
