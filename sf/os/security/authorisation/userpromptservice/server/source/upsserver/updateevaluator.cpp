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
* Implements used to process RUpsManagement::UpdateDecisionL.
*
*/


/**
 @file
*/

#include "upsserver.h"
#include <ups/upsdbw.h>
#include "updateevaluator.h"

namespace UserPromptService
{
inline CUpsSession *CUpdateEvaluator::UpsSession()
	{
	return static_cast<CUpsSession*>(iSession);
	}

inline CUpsServer *CUpdateEvaluator::UpsServer()
	{
	return static_cast<CUpsServer *>(
		&static_cast<CUpsSession*>(iSession)->iServer);
	}

CUpdateEvaluator* CUpdateEvaluator::NewLC(CUpsSession* aSession, const RMessage2& aMessage)
	{
	CUpdateEvaluator* self = new(ELeave) CUpdateEvaluator(aSession, aMessage);
	CleanupStack::PushL(self);
	self->ConstructL(aMessage);
	return self;
	}

CUpdateEvaluator::~CUpdateEvaluator()
/**
	Normally cleanup should be done when DoCleanup function is called by the framework.
	Sometime later, possibly after our parent CUpsSession has been deleted, this
	destructor will be run. In this case the framework will have cleared our iSession variable
	and we must do NOTHING.

	Unfortunately there is a special case where this object fails inside ConstructL, when we must do 
	some cleanup. We can detect this be seeing iSession (and hence UpsServer()) is non-NULL.
*/
	{
	CUpsSession *session = UpsSession();
	if(session)
		{
		/*lint -save -e1506*/ // ignore warning about calling a virtual function in a destructor
		DoCleanup();
		/*lint -restore*/
		}
	}

void CUpdateEvaluator::StartUpdate()
	/// Starts evaluating the database view
	{
	iUpdateView->EvaluateView(iStatus);
	SetActive();
	}

CUpdateEvaluator::CUpdateEvaluator(CUpsSession* aSession, const RMessage2& aMessage)
	:	CAsyncRequest(aSession, 0, aMessage),
		iUpdateDbHandle(UpsServer()->iDbHandle, this)
	{
	}

void CUpdateEvaluator::ConstructL(const RMessage2& aMessage)
	{
	// Read record id and new yes/no decision from client
	TUint32 recordId = aMessage.Int0();
	iNewResult = (aMessage.Int1() != 0);

	CDecisionFilter *filter= CDecisionFilter::NewLC();
	filter->SetRecordId(recordId, EEqual);
	
	// Create the CDecisionView object
	iUpdateView = iUpdateDbHandle->CreateViewL(*filter);

	CleanupStack::PopAndDestroy(filter);
}


void CUpdateEvaluator::DoCleanup()
/// implement CAsyncRequest
	{
	Cancel();

	delete iUpdateView;
	iUpdateView = 0;

	iUpdateDbHandle.Close();
	}

void CUpdateEvaluator::DoCancel()
	/// implement CActive - Cancel the database CreateView
	{
	ASSERT(iUpdateView != 0);
	iUpdateView->Cancel();
	}

	
void CUpdateEvaluator::RunL()
	/// implement CActive, override CAsyncRequset
	{
	User::LeaveIfError(iStatus.Int());

	// We have created the view, which should have matched a single record.....
	CDecisionRecord *record = iUpdateView->NextDecisionL();
	CleanupStack::PushL(record);
	if(record == 0)
		{
		User::Leave(KErrNotFound);
		}

	// Update the result
	record->iResult = iNewResult;

	// Prepare a filter with our complete key in it.
	CDecisionFilter *filter = CDecisionFilter::NewLC(record->iClientSid,
													 record->iEvaluatorId,
													 record->iServiceId,
													 record->iServerSid,
													 record->iFingerprint,
													 record->iClientEntity,
													 record->iMajorPolicyVersion);


	// Update the record
	iUpdateDbHandle->UpdateDecisionL(*filter, *record);

	// Cleanup	
	CleanupStack::PopAndDestroy(filter);
	CleanupStack::PopAndDestroy(record);

	CompleteAndMarkForDeletion(KErrNone);
	}

TInt CUpdateEvaluator::RunError(TInt aError)
	{
	iUpdateDbHandle.CloseMaster();
	return CAsyncRequest::RunError(aError);
	}

void CUpdateEvaluator::DbHandleAboutToBeDeleted()
/**
	Called just before the master database handle is shut.
	Need to cancel and cleanup/delete our view and fail the client request.
*/
	{
	// Make sure our request is cancelled
	Cancel();

	// Cleanup/delete our view object
	DoCleanup();
	
	// Abort the client view request.
	CompleteAndMarkForDeletion(KErrAbort);
	}

} // End of namespace UserPromptServer
// End of file
