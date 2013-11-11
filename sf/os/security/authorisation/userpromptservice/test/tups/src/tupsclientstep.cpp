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



#include "tupsclientstep.h"  
#include "tupsintegdefs.h"
#include "tupsproperty.h"
#include <e32property.h>


TServiceRequestParameters::TServiceRequestParameters() 
	:iServiceUID(0), iServerName(0),iDestination(0), iExpectedError(0),
	 iOpaqueData(0), iSelectDialogOption(0), iButtonsDisplayed(0),
	 iDialogCreatorInvoked(0), iPolicyEvaluatorInvoked(0), iAccessGranted(0),
	 iCloseSession(EFalse), iHoldEvaluatorOpen(EFalse), iHoldPrepareDialogOpen(EFalse), iHoldDisplayDialogOpen(EFalse),
	 iRequestDurationThreshold(0), iLeaveDialog(EFalse), iLeaveEvaluator(EFalse), iCancelUpsRequest(EFalse),
	 iPlatSecPass(EFalse),iForcePrompt(EFalse),iExpectedEvaluatorInfo(0),iSelectFingerprint(0),
	 iWaitUntilFileAppears(0)	 	 	 	 
	{	
	};   

COpenSession* COpenSession::NewL(const TPtrC& aServerName)
	{
	COpenSession * self = new (ELeave) COpenSession(); 
	CleanupStack::PushL(self);
	self->ConstructL(aServerName);
	CleanupStack::Pop(self);
	return(self);
	} // End of function
	
void COpenSession::ConstructL(const TPtrC& aServerName)
	{
	iServerName = aServerName;
	iPointerToServer = new RUpsTestServ;
	User::LeaveIfError(iPointerToServer->Connect(iServerName, ETrue));
    iPointerToSession = new RUpsTestSession;
	User::LeaveIfError(iPointerToSession->Open(*iPointerToServer));
	} // End of function
	
COpenSession::~COpenSession() 
	{	
	if (iPointerToSession)
		iPointerToSession->Close();
	if (iPointerToServer)
		iPointerToServer->Close();
	delete iPointerToSession;
    delete iPointerToServer;
    }  // End of function

CUpsClientStep::~CUpsClientStep()
/**
 * Destructor
 */
	{
	iArraySersToRequest.Close();   
  
	CloseAllOpenSessions();
	delete iPropertyReader;
  	__UHEAP_MARKEND;
	} // End of function


CUpsClientStep::CUpsClientStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KUPSClientStep);
	} // End of function


