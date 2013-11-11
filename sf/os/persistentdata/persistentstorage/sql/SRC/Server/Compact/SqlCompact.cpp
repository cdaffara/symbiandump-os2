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

#include "SqlAssert.h"
#include "SqlCompact.h"
#include "SqlCompactEntry.h"
#include "SqlCompactTimer.h"
#include "SqlUtil.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlCompactTraces.h"
#endif
#include "SqlTraceDef.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Initializes the background compaction settings with their default values.
*/
TSqlCompactSettings::TSqlCompactSettings() :
	iStepLength(KSqlCompactStepLengthMs),
	iFreePageThresholdKb(KSqlCompactFreePageThresholdKb)
	{
	}

#ifdef _DEBUG
/**
CSqlCompactSettings invariant.
*/
void TSqlCompactSettings::Invariant() const
	{
	__ASSERT_DEBUG(iStepLength > 0, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(iFreePageThresholdKb >= 0, __SQLPANIC(ESqlPanicInternalError));
	}
#endif//_DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
The granularity of the container maintained by CSqlCompactor

@see CSqlCompactor
@internalComponent
*/
const TInt KEntriesGranularity = 16;

/**
Creates a new CSqlCompactor instance.

@param aConnFactoryL MSqlCompactConn factory function.
@param aCompactStepInterval The time interval (ms) between the background compaction steps.

@return A pointer to the created CSqlCompactor instance

@leave KErrNoMemory, an out of memory condition has occurred;
                     Note that the function may also leave with some other database specific 
                     errors categorised as ESqlDbError, and other system-wide error codes.

@panic SqlDb 4 In _DEBUG mode. NULL aConnFactoryL.
@panic SqlDb 4 In _DEBUG mode. Zero or negative aCompactStepInterval.
*/
CSqlCompactor* CSqlCompactor::NewL(TSqlCompactConnFactoryL aConnFactoryL, TInt aCompactStepInterval)
	{
	__ASSERT_DEBUG(aConnFactoryL != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aCompactStepInterval > 0, __SQLPANIC2(ESqlPanicBadArgument));
	CSqlCompactor* self = new (ELeave) CSqlCompactor(aConnFactoryL);
	CleanupStack::PushL(self);
	self->ConstructL(aCompactStepInterval);
	CleanupStack::Pop(self);
	return self;
	}

/**
Destroys the CSqlCompactor instance. 
Any entries left in the container will be destroyed.
*/
CSqlCompactor::~CSqlCompactor()
	{
	for(TInt idx=iEntries.Count()-1;idx>=0;--idx)
		{
		__ASSERT_DEBUG(iEntries[idx] != NULL, __SQLPANIC(ESqlPanicInternalError));
		while(iEntries[idx]->Release() != 0)
			{
			}
		}
	iEntries.Close();
	delete iTimer;
	}

/**
Restarts the background compaction timer. 
This function should be called from the server side session object's ServiceL(). 
If ServiceL() is being executed at the moment, it is very likely that the SQL client(s) will issue another
request few ms later. In order to not delay the processing of the client(s) requests, ServiceL() should call
at the end RestartTimer(). If there are database entries scheduled for a background compaction, the compaction
will be delayed by the time interval used by the CSqlCompactTimer object (default value - KSqlCompactStepInterval).

@see CSqlCompactTimer
@see KSqlCompactStepInterval
*/
void CSqlCompactor::RestartTimer()
	{
	SQLCOMPACTOR_INVARIANT();
	iTimer->Restart();
	SQLCOMPACTOR_INVARIANT();
	}

/**
If an entry referring to a database with name aFullName does not exist in the container, a new entry will be created,
a connection with the database established.
If an entry with the specified name already exists, no new entry wil be created, the reference counter of the existing one 
will be incremented.

@param aFullName The full database name, including the path.
@param aSettings Per-database background compaction settings

@leave KErrNoMemory, an out of memory condition has occurred;
                     Note that the function may also leave with some other database specific 
                     errors categorised as ESqlDbError, and other system-wide error codes.

@panic SqlDb 4 In _DEBUG mode. Too short or too long database name (aFullName parameter)
@panic SqlDb 7 In _DEBUG mode. An entry with the specified name has been found but the entry is NULL.
*/
void CSqlCompactor::AddEntryL(const TDesC& aFullName, const TSqlCompactSettings& aSettings)
	{
	__ASSERT_DEBUG(aFullName.Length() > 0 && aFullName.Length() <= KMaxFileName, __SQLPANIC(ESqlPanicBadArgument));
	SQLCOMPACTOR_INVARIANT();
	CSqlCompactEntry* entry = NULL;
	TInt idx = iEntries.FindInOrder(aFullName, &CSqlCompactor::Search);
	if(idx == KErrNotFound)
		{
		SQL_TRACE_COMPACT(OstTraceExt4(TRACE_INTERNALS, CSQLCOMPACTOR_ADDENTRYL1, "0x%X;CSqlCompactor::AddEntryL;New entry;aFullName=%S;iStepLength=%d;iFreePageThreashold=%d", (TUint)this, __SQLPRNSTR(aFullName), aSettings.iStepLength, aSettings.iFreePageThresholdKb));
		entry = CSqlCompactEntry::NewLC(aFullName, iConnFactoryL, aSettings, *iTimer);
		TLinearOrder<CSqlCompactEntry> order(&CSqlCompactor::Compare);
		__SQLLEAVE_IF_ERROR(iEntries.InsertInOrder(entry, order));
		CleanupStack::Pop(entry);
		}
	else
		{
		SQL_TRACE_COMPACT(OstTraceExt4(TRACE_INTERNALS, CSQLCOMPACTOR_ADDENTRYL2, "0x%X;CSqlCompactor::AddEntryL;Reuse entry;aFullName=%S;iStepLength=%d;iFreePageThreashold=%d", (TUint)this, __SQLPRNSTR(aFullName), aSettings.iStepLength, aSettings.iFreePageThresholdKb));
		entry = iEntries[idx];
		__ASSERT_DEBUG(entry != NULL, __SQLPANIC(ESqlPanicInternalError));
		(void)entry->AddRef();
		}
	SQLCOMPACTOR_INVARIANT();
	}

/**
Decrements the reference counter of the specified entry.
If the counter reaches zero, the entry will be destroyed and removed from the container, 
the database connection - closed.

@param aFullName The full database name, including the path.
*/
void CSqlCompactor::ReleaseEntry(const TDesC& aFullName)
	{
	SQL_TRACE_COMPACT(OstTraceExt2(TRACE_INTERNALS, CSQLCOMPACTOR_RELEASEENTRY1, "0x%X;CSqlCompactor::ReleaseEntry;aFullName=%S", (TUint)this, __SQLPRNSTR(aFullName)));
	SQLCOMPACTOR_INVARIANT();
	TInt idx = iEntries.FindInOrder(aFullName, &CSqlCompactor::Search);
	__ASSERT_DEBUG(idx >= 0, __SQLPANIC(ESqlPanicInternalError));
	if(idx >= 0)
		{
		CSqlCompactEntry* entry = iEntries[idx];
		__ASSERT_DEBUG(entry != NULL, __SQLPANIC(ESqlPanicInternalError));
		if(entry)
			{
			if(entry->Release() == 0)
				{
				iEntries.Remove(idx);			
#ifdef _DEBUG
//This is used prevent the failure of the resource allocation checking for debug mode. 
				if(iEntries.Count() == 0)
				    {
                    iEntries.Reset();
				    }
#endif  
				}
			}
		}
	SQLCOMPACTOR_INVARIANT();
	}

/**
Initializes the CSqlCompactor data members with their default values.

@param aConnFactoryL MSqlCompactConn factory function.

@panic SqlDb 4 In _DEBUG mode. NULL aConnFactoryL.
*/
CSqlCompactor::CSqlCompactor(TSqlCompactConnFactoryL aConnFactoryL) :
	iConnFactoryL(aConnFactoryL),
	iEntries(KEntriesGranularity)
	{
	__ASSERT_DEBUG(aConnFactoryL != NULL, __SQLPANIC(ESqlPanicBadArgument));
	}

/**
Initializes the created CSqlCompactor instance.

@param aCompactStepInterval The time interval between the background compaction steps.

@panic SqlDb 4 In _DEBUG mode. Zero or negative aCompactStepInterval.
*/
void CSqlCompactor::ConstructL(TInt aCompactStepInterval)
	{
	__ASSERT_DEBUG(aCompactStepInterval > 0, __SQLPANIC(ESqlPanicBadArgument));
	iTimer = CSqlCompactTimer::NewL(aCompactStepInterval);
	}

/**
Static method used internally for performing a search in the container using database name as a key.

@param aFullName The full database name, including the path.
@param aEntry CSqlCompactor reference.
*/
/* static */TInt CSqlCompactor::Search(const TDesC* aFullName, const CSqlCompactEntry& aEntry)
	{
	__ASSERT_DEBUG(&aEntry != NULL, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aFullName != NULL, __SQLPANIC2(ESqlPanicInternalError));
	const TDesC& fullName = *aFullName;
	__ASSERT_DEBUG(fullName.Length() > 0 && fullName.Length() <= KMaxFileName, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aEntry.FullName().Length() > 0 && aEntry.FullName().Length() <= KMaxFileName, __SQLPANIC2(ESqlPanicInternalError));
	return fullName.CompareF(aEntry.FullName());
	}

/**
Static method used internally for performing a search in the container using a CSqlCompactEntry reference as a key.
*/
/* static */TInt CSqlCompactor::Compare(const CSqlCompactEntry& aLeft, const CSqlCompactEntry& aRight)
	{
	__ASSERT_DEBUG(&aLeft != NULL, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(&aRight != NULL, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aLeft.FullName().Length() > 0 && aLeft.FullName().Length() <= KMaxFileName, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aRight.FullName().Length() > 0 && aRight.FullName().Length() <= KMaxFileName, __SQLPANIC2(ESqlPanicInternalError));
	return aLeft.FullName().CompareF(aRight.FullName());
	}

#ifdef _DEBUG
/**
CSqlCompactor invariant.
*/
void CSqlCompactor::Invariant() const
	{
	__ASSERT_DEBUG(iConnFactoryL != NULL, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(iTimer != NULL, __SQLPANIC(ESqlPanicInternalError));
	iTimer->Invariant();
	for(TInt idx=iEntries.Count()-1;idx>=0;--idx)
		{
		__ASSERT_DEBUG(iEntries[idx] != NULL, __SQLPANIC(ESqlPanicInternalError));
		iEntries[idx]->Invariant();
		}
	}
#endif//_DEBUG
