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


#include "tups_policyevaluator.h"
#include "tups_policyevaluator_defs.h"
#include "tupsintegdefs.h"
#include <ecom/implementationproxy.h>
#include <ups/cliententity.h>
#include <ups/fingerprint.h>
#include <ups/upsdb.h>

using namespace UserPromptService;

CPolicyEvaluator* CTestPolicyEvaluator::CreatePolicyEvaluatorL()
/**
Factory method that instantiates a new policy evaluator ECOM plug-in.

@return A pointer to the new reference policy evaluator object.
*/
	{
	CTestPolicyEvaluator* self = new (ELeave)CTestPolicyEvaluator();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


static const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KTestPolicyEvaluatorImplementationId, CTestPolicyEvaluator::CreatePolicyEvaluatorL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
/**
Standard ECOM factory
*/
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}	
	

CTestPolicyEvaluator::CTestPolicyEvaluator()
/**
Constructor
*/
	: CPolicyEvaluator()
	{
	CActiveScheduler::Add(this);
	}
	
CTestPolicyEvaluator::~CTestPolicyEvaluator()
/**
Destructor
*/
	{
	Deque();
	
	iCheckIfStillNeedtoHold.Close();
	
	delete iDigest;
	delete iClientEntity;
	delete iUpsProperty;
	}


void CTestPolicyEvaluator::ConstructL()
/**
Second phase constructor, creates the message digest
*/
	{	
	iDigest = CMessageDigestFactory::NewDigestL(CMessageDigest::EMD5);
	iUpsProperty = CUpsProperty::NewL(KPropertyCreatorUid);
	}


// From CActive
void CTestPolicyEvaluator::DoCancel()
	{
	switch(iState)
		{
		case EHoldEvaluator:
			{
			iCheckIfStillNeedtoHold.Cancel();
			break;
			}
		
		default:
			{
			break;
			}
		}

	// And need to complete the clients request
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, KErrCancel);
		}
	}
	
	
TInt CTestPolicyEvaluator::RunError(TInt aError)
	{
	// If an error has occured, publish via the error code property
	TRAPD(err,iUpsProperty->SetL(KPolicyEvaluatorInstance,KPe_Error,CUpsProperty::EPolicyEvaluator,aError)); // Publish Error Message	
		
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, aError);
		}
		
	return err;
	}
	
	
void CTestPolicyEvaluator::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	switch (iState)
		{
		case EInitialisation:
			{
			DoInitialisationL();
			break;
			}
		
		case EGenerateFingerprints:
			{
			DoGenerateFingerprintsL();
			break;
			}
			
		// this state is used to hold a dialog open whilst waiting for something else
		// to allow the dialog to continue - typically a coordinating test step
		case EHoldEvaluator:
			{
			iState = EGenerateFingerprints;
			iStatus = KRequestPending;
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			SetActive();
			break;			
			}
			
		default:
			{
			ASSERT(EFalse);		
			}		
		}
	}
	