TVerdict CUpsClientStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	
 	__UHEAP_MARK;
 	
	// Read values to config servers from INI file. (ARRAY of values)
 	
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
	TName fUseServiceUID;
	fUseServiceUID.Format(_L("UseServiceUID_%d"),index);
	TName fUseServerName;
	fUseServerName.Format(_L("UseServerName_%d"),index);
	TName fDestination;
	fDestination.Format(_L("Destination_%d"),index);
	TName fExpectedError;
	fExpectedError.Format(_L("ExpectedError_%d"),index);
	TName fUseOpaqueData;
	fUseOpaqueData.Format(_L("UseOpaqueData_%d"),index);
	TName fSelectDialogOption;
	fSelectDialogOption.Format(_L("SelectDialogOption_%d"),index); 
	TName fButtonsDisplayed;
	fButtonsDisplayed.Format(_L("ButtonsDisplayed_%d"),index);
	TName fDialogCreatorInvoked;
	fDialogCreatorInvoked.Format(_L("DialogCreatorInvoked_%d"),index);
	TName fPolicyEvaluatorInvoked;
	fPolicyEvaluatorInvoked.Format(_L("PolicyEvaluatorInvoked_%d"),index);
	TName fAccessGranted;
	fAccessGranted.Format(_L("AccessGranted_%d"), index);
	TName fCloseSession;
	fCloseSession.Format(_L("CloseSession_%d"), index);
	TName fHoldEvaluatorOpen;
	fHoldEvaluatorOpen.Format(_L("HoldEvaluatorOpen_%d"), index);
	TName fHoldPrepareDialogOpen;
	fHoldPrepareDialogOpen.Format(_L("HoldPrepareDialogOpen_%d"), index);
	TName fHoldDisplayDialogOpen;
	fHoldDisplayDialogOpen.Format(_L("HoldDisplayDialogOpen_%d"), index);
	TName fRequestDurationThreshold;
	fRequestDurationThreshold.Format(_L("RequestDurationThreshold_%d"), index);
	TName fLeaveDialog;
	fLeaveDialog.Format(_L("LeaveDialog_%d"), index);
	TName fLeaveEvaluator;
	fLeaveEvaluator.Format(_L("LeaveEvaluator_%d"), index);
	TName fCancelUpsRequest;
	fCancelUpsRequest.Format(_L("CancelUpsRequest_%d"), index);
	TName fPlatSecPass;
	fPlatSecPass.Format(_L("PlatSecPass_%d"), index);
	TName fForcePrompt;
	fForcePrompt.Format(_L("ForcePrompt_%d"), index);
	TName fExpectedEvaInfo;
	fExpectedEvaInfo.Format(_L("ExpectedEvaluatorInfo_%d"), index);
	TName fSelectFingerprint;
	fSelectFingerprint.Format(_L("SelectFingerprint_%d"), index);
	TName fWaitUntilFileAppears;
	fWaitUntilFileAppears.Format(_L("WaitUntilFileAppears_%d"), index);
	
	TInt	useServiceUID;
	TPtrC   useServerName;
	TPtrC   destination;
	TInt	expectedError;
	TPtrC   useOpaqueData;
    TPtrC	selectDialogOption;
	TInt	buttonsDisplayed;
	TInt    dialogCreatorInvoked;
	TInt	policyEvaluatorInvoked;
	TPtrC	accessGranted;
	TBool	closeSession;
	TBool   holdEvaluatorOpen;
	TBool   holdPrepareDialogOpen;
	TBool   holdDisplayDialogOpen;
	TInt 	requestDurationThreshold;
	TBool   leaveDialog;
	TBool   leaveEvaluator;
	TBool   cancelUpsRequest;
	TBool   platSecPass;
	TBool   forcePrompt;
	TInt    expectedEvaInfo;
	TInt	selectFingerprint;
	TPtrC   waitUntilFileAppears;	
	
	while (GetHexFromConfig(ConfigSection(), fUseServiceUID,useServiceUID)
		&& GetStringFromConfig(ConfigSection(),fUseServerName,useServerName)
		&& GetStringFromConfig(ConfigSection(),fDestination,destination)
	  	&& GetStringFromConfig(ConfigSection(),fAccessGranted,accessGranted))
		{
	    TServiceRequestParameters newSerReq;
	    newSerReq.iServiceUID = useServiceUID; 
	    newSerReq.iServerName = useServerName;
	    newSerReq.iDestination = destination;
	    newSerReq.iAccessGranted = accessGranted;
	    
        // Close session is optional.
        if(GetBoolFromConfig(ConfigSection(),fCloseSession,closeSession))
	    	{
	    	newSerReq.iCloseSession = closeSession;
	    	}
	    
	    // use of dialog creator is optional.
	    if(GetStringFromConfig(ConfigSection(),fSelectDialogOption,selectDialogOption)
		&& GetIntFromConfig(ConfigSection(),fButtonsDisplayed,buttonsDisplayed)
		&& GetIntFromConfig(ConfigSection(),fDialogCreatorInvoked,dialogCreatorInvoked))
	    	{
		    newSerReq.iSelectDialogOption = selectDialogOption;
		    newSerReq.iButtonsDisplayed	= buttonsDisplayed;
		    newSerReq.iDialogCreatorInvoked	= dialogCreatorInvoked;
	    	} 
	    
	    // use of policy evaluator is optional.
	    if(GetIntFromConfig(ConfigSection(),fPolicyEvaluatorInvoked,policyEvaluatorInvoked))
	    	{
	       	newSerReq.iPolicyEvaluatorInvoked = policyEvaluatorInvoked;
	    	}
	    	    
	    // Opaque data is optional
	    if(GetStringFromConfig(ConfigSection(),fUseOpaqueData,useOpaqueData))
	    	{
	       	HBufC8* converter = HBufC8::NewLC(useOpaqueData.Length());
			converter->Des().Copy(useOpaqueData);	
             
	       	newSerReq.iOpaqueData = converter->Ptr() ;	
	       	CleanupStack::PopAndDestroy(); //converter 
	    	}
	    
	    // Expected error is optional.
	    if(GetIntFromConfig(ConfigSection(),fExpectedError,expectedError))
	    	{
			newSerReq.iExpectedError = expectedError;
	    	}
	    	
	    // hold policy evaluator open is optional.
	    if(GetBoolFromConfig(ConfigSection(),fHoldEvaluatorOpen, holdEvaluatorOpen))
	    	{
			newSerReq.iHoldEvaluatorOpen =	holdEvaluatorOpen;
	    	}
	    
	    // hold prepare dialog open is optional.
	    if(GetBoolFromConfig(ConfigSection(),fHoldPrepareDialogOpen, holdPrepareDialogOpen))
	    	{
			newSerReq.iHoldPrepareDialogOpen =	holdPrepareDialogOpen;
	    	}
	    	
	    // hold display dialog open is optional.
	    if(GetBoolFromConfig(ConfigSection(),fHoldDisplayDialogOpen, holdDisplayDialogOpen))
	    	{
			newSerReq.iHoldDisplayDialogOpen =	holdDisplayDialogOpen;
	    	}
	    
	    // request duration threshold - if a request takes 
	    // longer than this threshold it will fail the test
	    if(GetIntFromConfig(ConfigSection(),fRequestDurationThreshold, requestDurationThreshold))
	    	{
	       	newSerReq.iRequestDurationThreshold = requestDurationThreshold;
	    	}

	    // leave dialog is optional.
	    if(GetBoolFromConfig(ConfigSection(),fLeaveDialog,leaveDialog))
	    	{
	    	newSerReq.iLeaveDialog = leaveDialog;
	   		}
	    
	    // leave Evaluator is optional.
	    if(GetBoolFromConfig(ConfigSection(),fLeaveEvaluator,leaveEvaluator))
	    	{
			newSerReq.iLeaveEvaluator = leaveEvaluator;	
	    	}
		       
	    // Cancel ups request is optional.
	    if(GetBoolFromConfig(ConfigSection(),fCancelUpsRequest,cancelUpsRequest))
	    	{
	  		newSerReq.iCancelUpsRequest = cancelUpsRequest;		
	    	}
    
	    // plat sec pass is optional.
	    if(GetBoolFromConfig(ConfigSection(),fPlatSecPass,platSecPass))
	    	{
			newSerReq.iPlatSecPass = platSecPass;
	    	}
	    
	    // Force prompt is optional.
	    if(GetBoolFromConfig(ConfigSection(),fForcePrompt,forcePrompt))
	    	{
			newSerReq.iForcePrompt = forcePrompt;
	    	}
	    
	    // use of policy evaluator is optional.
	    if(GetIntFromConfig(ConfigSection(),fExpectedEvaInfo,expectedEvaInfo))
	    	{
	       	newSerReq.iExpectedEvaluatorInfo = expectedEvaInfo;
	    	}
	    
	    // Fingerprint selection is optional.
	    if(GetIntFromConfig(ConfigSection(),fSelectFingerprint,selectFingerprint))
	    	{
	       	newSerReq.iSelectFingerprint = selectFingerprint;
	    	}
	    
	    // iRepeatUntilFileAppears is optional.
	    if(GetStringFromConfig(ConfigSection(),fWaitUntilFileAppears,waitUntilFileAppears))
	    	{
	    	newSerReq.iWaitUntilFileAppears = waitUntilFileAppears;
	    	}
	    
	    // Add the new service to be requested to array.
	    iArraySersToRequest.Append(newSerReq);
	       
	    index++;
	    fUseServiceUID.Format(_L("UseServiceUID_%d"),index);
	    fUseServerName.Format(_L("UseServerName_%d"),index);
	    fDestination.Format(_L("Destination_%d"),index);
	    fExpectedError.Format(_L("ExpectedError_%d"),index);
	    fUseOpaqueData.Format(_L("UseOpaqueData_%d"),index);
	    fSelectDialogOption.Format(_L("SelectDialogOption_%d"),index);
	    fButtonsDisplayed.Format(_L("ButtonsDisplayed_%d"),index);
	    fDialogCreatorInvoked.Format(_L("DialogCreatorInvoked_%d"),index); 
	    fPolicyEvaluatorInvoked.Format(_L("PolicyEvaluatorInvoked_%d"),index);
	    fAccessGranted.Format(_L("AccessGranted_%d"), index);  
	    fCloseSession.Format(_L("CloseSession_%d"), index);
	    fHoldEvaluatorOpen.Format(_L("HoldEvaluatorOpen_%d"), index);
	    fHoldPrepareDialogOpen.Format(_L("HoldPrepareDialogOpen_%d"), index);
	    fHoldDisplayDialogOpen.Format(_L("HoldDisplayDialogOpen_%d"), index);
		fRequestDurationThreshold.Format(_L("RequestDurationThreshold_%d"), index);
	    fLeaveDialog.Format(_L("LeaveDialog_%d"), index);
	    fLeaveEvaluator.Format(_L("LeaveEvaluator_%d"), index);
	    fCancelUpsRequest.Format(_L("CancelUpsRequest_%d"), index);
	    fPlatSecPass.Format(_L("PlatSecPass_%d"), index);
	    fForcePrompt.Format(_L("ForcePrompt_%d"), index);
	   	fExpectedEvaInfo.Format(_L("ExpectedEvaluatorInfo_%d"), index);
	   	fSelectFingerprint.Format(_L("SelectFingerprint_%d"), index);
	   	fWaitUntilFileAppears.Format(_L("WaitUntilFileAppears_%d"), index);
	   	}	

	// now try for some clientStep specific stuff
	// this ini file entry specifies the property key value for the hold flag
	// If the hold flag property is true then monitor it until it becomes false
	// then continue
	TInt holdKey;
    if(GetIntFromConfig(ConfigSection(),_L("HoldClientStepKey"), holdKey))
	    {
		iHoldClientStepKey = holdKey;

		// as this property is present then set it to true using a direct call to p&s api because
		// the ups p&s api only handles policy evaluators and dialog creators and not test steps
		User::LeaveIfError(RProperty::Set(KPropertyCreatorUid, iHoldClientStepKey, 2));
	    }

	// Instantiates property reader and seter
	iPropertyReader= CUpsProperty::NewL(KPropertyCreatorUid); 

	// reads client name and SID
	TParse clientFullName;
	RThread client;
	clientFullName.Set(client.FullName(),NULL, NULL);
	iTEFServerName=clientFullName.Name();
	iExpectedClientSid = client.SecureId() ;
	client.Close();
			
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CUpsClientStep::doTestStepL()
	{
	// loop to request for different test services.
	TVerdict testResult= EPass;
	TInt numServicesToRequest = iArraySersToRequest.Count();

	if(numServicesToRequest > 0) 
		{
		// check if the test step needs to hold until another step allows it to continue
		if (iHoldClientStepKey)
			{
			HoldClientL();
			}

		// repeats the sequence of test services request as many times as specified in iStepRepeat
		for(TInt repeatTimes = 0;repeatTimes<iStepRepeat;++repeatTimes)
			{
			INFO_PRINTF3(_L("%S: Repeat number: %d"),&iTEFServerName,repeatTimes);
		
			for(TInt index=0;index < numServicesToRequest; ++index )
				{
				INFO_PRINTF6(_L("%S: Service request: %d. SysServer: %S Destination: %S  ServiceUID: %X "),&iTEFServerName, index, &iArraySersToRequest[index].iServerName ,&iArraySersToRequest[index].iDestination, iArraySersToRequest[index].iServiceUID);
				RUpsTestSession * sessionToBeUsed; 

				sessionToBeUsed = GetSessionToServerL(iArraySersToRequest[index].iServerName);			    

				SetTestDataL(index);
				
				sessionToBeUsed->UseTestService(iRequestData,iReturnData); 

				iServerId=sessionToBeUsed->GetServerSid();
				// Check results.
				if(CheckResultsL(index) == EFalse)
					{
					ERR_PRINTF2(_L("%S: Incorrect results"),&iTEFServerName);
					User::Leave(KErrGeneral);
					}

				// Checks from config value if session need to be closed.
				if(iArraySersToRequest[index].iCloseSession)
					{
					INFO_PRINTF3(_L("%S :Closing session to server: %S"),&iTEFServerName, &iArraySersToRequest[index].iServerName );    
					CloseSessionL(iArraySersToRequest[index].iServerName);
					}   
				}  // End of second loop
			}// End of first loop
		
		// Sets the status of the first instance of policy evaluator and dialog creator to
		// completed so that the coordinator step can progress in concurrent mode 	
		SetClientStatusCompleteL();
		}
	else
		{
		testResult= EFail;
		ERR_PRINTF2(_L("%S :Problem reading ini file"),&iTEFServerName);
		User::Leave(KErrGeneral);	
		}  
		
	return testResult;	
	}  // End of function


