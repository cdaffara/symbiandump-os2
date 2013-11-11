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

// System includes
#include <logwrap.h>

// User includes
#include "SCHLOG.h"


//Constants
const TInt KLogEventQueueGranularity = 3;

CSchLogManager::CSchLogManager()
:	CActive(EPriorityLow)
	{
	CActiveScheduler::Add(this);
	}

CSchLogManager::~CSchLogManager()
	{
	Cancel();
  	if(iPendingLogEvents)
  		{
  		iPendingLogEvents->ResetAndDestroy();
  		delete iPendingLogEvents;
  		}
	delete iLogEvent;
	if(iLogWrapper)
 		delete iLogWrapper;
	}
	
void CSchLogManager::ConstructL(RFs& aFileSession)
	{
	iFileSession = &aFileSession;
	iLogWrapper = NULL;
	iLogEvent = CLogEvent::NewL();
	iLogEvent->SetEventType(KLogTaskSchedulerEventTypeUid);
	iPendingLogEvents = new(ELeave) CArrayPtrFlat<CLogEvent>(KLogEventQueueGranularity);
	}

CSchLogManager* CSchLogManager::NewL(RFs& aFileSession)
	{
	CSchLogManager* self = new(ELeave) CSchLogManager();
	CleanupStack::PushL(self);
	self->ConstructL(aFileSession);
	CleanupStack::Pop(self);
	return self;
	}

/*
Creates a new log engine entry of type KLogTaskSchedulerEventTypeUid
with aError as its entry.
*/
void CSchLogManager::LogError(TInt aError)
	{
	// Add error code as a descriptor
	TBuf<10> errorCodeAsDescriptor;
	errorCodeAsDescriptor.Num(aError);
	iLogEvent->SetNumber(errorCodeAsDescriptor);
	// Add to log or queue
  	if (!IsActive())
    	{
		iStatus=KRequestPending;
	
		TRAPD(error, GetLogWrapperL()->Log().AddEvent(*iLogEvent, iStatus));
		if (error != KErrNone)
			return; 
		SetActive();
		}
	else
		{
		// add the request to the queue, it will be processed asap
		CLogEvent* event = 0;
		TRAPD(error, event = CLogEvent::NewL());
		if (KErrNone != error)
			{
			return; // event is discarded!
			}
		//coverity[cleanup_stack]
		/* NewL() and AppendL() are enclosed in TRAP and handled. 
		* So no need of pushing the variable event onto the cleanup stack
		*/
		// add to the queue
		TRAP(error, iPendingLogEvents->AppendL(event));
		if (KErrNone != error)
			{
			delete event; // event is discarded!
			return;
			}
		}	
	}

/*
Creates a new log engine entry of type KLogTaskSchedulerEventTypeUid 
and subject aSubject with aError as its entry.
*/
void CSchLogManager::LogError(const TDesC& aSubject, TInt aError)
	{
	//AddSubject
	iLogEvent->SetSubject(aSubject);
	LogError(aError);
	}
	
void CSchLogManager::RunL()
  	{
    if (iPendingLogEvents->Count()>0)
        {
        CLogEvent* nextEventPtr = iPendingLogEvents->At(0);
        iLogEvent->CopyL(*nextEventPtr);
		iStatus=KRequestPending;
		
		//If error occur trap it and ignore, so that log manager can continue 
        TRAPD(error, GetLogWrapperL()->Log().AddEvent(*iLogEvent, iStatus));
       	//Following code is only to ignore build warnings        
        if (error == KErrNone)
				error = KErrNone;
        SetActive();
        // delete the ongoing CLogEvent we just copied
        delete nextEventPtr;
        //remove the pointer from the queue
        iPendingLogEvents->Delete(0);
        }
  	}
  
void CSchLogManager::DoCancel()
  	{
    if(iLogWrapper)
        iLogWrapper->Log().Cancel();
   	}
/*
Creates a new CLogWrapper and initialise iLogWrapper if it doesn't exist otherwise 
return iLogWrapper
*/
CLogWrapper* CSchLogManager::GetLogWrapperL()
	{
	if(iLogWrapper)
		return iLogWrapper;
	
	iLogWrapper = CLogWrapper::NewL(*iFileSession, CActive::EPriorityStandard);

	return iLogWrapper;
	}

	