void CTestPolicyEvaluator::DoInitialisationL()
	{
	RDebug::Printf("[UPS PolicyEvaluator %d 0x%x] DoInitialisationL() Initialistion state started\n", KPolicyEvaluatorInstance, this);
	
	//Set the Policy Evaluator Status Property to "STARTED"...
	iUpsProperty->SetL(KPolicyEvaluatorInstance,KPe_Status,CUpsProperty::EPolicyEvaluator,KUpsPeStatusStarted);

	//Set the Client, Server and Service ID values within the Policy Evaluator UPS Properties
	iUpsProperty->SetL(KPolicyEvaluatorInstance,KPe_ClientSid,CUpsProperty::EPolicyEvaluator,iRequest->ClientSid());
	iUpsProperty->SetL(KPolicyEvaluatorInstance,KPe_ServerSid,CUpsProperty::EPolicyEvaluator,iRequest->ServerSid());
	iUpsProperty->SetL(KPolicyEvaluatorInstance,KPe_ServiceId,CUpsProperty::EPolicyEvaluator,iRequest->ServiceId().iUid);
	
	iStatus = KRequestPending;
	
	// check if the evaluator should be held open	
	TInt holdEvaluatorOpen = 0;
	TRAPD(err,iUpsProperty->GetL(KPolicyEvaluatorInstance, KPe_HoldEvaluatorOpen, CUpsProperty::EPolicyEvaluator, holdEvaluatorOpen));
	
	if (holdEvaluatorOpen && err == KErrNone)
		{
		RDebug::Printf("[UPS Policy Evaluator %d 0x%x] DoInitialisationL() detected an 'Evaluator' hold \n", KPolicyEvaluatorInstance, this);
		
		User::LeaveIfError(iCheckIfStillNeedtoHold.Attach(KPropertyCreatorUid, iUpsProperty->GetPropertyKey(KPe_HoldEvaluatorOpen, KPe_Start, KInstanceInterval, KPolicyEvaluatorInstance)));
		iCheckIfStillNeedtoHold.Subscribe(iStatus);	
		
		iState = EHoldEvaluator;
		
		//Set the Policy Evaluator Status Property to "EVALUATOR HOLD"...
		iUpsProperty->SetL(KPolicyEvaluatorInstance,KPe_Status,CUpsProperty::EPolicyEvaluator,KUpsPeStatusHolding);
			
		TInt holdState;
		iCheckIfStillNeedtoHold.Get(holdState);
		
	 	if(holdState)
		 	{
			// move to the next state
			SetActive();
			return;
			}
		
		// Property has already been updated to release us, so cancel the
		// subscribe and continue directly to complete the hold state
		iCheckIfStillNeedtoHold.Cancel();
		User::WaitForRequest(iStatus);
		}
		
	// Complete request and set the EGenerateFingerprints state
	iState = EGenerateFingerprints;
	iStatus = KRequestPending;
	TRequestStatus* status = &iStatus;
	SetActive();
	User::RequestComplete(status, err);	
	}
	
	
void CTestPolicyEvaluator::DoGenerateFingerprintsL()
	{
	// This plug-in doesn't use the opaque data	
	TInt opaqueDataLen = iRequest->OpaqueData().Length();
	if (opaqueDataLen > 0)
		{
		RDebug::Printf("Received %d bytes of opaque data", opaqueDataLen);
		}

	SetClientEntityL();

	// Create most specific hash first i.e. HASH(destination)
	// N.B. Normally, the destination field should be parsed so that only relevant data is hashed.		
	iDigest->Reset();
	const TDesC& d = iRequest->Destination();
	TPtrC8 p(reinterpret_cast<const TUint8*>(d.Ptr()), d.Length() * 2);
	TPtrC8 h(iDigest->Hash(p));

	CFingerprint* f = CFingerprint::NewLC(h, d);
	iFingerprints->AppendL(f);
	CleanupStack::Pop(f);

	// Retrieve the published "Leave" property value
	TInt runLeave = 0;
	iUpsProperty->GetL(KPolicyEvaluatorInstance,KPe_Leave,CUpsProperty::EPolicyEvaluator,runLeave);

	// Determine whether the Policy Evaluator should Leave
	if(runLeave)
		{
		User::Leave(KErrPeLeave);	
		}

	// Create a fingerprint that matches ALL local rate destinations
	_LIT(KLocalIndentifier, "+");

	iDigest->Reset();
	HBufC* localBuf = HBufC::NewLC(iRequest->Destination().Length()+1);
	TPtr localPtr = localBuf->Des();
	localPtr.Append(iRequest->Destination());
	localPtr.Append(KLocalIndentifier);

	TPtrC8 tempLocalPtr(reinterpret_cast<const TUint8*>(localPtr.Ptr()), localPtr.Length() * 2);
	TPtrC8 localHash(iDigest->Hash(tempLocalPtr));

	CleanupStack::PopAndDestroy(localBuf);

	_LIT16(KLocalDestinationDescription, "LOCAL Destinations");
	f = CFingerprint::NewLC(localHash, KLocalDestinationDescription);
	iFingerprints->AppendL(f);
	CleanupStack::Pop(f);

	// Create a fingerprint that matches ALL destinations (not actually a wildcard)
	// N.B. Since the format of the fingerprint is internal to the policy evaluator
	// there is no requirement to hash this value.
	_LIT8(KAllDestinations8, "*");
	_LIT16(KAllDestinationsDescription, "ALL Destinations");
	f = CFingerprint::NewLC(KAllDestinations8, KAllDestinationsDescription);
	iFingerprints->AppendL(f);
	CleanupStack::Pop(f);

	//Set the Policy Evaluator Status Property to "COMPLETED"...
	iUpsProperty->SetL(KPolicyEvaluatorInstance,KPe_Status,CUpsProperty::EPolicyEvaluator,KUpsPeStatusCompleted);

	User::RequestComplete(iClientStatus, KErrNone); 	
	}
	

