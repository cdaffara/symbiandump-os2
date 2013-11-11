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
* Implements CAuthoriser.	 See class and function definitions for
* more information.
*
*/


/**
 @file
*/

#include "upsserver.h"
#include "policycache.h"
#include "authoriser.h"
#include <ups/upsdbw.h>
#include "upslog.h"
#include <ups/dialogcreator.h>
#include <ups/policyevaluator.h>

namespace UserPromptService
{

CAuthoriser* CAuthoriser::NewL(RPolicyCacheCountedHandle &aPolicyCache,
								   CUpsSession* aSession, CUpsSubsession* aSubsession, TBool aServerCheckOk,
								   TThreadId &aClientTid, TProcessId &aClientPId,
								   const RMessage2& aMessage, const TServiceId& aServiceId,
								   RBuf &aDestination, RBuf8 &aOpaqueData)
	/**
	   Factory function allocates a new, initialized instance of CAuthoriser,
	   registered for the supplied session and subsession.
	   
	   If construction is successful, the caller should pass ownership to
	   the SCS framework (TransferToScsFrameworkL), and pop the
	   CAuthoriser from the cleanupstack. From this point onwards the
	   request should be completed by the CAuthoriser/CAsyncRequst code,
	   so the caller must not return or leave with any code except
	   KErrScsSetupAsync. This is why the StartProcessingRequest function
	   is defined as non-leaving.

	   @param	aPolicyCache	The server policy cache manager.
	   @param	aSession		Session on which this request was launched.
	   @param	aSubsession		Subsession on which this request was launched.
	   @param	aServerCheckOk	Did the system server checks pass?
	   @param	aClientThread	Handle to the user client thread.
	   @param	aMessage		Standard server-side handle to message.
	   @param	aDestination	We take ownership of the RBuf and close the callers handle
	   @param	aOpaqueData		We take ownership of the RBuf and close the callers handle
	   @return					New, initialized instance of CAuthoriser, which
	   is owned by the server's collection of outstanding
	   requests.
	*/
	{
	CAuthoriser* self = new(ELeave) CAuthoriser(aPolicyCache, aSession, aSubsession, aMessage);
	CleanupStack::PushL(self);
	self->ConstructL(aServerCheckOk, aClientTid, aClientPId, aServiceId, aDestination, aOpaqueData);
	CleanupStack::Pop(self);
	return self;
	}

CAuthoriser::CAuthoriser(RPolicyCacheCountedHandle &aPolicyCache,
						   CUpsSession* aSession, CUpsSubsession* aSubsession,
						   const RMessage2& aMessage)
	/**
	   This private constructor initializes the superclass and prevents direct instantiation.

	   @param	aPolicyCache		The server policy cache manager.
	   @param	aSession			Session on which this request was launched.
	   @param	aSubsession		Subsession on which this request was launched.
	   @param	aMessage		Standard server-side handle to message.
	   @param	aDestination	HBufC description, takes ownership.
	   @param	aOpaqueData		0 or HBufC opaqueData, takes ownership.
	*/
	:	CAsyncRequest(aSession, aSubsession, aMessage),
		iPolicyCache(aPolicyCache)
	{
	// empty.
	}

void CAuthoriser::ConstructL(TBool aServerCheckOk, TThreadId &aClientTid, TProcessId &aClientPid, const TServiceId& aServiceId, RBuf &aDestination, RBuf8 &aOpaqueData)
	/**
	   Second-phase constructor allocates the timer which is used to generate
	   a delay, and adds this object to the server's collection of outstanding
	   requests.
	*/
	{
	iState = ECheckPolicy;

	RProcess clientProcess;
	TInt r = clientProcess.Open(aClientPid);
	if(r != KErrNone)
		{
		User::Leave(KErrUpsBadClientProcessId);
		}
	CleanupClosePushL(clientProcess);
	
	// Create a CPromptReqest for the policy lookup
	iPromptRequest = CPromptRequest::NewL(clientProcess.SecureId(), // Client details
										  clientProcess.VendorId(),
										  aClientTid,
										  aClientPid,
										  iMessagePtr2.SecureId(), // Server SID
										  aServiceId, // Server service ID
										  aDestination, // Request detail, takes ownership
										  aOpaqueData,	//				   takes ownership
										  aServerCheckOk); 
	
	CleanupStack::PopAndDestroy(&clientProcess);
	}

void CAuthoriser::ProcessEventL(TAuthoriserEvent aEvent)
	{
	//	DEBUG_PRINTF4(_L8("0x%x CAuthoriser::ProcessEventL state %d, event %d\n"), this, iState, aEvent);
	_LIT(KServerPanicState, "UPS-ProcessEventL");
	switch(iState)
		{
		case ECheckPolicy:
			CheckPolicyStateL(aEvent);
			return;
		case ECreatingFingerprints:
			CreatingFingerprintsStateL(aEvent);
			return;
		case ECheckDatabase:
			CheckDatabaseStateL(aEvent);
			return;
		case EPreparingDialog:
			PreparingDialogStateL(aEvent);
			return;
		case EExecutingDialog:
			ExecutingDialogStateL(aEvent);
			return;
BULLSEYE_OFF
		}
	
	// Should not get here
	User::Panic(KServerPanicState, iState);
BULLSEYE_RESTORE
	}


void CAuthoriser::CheckPolicyStateL(TAuthoriserEvent aEvent)
	{
	switch(aEvent)
		{
		case EInternalRequestComplete:
			// Caused by the ups sub session code calling Wakeup
			// to start the request processing. We are now within the
			// active scheduler error handling framework.

			// This call will lookup the policy and move to the next
			// state, or leave.
			LookupPolicyL();
			break;

			BULLSEYE_OFF
		case EClearedToDisplayDialog:
			ASSERT(EFalse);
			break;
			BULLSEYE_RESTORE

		case ECancel:
			// This WILL happen if am authorise request is made and immediately cancelled (before CAuthoriser::Runl has
			// the chance to run). We should complete our "fake" internal request so we do not block
			// when we return to the DoCancel/CActive::Cancel which caused this event.
			CompleteSelf();
			break;
BULLSEYE_OFF
		}
BULLSEYE_RESTORE
	}

void CAuthoriser::CreatingFingerprintsStateL(TAuthoriserEvent aEvent)
	{
	switch(aEvent)
		{
		case EInternalRequestComplete:
			// This will move to the next state and lookup the fingerprints, or leave.
			LookupFingerprintsL();
			break;

			BULLSEYE_OFF
		case EClearedToDisplayDialog:
			ASSERT(EFalse);
			break;
			BULLSEYE_RESTORE

		case ECancel:
			// Cancel the request to create the fingerprints
			iPolicyEvaluator->Imp().Cancel();
			break;
BULLSEYE_OFF
		}
BULLSEYE_RESTORE
	}

void CAuthoriser::CheckDatabaseStateL(TAuthoriserEvent aEvent)
	{
	switch(aEvent)
		{
		case EInternalRequestComplete:
			// Normally the create fingerprints state code will call
			// LookupFingerprintsL to move to this state and do the
			// lookup, but if we need to re-lookup the fingerprints a
			// "fake" request will be issued and this code will be
			// used.
			LookupFingerprintsL();
			break;
		case EClearedToDisplayDialog:
			// Change state and ask the dialog creator to start creating
			// the dialog, or leave.
			PrepareDialogL();
			break;
		case ECancel:
			// Complete the fake request which was issued to try and
			// get us to redo the fingerprint lookup.
			CompleteSelf();
			break;
BULLSEYE_OFF
		}
BULLSEYE_RESTORE
	}

void CAuthoriser::PreparingDialogStateL(TAuthoriserEvent aEvent)
	{
	switch(aEvent)
		{
		case EInternalRequestComplete:
			// Created dialog, now change state to execute and display
			// it.
			ExecuteDialogL();
			break;

			BULLSEYE_OFF
		case EClearedToDisplayDialog:
			ASSERT(EFalse);
			break;
			BULLSEYE_RESTORE

		case ECancel:
			// Cancel the request to create the dialog
			iDialogCreator->Imp().Cancel();
			break;
BULLSEYE_OFF
		}
BULLSEYE_RESTORE
	}

void CAuthoriser::ExecutingDialogStateL(TAuthoriserEvent aEvent)
	{
	switch(aEvent)
		{
		case EInternalRequestComplete:
			// Dialog complete, process the result and complete the
			// client request.
			ProcessDialogResultL();
			break;

			BULLSEYE_OFF
		case EClearedToDisplayDialog:
			ASSERT(EFalse);
			break;
			BULLSEYE_RESTORE

		case ECancel:
			// Cancel the request to execute the dialog
			iDialogCreator->Imp().Cancel();
			break;
BULLSEYE_OFF
		}
BULLSEYE_RESTORE
	}

void CAuthoriser::Wakeup()
	/**
	   Raise and complete a request against self. Will cause RunL to
	   be run within the active scheduler.
	*/
	{
	iStatus = KRequestPending;
	SetActive();
	CompleteSelf();
	}

void CAuthoriser::CompleteSelf()
	{
	_LIT(KServerPanicState, "UPS-CompleteSelf");
	switch(iState)
		{
		case ECheckPolicy:
		case ECheckDatabase:
			break;

	BULLSEYE_OFF
		case ECreatingFingerprints:
		case EPreparingDialog:
		case EExecutingDialog:
			User::Panic(KServerPanicState, iState);
			/*lint -unreachable */
			return; // Not legal in these states
		}
	BULLSEYE_RESTORE
		
	if(iStatus == KRequestPending)
		{
		TRequestStatus *status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}



void CAuthoriser::ClearedToDisplayL()
	/**
	   The UPS server GateKeeper has granted us permission to proceed
	   with dialog generation and display.
	*/
	{
	ProcessEventL(EClearedToDisplayDialog);
	}






CAuthoriser::~CAuthoriser()
	/**
	   Close the timer which was initialized in ConstructL.
	*/
	{
	iDialogFingerprint = 0; // MUST not delete, owned by CDialogCreator
	
	delete iDialogCreator;
	iDialogCreator = 0;
	
	iDialogCreatorParams = 0; // MUST not delete, owned by evaluator
	
	iClientEntity = 0;	// MUST not delete, owned by evaluator
	
	iFingerprints.ResetAndDestroy();
	
	delete iPolicyEvaluator;
	iPolicyEvaluator = 0;	

	iPolicyCache.Release();
	iPolicy = 0; // MUST not delete, ownded by policy cache
	
	delete iPromptRequest;
	iPromptRequest = 0;
	}

void CAuthoriser::RunL()
	{
	// Wake up the next pending request now (if any), to make sure it
	// will be processed ahead of any incoming requests.
	UpsServer()->WakeupNextPendingL();
	
	// Reset our priority to standard
	if(Priority() != CActive::EPriorityStandard)
		{
		SetPriority(CActive::EPriorityStandard);
		}
	User::LeaveIfError(iStatus.Int());
	
	ProcessEventL(EInternalRequestComplete);
	}


void CAuthoriser::DoCleanup()
	{
	if(iPromptForced)
		{
		// Have previously called ForcePromptL on a database record and it told us to 
		// prompt,  so have previously marked the record as disputed.
		TRAP_IGNORE(UpsServer()->UnDisputeRecordIdL(iPromptForcedRecordId));
		iPromptForced = EFalse;
		}

	// Cancel current internal operation
	Cancel();
	
	// Let the server know we are done
	UpsServer()->AuthoriserDone(this);

	// The framework will complete the client request and schedule us
	// for deletion.
	}




void CAuthoriser::DoCancel()
	/**
	   Cancel the current operation.  This does not complete the client
	   request or mark this object for deletion.
	*/
	{
	TRAP_IGNORE(
		// Wake up the next pending request now (if any), to make sure it
		// will be processed ahead of any incoming requests.
		UpsServer()->WakeupNextPendingL();
		ProcessEventL(ECancel);
		);
	}

void CAuthoriser::LookupPolicyL()
	{
	// This returns a ptr into the cache, so we have to be careful not to use it after the cache is deleted.
	// This is why we wrap the cache in a RPolicyCacheCountedHandle.
	iPolicy = iPolicyCache->MatchL(*iPromptRequest);
	
	ASSERT(iPolicy);
	
	if(iPolicy->PromptRequired())
		{
		// Policy contains both yes and no responses...
		// Start creating the fingerprints
		CreateFingerprintsL();
		return;
		}
	
	// Return result to system server
	TUpsDecisionPckgBuf decisionBuf;
	decisionBuf() = MapCPolicyTOptions2TUpsDecision(iPolicy->Options());
	iMessagePtr2.WriteL(0, decisionBuf);
	
	CompleteAndMarkForDeletion(KErrNone);
	}


void CAuthoriser::CreateFingerprintsL()
	/**
	   Load Policy Evaluator and tell it to start generating the fingerprints
	*/
	{
	iState = ECreatingFingerprints;

	iPolicyEvaluator = UpsServer()->iPluginManager->CreateEvaluatorL(iPolicy->PolicyEvaluator());
	
	iPolicyEvaluator->Imp().GenerateFingerprints(*iPromptRequest, *iPolicy, 
												 iFingerprints, iClientEntity, iDialogCreatorParams, iStatus);
	
	SetActive();
	}

_LIT8(KDefaultFingerprint,"*");
//_LIT(KServerPanicDb, "UPS-BadDB");
static const char * const KCorruptDb = "UPS-BadDB error=%d line=%d\n";

void CAuthoriser::LookupFingerprintsL()
	{
	// Lookup in DB
	iState = ECheckDatabase;

	TPtrC8 clientEntityName(KNullDesC8());
	if(iClientEntity)
		{
		clientEntityName.Set(iClientEntity->Name());
		}
	
	CDecisionFilter *filter = CDecisionFilter::NewLC(iPromptRequest->ClientSid(),
													 iPolicy->PolicyEvaluator(),
													 iPromptRequest->ServiceId(),
													 iPromptRequest->ServerSid(),
													 KDefaultFingerprint(),
													 clientEntityName,
													 iPolicy->MajorVersion());
	//
	// Lookup the fingerprints in the DB and return the first match
	//
	CDecisionRecord *record = 0;
	TInt fingerprintCount = iFingerprints.Count();
	for(TInt i=0; i<fingerprintCount; ++i)
		{
		filter->SetFingerprintL(iFingerprints[i]->Fingerprint(), EEqual);
		record = 0;
		TRAPD(err, record = UpsServer()->iDbHandle->GetDecisionL(*filter));
		if(err != KErrNone)
			{
			HandleDbErrorL(err);
			}
		
		if(record)
			{
			break;
			}
		}
	
	if(record)
		{
		// Found record
		CleanupStack::PushL(record);
		// Read current eval info value from record.
		iDialogEvaluatorInfo = record->iEvaluatorInfo;

		if(UpsServer()->IsRecordIdDisputed(record->iRecordId))
			{
			// Matching record is under dispute so a dialog must be in progress.
			// Queue for later re-evaluation.
			UpsServer()->GateKeeperL(this);
			return;
			}

		if(iPromptForced && (record->iRecordId != iPromptForcedRecordId))
			{
			// Have previously called ForcePromptL on a database record and it told us to 
			// prompt, BUT we have now matched a different record ID! Presumably
			// someone else overwrote or deleted our record....
			//
			// We need to undispute the old record id and continue as if ForcePromptL had
			// never been called/returned yes. 
			iPromptForced = EFalse;
			UpsServer()->UnDisputeRecordIdL(iPromptForcedRecordId);
			}
		
		// Call ForcePrompt to see if we need to prompt anyway
		TUint newDialogEvaluatorInfo = iDialogEvaluatorInfo;
		TBool forcePrompt = iPolicyEvaluator->Imp().ForcePromptL(*record, newDialogEvaluatorInfo);
		if(forcePrompt)
			{
			iPromptForced = ETrue;
			iPromptForcedRecordId = record->iRecordId;
			UpsServer()->DisputeRecordIdL(iPromptForcedRecordId);
			}
		
		if(newDialogEvaluatorInfo != iDialogEvaluatorInfo)
			{
			// Eavluator info changes, update our member variable copy
			// so it is used in the dialog
			iDialogEvaluatorInfo = newDialogEvaluatorInfo;
			// If we ARE displaying a prompt, then when it completes,
			// we will delete the existing record. If the user choice
			// requires a new record we will use the evaluator value
			// from our member variable
			if(!forcePrompt)
				{
				// Not forcing a prompt, so update the DB now.
				record->iEvaluatorInfo = newDialogEvaluatorInfo;
				// If the update fails, we carry on anyway...
				TRAPD(err, UpsServer()->iDbHandle->UpdateDecisionL(*filter, *record));
				if(err != KErrNone)
					{
					HandleDbErrorL(err);
					}				
				}
			}
		
		if(!forcePrompt)
			{
			// Return the result found in the DB to the system server
			TUpsDecisionPckgBuf decisionBuf;
			decisionBuf() = MapCPolicyTOptions2TUpsDecision((record->iResult) ? (CPolicy::EAlways) : (CPolicy::ENever));
			iMessagePtr2.WriteL(0, decisionBuf);
			
			CleanupStack::PopAndDestroy(record);
			CleanupStack::PopAndDestroy(filter);
			CompleteAndMarkForDeletion(KErrNone);
			return;
			}
		
		// Prompt is being forced, so fall through
		CleanupStack::PopAndDestroy(record);
		}
	
	// Record not found, or prompt forced
	CleanupStack::PopAndDestroy(filter);
	
	// Queue for clearence to display a dialog
	UpsServer()->GateKeeperL(this);
	}

void CAuthoriser::PrepareDialogL()
	{
	iState = EPreparingDialog;
	
	iDialogCreator = UpsServer()->iPluginManager->CreateDialogCreatorL(iPolicy->DialogCreator());
	
	iDialogCreator->Imp().PrepareDialog(*iPromptRequest, *iPolicy, iFingerprints, 
										iClientEntity, iDialogCreatorParams, iStatus);
	SetActive();
	}




void CAuthoriser::ExecuteDialogL()
	{
	iState = EExecutingDialog;
	iDialogCreator->Imp().DisplayDialog(iDialogSelectedOption, iDialogFingerprint, iDialogEvaluatorInfo, iStatus);
	SetActive();
	}

void CAuthoriser::ProcessDialogResultL()
	{
	if(iPromptForced)
		{
		// Pormpt was forced therefore we must have matched an existing record in the
		// database.
		// Delete the old decision.
		CDecisionFilter *filter = CDecisionFilter::NewLC();
		filter->SetRecordId(iPromptForcedRecordId, EEqual);
		TRAPD(err, UpsServer()->iDbHandle->RemoveDecisionsL(*filter));
		CleanupStack::PopAndDestroy(filter);
		if(err != KErrNone)
			{
			HandleDbErrorL(err);
			}

		// No longer disputing the record ID because we have a new decision
		iPromptForced = EFalse;
		UpsServer()->UnDisputeRecordIdL(iPromptForcedRecordId);
		}
	
	TUpsDecisionPckgBuf decisionBuf;
	// Mask out any illegal responses - ie buttons which we did not
	// ask to be displayed...
	iDialogSelectedOption = CPolicy::TOptions(TUint(iDialogSelectedOption) & TUint(iPolicy->Options()));
	
	decisionBuf() = MapCPolicyTOptions2TUpsDecision(iDialogSelectedOption);
	
	if((iDialogSelectedOption & CPolicy::EAlways) || (iDialogSelectedOption & CPolicy::ENever))
		{

		__ASSERT_ALWAYS(NULL != iDialogFingerprint,
						User::Panic(KUpsServerName,
								KErrUpsBadFingerprintLength));	
	
		// Save yes/no result to database
		TPtrC8 clientEntityName(KNullDesC8());
		if(iClientEntity)
			{
			clientEntityName.Set(iClientEntity->Name());
			}
		
		CDecisionRecord *record = CDecisionRecord::NewLC(iPromptRequest->ClientSid(),
														 iPolicy->PolicyEvaluator(),
														 iPromptRequest->ServiceId(),
														 iPromptRequest->ServerSid(),
														 iDialogFingerprint->Fingerprint(),
														 clientEntityName,
														 iDialogFingerprint->Description(),
														 ((iDialogSelectedOption & CPolicy::EAlways) != 0),
														 iPolicy->MajorVersion(),
														 iDialogEvaluatorInfo);
		
		// Create new record - Failure is not fatal...
		TRAPD(err, UpsServer()->iDbHandle->CreateDecisionL(*record));
		if(err == KErrAlreadyExists)
			{
			/// You might think this will never happen, but under OOM conditions it can/does. The original database query can fail,
			/// even though there is a matching record in the database, we then choose to display a prompt, then the above code attempts to
			/// insert a "new" record and fails.... We recover by just updating the existing record.
			CDecisionFilter *filter = CDecisionFilter::NewLC(iPromptRequest->ClientSid(),
															 iPolicy->PolicyEvaluator(),
															 iPromptRequest->ServiceId(),
															 iPromptRequest->ServerSid(),
															 iDialogFingerprint->Fingerprint(),
													 		 clientEntityName,
													 		 iPolicy->MajorVersion());

			TRAP(err, UpsServer()->iDbHandle->UpdateDecisionL(*filter, *record));
			CleanupStack::PopAndDestroy(filter);
			}
		if(err != KErrNone)
			{
			HandleDbErrorL(err);
			}
		
		CleanupStack::PopAndDestroy(record);
		}

	iMessagePtr2.WriteL(0, decisionBuf);
	
	CompleteAndMarkForDeletion(KErrNone);
	}

TUpsDecision CAuthoriser::MapCPolicyTOptions2TUpsDecision(CPolicy::TOptions aOptions)
/**
	Map the specified policy option bitmap (normally only a single bit should be set) into 
	a TUpsDecision.

	If the server supports session decisions, then return never/always as EUpsDecSessionNo/EUpsDecSessionYes.

	Usually a single option should be specified, but we consider the options in the order which
	minimises UPS traffic...
*/
	{
	if( (aOptions & (CPolicy::ENever | CPolicy::ESessionNo)) != 0)
		{
		// A Never - If the session supports "session no" then return it to cut down on UPS traffic
		if(iPolicy->Options() & CPolicy::ESessionNo)
			{
			return EUpsDecSessionNo;
			}
		// Otherwise return "single shot no" so the server will re-query us later (we may silently answer).
		return EUpsDecNo;
		}

	if( (aOptions & (CPolicy::EAlways| CPolicy::ESessionYes)) != 0)
		{
		// An Always - If the session supports "session yes" then return it to cut down on UPS traffic
		if(iPolicy->Options() & CPolicy::ESessionYes)
			{
			return EUpsDecSessionYes;
			}
		// Otherwise return "single shot yes" so the server will re-query us later (so force prompt handling works).
		return EUpsDecYes;
		}

	if( (aOptions & CPolicy::ENo) != 0)
		{
		// A "single shot no"
		return EUpsDecNo;
		}

	// Only possibility left is a single shot yes
	// If it is not one, then map to no
	BULLSEYE_OFF
	if( (aOptions & CPolicy::EYes) == 0)
		{
		return EUpsDecNo; // No option set!
		}
	BULLSEYE_RESTORE

	// A "single shot yes"
	return EUpsDecYes;
	}

void CAuthoriser::HandleDbErrorL(TInt aError)
	{
	if((aError != KErrNoMemory) && (aError != KErrDiskFull))
		{
		RDebug::Printf(KCorruptDb, aError, __LINE__);
		UpsServer()->iDbHandle->DeleteDatabaseL(UpsServer()->iFs);
		UpsServer()->iDbHandle.Close(); // Will auto-reopen when next used
		User::Leave(aError);
		}
	UpsServer()->iDbHandle.Close(); // Will auto-reopen when next used
	}

CAuthoriserFifo* CAuthoriserFifo::NewL()
	/**
	   Create a new CAuthoriserFifo instance
	*/
	{
	CAuthoriserFifo *self = new(ELeave) CAuthoriserFifo;
	return self;
	}

CAuthoriserFifo::~CAuthoriserFifo()
	/**
	   Does not delete CAuthoriser objects in the FIFO, just frees
	   storage used by the FIFO itself.
	*/
	{
	iPtrArray.Close();
	}



void CAuthoriserFifo::PushL(CAuthoriser *aAuthoriser)
	/**
	   @param aAuthoriser The CAuthoriser to be pushed onto the FIFO.
	   
	   This class does NOT take ownership.
	*/
	{
	iPtrArray.AppendL(aAuthoriser);
	}

CAuthoriser *CAuthoriserFifo::PopL()
	{
	/**
	   @return A CAuthoriser ptr, or Leaves KErrUnderflow if FIFO is empty.
	*/
	TInt count = iPtrArray.Count();
	BULLSEYE_OFF
	if(count == 0)
		{
		User::Leave(KErrUnderflow);
		}
	BULLSEYE_RESTORE
	
	CAuthoriser *ret = iPtrArray[0];
	iPtrArray.Remove(0);
	return ret;
	}

void CAuthoriserFifo::RemoveL(CAuthoriser *aAuthoriser)
	/**
	   Remove the specified object from the FIFO.
	   
	   It is not considered to be an error if the object is not in the
	   FIFO.
	   
	   @param aAuthoriser The CAuthoriser to be removed from the FIFO.
	*/
	{
	TInt i = iPtrArray.Find(aAuthoriser);
	BULLSEYE_OFF
	if( i == KErrNotFound)
		{
		return; // Not found is not considered an error.
		}
	BULLSEYE_RESTORE
	
	User::LeaveIfError(i);
	iPtrArray.Remove(i);
	}


TBool CAuthoriserFifo::IsEmpty() const
	/**
		@return ETrue if empty
	 */
	{
	return (iPtrArray.Count() == 0);
	}

void CAuthoriserFifo::Compress()
	/**
	Reduce memory usage as much as possible. Typically used during OOM testing.
	*/
{
	iPtrArray.Compress();
}




} // End of namespace UserPromptService
// End of file
