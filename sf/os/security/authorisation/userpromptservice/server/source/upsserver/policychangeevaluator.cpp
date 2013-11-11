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
* Process RUpsManagement::NotifyPolicyFilesChanged.
*
*/


/**
 @file
*/

#include "upsserver.h"
#include <ups/upsdbw.h>
#include <e32property.h>
#include "policycache.h"
#include "policychangeevaluator.h"

namespace UserPromptService
{
inline CUpsSession *CPolicyChangeEvaluator::UpsSession()
	{
	return static_cast<CUpsSession*>(iSession);
	}

inline CUpsServer *CPolicyChangeEvaluator::UpsServer()
	{
	return static_cast<CUpsServer *>(
		&static_cast<CUpsSession*>(iSession)->iServer);
	}

CPolicyChangeEvaluator* CPolicyChangeEvaluator::NewLC(RPolicyCacheCountedHandle &aPolicyCacheHandle, CUpsSession* aSession, const RMessage2& aMessage)
	{
	CPolicyChangeEvaluator* self = new(ELeave) CPolicyChangeEvaluator(aPolicyCacheHandle, aSession, aMessage);
	CleanupStack::PushL(self);
	return self;
	}

CPolicyChangeEvaluator::CPolicyChangeEvaluator(RPolicyCacheCountedHandle &aPolicyCacheHandle, CUpsSession* aSession, const RMessage2& aMessage)
	:	CAsyncRequest(aSession, 0, aMessage),
		iPolicyCacheHandle(aPolicyCacheHandle),
		iUpdateDbHandle(UpsServer()->iDbHandle, this)
	{
	}

CPolicyChangeEvaluator::~CPolicyChangeEvaluator()
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


void CPolicyChangeEvaluator::StartUpdate()
	/// Starts evaluating the database view
	{
	iPolicyCacheHandle.NotifyOnRef1(iStatus);
	SetActive();
	}

void CPolicyChangeEvaluator::ProcessEventL(TPolicyChangeEvent aEvent)
/**
	Process either a DoCancel call or a request completing through RunL
*/
	{
	switch(iState)
		{
		case EWaitForPolicyCacheIdle:
			WaitForPolicyCacheIdleStateL(aEvent);
			return;
			
		case EScanDatabase:
			ScanDatabaseStateL(aEvent);
			return;
BULLSEYE_OFF
		}
BULLSEYE_RESTORE
	
	// Should not get here
	_LIT(KServerPanicState, "UPS-CPolicyChangeEvaluator");
	User::Panic(KServerPanicState, iState);
	}

void CPolicyChangeEvaluator::DoCleanup()
/// implement CAsyncRequest
	{
	Cancel();

	iSSPVarray.Close();
	
	delete iUpdateView;
	iUpdateView = 0;

	iUpdateDbHandle.Close();

	iPolicyCacheHandle.Release();
	}

void CPolicyChangeEvaluator::DoCancel()
	/// implement CActive - Cancel
	{
	TRAP_IGNORE(ProcessEventL(ECancel));
	}

	
void CPolicyChangeEvaluator::RunL()
	/// implement CActive, override CAsyncRequset
	{
	User::LeaveIfError(iStatus.Int());

	ProcessEventL(EInternalRequestComplete);
	}

TInt CPolicyChangeEvaluator::RunError(TInt aError)
	{
	iUpdateDbHandle.CloseMaster();
	return CAsyncRequest::RunError(aError);
	}

void CPolicyChangeEvaluator::DbHandleAboutToBeDeleted()
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

void CPolicyChangeEvaluator::WaitForPolicyCacheIdleStateL(TPolicyChangeEvent aEvent)
	{
	switch(aEvent)
		{
		case EInternalRequestComplete:
			{
			// Cache is idle so now create the view and walk the database
			CDecisionFilter *filter= CDecisionFilter::NewLC();
			iUpdateView = iUpdateDbHandle->CreateViewL(*filter);
			CleanupStack::PopAndDestroy(filter);

			// Update state and start evaluating the view
			iState = EScanDatabase;
			iUpdateView->EvaluateView(iStatus);
			SetActive();
			break;
			}
		case ECancel:
			iPolicyCacheHandle.CancelNotifyOnRef1();
			break;
BULLSEYE_OFF
		}
BULLSEYE_RESTORE

	}

void CPolicyChangeEvaluator::ScanDatabaseStateL(TPolicyChangeEvent aEvent)
	{
	switch(aEvent)
		{
		case EInternalRequestComplete:
			{
			// Finished evaluating the view
			BuildServerServicePolicyVersionTableL();
			
			// If the server policy cache handle has not already been
			// re-opened, open it.
			if(!UpsServer()->iPolicyCache.IsOpen())
				{
				UpsServer()->iPolicyCache.OpenL();
				}
			// Assign the server handle to ours - this will increment the reference count.
			iPolicyCacheHandle = UpsServer()->iPolicyCache;

			// Set the property, the value does not matter.
			// We do not need to change the value, setting to the same value still notifies all clients of
			// an update.
			User::LeaveIfError(RProperty::Set(KUpsServerUid, KUpsServiceConfigProperty, 42));

			// Process the iSSPVarray and delete uneeded database entries
			DeleteUnneededEntriesL();

			// All done.
			CompleteAndMarkForDeletion(KErrNone);
			break;
			}
		case ECancel:
			iUpdateView->Cancel();
			break;
BULLSEYE_OFF
		}
BULLSEYE_RESTORE
	}



TInt CPolicyChangeEvaluator::OrderServerServicePolicyVersion(const TServerServicePolicyVersion &aLhs,
									 			 						 const TServerServicePolicyVersion &aRhs)
{
	TInt t = aLhs.iServerId - aRhs.iServerId;
	if(t) return t;
	t = aLhs.iServiceId - aRhs.iServiceId;
	if(t) return t;
	t = aLhs.iMajorVersion - aRhs.iMajorVersion;
	return t;
}

void CPolicyChangeEvaluator::BuildServerServicePolicyVersionTableL()
	{
	TLinearOrder<TServerServicePolicyVersion> linearOrder(OrderServerServicePolicyVersion);

	while(CDecisionRecord *record = iUpdateView->NextDecisionL())
		{
		TServerServicePolicyVersion ent = { record->iServerSid.iId, 
											record->iServiceId.iUid,
											record->iMajorPolicyVersion };
		delete record;
		record = 0;
		TInt r = iSSPVarray.InsertInOrder(ent, linearOrder);
		if((r != KErrNone) && (r != KErrAlreadyExists))
			{
			User::Leave(r);
			}
		}


	}

static TBool CompareServiceId(const TServiceId* k, const TServiceConfig& t)
	{
	return TUint(k->iUid) == t.iServiceId;
	}

void CPolicyChangeEvaluator::DeleteUnneededEntriesL()
	{
	TInt count = iSSPVarray.Count();

	if(count == 0) return; // No database entries!

	TSecureId serverSid(0); // ID of server currently being processed. 0 is illegal
	RArray<TServiceConfig> serviceConfigArray;
	CleanupClosePushL(serviceConfigArray);
	for(TInt i=0; i<count; ++i)
		{
		const TServerServicePolicyVersion &sspv = iSSPVarray[i];
		TServiceId serviceId = {sspv.iServiceId};

		// The iSSPVarray is sorted by server/service/version. This means
		// all entries for a single server will be adjacent to each other
		if(serverSid != sspv.iServerId)
			{
			// 0 is not a valid server SID, so this will always run the first time
			// through the loop.
			serviceConfigArray.Close();
			serverSid = sspv.iServerId;
			iPolicyCacheHandle->ServiceConfigL(serverSid, serviceConfigArray);
			}

		CDecisionFilter *filter = CDecisionFilter::NewLC();
		filter->SetServerSid(serverSid, EEqual);
		filter->SetServiceId(serviceId, EEqual);

		// Now attempt to lookup the current ServiceId in the serviceconfig for 
		// the current server.
		TInt serviceIndex = serviceConfigArray.Find(serviceId, CompareServiceId);
		if(serviceIndex != KErrNotFound)
			{
			DEBUG_PRINTF5(_L8("i=%d Found server %x service %x at index %d\n"),
							i, sspv.iServerId, sspv.iServiceId, serviceIndex);
			TServiceConfig &serviceConfig = serviceConfigArray[serviceIndex];
			filter->SetMajorPolicyVersion(serviceConfig.iMajorVersion, ENotEqual);
			DEBUG_PRINTF4(_L8("Deleting OLD decisions where server=0x%x service=0x%x MajorVersion!=%d\n"),
							serverSid.iId, serviceId.iUid, serviceConfig.iMajorVersion);
			}
		else
			{
			DEBUG_PRINTF3(_L8("Deleting ALL decisions where server=0x%x service=0x%x MajorVersion==*\n"),
							serverSid.iId, serviceId.iUid);
			}

		TRAP_IGNORE(iUpdateDbHandle->RemoveDecisionsL(*filter));
		CleanupStack::PopAndDestroy(filter);
			
		};
	CleanupStack::PopAndDestroy(&serviceConfigArray);

	}

} // End of namespace UserPromptServer
// End of file


