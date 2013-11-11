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


#include "tupsdbmanagementstep.h"  
#include "tupsintegdefs.h"
#include <ups/upstypes.h>

using namespace UserPromptService;

_LIT(KOperationCreateView, "CreateView");
_LIT(KOperationCancelCloseView, "CancelCloseView");
_LIT(KOperationNextMatch, "NextMatch");
_LIT(KOperationRemoveDecision, "RemoveDecision");
_LIT(KOperationUpdateDecisionAllow, "UpdateDecisionAllow");
_LIT(KOperationUpdateDecisionDeny, "UpdateDecisionDeny");
_LIT(KOperationCancelUpdate, "CancelUpdate");
_LIT(KOperationDeleteDatabase, "DeleteDatabase");
_LIT(KOperationDecisionCount, "DecisionCount");


CUpsDbRequest* CUpsDbRequest::NewL()
	{
	CUpsDbRequest* me = new (ELeave) CUpsDbRequest();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;	
	}
	
CUpsDbRequest::~CUpsDbRequest()
	{
	delete iDecisionFilter;	
	}

CUpsDbRequest::CUpsDbRequest() 
	:iOperation(0), iClientSid(0), iEvaluatorId(0), iServiceId(0), iServerSid(0), iFingerprint(0), 
	iClientEntity(0), iDescription(0), iDecisionResult(0), iMajorPolicyVersion(0), iRecordId(0), iEvaluatorInfo(0), iExpectedDecisionCount(0)
	{	
	} 
	
void CUpsDbRequest::ConstructL()
	{
	iDecisionFilter = CDecisionFilter::NewL();	
	}
	
CUPSDbManagementStep::CUPSDbManagementStep()
	{
	SetTestStepName(KUPSDbManagementStep);
	}

CUPSDbManagementStep::~CUPSDbManagementStep()
	{
	iUpsManager.CancelAndCloseView();
	iUpsManager.Close();	
	iArraySersToRequest.ResetAndDestroy();
	INFO_PRINTF2(_L("END CELLS: %d"), User::CountAllocCells());  
	__UHEAP_MARKEND;
	}
	
