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


#include "tupscoordinatorstep.h"
#include "tupsintegdefs.h"
#include <e32property.h>

_LIT(KOperationWaitForState, "WaitForState");
_LIT(KOperationReleaseClient, "ReleaseClient");
_LIT(KOperationReleaseEvaluatorHold, "ReleaseEvaluatorHold");
_LIT(KOperationReleasePrepareDialogHold, "ReleasePrepareDialogHold");
_LIT(KOperationReleaseDisplayDialogHold, "ReleaseDisplayDialogHold");

CUPSCoordinatorStep::CUPSCoordinatorStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KUPSCoordinatorStep);
	}

CUPSCoordinatorStep::~CUPSCoordinatorStep()
/**
 * Destructor
 */
	{
	iArrayCoordinationOps.Close(); 
	delete iPropertyReader;
	
	__UHEAP_MARKEND;
	}
	
TCoordinationOperation::TCoordinationOperation() 
	:iOperation(0), iClientInstanceToOperate(0), iClientStateA(EClientState_NULL), 
	iClientStateB(EClientState_NULL), iClientStateC(EClientState_NULL)	 	 	 	 
	{	
	};

TVerdict CUPSCoordinatorStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;
	
	SetTestStepResult(EPass);
	
	// reads client name and SID
	TParse clientFullName;
	RThread clientThread;
	clientFullName.Set(clientThread.FullName(),NULL, NULL);
	iTEFServerName=clientFullName.Name();
	iExpectedClientSid = clientThread.SecureId() ;
	clientThread.Close();
	
	iPropertyReader = CUpsProperty::NewL(KPropertyCreatorUid);	// Instantiates property reader and setter
	
	// Number of Clients to Coordinate
    if(!GetIntFromConfig(ConfigSection(), _L("ClientCount"), iClientCount))
	    {
	    // Ensure that the number of clients being monitored is between 1 - 3
	    if(iClientCount < 1 && iClientCount > 3)
		    {
		    User::Leave(KErrArgument);	
		    }
	    }
	
	// get first client instance - we expect this to be the first client that runs
    if(!GetIntFromConfig(ConfigSection(), _L("ClientInstance_A"), iClientInstance[KClientA]))
	    {
	    User::Leave(KErrArgument);
	    }
	    
	if(!GetIntFromConfig(ConfigSection(), _L("ClientHoldKey_A"), iClientHoldKey[KClientA]))
	    {
	    iClientHoldKey[KClientA] = 0;
	    } 
	   
	if(iClientCount > 1) 
		{
		// get second and third client instances - we expect them to be the second and third clients to run
	    if(!GetIntFromConfig(ConfigSection(), _L("ClientInstance_B"), iClientInstance[KClientB]))
		    {
			User::Leave(KErrArgument);	  
		    }
		    
		if(!GetIntFromConfig(ConfigSection(), _L("ClientHoldKey_B"), iClientHoldKey[KClientB]))
		    {
		    iClientHoldKey[KClientB] = 0;
		    } 
		
		if(iClientCount > 2) 
			{    
			if(!GetIntFromConfig(ConfigSection(), _L("ClientInstance_C"), iClientInstance[KClientC]))
			    {
			    User::Leave(KErrArgument);
			    }
		    
		    if(!GetIntFromConfig(ConfigSection(), _L("ClientHoldKey_C"), iClientHoldKey[KClientC]))
			    {
			    iClientHoldKey[KClientC] = 0;
			    }    	
			}
		}
		
	// Read values for test sequence from INI file. (ARRAY of values)
	TInt index=0;
	TName fOperation;
	fOperation.Format(_L("Operation_%d"), index);
	TName fClientInstanceToOperate;
	fClientInstanceToOperate.Format(_L("ClientInstanceToOperate_%d"),index);
	TName fClientStateA;
	fClientStateA.Format(_L("ClientStateA_%d"),index);
	TName fClientStateB;
	fClientStateB.Format(_L("ClientStateB_%d"),index);
	TName fClientStateC;
	fClientStateC.Format(_L("ClientStateC_%d"),index);
	
	TPtrC 	operation;
	TInt	clientInstanceToOperate;
	TPtrC	clientStateA;
	TPtrC	clientStateB;
	TPtrC	clientStateC;
	
	while (GetStringFromConfig(ConfigSection(), fOperation, operation))
		{
		
	    TCoordinationOperation newOperation;
	    
	    newOperation.iOperation = operation; 
	    
	    // use of instance to perform an operation on is optional.
	    if(GetIntFromConfig(ConfigSection(),fClientInstanceToOperate,clientInstanceToOperate))
	    	{
	       	newOperation.iClientInstanceToOperate = clientInstanceToOperate;
	    	}
	    	
	    // use of client state A is optional, only required on waiting operations.
	    if(GetStringFromConfig(ConfigSection(),fClientStateA,clientStateA))
	    	{
	       	newOperation.iClientStateA = PtrCToClientState(clientStateA);
	    	}
	    	
	    // use of client state B is optional, only required on waiting operations.
	    if(GetStringFromConfig(ConfigSection(),fClientStateB,clientStateB))
	    	{
	       	newOperation.iClientStateB = PtrCToClientState(clientStateB);
	    	}
	    	
	    // use of client state C is optional, only required on waiting operations.
	    if(GetStringFromConfig(ConfigSection(),fClientStateC,clientStateC))
	    	{
	       	newOperation.iClientStateC = PtrCToClientState(clientStateC);
	    	}
	    	
	    // Add the new operation to array.
	    iArrayCoordinationOps.Append(newOperation);
	       
	    index++;
	    fOperation.Format(_L("Operation_%d"), index);
		fClientInstanceToOperate.Format(_L("ClientInstanceToOperate_%d"),index);
		fClientStateA.Format(_L("ClientStateA_%d"),index);
		fClientStateB.Format(_L("ClientStateB_%d"),index);
		fClientStateC.Format(_L("ClientStateC_%d"),index);
		}
	
	// Create the Publish & Subscribe Properties and Reset the PE and DC status 
	// values of the clients being monitored
	for (TInt i = 0; i < iClientCount; ++i)
		{
		User::LeaveIfError(iCheckClientProperty[i][KPolicyEvaluator].Attach(KPropertyCreatorUid, iPropertyReader->GetPropertyKey(KPe_Status, KPe_Start, KInstanceInterval,iClientInstance[i])));
		User::LeaveIfError(iCheckClientProperty[i][KDialogCreator].Attach(KPropertyCreatorUid, iPropertyReader->GetPropertyKey(KDc_Status, KDc_Start, KInstanceInterval, iClientInstance[i])));
		iPropertyReader->SetL(i+1,KPe_Status,CUpsProperty::EPolicyEvaluator, KUpsPeStatusReset);
		iPropertyReader->SetL(i+1,KDc_Status,CUpsProperty::EDialogCreator, KUpsDcStatusReset);
		}
		
	return TestStepResult();
	}


