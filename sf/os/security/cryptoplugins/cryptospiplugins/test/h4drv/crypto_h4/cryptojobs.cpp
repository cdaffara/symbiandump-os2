/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @internalComponent
 @released
*/
#include <kernel/kern_priv.h>
#include "cryptojobs.h"

EXPORT_C TraceFunction * &TraceFunction::HeadRef()
	{
	static TraceFunction *sHead = 0;
	return sHead;
	}



EXPORT_C DCryptoJobScheduler::DCryptoJobScheduler()
	{
	TRACE_FUNCTION("DCryptoJobScheduler");
	}

EXPORT_C DCryptoJobScheduler::~DCryptoJobScheduler()
	{
	TRACE_FUNCTION("~DCryptoJobScheduler");
	// At this point the LDD channels and PDD factory/chans should
	// have already been deleted...
	}


EXPORT_C void DCryptoJobScheduler::ScheduleJob(CryptoJob *aJob)
	{
	TRACE_FUNCTION("ScheduleJob");
	if(aJob->iInJobList)
		{
		// Already in a job list
		// If it is the current job and it is not running then try and schedule it
		// It was probably stalled waiting for the user to read and/or write data.
		if((aJob == iJobList) && (aJob->iState != CryptoJob::ERunning))
			{
			// Job is not running, and is first in list, so schedule
			// it without rescheduling the first job (which is us!)
			Schedule(EFalse);
			return;
			}
		// Attempt to reschedule current job, ie if it is not running
		// save its state and try running a different job.
		Schedule(ETrue);
		return;
		}

	// Not in list, so add it
	aJob->iJobScheduler = this;
	aJob->iNext = 0;
	if(aJob->iState == CryptoJob::ECreated)
		{
		aJob->iState = CryptoJob::EReadyForFirstRun;
		}
	if(iJobList == 0)
		{
		// Only job
		iJobList = aJob;
		aJob->iInJobList = ETrue;
		}
	else
		{
		// Insert as second on list.
		// 
		// This quick and easy to do because it does not require a
		// re-schedule or a full list walk. It is slightly unfair to
		// existing jobs, but usually there will only be the head job
		// anyway.
		CryptoJob *p = iJobList->iNext;
		iJobList->iNext = aJob;
		aJob->iNext = p;
		aJob->iInJobList = ETrue;
		}
	
	// Attempt re-schedule
	Schedule(ETrue);
	}

EXPORT_C void DCryptoJobScheduler::DeScheduleJob(CryptoJob *aJob)
	{
	TRACE_FUNCTION("DeScheduleJob");
	if((aJob->iState == CryptoJob::EReady) || (aJob->iState == CryptoJob::ERunning))
		{
		aJob->DoReleaseHw();
		}
	
	aJob->iState = CryptoJob::ECreated;
	
	// Hunt for and remove job from queue.
	// This is a linear search, BUT the list length is probably one...
	// Try and set pp to point to the pointer to the job we are removing
	CryptoJob **pp = &iJobList; 
	while(*pp != aJob && *pp)
		{
		pp = &((*pp)->iNext);
		}
	if(*pp == aJob)
		{
		*pp = (*pp)->iNext;
		aJob->iNext = 0;
		aJob->iJobScheduler = 0;
		aJob->iInJobList = EFalse;
		}
	}



EXPORT_C void DCryptoJobScheduler::SliceComplete(CryptoJob *aJob, TInt aStatus)
	{
	TRACE_FUNCTION("SliceComplete");
	// Need start next slice or swap to another job and start it
	if(aJob != iJobList)
		{
		Kern::Printf("DCryptoJobScheduler::SliceComplete - not running job 0x%x=0x%x",
					 aJob, iJobList);
		return;
		}
	
	if(aStatus != KErrNone)
		{
		JobComplete(aJob, aStatus);
		return;
		}

	Schedule(ETrue); // Change jobs and run top one
	return;
	}


EXPORT_C void DCryptoJobScheduler::JobComplete(CryptoJob *aJob, TInt aStatus)
	{
	TRACE_FUNCTION("JobComplete");
	if(aJob != iJobList)
		{
		Kern::Printf("DCryptoJobScheduler::JobComplete - not running job 0x%x=0x%x",
					 aJob, iJobList);
		return;
		}

	// Pop job and update state
	DeScheduleJob(aJob);
	
	aJob->iCallbacks->JobComplete(aStatus);

	Schedule(EFalse); // Run top job
	}

