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
//
 
#include "SCHTASK.H"
#include "taskfile.h"
#include "SCHEXEC.H"
#include <schinfointernal.h>

/**
Returns the index of the slot in the process environment data that contains the
shared file server session (RFs) handle passed from the Task Scheduler.

@return The index of the shared file server session handle
@see RFile::AdoptFromCreator
*/
EXPORT_C TInt TScheduledTaskFile::FsHandleIndex()
	{
	return KTaskFsHandleIndex;
	}

/**
Returns the index of the slot in the process environment data that contains the
shared file (RFile) handle passed from the Task Scheduler.

@return The index of the shared file handle
@see RFile::AdoptFromCreator
*/
EXPORT_C TInt TScheduledTaskFile::FileHandleIndex()
	{
	return KTaskFileHandleIndex;
	}

/** Creates the object from the specified stream.

@param aStream The stream containing the external representation of this object.
@return A new instance of a CScheduledTask object. */
EXPORT_C CScheduledTask* CScheduledTask::NewLC(RReadStream& aStream)
	{
	CScheduledTask* self = new(ELeave) CScheduledTask;
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	return self;
	}

CScheduledTask::CScheduledTask()
	{
	}
/**
@internalComponent
*/
CScheduledTask::CScheduledTask(TTaskInfo& aInfo, 
								HBufC* aData, 
								TScheduleType aScheduleType,
								const TSecurityInfo& aSecurityInfo)
:	iInfo(aInfo), 
	iData(aData),
	iScheduleType(aScheduleType),
	iSecurityInfo(aSecurityInfo)
	{
	iPLink.iPriority = aInfo.iPriority;
	}

CScheduledTask::~CScheduledTask()
	{
	delete iData;
	}
/**
Remove from queue
@internalComponent only used by server
*/
void CScheduledTask::Remove()
	{
	iPLink.Deque();
	}

/**
queue offset
@internalComponent only used by server
*/
TInt CScheduledTask::Offset()
	{
	return (_FOFF(CScheduledTask,iPLink));
	}


/** Gets the detailed information for the task.

@return Detailed information about a task. */
EXPORT_C const TTaskInfo& CScheduledTask::Info() const 
	{
	return iInfo;
	}
	
/** Gets a reference to the data to be passed to the program on execution.

@return A reference to the descriptor containing the data. */
EXPORT_C const HBufC& CScheduledTask::Data() const
	{
	return *iData;
	}

/** Gets the time when the task stops being valid.

If the executing program determines that this time is in the past, then it 
should not run the task.

@return The time when the task stops being valid */
EXPORT_C const TTsTime& CScheduledTask::ValidUntil() const
	{
	return iValidUntil;
	}

/**
Gets the schedules type.
*/	
EXPORT_C TScheduleType CScheduledTask::ScheduleType() const
	{
	return iScheduleType;
	}

/**
Gets the security information for this scheduled task.  This information
is the securityID, VenforID and capabilities of the client who created
the schedule responsible for invoking this task.
*/
EXPORT_C const TSecurityInfo& CScheduledTask::SecurityInfo() const
	{
	return iSecurityInfo;
	}
		
/**
Is this task due?
@internalComponent only used by server
*/
TBool CScheduledTask::Due() const
	{
	return iDue;
	}

/**
Decrement repeat counter
@internalComponent only used by server
*/
void CScheduledTask::DecRepeat()
	{
	iInfo.iRepeat--;
	}

/**
Set due flag.
@internalComponent only used by server
*/
void CScheduledTask::SetDue(TBool aDue)
	{
	iDue = aDue;
	}

/**
Mark task as being due.
@param aValidUntil time this task will be valid until. 
This information will be passed to the registered executable, 
so they may check whether the task should still be executed.
@internalComponent only used by server
*/
void CScheduledTask::OnDue(const TTsTime& aValidUntil)
	{
	iValidUntil = aValidUntil;
	iDue = ETrue;
	}

/**
Return flag that determines if task is transisent or persistent.
@internalComponent only used by server
*/	
TBool CScheduledTask::Persists() const
	{return iPersists;}

/**
Mark task as belonging to a persistent schedule.
@internalComponent only used by server
*/	
void CScheduledTask::SetPersists()
	{iPersists = ETrue;}
	
/**
externalize data to stream.
@internalComponent only used by server
*/
void CScheduledTask::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iData;
	aStream << iValidUntil; 
	aStream << iInfo;
	aStream.WriteInt32L(iScheduleId);
	aStream.WriteInt32L(iScheduleType);
	aStream << iSecurityInfo;
	aStream.WriteInt32L(iPersists);
	}

void CScheduledTask::InternalizeL(RReadStream& aStream)
	{
	iData = HBufC::NewL(aStream, KMaxTInt);
	aStream >> iValidUntil;
	aStream >> iInfo;
	iScheduleId = aStream.ReadInt32L();
	iScheduleType = static_cast<TScheduleType>(aStream.ReadInt32L());
	aStream >> iSecurityInfo;
	iPersists = aStream.ReadInt32L();;	
	}

/**
Gets the schedulesID
@internalComponent only used by server
*/
TInt CScheduledTask::ScheduleId() const
	{
	return iScheduleId;
	}

/**
Sets the schedules ID
@internalComponent only used by server
*/
void CScheduledTask::SetScheduleId(TInt aScheduleId)
	{
	iScheduleId = aScheduleId;
	}

	
// Streaming operators for TSecurityInfo	
RWriteStream& operator<<(RWriteStream& aWriteStream, 
						const TSecurityInfo& aSecurityInfo)
	{
	//write security policy
	aWriteStream.WriteUint32L(aSecurityInfo.iSecureId);
	aWriteStream.WriteUint32L(aSecurityInfo.iVendorId);
	// write capabilities by looping through all available
	for (TUint jj = 0; jj < ECapability_Limit; ++jj)
		if(aSecurityInfo.iCaps.HasCapability((TCapability)jj))
			aWriteStream.WriteUint8L(jj);
	aWriteStream.WriteUint8L(ECapability_HardLimit); // terminate capabilities with hard limit
	return aWriteStream;
	}

RReadStream& operator>>(RReadStream& aReadStream,
						TSecurityInfo& aSecurityInfo)
	{
	
	aSecurityInfo.iSecureId = aReadStream.ReadUint32L();
	aSecurityInfo.iVendorId = aReadStream.ReadUint32L();
	aSecurityInfo.iCaps.SetEmpty();
	TCapability cap = ECapability_None; 
	aSecurityInfo.iCaps.AddCapability(cap);		
	while (cap != ECapability_HardLimit )
		{
		aSecurityInfo.iCaps.AddCapability(cap);
		cap = (TCapability)aReadStream.ReadUint8L();
		}
	return aReadStream;		
	}	


	