TVerdict CUPSCoordinatorStep::doTestStepL()
	{
	// loop to request for different coordination operations
	SetTestStepResult(EPass);
	
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	// Initialise the Client States to Starting for the Clients being used
	for (TInt i = 0; i < iClientCount; ++i)
		{
		iClientState[i] = EClientState_Starting;
		}
	
	TInt numOperations = iArrayCoordinationOps.Count();

	if(numOperations > 0) 
		{
		
		for(TInt index=0;index < numOperations; ++index )
			{
			
		    INFO_PRINTF4(_L("%S: Coordination Operation: %d, Command: %S"),&iTEFServerName, index, &iArrayCoordinationOps[index].iOperation);	
		    
		    TPtrC op(iArrayCoordinationOps[index].iOperation);
		    
		    // *** WAIT FOR STATE ***
		    if (!op.CompareF(KOperationWaitForState))
		    	{
		    	INFO_PRINTF2(_L("%S: TUPS Coordinator - Wait on State Dialog"),&iTEFServerName);
		    	TPtrC clientAState(ClientStateToPtrC(iArrayCoordinationOps[index].iClientStateA));
		    	INFO_PRINTF3(_L("%S: TUPS Coordinator  - *WaitState* Client State 0: %S"),&iTEFServerName, &clientAState);
		    	TPtrC clientBState(ClientStateToPtrC(iArrayCoordinationOps[index].iClientStateB));
		    	INFO_PRINTF3(_L("%S: TUPS Coordinator  - *WaitState* Client State 1: %S"),&iTEFServerName, &clientBState);
		    	TPtrC clientCState(ClientStateToPtrC(iArrayCoordinationOps[index].iClientStateC));
		    	INFO_PRINTF3(_L("%S: TUPS Coordinator  - *WaitState* Client State 2: %S"),&iTEFServerName, &clientCState);
		    	
		    	RDebug::Printf("[UPS CoordinatorStep 0x%x] WaitForState - Waiting on Operation %d ...\n", this, index);
		    	
		    	User::LeaveIfError(WaitForStateL(index));
		    	
		    	RDebug::Printf("[UPS CoordinatorStep 0x%x] WaitForState - State met for Operation %d \n", this, index);
		    	}
		    // *** RELEASE CLIENT ***
		    else if (!op.CompareF(KOperationReleaseClient))
		    	{
		    	INFO_PRINTF3(_L("%S: TUPS Coordinator  - Release Client Instance %d"),&iTEFServerName, &iArrayCoordinationOps[index].iClientInstanceToOperate);
		    	
		    	TInt clientKeyLocator = (iArrayCoordinationOps[index].iClientInstanceToOperate) - 1;

				while (1)
					{
					TInt value;
					RProperty::Get(KPropertyCreatorUid, iClientHoldKey[clientKeyLocator], value);
					User::After(50000);
					if (value==1)
						break;
					}

		    	User::LeaveIfError(RProperty::Set(KPropertyCreatorUid, iClientHoldKey[clientKeyLocator], 0));
		    	
		    	RDebug::Printf("[UPS CoordinatorStep 0x%x] ReleaseClient - Release Client Instance %d \n", this, &iArrayCoordinationOps[index].iClientInstanceToOperate);
		    	}
		    // *** RELEASE EVALUATOR HOLD ***
		    else if (!op.CompareF(KOperationReleaseEvaluatorHold))
		    	{
		    	INFO_PRINTF3(_L("%S: TUPS Coordinator  - Release Evaluator Hold %d"),&iTEFServerName, &iArrayCoordinationOps[index].iClientInstanceToOperate);
		    	
		    	TInt clientInstanceLocator = (iArrayCoordinationOps[index].iClientInstanceToOperate) - 1;
		    	
		    	iPropertyReader->SetL(iClientInstance[clientInstanceLocator], KPe_HoldEvaluatorOpen, CUpsProperty::EPolicyEvaluator, 0);
		    	
		    	RDebug::Printf("[UPS CoordinatorStep 0x%x] ReleaseEvaluatorHold - Release Client Instance %d \n", this, &iArrayCoordinationOps[index].iClientInstanceToOperate);
		    	}
		    // *** RELEASE PREPARE DIALOG HOLD ***
		    else if (!op.CompareF(KOperationReleasePrepareDialogHold))
		    	{
		    	INFO_PRINTF3(_L("%S: TUPS Coordinator  - Release Prepare Dialog Hold %d"),&iTEFServerName, &iArrayCoordinationOps[index].iClientInstanceToOperate);
		    	
		    	TInt clientInstanceLocator = (iArrayCoordinationOps[index].iClientInstanceToOperate) - 1;
		    	
		    	iPropertyReader->SetL(iClientInstance[clientInstanceLocator], KDc_HoldPrepareDialogOpen, CUpsProperty::EDialogCreator, 0);
		    	
		    	RDebug::Printf("[UPS CoordinatorStep 0x%x] ReleasePrepareDialogHold - Release Client Instance %d \n", this, &iArrayCoordinationOps[index].iClientInstanceToOperate);
		    	}
		    // *** RELEASE DISPLAY DIALOG HOLD ***
		    else if (!op.CompareF(KOperationReleaseDisplayDialogHold))
		    	{
		    	INFO_PRINTF3(_L("%S: TUPS Coordinator  - Release Display Dialog Hold %d"),&iTEFServerName, &iArrayCoordinationOps[index].iClientInstanceToOperate);
		    	
		    	TInt clientInstanceLocator = (iArrayCoordinationOps[index].iClientInstanceToOperate) - 1;
		    	
		    	iPropertyReader->SetL(iClientInstance[clientInstanceLocator], KDc_HoldDisplayDialogOpen, CUpsProperty::EDialogCreator, 0);
		    	
		    	RDebug::Printf("[UPS CoordinatorStep 0x%x] ReleaseDisplayDialogHold - Release Client Instance %d \n", this, &iArrayCoordinationOps[index].iClientInstanceToOperate);
		    	}	
		    else
		    	{
		    	SetTestStepResult(EFail);
	     	    ERR_PRINTF2(_L("%S: Invalid operation"),&iTEFServerName);
				User::Leave(KErrNotSupported);	 			    	
		    	}
	    	
			}// End of loop
		
		} 
	else
		{
		SetTestStepResult(EFail);
		ERR_PRINTF2(_L("%S: Problem reading ini file"),&iTEFServerName);
		User::Leave(KErrGeneral);	
		} 
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells()); 
		
	return TestStepResult();
	}


