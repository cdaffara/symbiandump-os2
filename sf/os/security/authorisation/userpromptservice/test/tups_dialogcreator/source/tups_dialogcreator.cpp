/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#include "tups_dialogcreator.h"
#include "tups_dialogcreator_defs.h"
#include "tupsintegdefs.h"
#include <ecom/implementationproxy.h>
#include <e32debug.h>
#include <ups/promptrequest.h>
#include <scs/nullstream.h>
#include <s32mem.h>

CDialogCreator* CTestDialogCreator::CreateDialogCreatorL()
/**
Factory method that instantiates a new dialog creator ECOM plug-in.

@return A pointer to the new reference dialog creator object.
*/
	{
	CTestDialogCreator* self = new (ELeave)CTestDialogCreator();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

static const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KTestDialogCreatorImplementationId, CTestDialogCreator::CreateDialogCreatorL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
/**
Standard ECOM factory
*/
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}	
	

CTestDialogCreator::CTestDialogCreator() 
/**
Constructor
*/
	: CDialogCreator(), iPromptResult(), iPromptResultPckg(iPromptResult), iState(EIdle)

	{
	CActiveScheduler::Add(this);
	}
	
CTestDialogCreator::~CTestDialogCreator()
/**
Destructor
*/   
	{
	Deque();
	
	iPromptDataDes.Close();
	iNotifier.Close();
	iCheckIfStillNeedtoHold.Close();
	
	delete iPromptData;
	delete iUpsProperty;
	}

void CTestDialogCreator::ConstructL()
/**
Second phase constructor
*/
	{
	User::LeaveIfError(iNotifier.Connect());
	iUpsProperty = CUpsProperty::NewL(KPropertyCreatorUid);
	}

void CTestDialogCreator::DoCancel()
	{
	switch(iState)
		{
		case EProcessResult:
			{
			iNotifier.CancelNotifier(TUid::Uid(KTestNotifierImplementationId));
			break;
			}
	
		case EHoldPrepareDialog:
		case EHoldDisplayDialog:
			{
			iCheckIfStillNeedtoHold.Cancel();
			break;
			}
		
		default:
			{
			break;
			}
		}

	
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, KErrCancel);
		}
	}
	
TInt CTestDialogCreator::RunError(TInt aError)
	{
	// If an error has occured, publish via the error code property
	TRAPD(err,iUpsProperty->SetL(KDialogCreatorInstance,KDc_Error,CUpsProperty::EDialogCreator,aError)); // Publish Error Message	
	
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, aError);
		}
	return err;
	}

void CTestDialogCreator::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	switch (iState)
		{
		case EInitialisePrepare:
		case EInitialiseDisplay:
			{
			DoInitialisationL();
			break;	
			}
						
		// this state is used to hold a dialog open whilst waiting for something else
		// to allow the dialog to continue - typically a coordinating test step
		case EHoldPrepareDialog:
		case EHoldDisplayDialog:
			{
			iStatus = KRequestPending;
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			SetActive();
			
			// Set the necessary state depending on where the hold request occured
			if(iState == EHoldPrepareDialog)
				{
				iState = EPrepareDialog;	
				}
			else
				{
				iState = EDisplayDialog;	
				}
				
			break;	
			}
			
		case EPrepareDialog:
			{
			DoPrepareDialogL();
			break;
			}
			
		case EDisplayDialog:
			{
			RDebug::Printf("[UPS DialogCreator %d 0x%x] RunL() EDisplayDialog state started\n", KDialogCreatorInstance, this);
			
			// Set the DC Test UPS Buttons Displayed Property
			iUpsProperty->SetL(KDialogCreatorInstance,KDc_UpsRequestedButtons,CUpsProperty::EDialogCreator,iPolicy->Options());
	
			// Retrieve the published "Leave" property value
			TInt runLeave = 0;
			iUpsProperty->GetL(KDialogCreatorInstance,KDc_Leave,CUpsProperty::EDialogCreator,runLeave);
			
			// Determine whether the Dialog Creator should Leave
			if(runLeave)
				{
				User::Leave(KErrDcLeave);	
				}
			
			DoDisplayDialogL();
			break;
			}
			
		case EProcessResult:
			{
			DoProcessResultL();
			break;	
			}
				
		default:
			{
			ASSERT(EFalse);	
			}		
		}
	}
	