TVerdict CUPSDbManagementStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
 	__UHEAP_MARK;
 	
 	INFO_PRINTF2(_L("START CELLS: %d"), User::CountAllocCells());
 	
 	// reads client name and SID
	TParse clientFullName;
	RThread client;
	clientFullName.Set(client.FullName(),NULL, NULL);
	iTEFServerName=clientFullName.Name();
	iExpectedClientSid = client.SecureId() ;
	client.Close();
 	
    // Read how many times the test step needs to be repeated.
    TName fStepRepeat(_L("StepRepeat"));
    TInt  repeats;
    
    if(GetIntFromConfig(ConfigSection(),fStepRepeat,repeats))
    	{
    	iStepRepeat=repeats;
    	}
	else
		{	
		iStepRepeat=1;		
		}
    
    // Read values for test sequence from INI file. (ARRAY of values)
	TInt index=0;
	TName fOperation;
	fOperation.Format(_L("Operation_%d"), index);
	TName fClientSid;
	fClientSid.Format(_L("ClientSid_%d"),index);
	TName fEvaluatorId;
	fEvaluatorId.Format(_L("EvaluatorId_%d"),index);
	TName fServiceId;
	fServiceId.Format(_L("ServiceId_%d"),index);
	TName fServerSid;
	fServerSid.Format(_L("ServerSid_%d"),index);
	TName fFingerprint;
	fFingerprint.Format(_L("Fingerprint_%d"),index);
	TName fClientEntity;
	fClientEntity.Format(_L("ClientEntity_%d"),index);
	TName fDescription;
	fDescription.Format(_L("Description_%d"),index);
	TName fDecisionResult;
	fDecisionResult.Format(_L("DecisionResult_%d"),index);
	TName fMajorPolicyVersion;
	fMajorPolicyVersion.Format(_L("MajorPolicyVersion_%d"),index);
	TName fRecordId;
	fRecordId.Format(_L("RecordId_%d"),index);
	TName fEvaluatorInfo;
	fEvaluatorInfo.Format(_L("EvaluatorInfo_%d"),index);
	TName fExpectedDecisionCount;
	fExpectedDecisionCount.Format(_L("ExpectedDecisionCount_%d"),index);
	
	TPtrC 	operation;
	TInt	clientSid;
	TInt	evaluatorId;
	TInt	serviceId;
	TInt	serverSid;
	TPtrC	fingerprint;
	TPtrC	clientEntity;
	TPtrC	description;
	TPtrC	decisionResult;
	TInt	majorPolicyVersion;
	TInt 	recordId;
	TInt	evaluatorInfo;
	TInt 	expectedDecisionCount;
	
	while (GetStringFromConfig(ConfigSection(),fOperation,operation))
		{
		
		// Create an instance of a new request
		CUpsDbRequest* newRequest = CUpsDbRequest::NewL();
		CleanupStack::PushL(newRequest);
		
		// Set the operation to be performed
	    newRequest->iOperation = operation;
	    
	    if(GetHexFromConfig(ConfigSection(),fClientSid,clientSid))
		    {
		    newRequest->iClientSid = clientSid;
		  	newRequest->iDecisionFilter->SetClientSid(TSecureId(clientSid),EEqual);
		    }
	    
		if(GetHexFromConfig(ConfigSection(),fEvaluatorId,evaluatorId))
			{
			newRequest->iEvaluatorId = evaluatorId;	
			newRequest->iDecisionFilter->SetEvaluatorId(TUid::Uid(evaluatorId),EEqual);
			}
		
		if(GetHexFromConfig(ConfigSection(),fServiceId,serviceId))
			{
			newRequest->iServiceId = serviceId;	
			newRequest->iDecisionFilter->SetServiceId(TUid::Uid(serviceId),EEqual);
			}
	
		if(GetHexFromConfig(ConfigSection(),fServerSid,serverSid))
			{
			newRequest->iServerSid = serverSid;	
			newRequest->iDecisionFilter->SetServerSid(TSecureId(serverSid),EEqual);
			}
			
		if(GetStringFromConfig(ConfigSection(),fFingerprint,fingerprint))
			{
			HBufC8* fingerprintConverter = HBufC8::NewLC(fingerprint.Length());
			TPtr8 fingerprintPtr = fingerprintConverter->Des();
			fingerprintPtr.Copy(fingerprint); 	
             
	       	newRequest->iFingerprint = fingerprintPtr;
	       	
	       	HBufC8* binaryFingerprint = StringToBinaryLC(fingerprintPtr);
	       	
	      	newRequest->iDecisionFilter->SetFingerprintL(*binaryFingerprint,EEqual);	
	       	CleanupStack::PopAndDestroy(binaryFingerprint);
	       	CleanupStack::PopAndDestroy(fingerprintConverter); 
			}

		if(GetStringFromConfig(ConfigSection(),fClientEntity,clientEntity))
			{
			HBufC8* clientEntityConverter = HBufC8::NewLC(clientEntity.Length());
			TPtr8 clientEntityPtr = clientEntityConverter->Des();
			clientEntityPtr.Copy(clientEntity); 	
             
	       	newRequest->iClientEntity = clientEntityPtr;
	       	newRequest->iDecisionFilter->SetClientEntityL(clientEntityPtr,EEqual);		
	       	CleanupStack::PopAndDestroy(clientEntityConverter); 
			}
			
		if(GetStringFromConfig(ConfigSection(),fDescription,description))
			{
		    newRequest->iDescription = description;
			}
			
		if(GetStringFromConfig(ConfigSection(),fDecisionResult,decisionResult))
			{
			if(decisionResult.CompareF(_L("Yes"))==0 || decisionResult.CompareF(_L("No"))==0 || decisionResult.CompareF(_L(""))==0 )
				{
				newRequest->iDecisionResult = decisionResult;	
				}
			else 
				{
				ERR_PRINTF3(_L("%S: Unexpected Decision Result - %S"),&iTEFServerName, &decisionResult);
				}
			}

		if(GetIntFromConfig(ConfigSection(),fMajorPolicyVersion,majorPolicyVersion)) 
			{
			newRequest->iMajorPolicyVersion = majorPolicyVersion;
			newRequest->iDecisionFilter->SetMajorPolicyVersion(majorPolicyVersion,EEqual);		
			}
		
		if(GetIntFromConfig(ConfigSection(),fRecordId,recordId))
			{
			newRequest->iRecordId = recordId;
			newRequest->iDecisionFilter->SetRecordId(recordId,EEqual);		
			}
			
		if(GetIntFromConfig(ConfigSection(),fEvaluatorInfo,evaluatorInfo))
			{
			newRequest->iEvaluatorInfo = evaluatorInfo;		
			}
			
		if(GetIntFromConfig(ConfigSection(),fExpectedDecisionCount,expectedDecisionCount))
			{
			newRequest->iExpectedDecisionCount = expectedDecisionCount;		
			}
	    
	    // Add the new service to be requested to array.
	    iArraySersToRequest.AppendL(newRequest);
	    
	    CleanupStack::Pop(newRequest);
	       
	    index++;
	    fOperation.Format(_L("Operation_%d"), index);
	    fClientSid.Format(_L("ClientSid_%d"),index);
		fEvaluatorId.Format(_L("EvaluatorId_%d"),index);
		fServiceId.Format(_L("ServiceId_%d"),index);
		fServerSid.Format(_L("ServerSid_%d"),index);
		fFingerprint.Format(_L("Fingerprint_%d"),index);
		fClientEntity.Format(_L("ClientEntity_%d"),index);
		fDescription.Format(_L("Description_%d"),index);
		fDecisionResult.Format(_L("DecisionResult_%d"),index);
		fMajorPolicyVersion.Format(_L("MajorPolicyVersion_%d"),index);
		fRecordId.Format(_L("RecordId_%d"),index);
		fEvaluatorInfo.Format(_L("EvaluatorInfo_%d"),index);
		fExpectedDecisionCount.Format(_L("ExpectedDecisionCount_%d"),index);
		}
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CUPSDbManagementStep::doTestStepL()
	{
	// Connect to the UPS Manager
	User::LeaveIfError(iUpsManager.Connect());
	
	// loop to request for different test services.
	SetTestStepResult(EPass);
	TInt numServicesToRequest = iArraySersToRequest.Count();

	if(numServicesToRequest > 0) 
		{
		// repeats the sequence of test services request as many times as specified in iStepRepeat
		for(TInt repeatTimes = 0;repeatTimes<iStepRepeat;++repeatTimes)
			{
			
			INFO_PRINTF3(_L("%S: Repeat number: %d"),&iTEFServerName,repeatTimes);
			
			for(TInt index=0;index < numServicesToRequest; ++index )
				{
				
 			    INFO_PRINTF4(_L("%S: Database Request: %d, Command: %S"),&iTEFServerName, index, &iArraySersToRequest[index]->iOperation);	
 			    
 			    TRequestStatus requestStatus;	    
    			
 			    TPtrC op(iArraySersToRequest[index]->iOperation);
 			    
 			    // *** CREATE VIEW ***
 			    if (!op.CompareF(KOperationCreateView))
 			    	{
 					// Create view using the Decision Filter for the Request
 			    	iUpsManager.CreateView(*iArraySersToRequest[index]->iDecisionFilter,requestStatus);
 			    	
 			    	User::WaitForRequest(requestStatus);
 			    	
 			    	if(requestStatus != KErrNone)
	 			    	{
	 			    	SetTestStepResult(EFail);
	 			    	ERR_PRINTF3(_L("%S: Create View Failure - %d"),&iTEFServerName,requestStatus.Int());	
	 			    	User::Leave(requestStatus.Int());	
	 			    	}
 			    	}
 			    // *** CANCEL CLOSE VIEW ***
 			    else if (!op.CompareF(KOperationCancelCloseView))
 			    	{
 			    	iUpsManager.CancelAndCloseView();
 			    	
 			    	INFO_PRINTF2(_L("%S: UPS DB Management - Current View Cancelled and Closed"),&iTEFServerName);
 			    	}
 			    // *** NEXT MATCH ***	
 			    else if (!op.CompareF(KOperationNextMatch))
 			    	{
 			    	CDecisionRecord* record = NULL;
 			    	
 			    	// Retrieve the Next Decision according to the Current View
 			    	record = iUpsManager.NextMatchL();
 			    	CleanupStack::PushL(record);
 			    	
 			    	// If a record is found, check that the record matches any data specified in the INI
 			    	if(record != NULL)
	 			    	{
	 			    	if(CheckDecisionRecordL(index,*record) == EFalse)
		 			    	{
		 			    	iUpsManager.CancelAndCloseView();
		 			    	ERR_PRINTF2(_L("%S: Incorrect Results"),&iTEFServerName);
							User::Leave(KErrGeneral);	
		 			    	}	 			    		
	 			    	}
	 			    else
	 			    	{
	 			    	iUpsManager.CancelAndCloseView();
	 			    	SetTestStepResult(EFail);
		    			ERR_PRINTF2(_L("%S: NextMatchL - No Decision Retrieved"),&iTEFServerName);
						User::Leave(KErrEof);	
	 			    	}
	 			    	
	 			    CleanupStack::PopAndDestroy(record);
 			    	
 			    	}
 			    // *** REMOVE DECISION ***
 			    else if (!op.CompareF(KOperationRemoveDecision))
 			    	{
 			    	// Remove decisions from the database based on the contents of the Decision Filter
 			    	TRAPD(err, iUpsManager.RemoveDecisionsL(*iArraySersToRequest[index]->iDecisionFilter));
 			    	
 			    	if(err == KErrNone)
	 			    	{
	 			    	INFO_PRINTF2(_L("%S: Remove Decision - Decision(s) Successfully Removed"),&iTEFServerName);
	 			    	}
	 			    else
	 			    	{
	 			    	SetTestStepResult(EFail);
	 			    	ERR_PRINTF3(_L("%S: ** Error ** Remove Decision Failure - %d"),&iTEFServerName,err);
	 			    	User::Leave(err);	
	 			    	}
 			    	}
 			    // *** UPDATE DECISION ALLOW ***
 			    else if (!op.CompareF(KOperationUpdateDecisionAllow))
 			    	{
 			    	iUpsManager.UpdateDecision(iArraySersToRequest[index]->iRecordId, ETrue, requestStatus);
 			    	
 			    	User::WaitForRequest(requestStatus);
 			    	
 			    	if(requestStatus != KErrNone)
	 			    	{
	 			    	SetTestStepResult(EFail);
	 			    	ERR_PRINTF3(_L("%S: Update Decision 'Allow' Failure - %d"),&iTEFServerName,requestStatus.Int());	
	 			    	User::Leave(requestStatus.Int());	
	 			    	}
 			    	}
 			    // *** UPDATE DECISION DENY ***
 			    else if (!op.CompareF(KOperationUpdateDecisionDeny))
 			    	{
 			    	iUpsManager.UpdateDecision(iArraySersToRequest[index]->iRecordId, EFalse, requestStatus);
 			    	
 			    	User::WaitForRequest(requestStatus);
 			    	
 			    	if(requestStatus != KErrNone)
	 			    	{
	 			    	SetTestStepResult(EFail);
	 			    	ERR_PRINTF3(_L("%S: Update Decision 'Deny' Failure - %d"),&iTEFServerName,requestStatus.Int());	
	 			    	User::Leave(requestStatus.Int());	
	 			    	}
 			    	}
 			    // *** CANCEL DECISION UPDATE ***
 			    else if (!op.CompareF(KOperationCancelUpdate))
 			    	{
 			    	iUpsManager.CancelUpdateDecision();
 			    	
 			    	INFO_PRINTF2(_L("%S: UPS DB Management - Current Decision Update Cancelled"),&iTEFServerName);
 			    	}
 			    // *** DELETE DATABASE ***
 			    else if (!op.CompareF(KOperationDeleteDatabase))
 			    	{
 			    	// Delete the contents of the Decision Database
 			    	TRAPD(err, iUpsManager.DeleteDatabaseL());
 			    	
 			    	if(err == KErrNone)
	 			    	{
	 			    	INFO_PRINTF2(_L("%S: UPS DB Management - Database Deleted"),&iTEFServerName);
	 			    	}
	 			    else
	 			    	{
	 			    	SetTestStepResult(EFail);
	 			    	ERR_PRINTF3(_L("%S: ** Error ** Database Deletion Failure - %d"),&iTEFServerName,err);
	 			    	User::Leave(err);	
	 			    	}
 			    	}
 			    // *** DECISION COUNT ***
 			    else if (!op.CompareF(KOperationDecisionCount))
 			    	{
 			    	iUpsManager.CancelAndCloseView();
 			    	
 			    	// Create a new view using the constructed decision filter for the request
 			    	iUpsManager.CreateView(*iArraySersToRequest[index]->iDecisionFilter,requestStatus);
 			    	
 			    	User::WaitForRequest(requestStatus);
 			    	
 			    	if(requestStatus.Int() == KErrNone)
	 			    	{
	 			    	TInt decisionCount = 0;
	 			    	CDecisionRecord* record = NULL;
	 			    	
	 			    	TInt result = KErrNone;
	 			    	
	 			    	// Whilst records located using the "NextMatchL" method, increment the record count
						while(result == KErrNone)
							{
							TRAP(result, record = iUpsManager.NextMatchL());
							
							if(record == NULL)
								{
								break;
								}
							else
								{
								++decisionCount;	
								}
							
							delete record;
							record = NULL;
							}
														
						iUpsManager.CancelAndCloseView();
							
						if(decisionCount != iArraySersToRequest[index]->iExpectedDecisionCount)
		    				{
		    				SetTestStepResult(EFail);
		    				ERR_PRINTF2(_L("%S: Decision Count - Unexpected Number of Database Decisions"),&iTEFServerName);
		    				ERR_PRINTF3(_L("%S: Decisions expected: %d"),&iTEFServerName,iArraySersToRequest[index]->iExpectedDecisionCount);
		    				ERR_PRINTF3(_L("%S: Decisions in Database: %d"),&iTEFServerName,decisionCount);
							User::Leave(KErrNotFound);
		    				}
		    						    				
	 			    	}
	 			    else
	 			    	{
	 			    	SetTestStepResult(EFail);
	 			    	ERR_PRINTF3(_L("%S: Decision Count - Create View Failure - %d"),&iTEFServerName,requestStatus.Int());	
	 			    	User::Leave(requestStatus.Int());
	 			    	}
 			    	}	
 			    else
 			    	{
 			    	SetTestStepResult(EFail);
		     	    ERR_PRINTF2(_L("%S: Invalid operation"),&iTEFServerName);
					User::Leave(KErrNotSupported);	 			    	
 			    	}
	    	
				}  // End of second loop
			}// End of first loop
		
		} 
	else
		{
		SetTestStepResult(EFail);
		ERR_PRINTF2(_L("%S: Problem reading ini file"),&iTEFServerName);
		User::Leave(KErrGeneral);	
		}  
		
	return TestStepResult();
	}  // End of function


