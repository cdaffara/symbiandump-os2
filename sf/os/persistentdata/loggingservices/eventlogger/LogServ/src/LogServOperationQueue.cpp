// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "LogServOperationQueue.h"

// User includes
#include "logservpanic.h"
#include "LogServOperationBase.h"
#include "LogServTaskInterface.h"


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServOperationQueue (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServOperationQueue::CLogServOperationQueue(MLogServTaskInterface& aTaskInterface, TInt aPriority)
:	CActive(aPriority),
	iTaskInterface(aTaskInterface),
	iQueuePending(CLogServOperationBase::QueueOffset()),
	iQueueCompleted(CLogServOperationBase::QueueOffset())
	{
	CActiveScheduler::Add(this);
	}

CLogServOperationQueue::~CLogServOperationQueue()
	{
	Cancel();
	//
	CLogServOperationBase* item = NULL;
	//
	TSglQueIter<CLogServOperationBase> pendingIterator(iQueuePending);
	while ((item = pendingIterator++) != NULL)
		{
		delete item;
		}
	//
	TSglQueIter<CLogServOperationBase> completedIterator(iQueueCompleted);
	while ((item = completedIterator++) != NULL)
		{
		delete item;
		}
	}

CLogServOperationQueue* CLogServOperationQueue::NewL(MLogServTaskInterface& aTaskInterface, TInt aPriority)
	{
	CLogServOperationQueue* self = new(ELeave) CLogServOperationQueue(aTaskInterface, aPriority);
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServOperationQueue::OMOperationQueueAdd(CLogServOperationBase& aOp)
//
//	Add a new event to the queue and restart processing if necessary
//
	{
	LOGTEXT3("CLogServOperationQueue::OMOperationQueueAdd() - Type: %d, Id: %d", aOp.Type(), aOp.Id());
	//
	iQueuePending.AddLast(aOp);
	if	(!IsActive())
		CompleteRequest();
	//
	LOGTEXT("CLogServOperationQueue::OMOperationQueueAdd() - end");
	}

//
//	Removes an operation from the relevant server queue
//
void CLogServOperationQueue::OMOperationQueueRemove(CLogServOperationBase& aOperation)
	{
	const TLogOperationId operationId = aOperation.Id();
	const TLogServSessionId sessionId = aOperation.SessionId();
	//
	LOGTEXT3("CLogServOperationQueue::OMOperationQueueRemove() - Id: %d, SessionId: %d", operationId, sessionId);
	//
	if(QueueContainsOperation(iQueuePending, operationId, sessionId))
		{
		iQueuePending.Remove(aOperation);
		}
	else
		{
		iQueueCompleted.Remove(aOperation);
		}
	//
	LOGTEXT("CLogServOperationQueue::OMOperationQueueRemove() - end");
	}

void CLogServOperationQueue::OMGetResultL(TLogOperationId aId, TLogServSessionId aSessionId, const RMessage2& aMessageToWriteTo)
//
//	Get the result of a specific operation
//
	{
	LOGTEXT3("CLogServOperationQueue::OMGetResultL() - Id: %d, SessionId: %d", aId, aSessionId);

	// Look for the op
	CLogServOperationBase* op = FindOperation(iQueueCompleted, aId, aSessionId);

	// If we found it, then return the result
	if	(op != NULL)
		{
		op->WriteL(aMessageToWriteTo);
		delete op;
		}
	//
	LOGTEXT("CLogServOperationQueue::OMGetResultL() - end");
	}

void CLogServOperationQueue::OMCancel(TLogOperationId aOpId, TLogServSessionId aSessionId, TBool aCompleteRequest)
//
//	Cancel a specific operation
//
	{
	LOGTEXT4("CLogServOperationQueue::OMCancel() - Id: %d, SessionId: %d, CompleteRequest: %d", aOpId, aSessionId, aCompleteRequest);

	if	(iCurrentOperation && iCurrentOperation->SessionId() == aSessionId && (aOpId == KLogNullOperationId || iCurrentOperation->Id() == aOpId))
		{
		// We need to cancel the current active operation because it belongs to this session
		//
		// This should complete the request with KErrCancel causing RunL to leave
		// RunError completes the request and removes the operation from the pending queue
		iTaskInterface.TaskCancelCurrent();

		//
		// Do not complete the message when the operation completes
		//
		if (!aCompleteRequest)
			{
			iCurrentOperation->SetMessageCompletion(aCompleteRequest);
			}
		}
	else
		{
		// The active operation doesn't belong to this session - just remove any relevant operations from the queues
		//
		// We are taking advantage of the fact that a session will only ever have one operation outstanding
		// so a cancel request basically means - cancel everything belonging to the session
		CLogServOperationBase* op = NULL;
		while((op = FindOperation(iQueuePending, aOpId, aSessionId)) != NULL)
			{
			if	(aCompleteRequest)
				{
				op->Cancel();
				}

			// The operation dequeues itself from the containing queue in its destructor via
			// the MLogServOperationManager abstract interface.
			delete op;

			// We can exit now if we were looking for a particular operation
			if	(aOpId > KLogNullOperationId)
				{
				return;
				}
			}

		// Just delete the completed operation
		DeleteFromQueue(iQueueCompleted, aOpId, aSessionId);
		}
	//
	LOGTEXT("CLogServOperationQueue::OMCancel() - end");
	}

//
//	Stop/cancel and delete all operations belonging to a particular session
//	If the session has just died we don't want to complete the request - the kernel would panic us
//
void CLogServOperationQueue::OMCancel(TLogServSessionId aSessionId, TBool aCompleteRequest)
	{
	OMCancel(KLogNullOperationId, aSessionId, aCompleteRequest);
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServOperationQueue::RunL()
//
//	Called to indicate that the operation has completed - so complete the client request
//
	{
	LOGTEXT2("CLogServOperationQueue::RunL(%d)", iStatus.Int());

	// If the queue has just been restarted there won't be an existing operation
	if (iCurrentOperation)
		{
		LOGTEXT3("CLogServOperationQueue::RunL() - Current operation Type: %d, Id: %d,", iCurrentOperation->Type(), iCurrentOperation->Id());

		// Handle errors in RunError
		User::LeaveIfError(iStatus.Int());

		// Remember the type of operation for later
		const TLogOperationType type = iCurrentOperation->Type();

		// Complete the operation
		if	(iCurrentOperation->CompleteProcessingL(iStatus.Int()) == CLogServOperationBase::EOperationComplete)
			{
			LOGTEXT("CLogServOperationQueue::RunL() - Operation is complete, deleting it");
			delete iCurrentOperation;
			iCurrentOperation = NULL;
			}
		else
			{
			// The client is going to ask for some results.
			//
			// Move the operation to another queue until it's done
			// The session will actually delete the operation
			LOGTEXT("CLogServOperationQueue::RunL() - Operation result awaiting client fetch");
			iQueuePending.Remove(*iCurrentOperation);
			iQueueCompleted.AddLast(*iCurrentOperation);
			}

		// Any errors from maintenance will be ignored because the operation is null
		iCurrentOperation = NULL;

		LOGTEXT("CLogServOperationQueue::RunL() - Starting maintenance");
		iTaskInterface.TaskMaintenanceStartL(iStatus, type != ELogOperationEventGet);
		SetActive();
		}
	else
		{
		LOGTEXT("CLogServOperationQueue::RunL() - finding next operation");

		// Start the next operation
		StartNextOpL();
		}

	LOGTEXT("CLogServOperationQueue::RunL() - end");
	}

void CLogServOperationQueue::DoCancel()
//
//	Implements cancel policy. Only called if the maintenance is active?
//
	{
	iTaskInterface.TaskCancelCurrent();
	}

TInt CLogServOperationQueue::RunError(TInt aError)
//
//	Handle errors
//
	{
	LOGTEXT2("CLogServOperationQueue::RunError(%d)", aError);

	if	(iCurrentOperation)
		{
		LOGTEXT3("CLogServOperationQueue::RunError() - Current operation Type: %d, Id: %d,", iCurrentOperation->Type(), iCurrentOperation->Id());

		// Fail the client request with the error
		if	(iCurrentOperation->HaveMessagePointer())
			iCurrentOperation->Complete(aError);
		delete iCurrentOperation;
		iCurrentOperation = NULL;
		}

	// Restart by mending the database if necessary
	TRAPD(mendError, iTaskInterface.TaskMaintenanceStartL(iStatus, ETrue));
	if	(mendError)
		{
		LOGTEXT2("CLogServOperationQueue::RunError() - mend error: %d)", mendError);

		// Just ignore the maintenance error and complete ourselves so we continue to execute operations
		CompleteRequest(KErrNone);
		}
	else
		{
		LOGTEXT("CLogServOperationQueue::RunError() - setting active again");
		SetActive();
		}

	LOGTEXT("CLogServOperationQueue::RunError() - end");
	return KErrNone;
	}

void CLogServOperationQueue::CompleteRequest(TInt aCompletionCode)
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aCompletionCode);
	SetActive();
	}

void CLogServOperationQueue::StartNextOpL()
//
//	Start the next operation on the queue
//
	{
	LOGTEXT("CLogServOperationQueue::StartNextOpL()");

	iCurrentOperation = NULL;
	if (!IsActive() && !iQueuePending.IsEmpty())
		{
		iCurrentOperation = iQueuePending.First();

		LOGTEXT3("CLogServOperationQueue::StartNextOpL() - New next operation, Type: %d, Id: %d", iCurrentOperation->Type(), iCurrentOperation->Id());

		iCurrentOperation->StartL(iStatus);
		SetActive();
		}

	LOGTEXT("CLogServOperationQueue::StartNextOpL() - end");
	}

void CLogServOperationQueue::DeleteFromQueue(TSglQue<CLogServOperationBase>& aQueue, TLogOperationId aOperationId, TLogServSessionId aSessionId)
//
//	Delete all operations belonging to a particular session from a queue
//
	{
	LOGTEXT3("CLogServOperationQueue::DeleteFromQueue() - aId: %d, aSessionId: %d", aOperationId, aSessionId);

	CLogServOperationBase* op = NULL;
	while((op = FindOperation(aQueue, aOperationId, aSessionId)) != NULL)
		{
		delete op;

		// We can exit now if we were looking for a particular operation
		if	(aOperationId > KLogNullOperationId)
			{
			LOGTEXT("CLogServOperationQueue::DeleteFromQueue() - operation found successfully");
			return;
			}
		}

#ifdef LOGGING_ENABLED
	if	(aOperationId != KLogNullOperationId)
		{
		LOGTEXT2("CLogServOperationQueue::DeleteFromQueue() - operation id '%d' wasn't found!", aOperationId);
		}
#endif

	LOGTEXT("CLogServOperationQueue::DeleteFromQueue() - end");
	}

TBool CLogServOperationQueue::QueueContainsOperation(TSglQue<CLogServOperationBase>& aQueue, TLogOperationId aOperationId, TLogServSessionId aSessionId)
//
//	Finds an operation in a queue
//
	{
	LOGTEXT3("CLogServOperationQueue::QueueContainsOperation() - aId: %d, aSessionId: %d", aOperationId, aSessionId);

	TSglQueIter<CLogServOperationBase> iter(aQueue);
	CLogServOperationBase* item = NULL;
	
	while ((item = iter++) != NULL)
		{
		if	(item->Id() == aOperationId && item->SessionId() == aSessionId)
			{
			LOGTEXT("CLogServOperationQueue::QueueContainsOperation() - operation found within queue");
			return ETrue;
			}
		}
	
	LOGTEXT("CLogServOperationQueue::QueueContainsOperation() - queue doesn't contain operation");
	return EFalse;
	}

CLogServOperationBase* CLogServOperationQueue::FindOperation(TSglQue<CLogServOperationBase>& aQueue, TLogOperationId aOperationId, TLogServSessionId aSessionId)
//
//	Find the first operation in a queue belonging to a particular session
//
	{
	LOGTEXT3("CLogServOperationQueue::FindOperation() - aId: %d, aSessionId: %d", aOperationId, aSessionId);

	TSglQueIter<CLogServOperationBase> iter(aQueue);
	CLogServOperationBase* item = NULL;
	
	while ((item = iter++) != NULL)
		{
		if	(item->SessionId() == aSessionId && (aOperationId == KLogNullOperationId || item->Id() == aOperationId))
			{
			LOGTEXT("CLogServOperationQueue::FindOperation() - operation found successfully");
			return item;
			}
		}

	LOGTEXT("CLogServOperationQueue::FindOperation() - operation not found");
	return NULL;	
	}
