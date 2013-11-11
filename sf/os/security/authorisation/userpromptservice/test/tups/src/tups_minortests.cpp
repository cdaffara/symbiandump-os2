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


#include "tups_minortests.h" 
 

CNonProtServAuthorisation::CNonProtServAuthorisation()
/**
 * Constructor
 *
 * This test step is used to verify if a particular application can use the "Authorise"
 * method of the UPS server. 
 * UPS server normally only allows applications with "ProtServ" to use the authorize method.
 * To give different capabilites to this test step use the program "SETCAP" to produce different clones
 * with different capabilities. 
 */
	{
	SetTestStepName(KProtServAuthorisationStep);
	}

CNonProtServAuthorisation::~CNonProtServAuthorisation()
/**
 * Destructor
 */
	{
 	__UHEAP_MARKEND;
	}

TVerdict CNonProtServAuthorisation::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Read parameters to be use for "Authorise" request to UPS".
 * (only one request is made to Ups)
 */
	{
 	__UHEAP_MARK;
 
  	TName fUseServiceUID =_L("UseServiceUID_0");
	TName fDestination = _L("Destination_0");
	TName fExpectedError = _L("ExpectedError_0");
	TName fPlatSecPass = _L("PlatSecPass_0");
	TInt serviceUID;
	
	// Reads data from inin file
	if((GetHexFromConfig(ConfigSection(), fUseServiceUID,serviceUID)
		&& GetStringFromConfig(ConfigSection(),fDestination,iDestination)
		&& GetIntFromConfig(ConfigSection(),fExpectedError,iExpectedError)
	  	&& GetBoolFromConfig(ConfigSection(),fPlatSecPass,iPlatSecPass)))
		{
		  	iServiceUID.iUid = serviceUID;
		} 


 	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CNonProtServAuthorisation::doTestStepL()
	{
	
	TInt errorReturned;
	TRequestStatus rs;
	RThread thd;
	TUpsDecision dec = EUpsDecYes;
	
    UserPromptService::RUpsSession testSession;
    UserPromptService::RUpsSubsession testSubsession;
	
	testSession.Connect();
	testSubsession.Initialise(testSession, thd);

	// Request authorization (no apaque data).
 	testSubsession.Authorise(iPlatSecPass, iServiceUID, iDestination, dec, rs);
	User::WaitForRequest(rs);
    testSession.Close(); 
    
    // Checks the error returned by ups.
     errorReturned = rs.Int()  ;
 
    if(errorReturned != iExpectedError )
    	{
    	SetTestStepResult(EFail);
    	ERR_PRINTF2(_L("Error received: %d"),errorReturned);
    	ERR_PRINTF2(_L("Error expected: %d"),iExpectedError);
    	}  

	return TestStepResult();
	}

TVerdict CNonProtServAuthorisation::doTestStepPostambleL()
	{
	return TestStepResult();
	}

CDyingClient::CDyingClient()
/**
 * Constructor
 *
 * This test step is used to simulate a client that requests a service to a
 * system server and them is terminated before the system server receives a replay
 * from ups.
 *
 * This test step although is derived from CUpsClientStep it issues only a single 
 * request to UPS, also this step doesn't talk to a test system server, it has it own
 * implementation to request to UPS for authorizations.
 * This steps has a dummy thread that is used to simulate a client, this thread is 
 * close after issuing the authorization request.  
 * 
 */
	{
	SetTestStepName(KDyingClientStep);
	}

static TInt ThreadFunction(TAny *)
	{
	return KErrNone;
	}


TVerdict CDyingClient::doTestStepL()
	{
	
	// Only uses entry 0 on ini file, it is used on this case to set properties for
	// policy evaluator and dialog creator. 	
	SetTestDataL(0);
	
	// Wait for signal from test cordinator to start test sequence.
	if (iHoldClientStepKey)
		{
		HoldClientL();
		}
	
	// Prepare dummy thread to be closed
	RThread thd;
	TRequestStatus thdStatus;
	User::LeaveIfError(thd.Create(_L("MyThreadMustDie"), ThreadFunction, 4096, 4096, 4096, 0, EOwnerThread));

	thd.Rendezvous(thdStatus);
	thd.Resume();
 	User::WaitForRequest(thdStatus);

	// Creates subsession passing dummy thread as parameter.
	UserPromptService::RUpsSession testSession;
    UserPromptService::RUpsSubsession testSubsession;
    
    
	testSession.Connect();
   	testSubsession.Initialise(testSession, thd);
	
	TServiceId serviceId = TUid::Uid(iArraySersToRequest[0].iServiceUID);
	TBool platSecPass =  iArraySersToRequest[0].iPlatSecPass;
	TUpsDecision dec = EUpsDecNo;
	TPtrC serDest;
	serDest.Set(iArraySersToRequest[0].iDestination);
	TRequestStatus rs;
  
  	// Request authorization (no opaque data).
 	testSubsession.Authorise(platSecPass, serviceId, serDest, dec, rs);
 	INFO_PRINTF4(_L("%S: SerReq DyingClient: %S  SerUID: %X "),&iTEFServerName, &serDest, &serviceId );
 	
 	// Wait for signal from test coordinator to terminate client (dummy thread)
 	if (iHoldClientStepKey)
		{
		HoldClientL();
		INFO_PRINTF2(_L("%S: hold released CDyingClient"),&iTEFServerName );
 	
		}
 	
 	// closes dummy thread.
  	thd.Close();
 	 		
 	// Signals to test coordinator that client was terminated.
 	SetClientStatusCompleteL();
	
	INFO_PRINTF2(_L("%S: Ready for request return CDyingClient"),&iTEFServerName );
	// Return point for authorise request.
	User::WaitForRequest(rs);
	INFO_PRINTF2(_L("%S: Request completed CDyingClient"),&iTEFServerName );
	
    testSession.Close();
    
    // Check results
    TInt errorReturned = rs.Int() ;
    TPtrC decisionReceived(TUpsDecisionToString(dec));

    // Check results.
  	if(errorReturned != iArraySersToRequest[0].iExpectedError ||
  		iArraySersToRequest[0].iAccessGranted != decisionReceived )
		{
		ERR_PRINTF4(_L("%S: Decision expected: %S ,Decision received: %S"),&iTEFServerName, &iArraySersToRequest[0].iAccessGranted, &decisionReceived);  
	 	ERR_PRINTF4(_L("%S: Error received: %d ,Error expected: %d"),&iTEFServerName , errorReturned , iArraySersToRequest[0].iExpectedError);
    	ERR_PRINTF2(_L("%S: Incorrect results"),&iTEFServerName);
		User::Leave(KErrGeneral);
		}
    
  	return TestStepResult();
	}
	


CCancelUpsPrompt::CCancelUpsPrompt()
/**
 * Constructor
 *
 * This test step is used to simulate a cancel request and it is to be used in conjuntion 
 * to a test coordinator. 
 *
 * This test step although is derived from CUpsClientStep it issues only a single 
 * request to UPS, also this step doesn't talk to a test system server, it has it own
 * implementation to request to UPS for authorizations.
 * 
 */
	{
	SetTestStepName(KCancelPromptStep);
	}


TVerdict CCancelUpsPrompt::doTestStepL()
	{
	
	// Only uses entry 0 on ini file, it is used on this case to set properties for
	// policy evaluator and dialog creator. 	
	SetTestDataL(0);
	
	// Wait for signal from test cordinator to start test sequence.
	if (iHoldClientStepKey)
		{
		HoldClientL();
		}
	
	// Prepare dummy thread 
	RThread thd;
	TRequestStatus thdStatus;
	User::LeaveIfError(thd.Create(_L("MyThread"), ThreadFunction, 4096, 4096, 4096, 0, EOwnerThread));

	thd.Rendezvous(thdStatus);
	thd.Resume();
 	User::WaitForRequest(thdStatus);

	// Creates subsession passing dummy thread as parameter.
	UserPromptService::RUpsSession testSession;
    UserPromptService::RUpsSubsession testSubsession;
         
	testSession.Connect(); 
   	testSubsession.Initialise(testSession, thd);
	
	TServiceId serviceId = TUid::Uid(iArraySersToRequest[0].iServiceUID);
	TBool platSecPass =  iArraySersToRequest[0].iPlatSecPass;
	TUpsDecision dec = EUpsDecNo;
	TPtrC serDest;
	serDest.Set(iArraySersToRequest[0].iDestination);
	TRequestStatus rs;
  
	// Request authorization (no opaque data).
    testSubsession.Authorise(platSecPass, serviceId, serDest, dec, rs);
    INFO_PRINTF4(_L("%S: SerReq CancelUpsPrompt: %S  ServUID: %X "),&iTEFServerName, &serDest, &serviceId );
 	
 	
 	// Wait for signal from test coordinator to terminate client (dummy thread)
 	if (iHoldClientStepKey)
		{
		HoldClientL();
		INFO_PRINTF2(_L("%S: hold released CCancelUpsPrompt"),&iTEFServerName );
		}
 	 
 	// Cancels request.
 	testSubsession.CancelPrompt();
 
 	// Signals to test coordinator that client was terminated.
 	SetClientStatusCompleteL();
	
	// Return point for authorise request.
	User::WaitForRequest(rs);
	INFO_PRINTF2(_L("%S: Request completed CCancelUpsPrompt"),&iTEFServerName );
	
    testSession.Close();
    
    thd.Close();
    
    // Check results
    TInt errorReturned = rs.Int() ;
    TPtrC decisionReceived(TUpsDecisionToString(dec));

    // Check results.
 	if(errorReturned != iArraySersToRequest[0].iExpectedError ||
 		iArraySersToRequest[0].iAccessGranted != decisionReceived )
		{
		ERR_PRINTF4(_L("%S: Decision expected: %S ,Decision received: %S"),&iTEFServerName, &iArraySersToRequest[0].iAccessGranted, &decisionReceived);  
	 	ERR_PRINTF4(_L("%S: Error received: %d ,Error expected: %d"),&iTEFServerName , errorReturned , iArraySersToRequest[0].iExpectedError);
    	ERR_PRINTF2(_L("%S: Incorrect results"),&iTEFServerName);
		User::Leave(KErrGeneral);
		}
    
  	return TestStepResult();
	}



CSimpleUpsRequest::CSimpleUpsRequest()
/**
 * Constructor
 *
 * This test step is used to simulate a cancel request and it is to be used in conjuntion 
 * to a test coordinator. 
 *
 * This test step although is derived from CUpsClientStep it issues only a single 
 * request to UPS, also this step doesn't talk to a test system server, it has it own
 * implementation to request to UPS for authorizations.
 * 
 */
	{
	SetTestStepName(KSimpleRequestStep);
	}


TVerdict CSimpleUpsRequest::doTestStepL()
	{
	
	// Only uses entry 0 on ini file, it is used on this case to set properties for
	// policy evaluator and dialog creator. 	
	SetTestDataL(0);
	
	// Prepare dummy thread 
	RThread thd;
	TRequestStatus thdStatus;
	User::LeaveIfError(thd.Create(_L("MyThread"), ThreadFunction, 4096, 4096, 4096, 0, EOwnerThread));

	thd.Rendezvous(thdStatus);
	thd.Resume();
 	User::WaitForRequest(thdStatus);

	// Creates subsession passing dummy thread as parameter.
	UserPromptService::RUpsSession testSession;
    UserPromptService::RUpsSubsession testSubsession;
         
	testSession.Connect(); 
   	testSubsession.Initialise(testSession, thd);
	
	TServiceId serviceId = TUid::Uid(iArraySersToRequest[0].iServiceUID);
	TBool platSecPass =  iArraySersToRequest[0].iPlatSecPass;
	TUpsDecision dec = EUpsDecNo;
	TPtrC serDest;
	serDest.Set(iArraySersToRequest[0].iDestination); 
	TRequestStatus rs;
  
	// Request authorization (no opaque data).
 	testSubsession.Authorise(platSecPass, serviceId, serDest, dec, rs);
 	INFO_PRINTF4(_L("%S: SerReq SimpleUpsRequest: %S  ServUID: %X "),&iTEFServerName, &serDest, &serviceId );
 	 	
 	// Return point for authorise request.
	User::WaitForRequest(rs);
	INFO_PRINTF2(_L("%S: Request completed CSimpleUpsRequest"),&iTEFServerName );
	
    testSession.Close();
    
    thd.Close();
    
    // Check results
    TInt errorReturned = rs.Int() ;
    TPtrC decisionReceived(TUpsDecisionToString(dec));

    // Check results.
 	if(errorReturned != iArraySersToRequest[0].iExpectedError ||
 		iArraySersToRequest[0].iAccessGranted != decisionReceived )
		{
		ERR_PRINTF4(_L("%S: Decision expected: %S ,Decision received: %S"),&iTEFServerName, &iArraySersToRequest[0].iAccessGranted, &decisionReceived);  
	 	ERR_PRINTF4(_L("%S: Error received: %d ,Error expected: %d"),&iTEFServerName , errorReturned , iArraySersToRequest[0].iExpectedError);
    	ERR_PRINTF2(_L("%S: Incorrect results"),&iTEFServerName);
		User::Leave(KErrGeneral);
		}
    
  	return TestStepResult();
	}

CMultiTread::CMultiTread()
/**
 * Constructor
 *
 * This test step is used to simulate a a multi thread enviroment.
 *
 * This test step although is derived from CUpsClientStep it doesn't request
 * UPS authorization it self, it transfer that responsability to another thread. 
 * 
 */
 	{
	SetTestStepName(KMultithreadStep);
	}

/**
*
* This thread is used to create a new UPS subsession and request for authorization
* Note: this therad doesn't own the UPS session.
*
*/

static TInt ThreadForSubsession(TAny *aPointer)
	{
		
	// Instantiates pointer to parent test step
	CMultiTread *testStep =(CMultiTread *) aPointer;
	
	testStep->SetTestDataL(1);
	
	// Creates another thread that will be used for the Initialise method. 
	RThread anotherThread;
	UserPromptService::RUpsSubsession testSubSession;
	
	TRequestStatus thdStatus;
	User::LeaveIfError(anotherThread.Create(_L("AnotherThread"), ThreadFunction, 4096, 4096, 4096, 0, EOwnerThread));

	anotherThread.Rendezvous(thdStatus);
	anotherThread.Resume();
 	User::WaitForRequest(thdStatus);
 	
 	// Creates new subsession to be used for initialise
 	
 	testSubSession.Initialise(testStep->iTestSession, anotherThread);
 	
 	TServiceId serviceId = TUid::Uid(testStep->iArraySersToRequest[1].iServiceUID);
 	TBool platSecPass =  testStep->iArraySersToRequest[1].iPlatSecPass;
	TUpsDecision dec = EUpsDecNo;
	TPtrC serDest;
 	serDest.Set(testStep->iArraySersToRequest[1].iDestination); 
	TRequestStatus rs;  
 	 
 	// UPS authorization, request 1.
    testSubSession.Authorise(platSecPass, serviceId, serDest, dec, rs);
  	// Return point for authorise request.
	User::WaitForRequest(rs);
	
	anotherThread.Close();
	
    // Copy results into instaves of parent test step.
    testStep->iErrorReturned_1 = rs.Int() ;
    testStep->iDecisionReceived_1.Set(testStep->TUpsDecisionToString(dec));

   	return 0;
	}


TVerdict CMultiTread::doTestStepL()
	{
	
	// Only uses entry 0 on ini file, it is used on this case to set properties for
	// policy evaluator and dialog creator. 	
	SetTestDataL(0);
	
	// Creates session. 
	iTestSession.Connect(); 
  	iTestSession.ShareAuto();

    // creates thread for request 0
    RThread thread0;
	UserPromptService::RUpsSubsession testSubSession_0;
	
	TRequestStatus thdStatus_0;
	User::LeaveIfError(thread0.Create(_L("Thread_0"), ThreadFunction, 4096, 4096, 4096, 0, EOwnerThread));

	thread0.Rendezvous(thdStatus_0);
	thread0.Resume();
 	User::WaitForRequest(thdStatus_0);
	// requests service 0
	
	testSubSession_0.Initialise(iTestSession, thread0);
 	
 	TServiceId serviceId = TUid::Uid(iArraySersToRequest[0].iServiceUID);
 	TBool platSecPass =  iArraySersToRequest[0].iPlatSecPass;
	TUpsDecision dec_0 = EUpsDecNo;
	TPtrC serDest;
 	serDest.Set(iArraySersToRequest[0].iDestination); 
	TRequestStatus rs0;  
	
	testSubSession_0.Authorise(platSecPass, serviceId, serDest, dec_0, rs0);
  	// Return point for authorise request.
	User::WaitForRequest(rs0);
	
	TInt errorReturned_0 = rs0.Int() ;
	TPtrC decisionReceived_0(TUpsDecisionToString(dec_0));
		
	// Prepares thread which creates another subsession.
	TRequestStatus thdStatus;
	User::LeaveIfError(iThd.Create(_L("ThreadForSubsession"), ThreadForSubsession, 4096, 4096, 4096, this, EOwnerThread));

	iThd.Rendezvous(thdStatus);
	iThd.Resume();
 	User::WaitForRequest(thdStatus);

    iTestSession.Close();
    thread0.Close();
    iThd.Close();
    
     // Check results from values obtained by request 0
 	if(errorReturned_0 != iArraySersToRequest[0].iExpectedError ||
 		decisionReceived_0 != iArraySersToRequest[0].iAccessGranted  )
		{
		ERR_PRINTF4(_L("%S: Decision expected: %S ,Decision received: %S"),&iTEFServerName, &iArraySersToRequest[0].iAccessGranted, &decisionReceived_0);  
	 	ERR_PRINTF4(_L("%S: Error received: %d ,Error expected: %d"),&iTEFServerName , errorReturned_0 , iArraySersToRequest[0].iExpectedError);
    	ERR_PRINTF2(_L("%S: Incorrect results"),&iTEFServerName);
		User::Leave(KErrGeneral);
		}
		
	 // Check results from values obtained by request 1
 	if(iErrorReturned_1 != iArraySersToRequest[1].iExpectedError ||
 		iDecisionReceived_1 != iArraySersToRequest[1].iAccessGranted   )
		{
		ERR_PRINTF4(_L("%S: Decision expected: %S ,Decision received: %S"),&iTEFServerName, &iArraySersToRequest[0].iAccessGranted, &iDecisionReceived_1);  
	 	ERR_PRINTF4(_L("%S: Error received: %d ,Error expected: %d"),&iTEFServerName , iErrorReturned_1 , iArraySersToRequest[0].iExpectedError);
    	ERR_PRINTF2(_L("%S: Incorrect results"),&iTEFServerName);
		User::Leave(KErrGeneral);
		}
    
  	return TestStepResult();
	}


