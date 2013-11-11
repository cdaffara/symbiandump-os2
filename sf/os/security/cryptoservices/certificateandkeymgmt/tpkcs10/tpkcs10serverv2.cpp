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
* tpkcs10minortests.cpp
*
*/


#include "tpkcs10serverv2.h"
#include "tcertrequeststep.h"
#include "tpkcs10negatives.h"
#include "tpkcs10minortests.h"

_LIT(KServerName,"tpkcs10serverv2");
CTPKCS10Server* CTPKCS10Server::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTPKCS10Server * server = new (ELeave) CTPKCS10Server();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTPKCS10Server* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTPKCS10Server::NewL());
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
 * Secure variant only
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


CTestStep* CTPKCS10Server::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
              if(aStepName == KCertificateRequestStep)
              {
              	testStep = new CCertificateRequestStep();
              	return testStep;	
              }
              else if(aStepName == KPKCS10ReuseAttributeStep)
              {
              	testStep = new CPKCS10ReuseAttributeStep();
              	return testStep;
              }
              else if(aStepName == KPKCS10ReuseChallengePasswordAttr)
              {
              	testStep = new CPKCS10ReuseChallengePasswordAttrStep();
              	return testStep;
              }
              else if(aStepName == KPKCS10ReuseExtensionAttr)
              {
              	testStep = new CPKCS10ReuseExtensionAttrStep();
              	return testStep;
              }
              else if(aStepName == KPKCS10NegPKCS10GenericAttr)
              {
              	testStep = new CPKCS10NegPKCS10GenericAttr();
              	return testStep;
              }
              else if (aStepName == KPKCS10NegPKCS9ChallengePasswordAttr)
              {
                testStep = new CPKCS10NegPKCS9ChallengePasswordAttr();
                return testStep;
              }
			  else if (aStepName == KPKCS10NegPKCS9ExtensionRequestAttr)
			  {
                testStep = new CPKCS10NegPKCS9ExtensionRequestAttr();
                return testStep;
			  }
			  else if (aStepName == KPKCS10NegPKCS10Attributes)
			  {
                testStep = new CPKCS10NegPKCS10Attributes();
                return testStep;
			  }
			  else if (aStepName == KPKCS10NegPKCS10Request)
			  {
                testStep = new CPKCS10NegPKCS10Request();
                return testStep;
			  }
			  else if (aStepName == KPKCS10CancelRequestA)
			  {
                testStep = new CPKCS10CancelRequestAStep();
                return testStep;
			  }

		
	return testStep;
	}
