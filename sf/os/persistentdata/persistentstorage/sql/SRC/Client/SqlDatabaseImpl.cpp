// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SqlSecurityImpl.h"	//CSqlSecurityPolicy
#include "SqlDatabaseImpl.h"	//CSqlDatabaseImpl
#include <e32debug.h>

/**
Manual compaction - max compacton step execution time in microseconds.

@internalComponent
*/
static const TInt KCompactMaxStepTimeUs = 100000;

/**
Minimal amount of free database space to be removed by the compaction steps.

@internalComponent
*/
static const TInt KCompactMinStepSize =  2 * 1024;

/**
The amount of free database space to be removed by the first compaction step.

@internalComponent
*/
static const TInt KCompactStartStepSize = 32 * 1024;

/**
The aim of the function is to determine the maximum size of space to be freed, which fits within the time constraint.
The decision is based on the time spent on the pervious compaction step.
If the time is bigger than KCompactMaxStepTimeUs then the space will be reduced by factor of 2 (slow media),
bet will never be less than KCompactMinStepSize.
If the time is less than the KCompactMaxStepTimeUs/2 then the space will be increased by factor of 2 (fast media).

@param aRemaining The remaining free database space.
@param aStep The size of the space removed by the previous compaction step.
@param aTime The execution time of the previous compaction step.

@return The size in bytes if the next comaction step - the amount of space to be removed.

@see KMaxStepTimeUs
@see KMinStepSize
@see KStartStepSize

@internalComponent
*/
static TInt CalcCompactionStep(TInt aRemaining, TInt aStep, TInt aTime)
	{
	if(aTime > KCompactMaxStepTimeUs)
		{
		aStep /= 2;
		if(aStep < KCompactMinStepSize)
			{
			aStep = KCompactMinStepSize; 
			}
		}
	else if(aTime <= (KCompactMaxStepTimeUs / 2))
		{
		aStep *= 2;
		}
	if(aRemaining < aStep)
		{//If, for example, aStep is 4000 bytes, aRemaining is 2000 bytes, then the step should be 2000,  
		 //because that is what is left in the database as a free space.
		aStep = aRemaining;
		}
	return aStep;
	}
 
/**
Creates a new CSqlDatabaseImpl instance.

CSqlDatabaseImpl implements RSqlDatabase, which means that CSqlDatabaseImpl instance will be created from 
RSqlDatabase functions doing RSqlDatabase instance initialization - Create() and Open().

@param aFunction It may have one of the following values: 																																																																																																																																																																																																																																																																																																																																																																																																							
 ESqlSrvDbCreate       - Create a shared non-secure or private secure database;
 ESqlSrvDbCreateSecure - Create a shared secure database;
 ESqlSrvDbOpen         - Open a shared non-secure, shared secure or private secure database;
	
@param aDbFileName The name of the file that is to host the database.
				   If it is a secure database, then the format of the name is 
				   \<drive\>:\<[SID]database file name excluding the path\>. "[SID]" refers to the application SID.
				   If it is a non-secure database then aDbFileName should contain the full path name of the file 
				   that is to host the database.
@param aSecurityPolicy The container for the security policies. 
				   aSecurityPolicy is NULL if aDbFileName refers to a non-secure database.
@param aConfig the configuration string "PARAM=VALUE;...."

@return A pointer to the created CSqlDatabaseImpl instance.

@leave KErrNoMemory, an out of memory condition has occurred;
       KErrBadName, the file name is invalid - it has either a zero length or it is the name of a directory;
       KErrArgument, system table name found in the security policies (aSecurityPolicy);
       KErrAlreadyExists, the file already exists;
       KErrNotReady, the drive does not exist or is not ready;
       KErrInUse, the file is already open;
       KErrNotFound, database file not found;
       KErrGeneral, missing or invalid security policies (if the database to be opened is a secure database);
       KErrNotSupported, incompatible SQL security version (if the database to be opened is a secure database).
       KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that the function may leave with database specific errors categorised as ESqlDbError and
                      other system-wide error codes.
                      
@see RSqlDatabase
@see RSqlDatabase::Create()
@see RSqlDatabase::Open()
@see TSqlSrvFunction
@see CSqlSecurityPolicy
*/
CSqlDatabaseImpl* CSqlDatabaseImpl::NewL(TSqlSrvFunction aFunction, const TDesC& aDbFileName, 
										 const CSqlSecurityPolicy* aSecurityPolicy,
										 const TDesC8* aConfig)
	{
	CSqlDatabaseImpl* self = new (ELeave) CSqlDatabaseImpl;
	CleanupStack::PushL(self);
	self->ConstructL(aFunction, aDbFileName, aSecurityPolicy, aConfig);
	CleanupStack::Pop(self);
	return self;
	}