TInt CUPSCoordinatorStep::WaitForStateL(TInt aOperation)
/**
 * WaitForState
 * This method expects waits for the state of the clients to change according to the 
 * states provided in the Operation issued within the INI configuration file
 * Return: 1 = success 
 *		  -1 = failure
 */
	{
	// return value of the function
	TInt ret = 0;
	
    INFO_PRINTF3(_L("%S: TUPS Coordinator  - WaitForStateL() - Checking State: Operation %d"),&iTEFServerName, aOperation);
	RDebug::Printf("[UPS CoordinatorStep 0x%x] WaitForStateL() - Checking State: Operation %d \n", this, aOperation);

	while (!ret)
		{
		// Determines the current state of each of the clients by evaluating the 
		// status properties of the Policy Evaluator and Dialog Creator being invoked
		for (TInt j = 0; j < iClientCount; ++j)
			{
			User::LeaveIfError(GetAndProcessClientStatusL(j));
			}
		
		// Depending on the number of clients being monitored, each of the client states are
		// checked to determine whether the desired state has been met
		if(iClientCount == 1)
			{			
			// Check to see if required state has been achieved
			if (iClientState[KClientA] == iArrayCoordinationOps[aOperation].iClientStateA)
				{
				ret = 1;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - WaitForStateL() - State Achieved: Operation %d"),&iTEFServerName, aOperation);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] WaitForStateL() - State Achieved : Operation %d \n", this, aOperation);
				}

			// Check for erroneous conditions
			if (iClientState[KClientA] > iArrayCoordinationOps[aOperation].iClientStateA)
				{
				ret = -1;
				ERR_PRINTF3(_L("%S: TUPS Coordinator  - WaitForStateL() - Bad State: Operation %d"),&iTEFServerName, aOperation);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] WaitForStateL() - Bad State : Operation %d \n", this, aOperation);
				}	
			}
		else if(iClientCount == 2)
			{
			// Check to see if required state has been achieved
			if ((iClientState[KClientA] == iArrayCoordinationOps[aOperation].iClientStateA) && 
				(iClientState[KClientB] == iArrayCoordinationOps[aOperation].iClientStateB))
				{
				ret = 1;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - WaitForStateL() - State Achieved: Operation %d"),&iTEFServerName, aOperation);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] WaitForStateL() - State Achieved : Operation %d \n", this, aOperation);
				}

			// Check for erroneous conditions
			if ((iClientState[KClientA] > iArrayCoordinationOps[aOperation].iClientStateA) ||
				(iClientState[KClientB] > iArrayCoordinationOps[aOperation].iClientStateB))
				{
				ret = -1;
				ERR_PRINTF3(_L("%S: TUPS Coordinator  - WaitForStateL() - Bad State: Operation %d"),&iTEFServerName, aOperation);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] WaitForStateL() - Bad State : Operation %d \n", this, aOperation);
				}	
			}
		else if(iClientCount == 3)
			{
			// Check to see if required state has been achieved
			if ((iClientState[KClientA] == iArrayCoordinationOps[aOperation].iClientStateA) && 
				(iClientState[KClientB] == iArrayCoordinationOps[aOperation].iClientStateB) &&
				(iClientState[KClientC] == iArrayCoordinationOps[aOperation].iClientStateC))
				{
				ret = 1;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - WaitForStateL() - State Achieved: Operation %d"),&iTEFServerName, aOperation);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] WaitForStateL() - State Achieved : Operation %d \n", this, aOperation);
				}

			// Check for erroneous conditions
			if ((iClientState[KClientA] > iArrayCoordinationOps[aOperation].iClientStateA) ||
				(iClientState[KClientB] > iArrayCoordinationOps[aOperation].iClientStateB) || 
				(iClientState[KClientC] > iArrayCoordinationOps[aOperation].iClientStateC))
				{
				ret = -1;
				ERR_PRINTF3(_L("%S: TUPS Coordinator  - WaitForStateL() - Bad State: Operation %d"),&iTEFServerName, aOperation);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] WaitForStateL() - Bad State : Operation %d \n", this, aOperation);
				}	
			}
		else
			{
			// Note: Should never get into this state
			ERR_PRINTF3(_L("%S: TUPS Coordinator  - WaitForStateL() - Invalid Client Count: Operation %d"),&iTEFServerName, aOperation);
			RDebug::Printf("[UPS CoordinatorStep 0x%x] WaitForStateL() - Invalid Client Count : Operation %d \n", this, aOperation);
			User::Leave(KErrNotSupported);	
			}
		
		
		if (!ret)
			{
			User::After(500000);
			continue;
					
			}
		else
			{
			// Cancel the property subscriptions for each client
			for (TInt k = 0; k < iClientCount; ++k)
				{
				iCheckClientProperty[k][KPolicyEvaluator].Cancel();
				iCheckClientProperty[k][KDialogCreator].Cancel();
				}
			}
			
		}
		
	return ret;
	}	
	

