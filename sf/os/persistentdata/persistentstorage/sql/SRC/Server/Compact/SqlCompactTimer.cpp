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

#include "SqlCompactTimer.h"
#include "SqlAssert.h"
#include "SqlCompactEntry.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlCompactTimerTraces.h"
#endif
#include "SqlTraceDef.h"

/**
Creates new CSqlCompactTimer instance.

@param aIntervalMs The timer intrerval (ms).

@panic SqlDb 4 In _DEBUG mode. Zero or negative aIntervalMs.
*/
CSqlCompactTimer* CSqlCompactTimer::NewL(TInt aIntervalMs)
	{
	SQL_TRACE_COMPACT(OstTrace1(TRACE_INTERNALS, CSQLCOMPACTTIMER_NEWL_ENTRY, "Entry;0;CSqlCompactTimer::NewL;aIntervalMs=%d", aIntervalMs));
	__ASSERT_DEBUG(aIntervalMs > 0, __SQLPANIC2(ESqlPanicBadArgument));
	CSqlCompactTimer* self = new (ELeave) CSqlCompactTimer(aIntervalMs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	SQL_TRACE_COMPACT(OstTraceExt2(TRACE_INTERNALS, CSQLCOMPACTTIMER_NEWL_EXIT, "Exit;0x%X;CSqlCompactTimer::NewL;aIntervalMs=%d", (TUint)self, aIntervalMs));
	return self;
	}

/**
Destroys the object.
*/
CSqlCompactTimer::~CSqlCompactTimer()
	{
	SQL_TRACE_COMPACT(OstTrace1(TRACE_INTERNALS, CSQLCOMPACTTIMER_CSQLCOMPACTTIMER2, "0x%X;CSqlCompactTimer::~CSqlCompactTimer", (TUint)this));
	Cancel();
	}

/**
If the queue is not empty, the timer will be restarted.
*/
void CSqlCompactTimer::Restart()
	{
	SQLCOMPACTTIMER_INVARIANT();
	if(!iQueue.IsEmpty())
		{
		Cancel();
		After(iIntervalMicroSec);
		}
	SQLCOMPACTTIMER_INVARIANT();
	}

/**
Adds a database entry, that requires a background compaction, to the queue.

@param aEntry The database entry to be compacted.

@see CSqlCompactEntry
*/
void CSqlCompactTimer::Queue(CSqlCompactEntry& aEntry)
	{
	SQL_TRACE_COMPACT(OstTraceExt3(TRACE_INTERNALS, CSQLCOMPACTTIMER_QUEUE, "0x%X;CSqlCompactTimer::Queue;aEntry=0x%X;Name=%S", (TUint)this, (TUint)&aEntry,__SQLPRNSTR(aEntry.FullName())));
	SQLCOMPACTTIMER_INVARIANT();
	iQueue.AddFirst(aEntry);
	if(!IsActive())
		{
		After(iIntervalMicroSec);
		}
	SQLCOMPACTTIMER_INVARIANT();
	}

/**
Removes the specified database entry from the queue.

@param aEntry The database entry to be removed from the queue.

@see CSqlCompactEntry
*/
void CSqlCompactTimer::DeQueue(CSqlCompactEntry& aEntry)
	{
	SQL_TRACE_COMPACT(OstTraceExt2(TRACE_INTERNALS, CSQLCOMPACTTIMER_DEQUEUE, "0x%X;CSqlCompactTimer::DeQueue;aEntry=0x%X", (TUint)this, (TUint)&aEntry));
	SQLCOMPACTTIMER_INVARIANT();
	iQueue.Remove(aEntry);
	if(iQueue.IsEmpty())
		{
		Cancel();
		}
	SQLCOMPACTTIMER_INVARIANT();
	}

/**
Initializes the CSqlCompactTimer instance.

@param aIntervalMs The timer intrerval (ms).

@panic SqlDb 4 In _DEBUG mode. Zero or negative aIntervalMs.
*/
CSqlCompactTimer::CSqlCompactTimer(TInt aIntervalMs) :
	CTimer(CActive::EPriorityIdle),
	iIntervalMicroSec(aIntervalMs * 1000),
	iQueue(_FOFF(CSqlCompactEntry, iLink))
	{
	__ASSERT_DEBUG(aIntervalMs > 0, __SQLPANIC(ESqlPanicBadArgument));
	}

/**
Initializes the created CSqlCompactTimer instance.
*/
void CSqlCompactTimer::ConstructL()
	{
	CTimer::ConstructL();	
	CActiveScheduler::Add(this);
	SQLCOMPACTTIMER_INVARIANT();
	}

/**
CActive::RunL() implementation.
The RunL() implementation picks-up the last CSqlCompactEntry object from the queue and calls its Compact() method.
At the end of the call, if the queue is not empty (CSqlCompactEntry::Compact() may remove the object from the queue if
the compaction has been completed), the timer will be reactivated.

@panic SqlDb 7 The queue is empty.
@panic SqlDb 7 In _DEBUG mode. The last entry in the queue is NULL.
*/
void CSqlCompactTimer::RunL()
	{
	SQL_TRACE_COMPACT(OstTrace1(TRACE_INTERNALS, CSQLCOMPACTTIMER_RUNL, "0x%X;CSqlCompactTimer::RunL", (TUint)this));
	SQLCOMPACTTIMER_INVARIANT();
	__ASSERT_ALWAYS(!iQueue.IsEmpty(), __SQLPANIC(ESqlPanicInternalError));	
	CSqlCompactEntry* entry = iQueue.Last();
	SQL_TRACE_COMPACT(OstTraceExt4(TRACE_INTERNALS, CSQLCOMPACTTIMER_RUNL2, "0x%X;CSqlCompactTimer::RunL;Compact;entry=0x%X;Name=%S;iQueue.IsEmpty()=%d", (TUint)this, (TUint)entry,__SQLPRNSTR(entry->FullName()), (TInt)iQueue.IsEmpty()));
	__ASSERT_DEBUG(entry, __SQLPANIC(ESqlPanicInternalError));	
	(void)entry->Compact();
	if(!iQueue.IsEmpty())
		{
		After(iIntervalMicroSec);
		}
	SQLCOMPACTTIMER_INVARIANT();
	}

#ifdef _DEBUG
/**
CSqlCompactTimer invariant.
*/
void CSqlCompactTimer::Invariant() const
	{
	__ASSERT_DEBUG(iIntervalMicroSec > 0, __SQLPANIC(ESqlPanicInternalError));
	if(!iQueue.IsEmpty())
		{
		CSqlCompactEntry* entry = iQueue.Last();
		__ASSERT_DEBUG(entry != NULL, __SQLPANIC(ESqlPanicInternalError));
		}
	}
#endif//_DEBUG