TVerdict CUPSDbManagementStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
	
	
TBool CUPSDbManagementStep::CheckDecisionRecordL(TInt aIndex, const CDecisionRecord& aRecord)
/**
 * Checks the expected record values against those stored within CDecisionRecord retrieved from the database
 *
 * @param  - TInt aIndex: Database request index used to access the required instance of CUpsDbRequest held 
 * 			 within the request array
 * @param  - const CDecisionRecord& aRecord: Decision Record retrieved from the database
 * @return - TBool: Boolean to determine the result of the value checks (ETrue = Pass, EFalse = Fail)
 **/
	{
	
	TBool checkPassed = ETrue;
		

	if(iArraySersToRequest[aIndex]->iClientSid != aRecord.iClientSid)
		{
		ERR_PRINTF2(_L("%S: 'Client SID' value mismatch"),&iTEFServerName);
		checkPassed = EFalse;	
		}

	if(iArraySersToRequest[aIndex]->iEvaluatorId != aRecord.iEvaluatorId.iUid)
		{
		ERR_PRINTF2(_L("%S: 'Evaluator ID' value mismatch"),&iTEFServerName);
		checkPassed = EFalse;	
		}	

	if(iArraySersToRequest[aIndex]->iServiceId != aRecord.iServiceId.iUid)
		{
		ERR_PRINTF2(_L("%S: 'Service ID' value mismatch"),&iTEFServerName);
		checkPassed = EFalse;	
		}	

	if(iArraySersToRequest[aIndex]->iServerSid != aRecord.iServerSid)
		{
		ERR_PRINTF2(_L("%S: 'Server SID' value mismatch"),&iTEFServerName);
		checkPassed = EFalse;	
		}	

	HBufC8* recordFingerprint = StringToHexLC(aRecord.iFingerprint);
	
	if(iArraySersToRequest[aIndex]->iFingerprint != *recordFingerprint)
		{
		HBufC* expectedFingerprint = HBufC::NewLC(iArraySersToRequest[aIndex]->iFingerprint.Length());
		HBufC* receivedFingerprint = HBufC::NewLC(recordFingerprint->Length());
		
		expectedFingerprint->Des().Copy(iArraySersToRequest[aIndex]->iFingerprint);
		receivedFingerprint->Des().Copy(*recordFingerprint);
		
		ERR_PRINTF2(_L("%S: 'Fingerprint' value mismatch."),&iTEFServerName);
		ERR_PRINTF3(_L("%S: Fingerprint expected: %S"),&iTEFServerName,expectedFingerprint);
    	ERR_PRINTF3(_L("%S: Fingerprint received: %S"),&iTEFServerName,receivedFingerprint);
		
		CleanupStack::PopAndDestroy(receivedFingerprint);
		CleanupStack::PopAndDestroy(expectedFingerprint);
		checkPassed = EFalse;	
		}
		
	CleanupStack::PopAndDestroy(recordFingerprint);	

	if(iArraySersToRequest[aIndex]->iClientEntity != aRecord.iClientEntity)
		{
		HBufC* expectedClientEntity = HBufC::NewLC(iArraySersToRequest[aIndex]->iClientEntity.Length());
		HBufC* receivedClientEntity = HBufC::NewLC(aRecord.iClientEntity.Length());
		
		expectedClientEntity->Des().Copy(iArraySersToRequest[aIndex]->iClientEntity);
		receivedClientEntity->Des().Copy(aRecord.iClientEntity);
		
		ERR_PRINTF2(_L("%S: 'Client Entity' value mismatch."),&iTEFServerName);
		
		CleanupStack::PopAndDestroy(receivedClientEntity);
		CleanupStack::PopAndDestroy(expectedClientEntity);
		checkPassed = EFalse;
		}	

	if(iArraySersToRequest[aIndex]->iDescription.CompareF(aRecord.iDescription) != 0)
		{
		TPtrC retrievedDescription(aRecord.iDescription);
		ERR_PRINTF2(_L("%S: 'Description' value mismatch."),&iTEFServerName);
    	checkPassed = EFalse;
		}
	
	TInt expectedDecisionResult = 0;
	
	if(iArraySersToRequest[aIndex]->iDecisionResult.CompareF(_L("Yes")) == 0)
		{
		expectedDecisionResult = 1;	
		}
	
	if(expectedDecisionResult != aRecord.iResult)
		{
		ERR_PRINTF2(_L("%S: 'Decision Result' value mismatch."),&iTEFServerName);
		checkPassed = EFalse;
		}

	if(iArraySersToRequest[aIndex]->iMajorPolicyVersion != aRecord.iMajorPolicyVersion)
		{
		ERR_PRINTF2(_L("%S: 'Major Policy Version' value mismatch."),&iTEFServerName);
		checkPassed = EFalse;	
		}	

	if(iArraySersToRequest[aIndex]->iEvaluatorInfo != aRecord.iEvaluatorInfo)
		{
		ERR_PRINTF2(_L("%S: 'Evaluator Info' value mismatch."),&iTEFServerName);
		checkPassed = EFalse;	
		}	

	if(iArraySersToRequest[aIndex]->iRecordId != aRecord.iRecordId)
		{
		ERR_PRINTF2(_L("%S: 'Record ID' value mismatch."),&iTEFServerName);
		checkPassed = EFalse;	
		}	
	
	return checkPassed;
	}
	

	