TVerdict CUpsClientStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}


void CUpsClientStep::HoldClientL(void)
	{
	RProperty checkIfStillNeedtoHold;
	User::LeaveIfError(checkIfStillNeedtoHold.Attach(KPropertyCreatorUid, iHoldClientStepKey));
	CleanupClosePushL(checkIfStillNeedtoHold);
	checkIfStillNeedtoHold.Set(KPropertyCreatorUid, iHoldClientStepKey, 1);
    TRequestStatus holdStatus;
 	checkIfStillNeedtoHold.Subscribe(holdStatus);
 	
	TInt holdState;

	checkIfStillNeedtoHold.Get(KPropertyCreatorUid, iHoldClientStepKey, holdState);
	if(!holdState)
 		{
 		checkIfStillNeedtoHold.Cancel();
 		}
 	else
 		{
 		User::WaitForRequest(holdStatus);
 		}

	RDebug::Printf("[UPS ClientStep 0x%x] HoldClient() stopped holding\n", this);
	
	// set the hold state back to 1 for future holds
	checkIfStillNeedtoHold.Set(KPropertyCreatorUid, iHoldClientStepKey, 2);
	CleanupStack::PopAndDestroy(&checkIfStillNeedtoHold);
	}


TBool CUpsClientStep::FindOpenSession(const TDesC& aServerName,TInt &position)
	{
	TBool sessionFound = EFalse;
	TInt sessionsCount = iArraySessionsInUse.Count();
	for(TInt index = 0;index < sessionsCount; ++index)
		{
		if(!aServerName.CompareF(iArraySessionsInUse[index]->iServerName))
			{
			position = index;
			sessionFound = ETrue;
			break;
			}
		} 
	return sessionFound;
	} // End of function