TInt CUPSCoordinatorStep::GetAndProcessClientStatusL(TInt aClient)
	{
	TInt ret = 1;

	// get the status of the clients policy evaluator and dialog creator
	TInt peStatus = 0;
	iPropertyReader->GetL(iClientInstance[aClient], KPe_Status, CUpsProperty::EPolicyEvaluator, peStatus);
	TInt dcStatus = 0;
	iPropertyReader->GetL(iClientInstance[aClient], KDc_Status, CUpsProperty::EDialogCreator, dcStatus);

	RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d PE=%d DC=%d\n", this, aClient, peStatus, dcStatus);
	RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d State=%d\n", this, aClient, iClientState[aClient]);
	// evaluate the client state according to the states of its policy evaluator and dialog creator
	TBool stateOk = EFalse;
	
	switch (iClientState[aClient])
		{
		case EClientState_Starting:
			{
			//Policy Evaluator Status Logic
			if (peStatus == KUpsPeStatusStarted)
				{
				iClientState[aClient] = EClientState_PolicyEvaluatorStarted;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_PolicyEvaluatorStarted"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d PE Started\n", this, aClient);
				stateOk = ETrue;
				}
			else if (peStatus == KUpsPeStatusHolding)
				{
				iClientState[aClient] = EClientState_PolicyEvaluatorHolding;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_PolicyEvaluatorHolding"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d PE Holding\n", this, aClient);
				stateOk = ETrue;
				}			
			else if (peStatus == KUpsPeStatusCompleted)
				{
				iClientState[aClient] = EClientState_PolicyEvaluatorCompleted;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_PolicyEvaluatorCompleted"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d PE Completed\n", this, aClient);
				stateOk = ETrue;
				}
			else if(peStatus == KUpsClientCompleted || dcStatus == KUpsClientCompleted)
				{
				iClientState[aClient] = EClientState_Finished;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_Finished"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() Client Finished %d \n", this, aClient);
				stateOk = ETrue;
				}

				
			//Dialog Creator Status Logic
			if ((dcStatus == 0) || (dcStatus == KUpsDcStatusReset) || (peStatus == KUpsPeStatusReset))
				{
				stateOk = ETrue;
				}
				
			break;
			}	
			
			
		case EClientState_PolicyEvaluatorStarted:
			{
			//Policy Evaluator Status Logic
			if (peStatus == KUpsPeStatusReset || 
				peStatus == KUpsPeStatusStarted)
				{
				stateOk = ETrue;
				}
			else if (peStatus == KUpsPeStatusHolding)
				{
				iClientState[aClient] = EClientState_PolicyEvaluatorHolding;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_PolicyEvaluatorHolding"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d PE Holding\n", this, aClient);
				stateOk = ETrue;
				}
			else if(peStatus == KUpsPeStatusCompleted)
				{
				iClientState[aClient] = EClientState_PolicyEvaluatorCompleted;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_PolicyEvaluatorCompleted"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d PE Completed\n", this, aClient);
				stateOk = ETrue;
				}
			else if(peStatus == KUpsClientCompleted || dcStatus == KUpsClientCompleted)
				{
				iClientState[aClient] = EClientState_Finished;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_Finished"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() Client Finished %d \n", this, aClient);
				stateOk = ETrue;
				}
			break;
			}
			
			
		case EClientState_PolicyEvaluatorHolding:
			{
			//Policy Evaluator Status Logic
			if (peStatus == KUpsPeStatusReset || 
				peStatus == KUpsPeStatusStarted ||
				peStatus == KUpsPeStatusHolding)
				{
				stateOk = ETrue;
				}
			else if(peStatus == KUpsPeStatusCompleted)
				{
				iClientState[aClient] = EClientState_PolicyEvaluatorCompleted;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_PolicyEvaluatorCompleted"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d PE Completed\n", this, aClient);
				stateOk = ETrue;
				}
			else if(peStatus == KUpsClientCompleted || dcStatus == KUpsClientCompleted)
				{
				iClientState[aClient] = EClientState_Finished;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_Finished"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() Client Finished %d \n", this, aClient);
				stateOk = ETrue;
				}
				
			break;
			}

		case EClientState_PolicyEvaluatorCompleted:
			{
			//Policy Evaluator Status Logic
			if (peStatus == KUpsPeStatusReset || 
				peStatus == KUpsPeStatusStarted ||
				peStatus == KUpsPeStatusHolding ||
				peStatus == KUpsPeStatusCompleted )
				{
				stateOk = ETrue;
				}
			
			//Dialog Creator Status Logic	
			if ((dcStatus == 0) || (dcStatus == KUpsDcStatusReset))
				{
				stateOk = ETrue;
				}
			else if (dcStatus == KUpsDcStatusStarted)
				{
				iClientState[aClient] = EClientState_DialogCreatorStarted;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_DialogCreatorStarted"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d DC Started\n", this, aClient);
				stateOk = ETrue;
				}
			else if (dcStatus == KUpsDcStatusPrepareHold)
				{
				iClientState[aClient] = EClientState_DialogCreatorPrepareHolding;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_DialogCreatorPrepareHolding"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d DC Prepare Dialog Holding \n", this, aClient);
				stateOk = ETrue;
				}
			else if (dcStatus == KUpsDcStatusDisplayHold)
				{
				iClientState[aClient] = EClientState_DialogCreatorDisplayHolding;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_DialogCreatorDisplayHolding"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d DC Display Dialog Holding \n", this, aClient);
				stateOk = ETrue;
				}
			else if (dcStatus == KUpsDcStatusCompleted)
				{
				iClientState[aClient] = EClientState_DialogCreatorCompleted;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_DialogCreatorCompleted"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d dc completed\n", this, aClient);
				stateOk = ETrue;
				}
			else if(peStatus == KUpsClientCompleted || dcStatus == KUpsClientCompleted)
				{
				iClientState[aClient] = EClientState_Finished;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_DialogCreatorCompleted"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d DC Completed\n", this, aClient);
				stateOk = ETrue;	
				}
				
			break;
			}	
			

		case EClientState_DialogCreatorStarted:
			{
			//Policy Evaluator Status Logic
			if (peStatus == KUpsPeStatusReset || 
				peStatus == KUpsPeStatusStarted ||
				peStatus == KUpsPeStatusHolding ||
				peStatus == KUpsPeStatusCompleted )
				{
				stateOk = ETrue;
				}
				
			//Dialog Creator Status Logic	
			if (dcStatus == KUpsDcStatusReset || 
				dcStatus == KUpsDcStatusStarted)
				{
				stateOk = ETrue;
				}
			else if (dcStatus == KUpsDcStatusPrepareHold)
				{
				iClientState[aClient] = EClientState_DialogCreatorPrepareHolding;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_DialogCreatorPrepareHolding"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d DC Prepare Dialog Holding \n", this, aClient);
				stateOk = ETrue;
				}
			else if (dcStatus == KUpsDcStatusDisplayHold)
				{
				iClientState[aClient] = EClientState_DialogCreatorDisplayHolding;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_DialogCreatorDisplayHolding"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d DC Display Dialog Holding \n", this, aClient);
				stateOk = ETrue;
				}
			else if (dcStatus == KUpsDcStatusCompleted)
				{
				iClientState[aClient] = EClientState_DialogCreatorCompleted;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_DialogCreatorCompleted"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d dc completed\n", this, aClient);
				stateOk = ETrue;
				}
			else if(peStatus == KUpsClientCompleted || dcStatus == KUpsClientCompleted)
				{
				iClientState[aClient] = EClientState_Finished;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_Finished"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() Client Finished %d \n", this, aClient);
				stateOk = ETrue;
				}
					
			break;
			}
			
			
		case EClientState_DialogCreatorPrepareHolding:
			{	
			//Dialog Creator Status Logic	
			if (dcStatus == KUpsDcStatusReset || 
				dcStatus == KUpsDcStatusStarted ||
				dcStatus == KUpsDcStatusPrepareHold )
				{
				stateOk = ETrue;
				}
			else if (dcStatus == KUpsDcStatusDisplayHold)
				{
				iClientState[aClient] = EClientState_DialogCreatorDisplayHolding;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_DialogCreatorDisplayHolding"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d DC Display Dialog Holding \n", this, aClient);
				stateOk = ETrue;
				}
			else if (dcStatus == KUpsDcStatusCompleted)
				{
				iClientState[aClient] = EClientState_DialogCreatorCompleted;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_DialogCreatorCompleted"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d dc completed\n", this, aClient);
				stateOk = ETrue;
				}
			else if(peStatus == KUpsClientCompleted || dcStatus == KUpsClientCompleted)
				{
				iClientState[aClient] = EClientState_Finished;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_Finished"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() Client Finished %d \n", this, aClient);
				stateOk = ETrue;
				}
					
			break;
			}
			
			
		case EClientState_DialogCreatorDisplayHolding:
			{	
			//Dialog Creator Status Logic	
			if (dcStatus == KUpsDcStatusReset || 
				dcStatus == KUpsDcStatusStarted ||
				dcStatus == KUpsDcStatusPrepareHold ||
				dcStatus == KUpsDcStatusDisplayHold)
				{
				stateOk = ETrue;
				}
			else if (dcStatus == KUpsDcStatusCompleted)
				{
				iClientState[aClient] = EClientState_DialogCreatorCompleted;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_DialogCreatorCompleted"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() client %d dc completed\n", this, aClient);
				stateOk = ETrue;
				}
			else if(peStatus == KUpsClientCompleted || dcStatus == KUpsClientCompleted)
				{
				iClientState[aClient] = EClientState_Finished;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_Finished"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() Client Finished %d \n", this, aClient);
				stateOk = ETrue;
				}
					
			break;
			}
			
			
		case EClientState_DialogCreatorCompleted:
			{
			//Dialog Creator Status Logic
			if (dcStatus == KUpsDcStatusReset || 
				dcStatus == KUpsDcStatusStarted ||
				dcStatus == KUpsDcStatusPrepareHold ||
				dcStatus == KUpsDcStatusDisplayHold ||
				dcStatus == KUpsDcStatusCompleted )
				{
				stateOk = ETrue;
				}
			else if(peStatus == KUpsClientCompleted || dcStatus == KUpsClientCompleted)
				{
				iClientState[aClient] = EClientState_Finished;
				INFO_PRINTF3(_L("%S: TUPS Coordinator  - *State Change* Client %d : EClientState_Finished"),&iTEFServerName, aClient);
				RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() Client Finished %d \n", this, aClient);
				stateOk = ETrue;
				}
				
			break;	
			}
			
			
		case EClientState_Finished:
			{
			if (peStatus <= KUpsClientCompleted && dcStatus <= KUpsClientCompleted)
				{
				stateOk = ETrue;
				}
				
			break;	
			}
			
			
		default:
			{
			break;	
			}
			
		}
		
	// check if current state values are ok
	if (!stateOk)
		{
		// not ok so abort
		ret = -1;
		RDebug::Printf("[UPS CoordinatorStep 0x%x] GetAndProcessClientStatus() error\n", this);
		}
	return ret;
	}

	