void CTestDialogCreator::DoInitialisationL()
	{
	// Set the Dialog Creator Status Property to "STARTED"...
	iUpsProperty->SetL(KDialogCreatorInstance,KDc_Status,CUpsProperty::EDialogCreator,KUpsDcStatusStarted);
	
	// Set the Client, Server and Service ID values within the Dialog Creator UPS Properties
	iUpsProperty->SetL(KDialogCreatorInstance,KDc_ClientSid,CUpsProperty::EDialogCreator,iRequest->ClientSid());
	iUpsProperty->SetL(KDialogCreatorInstance,KDc_ServerSid,CUpsProperty::EDialogCreator,iRequest->ServerSid());
	iUpsProperty->SetL(KDialogCreatorInstance,KDc_ServiceId,CUpsProperty::EDialogCreator,iRequest->ServiceId().iUid);
	
	TInt err = 0;
	
	if(iState == EInitialisePrepare)
		{
		// check if the dialog creator should be held in the 'Prepare Dialog' state	
		TInt holdPrepareDialog = 0;
		TRAP(err,iUpsProperty->GetL(KDialogCreatorInstance, KDc_HoldPrepareDialogOpen, CUpsProperty::EDialogCreator, holdPrepareDialog));
		
		if (holdPrepareDialog && err == KErrNone)
			{
			RDebug::Printf("[UPS DialogCreator %d 0x%x] PrepareDialog() detected a 'Prepare Dialog' hold \n", KDialogCreatorInstance, this);
			
			User::LeaveIfError(iCheckIfStillNeedtoHold.Attach(KPropertyCreatorUid, iUpsProperty->GetPropertyKey(KDc_HoldPrepareDialogOpen, KDc_Start, KInstanceInterval, KDialogCreatorInstance)));
			iCheckIfStillNeedtoHold.Subscribe(iStatus);	
			
			iState = EHoldPrepareDialog;
			
			//Set the Dialog Creator Status Property to "PREPARE DIALOG HOLD"...
			iUpsProperty->SetL(KDialogCreatorInstance,KDc_Status,CUpsProperty::EDialogCreator,KUpsDcStatusPrepareHold);
				
			TInt holdState;
			iCheckIfStillNeedtoHold.Get(holdState);
			
		 	if(holdState)
			 	{
				// move to the next state
				SetActive();
				return;
				}
			
			// Property has already been updated to release us, so cancel the
			// subscribe and continue directly to complete the display state
			iCheckIfStillNeedtoHold.Cancel();
			User::WaitForRequest(iStatus);
			}
		
		// Set Dialog Creator state to EPrepareDialog	
		iState = EPrepareDialog;	
		}
	else
		{
		// check if the dialog creator should be held in the 'Display Dialog' state		
		TInt holdDisplayDialog = 0;
		TRAP(err,iUpsProperty->GetL(KDialogCreatorInstance, KDc_HoldDisplayDialogOpen, CUpsProperty::EDialogCreator, holdDisplayDialog));
		
		if (holdDisplayDialog && err == KErrNone)
			{
			RDebug::Printf("[UPS DialogCreator %d 0x%x] DisplayDialog() detected a 'Display Dialog' hold \n", KDialogCreatorInstance, this);
			
			User::LeaveIfError(iCheckIfStillNeedtoHold.Attach(KPropertyCreatorUid, iUpsProperty->GetPropertyKey(KDc_HoldDisplayDialogOpen, KDc_Start, KInstanceInterval, KDialogCreatorInstance)));
			iCheckIfStillNeedtoHold.Subscribe(iStatus);	
			
			iState = EHoldDisplayDialog;
			
			//Set the Dialog Creator Status Property to "DISPLAY DIALOG HOLD"...
			iUpsProperty->SetL(KDialogCreatorInstance,KDc_Status,CUpsProperty::EDialogCreator,KUpsDcStatusDisplayHold);
				
			TInt holdState;
			iCheckIfStillNeedtoHold.Get(holdState);
			
		 	if(holdState)
			 	{
				// move to the next state
				SetActive();
				return;
				}
			
			// Property has already been updated to release us, so cancel the
			// subscribe and continue directly to complete the display state
			iCheckIfStillNeedtoHold.Cancel();
			User::WaitForRequest(iStatus);
			}
		
		// Set Dialog Creator state to EDisplayDialog	
		iState = EDisplayDialog;	
		}
	
	// Kick off dialog creator state machine
	iStatus = KRequestPending;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, err);
	SetActive();
	}