void CTestPolicyEvaluator::SetClientEntityL()
/**
Sets the name of the entity within the client process that made the request.
This is optional functionality designed to allow permissions to be set for scripts
where the scripting host process can execute a number of different scripts.
*/
	{
	// This doesn't really support client entities, so for testing purposes we
	// extract the client entity name from the opaque data.
	_LIT8(KClientEntityTagStart, "<ce>");
	_LIT8(KClientEntityTagEnd, "</ce>");
	TInt startPos;
	TInt endPos;
	if ((startPos = iRequest->OpaqueData().FindF(KClientEntityTagStart)) != KErrNotFound)
		{
		startPos += KClientEntityTagStart().Length();
		if ((endPos = iRequest->OpaqueData().FindF(KClientEntityTagEnd)) != KErrNotFound)	
			{
			if (endPos > startPos)	
				{
				TPtrC8 entityName = iRequest->OpaqueData().Mid(startPos, endPos - startPos);
				iClientEntity = CClientEntity::NewL(entityName);
				*iClientEntityPtr = iClientEntity;			
				}	
			}
		}
	}

	
void CTestPolicyEvaluator::GenerateFingerprints(
	const CPromptRequest& aRequest, const CPolicy& aPolicy, 
	RPointerArray<CFingerprint>& aFingerprints, const CClientEntity*& aClientEntity, 
	const TAny*& aDialogCreatorParams, TRequestStatus& aStatus)
	{
	iRequest = &aRequest;
	iPolicy = &aPolicy;
	iFingerprints = &aFingerprints;	
	
	iClientEntityPtr = &aClientEntity;	
	aDialogCreatorParams = 0;			// Not used by this reference plug-in
	
	iClientStatus = &aStatus;
	aStatus = KRequestPending;
		
	// Kick off the Policy Evaluator state machine
	iState = EInitialisation;
	iStatus = KRequestPending;
	TRequestStatus* status = &iStatus;
	SetActive();
	User::RequestComplete(status, KErrNone);
	}


TBool CTestPolicyEvaluator::ForcePromptL(const CDecisionRecord& aDecision, TUint& aNewEvaluatorInfo)
	{
	// Increment the EvaluatorInfo attribute to count the number of calls to Force Prompt for
	// for the particular policy evaluator instance 
	(void) aDecision;
	aNewEvaluatorInfo = aDecision.iEvaluatorInfo + 1;
	
	iUpsProperty->SetL(KPolicyEvaluatorInstance,KPe_EvaluatorInfo,CUpsProperty::EPolicyEvaluator,aNewEvaluatorInfo);
	
	// Retrieve the published "Force Prompt" property value
	TInt forcePrompt = 0;
	iUpsProperty->GetL(KPolicyEvaluatorInstance,KPe_ForcePrompt,CUpsProperty::EPolicyEvaluator,forcePrompt);
	
	// Determine whether the Policy Evaluator should Force Prompt
	if(forcePrompt)
		{
		return ETrue;	
		}
	else
		{
		return EFalse;	
		}
	}