RUpsTestSession* CUpsClientStep::GetSessionToServerL(const TDesC& aServerName) 
	{
	// Finds out if a session is already open for server requested
	TInt foundPos;
	RUpsTestSession* sessionToReturn;
	if(!FindOpenSession(aServerName,foundPos))
		{
		// if open session for intended server not found it creates a new one
		COpenSession* sessionToAppend=COpenSession::NewL(aServerName);
		iArraySessionsInUse.AppendL(sessionToAppend);		
		foundPos = iArraySessionsInUse.Count()-1; // position of new session is last one in array. 
		}
	sessionToReturn = iArraySessionsInUse[foundPos]->iPointerToSession;
	
	return sessionToReturn;
	} // End of function
	
	
void CUpsClientStep::CloseSessionL(const TDesC& aServerName) 
	{
	// Finds out if a session is already open for server requested
	TInt foundPos;
	
	if(!FindOpenSession(aServerName,foundPos))
		{
		// if a session for this server was not found it leaves.
		ERR_PRINTF2(_L("%S: The server to be closed was not found"),&iTEFServerName);
		User::Leave(KErrGeneral); 
		}
	delete iArraySessionsInUse[foundPos];  // deletes instance.
	iArraySessionsInUse.Remove(foundPos);  // removes empty slot

	} // End of function