void CTestDialogCreator::DoPrepareDialogL()
	{	
	iPromptData = CPromptData::NewL();

	// Get the client name for the client process
	ResolveClientNameL(iRequest->ClientSid());
	
	// Get the vendor name for the client process
	ResolveVendorName(iRequest->ClientVid());
	
	// Server / Service localized names generated in notifier plug-in. 
	iPromptData->iServerSid = iRequest->ServerSid();
	iPromptData->iServiceId = iRequest->ServiceId();
	
	// Use the options specified by the policy
	iPromptData->iOptions = iPolicy->Options();
	
	// Add the descriptions of the fingerprints. This could be used
	// to allow the user to grant access to all destinations 
	// or a single destination.
	TInt count = iFingerprints->Count();
	for (TInt i = 0; i < count; ++i)
		{
		HBufC* description = (*iFingerprints)[i]->Description().AllocLC();
		iPromptData->iDescriptions.AppendL(description);
		CleanupStack::Pop(description);
		}
		
	User::RequestComplete(iClientStatus, KErrNone);		
	// DisplayDialog is invoked by the UPS, this just verifies 
	// that PrepareDialog was called first.
	iState = EDisplayDialog;
	}

void CTestDialogCreator::DoDisplayDialogL()
/**
Uses the notifier framework to display the dialog.
*/
	{
	// Externalize the prompt data to a descriptor
	RNullWriteStream ns;
	ns << *iPromptData;
	ns.CommitL();
	iPromptDataDes.Create(ns.BytesWritten());
	RDesWriteStream ws;	
	ws.Open(iPromptDataDes);
	ws << *iPromptData;
	ws.CommitL();
		
	//TEST: Determine which button should be pressed from the UPS Properties
	TInt buttonPress;
	TInt selectFingerprint;
	iUpsProperty->GetL(KDialogCreatorInstance,KDc_WhichButtonToPress,CUpsProperty::EDialogCreator,buttonPress);
	iUpsProperty->GetL(KDialogCreatorInstance,KDc_SelectFingerprint,CUpsProperty::EDialogCreator,selectFingerprint);
	
	TRequestStatus* status = &iStatus;

	iPromptResult.iDestination = KErrNotFound;
	
	switch(buttonPress)
		{
		case KYes:
			iPromptResult.iSelected = CPolicy::EYes;
			iStatus = KRequestPending;
			User::RequestComplete(status, KErrNone);
			break;
		case KNo:
			iPromptResult.iSelected = CPolicy::ENo;
			iStatus = KRequestPending;
			User::RequestComplete(status, KErrNone);
			break;
		case KSessionYes:
			iPromptResult.iSelected = CPolicy::ESessionYes;
			iStatus = KRequestPending;
			User::RequestComplete(status, KErrNone);
			break;
		case KSessionNo:
			iPromptResult.iSelected = CPolicy::ESessionNo;
			iStatus = KRequestPending;
			User::RequestComplete(status, KErrNone);
			break;
		case KAlways:
			iPromptResult.iSelected = CPolicy::EAlways;
			iPromptResult.iDestination = selectFingerprint;
			iStatus = KRequestPending;
			User::RequestComplete(status, KErrNone);
			break;
		case KNever:
			iPromptResult.iSelected = CPolicy::ENever;
			iPromptResult.iDestination = selectFingerprint;	
			iStatus = KRequestPending;
			User::RequestComplete(status, KErrNone);
			break;
		case KNone:	
			iNotifier.StartNotifierAndGetResponse(iStatus, TUid::Uid(KTestNotifierImplementationId), iPromptDataDes, iPromptResultPckg);
			break;
		default:
			ASSERT(EFalse);
		}
	
	// Set the "Which Button To Press" UPS Property back to KNone
	iUpsProperty->SetL(KDialogCreatorInstance,KDc_WhichButtonToPress,CUpsProperty::EDialogCreator,KNone);
	
	SetActive();
	iState = EProcessResult;
	}
	