HBufC8* CUPSDbManagementStep::StringToHexLC(const TDes8 &aString)
/**
 * Function to convert the contents of a TDes8 into a Hexadecimal format
 *
 * @param  - cosnt TDes8 aString: String to convert into Hexadecimal
 * @return - HBufC8*: Converted Hexadecial string representation
 **/
	{
	HBufC8* parsedString = HBufC8::NewLC(aString.Length()*2);
	TBuf8<2> pair;
	_LIT8(KFormatHex,"%02x"); 
		
	TPtr8 ptr(parsedString->Des());
   	for(TInt i = 0; i<aString.Length() ; i++)
    	{
		TInt num  = (aString)[i];
    	pair.Format(KFormatHex,num);
    	ptr.Append(pair);
    	}
	    
	return parsedString;
	}
	
	
HBufC8* CUPSDbManagementStep::StringToBinaryLC(const TDes8 &aString)
/**
 * Function to convert the contents of a TDes8 into a Binary format
 *
 * @param  - cosnt TDes8 aString: String to convert into Hexadecimal
 * @return - HBufC8*: Converted Binary string representation
 **/
	{
	HBufC8* parsedString = HBufC8::NewLC(aString.Length()/2);
	TBuf8<1> binChar;
	_LIT8(KFormatBinary,"%c"); 
		
	TPtr8 ptr(parsedString->Des());
   	for(TInt i = 0; i<aString.Length()/2 ; i++)
    	{
    	TPtrC8 tempPtr(aString.Mid(i*2,2));
    	TLex8 lex(tempPtr);
    	TUint val=0;
    	lex.Val(val, EHex);
    	binChar.Format(KFormatBinary,val);
    	ptr.Append(binChar);
    	}
	    
	return parsedString;
	}