/**
Initializes the created CSqlDatabaseImpl instance.

@param aFunction It may have one of the following values:
  ESqlSrvDbCreate       - Create a shared non-secure or private secure database;
  ESqlSrvDbCreateSecure - Create a shared secure database;
  ESqlSrvDbOpen         - Open a shared non-secure, shared secure or private secure database;

@param aDbFileName The name of the file that is to host the database.
				   If it is a secure database, then the format of the name is 
				   \<drive\>:\<[SID]database file name excluding the path\>. "[SID]" refers to the application SID.
				   If it is a non-secure database then aDbFileName should contain the full path name of the file 
				   that is to host the database.
@param aSecurityPolicy The container for the security policies. 
				   aSecurityPolicy is NULL if aDbFileName refers to a non-secure database.
@param aConfig the configuration string "PARAM=VALUE;...."

@leave KErrNoMemory, an out of memory condition has occurred;
       KErrBadName, the file name is invalid - it has either a zero length or it is the name of a directory;
       KErrArgument, system table name found in the security policies (aSecurityPolicy);
       KErrAlreadyExists, the file already exists;
       KErrNotReady, the drive does not exist or is not ready;
       KErrInUse, the file is already open;
       KErrNotFound, database file not found;
       KErrGeneral, missing or invalid security policies (if the database to be opened is a secure database);
       KErrNotSupported, incompatible SQL security version (if the database to be opened is a secure database).
       KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that the function may leave with database specific errors categorised as ESqlDbError and
                      other system-wide error codes.

@see CSqlDatabaseImpl::NewL()

@panic SqlDb 4 In _DEBUG mode. aSecurityPolicy is NULL, but the request is for opening/creating a secure database.
*/
void CSqlDatabaseImpl::ConstructL(TSqlSrvFunction aFunction, const TDesC& aDbFileName, 
								  const CSqlSecurityPolicy* aSecurityPolicy, const TDesC8* aConfig)
	{
	TPtrC8 securityPolicyData;
	if(aFunction == ESqlSrvDbCreateSecure)	
		{
		__ASSERT_DEBUG(aSecurityPolicy != NULL, __SQLPANIC(ESqlPanicBadArgument));
		const RSqlBufFlat& bufFlat = aSecurityPolicy->BufFlat();
		securityPolicyData.Set(bufFlat.BufDes());
		}
	__SQLLEAVE_IF_ERROR(Session().Connect(aFunction, aDbFileName, securityPolicyData, aConfig));
	}

/**
Frees the allocated by CSqlDatabaseImpl instance memory and other resources.
*/
CSqlDatabaseImpl::~CSqlDatabaseImpl()
	{
	Session().Close();
	}

/**
Creates and returns a copy of the database security policies object.
The caller is responsible for destroying the returned CSqlSecurityPolicy instance.

Implements RSqlDatabase::GetSecurityPolicyL().

@return A copy of the database security policies object. 
        The returned copy must be destroyed by the caller.
        
@leave KErrNotSupported, the current database is not a secure database;
       KErrNoMemory, an out of memory condition has occurred;

@see RSqlDatabase
@see RSqlDatabase::GetSecurityPolicyL()
*/
CSqlSecurityPolicy* CSqlDatabaseImpl::CloneSecurityPolicyL()
	{
	TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysFail);
	CSqlSecurityPolicy* dbPolicy = CSqlSecurityPolicy::NewLC(defaultPolicy);
	__SQLLEAVE_IF_ERROR(Session().GetSecurityPolicy(dbPolicy->BufFlat()));
	CleanupStack::Pop(dbPolicy);
	return dbPolicy;
	}

/**
Implements RSqlDatabase::Compact().

@param aSize Can be one of:
				 RSqlDatabase::EMaxCompaction - requests a full database compaction. All free pages
				  (if any exists) will be removed;
				 Positive integer value - the server will attempt to compact the database removing 
				  at most aSize bytes from the database file, rounded up to the nearest page count, 
				  e.g. request for removing 1 byte will remove one free page from the database;
@param aDbName The attached database name or KNullDesC for the main database

@return Zero or positive integer - the operation has completed succesfully, the return value is the
         						   size of the removed free space in bytes,
		KErrArgument, Invalid aSize value;
        KErrBadName, Invalid (too long) attached database name;
        KSqlErrReadOnly, Read-only database;
        KSqlErrGeneral, There is no an attached database with aDbName name;
                  Note that database specific errors categorised as ESqlDbError, and
                  other system-wide error codes may also be returned.

Usage of the IPC call arguments:
Arg 0: [out]	How much space in bytes should be compacted, all free pages should be removed if the
				parameter value is RSqlDatabase::EMaxCompaction.
Arg 1: [out]	The database name length in characters
Arg 2: [out]	The attached database name or KNullDesC for the main database
*/
TInt CSqlDatabaseImpl::Compact(TInt aSize, const TDesC& aDbName)
	{
	if(aSize < 0)
		{
		if(aSize != RSqlDatabase::EMaxCompaction)
			{
			return KErrArgument;
			}
		aSize = KMaxTInt;
		}
	TInt remaining = aSize;
	TInt compacted = 0;
	TInt step = KCompactStartStepSize;
	TInt rc = 0;
	TTimeIntervalMicroSeconds interval(0);
	while(remaining > 0)
		{
		step = ::CalcCompactionStep(remaining, step, interval.Int64());
		TTime start;
		start.HomeTime();
		rc = Session().SendReceive(ESqlSrvDbCompact, TIpcArgs(step, aDbName.Length(), &aDbName));
		if(rc <= 0)
			{
			break;	
			}
		TTime end;
		end.HomeTime();
		interval = end.MicroSecondsFrom(start);
		remaining -= rc;				
		compacted += rc;
		}
	return rc < 0 ? rc : compacted;
	}

/**
Usage of the IPC call arguments:
Arg 0: [out]	How much space in bytes should be compacted, all free pages should be removed if the
				parameter value is RSqlDatabase::EMaxCompaction.
Arg 1: [out]	The database name length in characters
Arg 2: [out]	The attached database name or KNullDesC for the main database
*/
void CSqlDatabaseImpl::Compact(TInt aSize, const TDesC& aDbName, TRequestStatus& aStatus)
	{
	if(aSize == 0)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, 0);
		return;
		}
	Session().SendReceive(ESqlSrvDbCompact, TIpcArgs(aSize, aDbName.Length(), &aDbName), aStatus);
	}
