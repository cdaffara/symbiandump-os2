/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tpkcs7server.h"
#include "tpkcs7step.h"
#include "tcmsstep.h"

_LIT(KServerName,"tPKCS7Server");

CTPKCS7Server* CTPKCS7Server::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTPKCS7Server * server = new (ELeave) CTPKCS7Server();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
RFs& CTPKCS7Server::Fs()
		{
		return iFs;
		};

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTPKCS7Server* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTPKCS7Server::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// Only a DLL on emulator for typhoon and earlier

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

CTestStep* CTPKCS7Server::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
     CTPKCS7StepBase* testStep = NULL;

   	  	
	// __EDIT_ME__ - Create your own test steps here
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	if(aStepName == KTPKCS7CorruptTest)
		testStep = new CTPKCS7CorruptTest();
	else if(aStepName == KTPKCS7ValidTest)
		testStep = new CTPKCS7ValidTest();
	else if(aStepName == KTPKCS7ContentTest)
		testStep = new CTPKCS7CorruptTest();
	else if(aStepName == KTPKCS7CertificateTest)
		testStep = new CTPKCS7CertificateTest();
	else if(aStepName == KTPKCS7SignerTest)
		testStep = new CTPKCS7SignerTest();
	else if(aStepName == KTPKCS7EncryptedDataTest)
	    testStep = new CTPKCS7EncryptedDataTest();
	else if(aStepName == KTPKCS7DigestInfoTest)
	    testStep = new CTPKCS7DigestInfoTest();
	else if(aStepName == KTPKCS7EncryptedDataCorruptTest)
	    testStep = new CTPKCS7EncryptedDataCorruptTest();
	else if(aStepName == KTPKCS7DigestInfoCorruptTest)
	    testStep = new CTPKCS7DigestInfoCorruptTest();
	else if(aStepName == KTPKCS7OOMTest)
	   testStep = new CTPKCS7OOMTest();
			
 	if (testStep)
 		{
 		testStep->SetServer(this);
 		}
 	
 	CTestStep* cmsTestStep(NULL);	
 	if (aStepName == KTCmsSignedDataTest)
	 	{
	 	cmsTestStep = new CTCmsSignedDataStep();
	 	return cmsTestStep;
	 	}
	 else if (aStepName == KTCmsSignedDataDecodingTest)
				{
				cmsTestStep = new CTCmsSignedDataDecodingStep();
				return cmsTestStep;		 	
				}
		   else if (aStepName == KTCmsDataTest)
				    {
					cmsTestStep = new CTCmsDataStep();
					return cmsTestStep;			   	
				    }
			    else if (aStepName == KTCmsDataDecodingTest)
					    {
						cmsTestStep = new CTCmsDataDecodingStep();
						return cmsTestStep;			   	
					    }
					 else if (aStepName == KTCmsContentInfoDecodingTest)
								{
								cmsTestStep = new CTCmsContentInfoDecodingStep();
								return cmsTestStep;			   	
							 	}
					 
				
/*	 	
	else if (aStepName == KTPKCS7SignedDataTest)
			{
		 	cmsTestStep = new CTPKCS7SignedDataStep();
		 	return cmsTestStep;
			}
*/	
	 	
	return testStep;
	}

