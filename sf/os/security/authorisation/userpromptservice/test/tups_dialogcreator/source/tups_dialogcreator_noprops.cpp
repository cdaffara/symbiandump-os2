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


#include "tups_dialogcreator_noprops.h"
#include "tups_dialogcreator_defs.h"
#include <ecom/implementationproxy.h>
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
	delete iPromptData;
	iPromptDataDes.Close();
	iNotifier.Close();
	}

void CTestDialogCreator::ConstructL()
/**
Second phase constructor
*/
	{
	User::LeaveIfError(iNotifier.Connect());
	}

void CTestDialogCreator::DoCancel()
	{
	if (iState == EProcessResult)
		{
		iNotifier.CancelNotifier(TUid::Uid(KTestNotifierImplementationId));
		}
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, KErrCancel);
		}
	}
	
TInt CTestDialogCreator::RunError(TInt aError)
	{
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, aError);
		}
	return KErrNone;
	}

void CTestDialogCreator::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	switch (iState)
		{
		case EPrepareDialog:
			DoPrepareDialogL();
			break;
		case EDisplayDialog:
			DoDisplayDialogL();
			break;
		case EProcessResult:
			DoProcessResultL();
			break;
		default:
			ASSERT(EFalse);			
		}
	}
	
void CTestDialogCreator::DoPrepareDialogL()
	{
	iPromptData = CPromptData::NewL();

	// Only one state at the moment but more should be
	// added for long running operators e.g. querying the SIS registry
	// or resolving the client entity.
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
	
	TRequestStatus* status = &iStatus;

	iPromptResult.iDestination = KErrNotFound;
	
	/**
	The switch decision below determines the dialog result based on the instance number 
	of the Dialog Creator, so that a static response to each prompt can be given. If the
	instance number (hard coded) does not fall within the given range, a dialog is displayed
	by calling a notifier instead
	*/
	switch(KDialogCreatorInstance)
		{
		case 4:
			iPromptResult.iSelected = CPolicy::ESessionYes;
			iStatus = KRequestPending;
			User::RequestComplete(status, KErrNone);
			break;
		case 5:
			iPromptResult.iSelected = CPolicy::EAlways;
			iPromptResult.iDestination = 0;
			iStatus = KRequestPending;
			User::RequestComplete(status, KErrNone);
			break;
		case 6:
			iPromptResult.iSelected = CPolicy::ENever;
			iPromptResult.iDestination = 0;
			iStatus = KRequestPending;
			User::RequestComplete(status, KErrNone);
			break;
		default:
				iNotifier.StartNotifierAndGetResponse(iStatus, TUid::Uid(KTestNotifierImplementationId), iPromptDataDes, iPromptResultPckg);
		}
	
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
			ASSERT(EFalse);	// should never happen
			}
		}			
	*iOptionSelected =	iPromptResult.iSelected;

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
	RThread clientThread;
	User::LeaveIfError(clientThread.Open(iRequest->ClientThreadId()));
	CleanupClosePushL(clientThread);
		
	RProcess clientProcess;
	User::LeaveIfError(clientThread.Process(clientProcess));
	CleanupClosePushL(clientProcess);
	iPromptData->iClientName.Create(clientProcess.FileName());		
	CleanupStack::PopAndDestroy(2, &clientThread); // clientThread, clientProcess		
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

	// Kick off dialog creator state machine
	iState = EPrepareDialog;
	iStatus = KRequestPending;
	TRequestStatus* status = &iStatus;
	SetActive();
	User::RequestComplete(status, KErrNone);
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
	
	// Start state machine
	ASSERT(iState == EDisplayDialog); // PrepareDialog should have been called first
	iStatus = KRequestPending;
	TRequestStatus* status = &iStatus;
	SetActive();
	User::RequestComplete(status, KErrNone);
	}

