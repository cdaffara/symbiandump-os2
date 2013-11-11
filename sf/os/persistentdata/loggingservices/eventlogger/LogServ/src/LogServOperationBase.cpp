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

#include "LogServOperationBase.h"
#include "logservpanic.h"
#include "LogServOperationManager.h"


CLogServOperationBase::CLogServOperationBase(MLogServTaskInterface& aTaskInterface, 
											 MLogServOperationManager& aOperationManager, 
											 const RMessage2& aMessage, 
											 CLogPackage& aPackage,
					                         const TLogClientServerData& aClientServerData,
					                         TLogServSessionId aSessionId) :
	iTaskInterface(aTaskInterface), 
	iOperationManager(aOperationManager), 
	iPackage(aPackage), 
	iMessage(aMessage),
	iClientServerData(aClientServerData),
	iSessionId(aSessionId),
	iMessageCompletion(ETrue)
	{
	// Queue this operations
	OperationManager().OMOperationQueueAdd(*this);
	}

CLogServOperationBase::~CLogServOperationBase()
	{
	// Dequeue this operations
	OperationManager().OMOperationQueueRemove(*this);
	}

//
//	By default operations don't write back to the client
//
void CLogServOperationBase::WriteL(const RMessage2& /*aMessage*/)
	{
	}

//
//	Finish processing for this operation
//
CLogServOperationBase::TCompletionStatus CLogServOperationBase::CompleteProcessingL(TInt /*aFinalCompletionCode*/)
	{
	Complete(KErrNone);
	//
	return CLogServOperationBase::EOperationComplete;
	}

//
//	Complete the client request
//
void CLogServOperationBase::Complete(TInt aCompletionCode)
	{
	__ASSERT_ALWAYS(! iMessage.IsNull(), Panic(ELogNoOutstandingAsyncRequest));
	
	if (iMessageCompletion)
	  {
	  iMessage.Complete(aCompletionCode);
	  }

	// Unnecessary?
	iMessage = RMessage2();
	}

//
//	Return the offset of this item within a containing queue
//
TUint CLogServOperationBase::QueueOffset()
	{
	return _FOFF(CLogServOperationBase, iServerLink);
	}

