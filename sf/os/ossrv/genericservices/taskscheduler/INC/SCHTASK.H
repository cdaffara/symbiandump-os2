// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Representation of a task: 
// Includes both user-defined data (iInfo+iData) and data set by system
// construction from stream & accessors are exported for task executors
// 
//

#if !defined(__SCHTASK_H__)
#define __SCHTASK_H__

#if !defined(__SCHINFO_H__)
#include <schinfo.h>
#endif 

#include <s32file.h>



/**

When SYMBIAN_SECURE_TASK_SCHEDULER is defined, platform security
support functionality is included in Task Scheduler.

This means that:

(i) Scheduled executables will be passed a shared file handle to
the scheduled task file by the Task Scheduler.

(ii) The CScheduledTask::SecurityInfo() API is availble to
retrieve security information about the schedule creator.
@publishedPartner
@released
*/
//Only to be used internally and by Phone Manufacturers
#ifndef SYMBIAN_SECURE_TASK_SCHEDULER
#define SYMBIAN_SECURE_TASK_SCHEDULER
#endif

/**
Class used by registered programs to access the scheduled task file store.

When tasks are due, the Task Scheduler encapsulates task information within 
CScheduledTask objects, and externalises them to a direct file store.

This file store is located in the task scheduler's private data cage and thus
cannot be accessed directly.  Instead the task scheduler passes a shared RFs and
RFile handle to the registered program.

The registered program can use the RFile::AdoptFromCreator API in conjunction
with the APIs provided by this class to access the scheduled task file store as
shown in the following example:

@code
	RFile file;
	TInt error = file.AdoptFromCreator(TScheduledTaskFile::FsHandleIndex(),
											TScheduledTaskFile::FileHandleIndex());
@endcode
@see CScheduledTask
@see RFile::AdoptFromCreator
@publishedAll
@released
*/
class TScheduledTaskFile
	{
public:
	IMPORT_C static TInt FsHandleIndex();
	IMPORT_C static TInt FileHandleIndex();
	};

/** 
The representation of a scheduled task that is passed to registered programs.

When tasks are due, the Task Scheduler encapsulates task information within 
CScheduledTask objects, and externalises them to a direct file store.

The root stream of the direct file store contains a 32 bit value, followed 
by the external representations of one or more CScheduledTask objects. The 
32 bit value is interpreted as a TInt32 and contains the number of CScheduledTask 
objects that follow in the stream.

The registered program can create successive CScheduledTask objects from this 
stream using the static NewLC() function.
 
@see TScheduledTaskFile
@see RStoreReadStream 
@publishedAll
@released
*/
class CScheduledTask : public CBase
	{
public:
	IMPORT_C static CScheduledTask* NewLC(RReadStream& aStream);
	~CScheduledTask();
	IMPORT_C const TTaskInfo& Info() const;
	IMPORT_C const HBufC& Data() const;
	IMPORT_C const TTsTime& ValidUntil() const;
	IMPORT_C TScheduleType ScheduleType() const;
	IMPORT_C const TSecurityInfo& SecurityInfo() const;
	
	// These methods only used by task scheduler server
	CScheduledTask(TTaskInfo& aInfo, 
					HBufC* aData, 
					TScheduleType aScheduleType,
					const TSecurityInfo& aSecurityInfo);

	TBool Due() const;
	void SetDue(TBool aDue);
	void OnDue(const TTsTime& aValidUntil);
	void DecRepeat();
	void Remove();
	void ExternalizeL(RWriteStream& aStream) const;
	//
	TInt ScheduleId() const;
	void SetScheduleId(TInt aScheduleId);	
	//
	TBool Persists() const;
	void SetPersists();
	//
	static TInt Offset();
	//
private:
	CScheduledTask();	
	void InternalizeL(RReadStream& aStream);
private:
	TPriQueLink iPLink;
	TTaskInfo iInfo;
	HBufC* iData;
	TTsTime iValidUntil;
	TBool iDue;
	TInt iScheduleId;
	TScheduleType iScheduleType;
	TSecurityInfo iSecurityInfo;
	TBool iPersists;
	};
	

#endif