void CUpsClientStep::CloseAllOpenSessions()
	{
	while(iArraySessionsInUse.Count()>0)
		{   // delete and remove first session of array.
		delete iArraySessionsInUse[0];
	    iArraySessionsInUse.Remove(0);	
		}
	iArraySessionsInUse.Close();
	} // End of function


void CUpsClientStep::SetTestDataL(TInt aIndex)
	{
	// Set data for request.
	iReturnData.iError=KErrNone;
	iServerId=0;
	iRequestData.iMeasureResponceTime = EFalse;
	iRequestData.iServiceUid = TUid::Uid(iArraySersToRequest[aIndex].iServiceUID);

	if(iArraySersToRequest[aIndex].iOpaqueData.CompareF(_L8(""))==0)
		{
		iRequestData.iOpaquePresent = EFalse;
		}
	else
		{
		iRequestData.iOpaquePresent = ETrue;
		iRequestData.iOpaqueData =iArraySersToRequest[aIndex].iOpaqueData;	
		}
		
	iRequestData.iDestination = iArraySersToRequest[aIndex].iDestination;
	iRequestData.iCancelRequest = iArraySersToRequest[aIndex].iCancelUpsRequest;
	iRequestData.iPlatSecPass = iArraySersToRequest[aIndex].iPlatSecPass;
	
	// set properties for policy evaluator.
	if (iArraySersToRequest[aIndex].iPolicyEvaluatorInvoked != 0)
		{
		//Reset various publisher and suscribe properties for policy evaluator.
		TInt instance = iArraySersToRequest[aIndex].iPolicyEvaluatorInvoked;
	  	iPropertyReader->SetL(instance,KPe_Status,CUpsProperty::EPolicyEvaluator, KUpsPeStatusReset);
	  	iPropertyReader->SetL(instance,KPe_ClientSid,CUpsProperty::EPolicyEvaluator, 0);
	  	iPropertyReader->SetL(instance,KPe_ServerSid,CUpsProperty::EPolicyEvaluator, 0);
	  	TBool leaveEvaluator = iArraySersToRequest[aIndex].iLeaveEvaluator; 
	  	iPropertyReader->SetL(instance,KPe_Leave,CUpsProperty::EPolicyEvaluator, leaveEvaluator);
	  	iPropertyReader->SetL(instance,KPe_Error,CUpsProperty::EPolicyEvaluator, KErrNone);
	  	TBool holdEvaluatorOpen = iArraySersToRequest[aIndex].iHoldEvaluatorOpen;
	  	iPropertyReader->SetL(instance,KPe_HoldEvaluatorOpen,CUpsProperty::EPolicyEvaluator, holdEvaluatorOpen);
	  	TBool forcePrompt = iArraySersToRequest[aIndex].iForcePrompt; 
	  	iPropertyReader->SetL(instance,KPe_ForcePrompt,CUpsProperty::EPolicyEvaluator, forcePrompt);
	  	iPropertyReader->SetL(instance,KPe_EvaluatorInfo,CUpsProperty::EPolicyEvaluator, 0);
		}
		
	// set properties for dialog creator.
	if (iArraySersToRequest[aIndex].iDialogCreatorInvoked != 0)
		{
		//Reset various publisher and suscribe properties for dialog creator.
		TInt instance = iArraySersToRequest[aIndex].iDialogCreatorInvoked;
	  	iPropertyReader->SetL(instance,KDc_Status,CUpsProperty::EDialogCreator, KUpsDcStatusReset);
	  	iPropertyReader->SetL(instance,KDc_ClientSid,CUpsProperty::EDialogCreator, 0);
	  	iPropertyReader->SetL(instance,KDc_ServerSid,CUpsProperty::EDialogCreator, 0);
	  	TBool dialogLeave = iArraySersToRequest[aIndex].iLeaveDialog ;
	  	iPropertyReader->SetL(instance,KDc_Leave,CUpsProperty::EDialogCreator, dialogLeave);
	  	iPropertyReader->SetL(instance,KDc_Error,CUpsProperty::EDialogCreator, KErrNone);
	  	iPropertyReader->SetL(instance,KDc_UpsRequestedButtons,CUpsProperty::EDialogCreator, 0);
	    TInt buttonToPress = ButtonToInt(iArraySersToRequest[aIndex].iSelectDialogOption);
	  	iPropertyReader->SetL(instance,KDc_WhichButtonToPress,CUpsProperty::EDialogCreator, buttonToPress);
	  	TBool holdPrepareDialogOpen = iArraySersToRequest[aIndex].iHoldPrepareDialogOpen;
	  	iPropertyReader->SetL(instance,KDc_HoldPrepareDialogOpen,CUpsProperty::EDialogCreator, holdPrepareDialogOpen);
	  	TBool holdDisplayDialogOpen = iArraySersToRequest[aIndex].iHoldDisplayDialogOpen;
	  	iPropertyReader->SetL(instance,KDc_HoldDisplayDialogOpen,CUpsProperty::EDialogCreator, holdDisplayDialogOpen);
	  	TBool selectFingerprint = iArraySersToRequest[aIndex].iSelectFingerprint ;
	  	iPropertyReader->SetL(instance,KDc_SelectFingerprint,CUpsProperty::EDialogCreator, selectFingerprint);
		}

	}  // End of function.


