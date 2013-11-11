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
* Implements CViewEvaluator.	 See class and function definitions for
* more information.
*
*/


/**
 @file
*/
#include "upsserver.h"
#include "viewevaluator.h"
#include <ups/upsdbw.h>
#include <scs/ipcstream.h>
#include <scs/nullstream.h>

namespace UserPromptService
{
inline CUpsSession *CViewEvaluator::UpsSession()
	{
	return static_cast<CUpsSession*>(iSession);
	}

CViewEvaluator* CViewEvaluator::NewLC(CUpsSession* aSession, const RMessage2& aMessage)
	{
	CViewEvaluator* self = new(ELeave) CViewEvaluator(aSession, aMessage);
	CleanupStack::PushL(self);
	self->ConstructL(aMessage);
	return self;
	}

CViewEvaluator::~CViewEvaluator()
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
		/*lint -save -e1506 */ // ignore warning about calling virtual function in destructor
		DoCleanup();
		/*lint -restore */
		}
	}

void CViewEvaluator::StartEvaluatingView()
	/// Starts evaluating the database view
	{
	UpsSession()->iManagementView->EvaluateView(iStatus);
	SetActive();
	}

CViewEvaluator::CViewEvaluator(CUpsSession* aSession, const RMessage2& aMessage)
	:	CAsyncRequest(aSession, 0, aMessage)
	{
	}

void CViewEvaluator::ConstructL(const RMessage2& aMessage)
	{
	// Read filter from the client
	RIpcReadStream ipcstream;
	ipcstream.Open(aMessage, 0);
	CleanupClosePushL(ipcstream);
	CDecisionFilter *filter = CDecisionFilter::NewLC();
	ipcstream >> *filter;

	// Set the session slave DB handle callback to us, so we know if the handle is about to be 
	// deleted...
	//RDebug::Printf("CViewEvaluator::ConstructL calling SetCallback(%x)\n", this);
	UpsSession()->iDbViewHandle.SetCallback(this);

	// Create the CDecisionView object
	// nb. We do not need to check iManagementView is NULL here because CUpsSession would have failed the request with KErrServerBusy if it were not.
	UpsSession()->iManagementView = UpsSession()->iDbViewHandle->CreateViewL(*filter);

	CleanupStack::PopAndDestroy(filter);
	CleanupStack::PopAndDestroy(&ipcstream);
}


void CViewEvaluator::DoCleanup()
/// implement CAsyncRequest
	{
	Cancel();
	// Reset slave DB handle callback to the session object
	//RDebug::Printf("CViewEvaluator::DoCleanup - %x calling SetCallback(%x)\n", this, UpsSession());
	UpsSession()->iDbViewHandle.SetCallback(UpsSession());
	}

void CViewEvaluator::DoCancel()
	/// implement CActive - Cancel the database CreateView
	{
	CDecisionView *view = UpsSession()->iManagementView;
	ASSERT(view != 0);
	view->Cancel();

	// Cancelled so cleanup view 
	UpsSession()->CleanupView();
	}

	
void CViewEvaluator::RunL()
	/// implement CActive, override CAsyncRequset
	{
	User::LeaveIfError(iStatus.Int());
	UpsSession()->PrefetchRecordAndWriteLengthToClientL(iMessagePtr2);
	CompleteAndMarkForDeletion(KErrNone);
	}

TInt CViewEvaluator::RunError(TInt aError)
	{
	// Something bad happened so delete the view objects
	UpsSession()->CleanupView();
	return CAsyncRequest::RunError(aError);
	}

void CViewEvaluator::DbHandleAboutToBeDeleted()
/**
	Called just before the master database handle is shut.
	Need to cancel and cleanup/delete our view and fail the client request.
*/
	{
	// Make sure our request is cancelled
	// If the view create is in progress, or has completed but our RunL hasn't had a chance to run yet, we will still be active
	// so the DoCancel will get called and cleanup the view.
	// If RunL fails, RunError will cleanup the view, if it completes we will no longer be registered to be called.
	Cancel();

	// Abort the client view request.
	CompleteAndMarkForDeletion(KErrAbort);
	}
}
// End of file