TClientState CUPSCoordinatorStep::PtrCToClientState(const TPtrC& aString)
{
	if(aString.CompareF(_L("EClientState_Starting"))==0)
	{
		return EClientState_Starting;
	}
	else if(aString.CompareF(_L("EClientState_PolicyEvaluatorStarted"))==0)
	{
		return EClientState_PolicyEvaluatorStarted;
	}
	else if(aString.CompareF(_L("EClientState_PolicyEvaluatorHolding"))==0)
	{
		return EClientState_PolicyEvaluatorHolding;
	}
	else if(aString.CompareF(_L("EClientState_PolicyEvaluatorCompleted"))==0)
	{
		return EClientState_PolicyEvaluatorCompleted;
	}
	else if(aString.CompareF(_L("EClientState_DialogCreatorStarted"))==0)
	{
		return EClientState_DialogCreatorStarted;
	}
	else if(aString.CompareF(_L("EClientState_DialogCreatorPrepareHolding"))==0)
	{
		return EClientState_DialogCreatorPrepareHolding;
	}
	else if(aString.CompareF(_L("EClientState_DialogCreatorDisplayHolding"))==0)
	{
		return EClientState_DialogCreatorDisplayHolding;
	}
	else if(aString.CompareF(_L("EClientState_DialogCreatorCompleted"))==0)
	{
		return EClientState_DialogCreatorCompleted;
	}
	else if(aString.CompareF(_L("EClientState_Finished"))==0)
	{
		return EClientState_Finished;
	}
	else return EClientState_NULL;
			
}  // End of function.