void CUpsClientStep::SetClientStatusCompleteL()
/**
 * Sets the status of the first policy evaluator invoked by the client to "KUpsClientCompleted" so that
 * the coordinator step can determine when the step has completely finish and progress in Concurrent mode
 *
 * NOTE: When using the Coordinator in this manner, ALL requests made to UPS must invoke the same instance
 * of the Policy Evaluator and Dialog Creator! e.g. Policy Evalutor 1 and Dialog Creator 1
 */	
 	{
	TInt requestCount = iArraySersToRequest.Count();
		
	for(TInt i=0; i<requestCount; ++i)
		{
		if (iArraySersToRequest[i].iPolicyEvaluatorInvoked != 0)
			{	 
			TInt instance = iArraySersToRequest[i].iPolicyEvaluatorInvoked;
			iPropertyReader->SetL(instance,KPe_Status,CUpsProperty::EPolicyEvaluator, KUpsClientCompleted);
			iPropertyReader->SetL(instance,KDc_Status,CUpsProperty::EDialogCreator, KUpsClientCompleted);
			break;
			}	
		}
	}
	
	
TUpsDecision CUpsClientStep::StringToTUpsDecision(const TPtrC& aString)
	{
	if(aString.CompareF(_L("Yes"))==0)
		{
		return TUpsDecision(EUpsDecYes);
		}
	else if(aString.CompareF(_L("No"))==0)
		{
		return TUpsDecision(EUpsDecNo);
		}
	else if(aString.CompareF(_L("SessionYes"))==0)
		{
		return TUpsDecision(EUpsDecSessionYes);
		}
	else
		{
		return TUpsDecision(EUpsDecSessionNo);
		}	
	} // End of function.


TPtrC	CUpsClientStep::TUpsDecisionToString(TUpsDecision aDecision)
	{
	if(aDecision == EUpsDecYes)
		{
		return _L("Yes");
		}
	else if(aDecision == EUpsDecNo)
		{
		return _L("No");
		}
	else if(aDecision == EUpsDecSessionYes)
		{
		return _L("SessionYes");
		}
	else if(aDecision == EUpsDecSessionNo)
		{
		return _L("SessionNo");
		}
	else 
		{
		return _L("Undefined");
		}
	} // End of function.