void CTestDialogCreator::DoProcessResultL()
/**
Processes the result returned by the notifier.
*/
	{
	if (iPromptResult.iSelected == CPolicy::EAlways ||
		iPromptResult.iSelected == CPolicy::ENever)
		{
		// The Always or Never option was selected so return the fingerprint 
		// for the new decision record.
		// 
		// In this implementation a copy of the original fingerprint is returned. However,
		// it is permitted to return a different fingerprint e.g. a modifier description.		
		if (iPromptResult.iDestination >= 0 && iPromptResult.iDestination < iFingerprints->Count())		
			{
			*iFingerprint = (*iFingerprints)[iPromptResult.iDestination];
			}
		else
			{
			ASSERT(EFalse);	// should never happen, unless an invlaid property is passed 
			}
		}			
	*iOptionSelected =	iPromptResult.iSelected;
	
	//Set the Dialog Creator Status Property to "COMPLETED"...
	iUpsProperty->SetL(KDialogCreatorInstance,KDc_Status,CUpsProperty::EDialogCreator,KUpsDcStatusCompleted);
	
	iState = EIdle;
	User::RequestComplete(iClientStatus, KErrNone);	
	}

void CTestDialogCreator::ResolveVendorName(const TVendorId& aVid)
/**
Looks up the localized vendor name for the client process and writes
this to iPromptData->iVendorName.

@param aVid	The vendor id of the client process.
*/
	{
	
	if (aVid.iId == 0x70000001)
		{
		_LIT(KSymbian, "Symbian Software Ltd");
		iPromptData->iVendorName.Create(KSymbian);
		}
	else 
		{
		_LIT(KUnknown, "Unknown vendor");
		iPromptData->iVendorName.Create(KUnknown);
		}

	}
	
void CTestDialogCreator::ResolveClientNameL(const TSecureId& /*aSid*/)
/**
Generates a human readable name for the client process. In order of 
preference the following data is returned

- The AppArc caption name.
- The localized package name that owns this SID.
- A value from a lookup table.
- The filename for the client process executable.

@param aSid	The secure id of the client process.
*/
	{		
	// Fall back to the filename of the client process
	RProcess clientProcess;
	User::LeaveIfError(clientProcess.Open(iRequest->ClientProcessId()));
	CleanupClosePushL(clientProcess);
	iPromptData->iClientName.Create(clientProcess.FileName());		
	CleanupStack::PopAndDestroy(&clientProcess);
	}

// From CDialogCreator
void CTestDialogCreator::PrepareDialog(
	const UserPromptService::CPromptRequest& aRequest, const CPolicy& aPolicy,			
	const RPointerArray<CFingerprint>& aFingerprints, const CClientEntity* aClientEntity,
	const TAny* aEvalPrivateData, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iClientStatus = &aStatus;
	
	iRequest = &aRequest;
	iPolicy = &aPolicy;
	iFingerprints = &aFingerprints;
	iEvalPrivateData = aEvalPrivateData;
	(void) aClientEntity;

	iState = EInitialisePrepare;
	iStatus = KRequestPending;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}
	
void CTestDialogCreator::DisplayDialog(CPolicy::TOptions& aOptions, const CFingerprint*& aFingerprint,
		TUint& aEvaluatorInfo, TRequestStatus& aStatus)
	{	
	aStatus = KRequestPending;
	iClientStatus = &aStatus;
	
	iOptionSelected = &aOptions;
	iFingerprint = &aFingerprint;
	aFingerprint = 0;
	iEvaluatorInfo = &aEvaluatorInfo;
	iClientStatus = &aStatus;

	iState = EInitialiseDisplay;
	iStatus = KRequestPending;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}