void DCryptoJobScheduler::Schedule(TBool aReschedule)
	{
	TRACE_FUNCTION("Schedule");
	if(iJobList == 0)
		{
		return;
		}

	if(iJobList->iState == CryptoJob::ERunning)
		{
		// h/w busy so can not do anything now.
		return;
		}
	
	if((iJobList->iNext == 0) && (iJobList->iState == CryptoJob::EStalled))
		{
		// Only one job in list and its stalled. Leave it on the h/w
		// as an optimisation.
		return;
		}

	if(iJobList->iNext)
		{
		// More than one job in list.
		// If top job is stalled, or reschedule is requested then swap jobs
		// Only the top job can ever be marked as EStalled.
		// (Only running job can stall and new jobs get inserted second in the list)
		TBool stalledJob = (iJobList->iState == CryptoJob::EStalled);
		if(stalledJob || aReschedule)
			{
			//
			// Pop current job from front of list
			//
			CryptoJob *oldJob = iJobList;
			iJobList = iJobList->iNext;
			oldJob->iNext = 0;
			if(oldJob->DoSaveState())
				{
				// State was saved
				oldJob->iState = CryptoJob::EReadySavedState;
				}
			else
				{
				// No state was saved
				oldJob->iState = CryptoJob::EReadyNoSavedState;
				}
			if(stalledJob)
				{
				oldJob->iInJobList = EFalse;
				}
			else
				{
				//
				// Append oldJob to end of list
				//
				// Find ptr to last job
				CryptoJob **pp = &iJobList;
				while(*pp)
					{
					pp = &(*pp)->iNext;
					}
				// Append
				*pp = oldJob;
				}
			}
		}
	
	//
	// Run new job
	//
	CryptoJob *firstJob = iJobList;

	switch(firstJob->iState)
		{
		case CryptoJob::EReady:
			firstJob->DoSlice(EFalse);
			break;

		case CryptoJob::EReadyForFirstRun:
			firstJob->iState = CryptoJob::EReady;
			firstJob->DoSlice(ETrue);
			break;

		case CryptoJob::EReadyNoSavedState:
			firstJob->iState = CryptoJob::EReady;
			firstJob->DoSlice(EFalse);
			break;

		case CryptoJob::EReadySavedState:
			firstJob->iState = CryptoJob::EReady;
			firstJob->DoRestoreState();
			firstJob->DoSlice(EFalse);
			break;
			
		case CryptoJob::ECreated:
		case CryptoJob::EStalled:
		default:
			Kern::Printf("DCryptoJobScheduler::Schedule bad state %d", iJobList->iState);
			DeScheduleJob(firstJob); // Abort/remove from list
			Schedule(EFalse);
			return;
		}

	return;
	}


EXPORT_C CryptoJob::CryptoJob()
	: iState(ECreated),
	  iJobScheduler(0),
	  iCallbacks(0),
	  iNext(0),
	  iInJobList(EFalse)
	{
	TRACE_FUNCTION("CryptoJob");
	}

EXPORT_C CryptoJob::~CryptoJob()
	{
	TRACE_FUNCTION("~CryptoJob");
	// Do not call DeScheduleJob from here because it will crash....  The
	// derived class destructor has already run and the object is now
	// considered to be of type CryptoJob (not the derived class) so
	// most virtual functions are pure virtual and calling them will
	// cause a crash.
	}

EXPORT_C void CryptoJob::Stalled()
	{
	TRACE_FUNCTION("Stalled");
	iState = EStalled;
	iJobScheduler->SliceComplete(this, KErrNone);	
	}

EXPORT_C void CryptoJob::Resume()
	{
	TRACE_FUNCTION("Resume");
	if(iState == EStalled)
		{
		iState = EReady;
		}
	iJobScheduler->ScheduleJob(this);	
	}

EXPORT_C void CryptoJob::DeScheduleJob()
	{
	TRACE_FUNCTION("DeScheduleJob");
	if(iJobScheduler)
		{
		iJobScheduler->DeScheduleJob(this);
		}
	}

EXPORT_C void CryptoJob::SetRunning(TBool aRunning)
	{
	TRACE_FUNCTION("SetRunning");
	if(aRunning)
		{
		if((iState != EReady) && (iState != ERunning))
			{
			Kern::Printf("CryptoJob::SetRunning(%d) iState is %d this=%x", aRunning, iState, this);
			Kern::Fault("CryptoJob", 42);
			}
		iState = ERunning;
		}
	else
		{
		if((iState != ERunning) && (iState != EReady))
			{
			Kern::Printf("CryptoJob::SetRunning(%d) iState is %d this=%x", aRunning, iState, this);
			Kern::Fault("CryptoJob", 43);
			}
		iState = EReady;
		}
	}

EXPORT_C CryptoJob::CryptoJobState CryptoJob::State() const
	{
	TRACE_FUNCTION("State");
	return iState;
	}


// End of file