TInt CUpsClientStep::ButtonToInt(const TPtrC& aButton)
	{
	if(aButton.CompareF(_L("Yes"))==0)
		{
		return KYes;
		}
	else if(aButton.CompareF(_L("No"))==0)
		{
		return KNo;
		}
	else if(aButton.CompareF(_L("Always"))==0)
		{
		return KAlways;
		}
	else if(aButton.CompareF(_L("SessionYes"))==0)
		{
		return KSessionYes;
		}
	else if(aButton.CompareF(_L("SessionNo"))==0)
		{
		return KSessionNo;
		}
	else if(aButton.CompareF(_L("Never"))==0)
		{
		return KNever;
		}
	else return KNone;
				
	}  // End of function.


TBool CUpsClientStep::CheckResultsL(TInt aIndex)
	{
	TBool theReturn = ETrue; 
	
	if(iArraySersToRequest[aIndex].iExpectedError == KErrNone)   
		{
		TUpsDecision expectedDecision = StringToTUpsDecision(iArraySersToRequest[aIndex].iAccessGranted);
		if(expectedDecision != iReturnData.iDecision)
	 		{
	 		TPtrC decisionReceived(TUpsDecisionToString(iReturnData.iDecision));
	 		ERR_PRINTF4(_L("%S: Decision expected: %S ,Decision received: %S"),&iTEFServerName, &iArraySersToRequest[aIndex].iAccessGranted, &decisionReceived);  
	 		theReturn=EFalse;	
	 		}		
		}

 	if(iArraySersToRequest[aIndex].iExpectedError != iReturnData.iError) 
 		{
 		// Incorrect error code returned.
 		theReturn=EFalse;
 		ERR_PRINTF3(_L("%S: Error code expected: %d"),&iTEFServerName,iArraySersToRequest[aIndex].iExpectedError);
 		ERR_PRINTF3(_L("%S: Error code Received: %d"),&iTEFServerName,iReturnData.iError);
 		}
 	
 	// checks returned properties from dialog creator & policy evaluator.
 	if ( CheckDialogCreatorResultsL(aIndex) == EFalse || CheckPolicyEvaluatorResultsL(aIndex) == EFalse)
 		{
 			theReturn = EFalse;
 		}

 	// display the request duration and check against the ini file value, if supplied
	INFO_PRINTF3(_L("%S: UPS decision request duration: %u milliseconds"),&iTEFServerName, iReturnData.iRequestDuration);
	if (iArraySersToRequest[aIndex].iRequestDurationThreshold > 0)
		{
		// check whether the request duration was too long
		if (static_cast<TInt>(iReturnData.iRequestDuration) > iArraySersToRequest[aIndex].iRequestDurationThreshold)
			{
	 		ERR_PRINTF3(_L("%S: UPS decision request took too long: %u"), &iTEFServerName, iArraySersToRequest[aIndex].iRequestDurationThreshold);
	 		theReturn = EFalse;
			}
		}

	return theReturn;
	}  // End of function.