TPtrC CUPSCoordinatorStep::ClientStateToPtrC(TClientState aState)
{
	if(aState==EClientState_Starting)
	{
		return _L("EClientState_Starting");
	}
	else if(aState==EClientState_PolicyEvaluatorStarted)
	{
		return _L("EClientState_PolicyEvaluatorStarted");
	}
	else if(aState==EClientState_PolicyEvaluatorHolding)
	{
		return _L("EClientState_PolicyEvaluatorHolding");
	}
	else if(aState==EClientState_PolicyEvaluatorCompleted)
	{
		return _L("EClientState_PolicyEvaluatorCompleted");
	}
	else if(aState==EClientState_DialogCreatorStarted)
	{
		return _L("EClientState_DialogCreatorStarted");
	}
	else if(aState==EClientState_DialogCreatorPrepareHolding)
	{
		return _L("EClientState_DialogCreatorPrepareHolding");
	}
	else if(aState==EClientState_DialogCreatorDisplayHolding)
	{
		return _L("EClientState_DialogCreatorDisplayHolding");
	}
	else if(aState==EClientState_DialogCreatorCompleted)
	{
		return _L("EClientState_DialogCreatorCompleted");
	}
	else if(aState==EClientState_Finished)
	{
		return _L("EClientState_Finished");
	}
	else return _L("EClientState_NULL");
			
}  // End of function.
