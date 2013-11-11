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

#ifndef __LOGSERVOPERATIONQUEUE_H__
#define __LOGSERVOPERATIONQUEUE_H__

#include <e32base.h>
#include "LogServDefs.h"
#include "LogServShared.h"
#include "LogServOperationManager.h"

// Classes referenced
class MLogServTaskInterface;
class CLogServOperationBase;

/**
Implements the MLogServOperationManager interface.
The class maintains two queues of operations:
    - "pending operations" queue. All pending operations are kept in here for execution;
    - "completed operations" queue. If particular operation returns results for the client, that
                operation will be moved to here after the execution;
CLogServOperationQueue implements MLogServOperationManager and dereives from CActive.
Every time when a new operation is added to the queue, CLogServOperationQueue object completes itsef
and schedules itself for an execution. When the active scheduler calls the RunL() method of the class,
an operation will be picked up from the pending queue and started.                

@see MLogServOperationManager
@see MLogServTaskInterface
@see CLogServOperationBase
 
@internalComponent
*/
class CLogServOperationQueue : public CActive, public MLogServOperationManager
	{
public:
	static CLogServOperationQueue* NewL(MLogServTaskInterface& aTaskInterface, TInt aPriority);
	~CLogServOperationQueue();

private:
	CLogServOperationQueue(MLogServTaskInterface& aTaskInterface, TInt aPriority);

private: // From MLogServOperationManager
	void OMOperationQueueAdd(CLogServOperationBase& aOperation);
	void OMOperationQueueRemove(CLogServOperationBase& aOperation);
	void OMGetResultL(TLogOperationId aId, TLogServSessionId aSessionId, const RMessage2& aMessageToWriteTo);
	void OMCancel(TLogOperationId aId, TLogServSessionId aSessionId, TBool aCompleteRequest);
	void OMCancel(TLogServSessionId aSessionId, TBool aCompleteRequest);

private: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private: // Internal methods
	void CompleteRequest(TInt aCompletionCode = KErrNone);
	void StartNextOpL();		
	void DeleteFromQueue(TSglQue<CLogServOperationBase>& aQueue, TLogOperationId aOperationId, TLogServSessionId aSessionId);
	TBool QueueContainsOperation(TSglQue<CLogServOperationBase>& aQueue, TLogOperationId aOperationId, TLogServSessionId aSessionId);
	CLogServOperationBase* FindOperation(TSglQue<CLogServOperationBase>& aQueue, TLogOperationId aOperationId, TLogServSessionId aSessionId);

private: // Member data

	MLogServTaskInterface& iTaskInterface;
	CLogServOperationBase* iCurrentOperation;
	TSglQue<CLogServOperationBase> iQueuePending;
	TSglQue<CLogServOperationBase> iQueueCompleted;
	};

#endif