TBool CUpsClientStep::CheckDialogCreatorResultsL(TInt aIndex)
	{
	TBool checkPassed = ETrue;
	
	// checks return properties from dialog creator.
 	if (iArraySersToRequest[aIndex].iDialogCreatorInvoked != 0)
		{
		TInt instanceDia = iArraySersToRequest[aIndex].iDialogCreatorInvoked;
		TInt dialogError = 0;
		iPropertyReader->GetL(instanceDia,KDc_Error,CUpsProperty::EDialogCreator, dialogError);
		
		// Checks that any error encountered by dialog creator is transmited by UPS to system server
		if(dialogError != iReturnData.iError && dialogError!=KErrNone)
			{
			ERR_PRINTF3(_L("%S: Dialog creator found an error: %d"),&iTEFServerName,dialogError);
			checkPassed = EFalse; 	
			}
		
		TInt clientSidDia=0;
		iPropertyReader->GetL(instanceDia,KDc_ClientSid,CUpsProperty::EDialogCreator, clientSidDia);
	     
	    if(iExpectedClientSid != clientSidDia)
		    {
		    ERR_PRINTF2(_L("%S: Client SID returned by dialog creator is not what is expected"),&iTEFServerName);
			checkPassed = EFalse;
		    }
	    
	    TInt buttonsDisplayed=0;
		iPropertyReader->GetL(instanceDia,KDc_UpsRequestedButtons,CUpsProperty::EDialogCreator, buttonsDisplayed);
	    TInt32 expectedButtonsDisplayed=iArraySersToRequest[aIndex].iButtonsDisplayed;
	    if(expectedButtonsDisplayed != buttonsDisplayed)
		    {
		   	ERR_PRINTF2(_L("%S: Incorrect buttons displayed."),&iTEFServerName);
		   	ERR_PRINTF3(_L("%S: Buttons displayed expected: %d"),&iTEFServerName,expectedButtonsDisplayed);
		   	ERR_PRINTF3(_L("%S: Buttons displayed received: %d"),&iTEFServerName,buttonsDisplayed);
			checkPassed = EFalse;
		    }
	    
	    TInt serverSidDia=0;
		iPropertyReader->GetL(instanceDia,KDc_ServerSid,CUpsProperty::EDialogCreator, serverSidDia);
	     
	    if(iServerId != serverSidDia)
		    {
		   	ERR_PRINTF2(_L("%S: Server SID returned by dialog creator is not what is expected"),&iTEFServerName);
			checkPassed = EFalse;
		    } 
	    
	    TInt serviceSidDia=0;
		iPropertyReader->GetL(instanceDia,KDc_ServiceId,CUpsProperty::EDialogCreator, serviceSidDia);
		if(serviceSidDia != iArraySersToRequest[aIndex].iServiceUID)
		    {
		   	ERR_PRINTF3(_L("%S: Service ID reported by dialog creator is not what is expected: %d"),&iTEFServerName,serviceSidDia);
			checkPassed = EFalse;
		    }  
		}
	
	return checkPassed;
	}  // End of function.


TBool CUpsClientStep::CheckPolicyEvaluatorResultsL(TInt aIndex)
	{
	TBool checkPassed = ETrue;
	
	if (iArraySersToRequest[aIndex].iPolicyEvaluatorInvoked != 0)
		{
		TInt instanceEva = iArraySersToRequest[aIndex].iPolicyEvaluatorInvoked;
		TInt evaluatorError = 0;
		
		// Checks that any error encountered by policy evaluator is transmited by UPS to system server
		iPropertyReader->GetL(instanceEva,KPe_Error,CUpsProperty::EPolicyEvaluator, evaluatorError);
		if(evaluatorError != iReturnData.iError && evaluatorError!=KErrNone)
			{
			ERR_PRINTF3(_L("%S: Policy evaluator found an error: %d"),&iTEFServerName,evaluatorError);
			checkPassed = EFalse; 	
			}
		
		TInt clientSidEva=0;
		iPropertyReader->GetL(instanceEva,KPe_ClientSid,CUpsProperty::EPolicyEvaluator, clientSidEva);
	     
	    if(iExpectedClientSid != clientSidEva)
		    {
		   	ERR_PRINTF2(_L("%S: Client SID returned by policy evaluator is not what is expected"),&iTEFServerName);
			checkPassed = EFalse;
		    }
	    
	    TInt serverSidEva=0;
		iPropertyReader->GetL(instanceEva,KPe_ServerSid,CUpsProperty::EPolicyEvaluator, serverSidEva);
	     
	    if(iServerId != serverSidEva)
		    {
		   	ERR_PRINTF2(_L("%S: Server SID returned by policy evaluator is not what is expected"),&iTEFServerName);
			checkPassed = EFalse;
		    } 
	    
	    TInt serviceSidEva = 0;
		iPropertyReader->GetL(instanceEva,KPe_ServiceId,CUpsProperty::EPolicyEvaluator, serviceSidEva);
		if(serviceSidEva != iArraySersToRequest[aIndex].iServiceUID)
		    {
		   	ERR_PRINTF3(_L("%S: Service ID reported by policy evaluator is not what is expected: %d"),&iTEFServerName,serviceSidEva);
			checkPassed = EFalse;
		    }
	    if( iArraySersToRequest[aIndex].iForcePrompt)
		    {
	    	// Read evaluator info
	    	TInt evaluatorInfo = 0;
			iPropertyReader->GetL(instanceEva,KPe_EvaluatorInfo,CUpsProperty::EPolicyEvaluator, evaluatorInfo);
	    	if(evaluatorInfo != iArraySersToRequest[aIndex].iExpectedEvaluatorInfo)
	    		{
	    		ERR_PRINTF4(_L("%S: incorrect evaluator info:Expected: %d Received: %d"),&iTEFServerName,iArraySersToRequest[aIndex].iExpectedEvaluatorInfo,evaluatorInfo);
	    		checkPassed = EFalse;
	    		}
		    }
		}
		
	return checkPassed;
	}  // End of function.
