// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements the test server of the Streaming CAF test harness
// 
//

#include "tscafserver.h" 			// TEF Test Server
#include "tscafstep.h" 				// TEF Steps Test Code
#include "tstaunittest.h"
#include "tkeystreamsink.h"
#include "tpostacquisition.h"
#include "tipsec.h"
#include "treferenceagent.h"
#include "startupcommsstep.h"

// Consumer interface test steps
#include "tkeystreamdecoderconstruct.h"
#include "tkeystreamdecoderattributes.h"
#include "tmultikeystreamdecoders.h"

// Utility Steps
#include "tsetuprightsobjects.h"

CScafServer* CScafServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{	
	CScafServer* server = new (ELeave) CScafServer();
	CleanupStack::PushL(server);
	User::LeaveIfError(server->iFs.Connect());
	User::LeaveIfError(server->iFs.ShareProtected());
	User::LeaveIfError(server->iFs.CreatePrivatePath(server->iFs.GetSystemDrive()));
	server->ConstructL(KStaServerName);
	CleanupStack::Pop(server);
	return server;
	}
	
CScafServer::~CScafServer()
/**
 	Destructor.
 */
	{
	iFs.Close();	
	}

LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	
	CActiveScheduler *sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CScafServer* server = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,server = CScafServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
		
	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }
	

CTestStep* CScafServer::CreateTestStep(const TDesC& aStepName)
/**
	Creates a new test step.
	@param aStepName The name of the requested test step.
	@return A pointer to the object of the requested test step.
 */
	{
	// Initialise test step object to NULL if no TEF steps are assigned
	CTestStep* testStep = NULL;

	if(aStepName == KScafTestAgentServerStep)
		testStep = new CScafTestAgentServer(*this);
	else if (aStepName == KScafKeyStreamSinkStep)
		testStep = new CScafKeyStreamSink(*this);
	else if (aStepName == KScafConcurrentKeyStreamSinksStep)
		testStep = new CScafConcurrentKeyStreamSinks(*this);
	else if (aStepName == KScafPostAcquisitionStep)
		testStep = new CScafPostAcquisition(*this);
	else if (aStepName == KScafSetupRightsObjectsStep)
		testStep = new CScafSetupRightsObjects(*this);
	else if (aStepName == KScafKeyStreamDecoderConstruct)
		testStep = new CScafKeyStreamDecoderConstruct(*this);
	else if (aStepName == KScafKeyStreamDecoderAttributes)
		testStep = new CScafKeyStreamDecoderAttributes(*this);
	else if (aStepName == KScafMultiKeyStreamDecoders)
		testStep = new CScafMultiKeyStreamDecoders(*this);
	else if (aStepName == KStartupCommsStep)
		testStep = new CStartupCommsStep();
	// IPSec tests
	else if (aStepName == KScafIpSec)
		testStep = new CScafIpSec(*this);	
	else if (aStepName == KScafIpSecDecoderIntegration)
		testStep = new CScafIpSecDecoderIntegration(*this);	
	else if (aStepName == KScafIpSecSadbVerification)
		testStep = new CScafIpSecSadbVerification(*this);
	else if (aStepName == KScafIpSecSadbVerificationConcurrency)
		testStep = new CScafIpSecSadbVerificationConcurrency(*this);
	else if (aStepName == KScafIpSecNegative)
		testStep = new CScafIpSecNegative(*this);		
	else if (aStepName == KScafIpSecMultipleSinks)
		testStep = new CScafIpSecMultipleSinks(*this);	
	else if (aStepName == KScafIpSecAlgorithmChange)
		testStep = new CScafIpSecAlgorithmChange(*this);	
	else if (aStepName == KScafIpSecSARemoval)
		testStep = new CScafIpSecSARemoval(*this);	
	//Reference agent test
	else if (aStepName == KScafRefAgent)
		testStep = new CScafRefAgent(*this);		

	return testStep;
	}
