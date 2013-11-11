// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @released
*/

#include "t_burtestserver.h"
#include "t_teststepbackup.h"
#include "t_teststeprestore.h"
#include "t_teststepbackupasync.h"
#include "t_teststeprestoreasync.h"

_LIT( KServerName, "BURTestServer");

using namespace conn;

namespace bur_ts
	{

	CBURTestServer* CBURTestServer::NewL()
		/**
		Symbian OS Constructor
		Called inside the MainL() function to create and start the
		CTestServer derived server.
		
		@internalComponent
		@released
		
		@return - A pointer to an instance of the test server
		*/
		{
		CBURTestServer* testServer = new (ELeave) CBURTestServer();
		CleanupStack::PushL(testServer);
		
		// Either use a StartL or ConstructL, the latter will permit
		// Server Logging.
		//server->StartL(KServerName); 
		testServer->ConstructL(KServerName);
		CleanupStack::Pop(testServer);
		return testServer;
		}

	CBURTestServer::~CBURTestServer()
		/**
		C++ Destructor
		
		@internalComponent
		@released
		*/
		{}
	
	CTestStep* CBURTestServer::CreateTestStep(const TDesC& aStepName)
		/**
		Implementation of CTestServer pure virtual
		Creates and returns a bur_ts::CBURTestStepBase object
		
		@internalComponent
		@released
		
		@return - A CTestStep derived instance
		*/
		{
		CBURTestStepBase* testStep = NULL;
		
		// Create a test step based on the string name passed:
		TInt error = KErrNone;
		if (aStepName == KBURTestBackup)
			{
			TRAP(error, testStep = CBURTestStepBackup::NewL(*this));
			}
		else if (aStepName == KBURTestRestore)
			{
			TRAP(error, testStep = CBURTestStepRestore::NewL(*this));
			}
		else if (aStepName == KBURTestBackupAsync)
			{
			TRAP(error, testStep = CBURTestStepBackupAsync::NewL(*this))
			}
		else
			{
			TRAP(error, testStep = CBURTestStepRestoreAsync::NewL(*this));
			}
			
		if (error != KErrNone)
			{
			User::Leave(error);
			}

		return testStep;
		}
	
	}	// end namespace

//

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
	/**
	Much simpler, uses the new Rendezvous() call to sync with the client
	*/
	{
	// Leave the hooks in for platform security
	#if (defined __DATA_CAGING__)
		RProcess().DataCaging(RProcess::EDataCagingOn);
		RProcess().SecureApi(RProcess::ESecureApiOn);
	#endif
	
	// Rename the thread:
	User::RenameThread(KServerName);
	
	CActiveScheduler* scheduler = NULL;
	scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	

	bur_ts::CBURTestServer* testServer = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err, testServer = bur_ts::CBURTestServer::NewL());
	if(!err)
		{
		CleanupStack::PushL(testServer);
		
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		CleanupStack::Pop(testServer);
		}
	
	CleanupStack::Pop(scheduler);
	delete testServer;
	delete scheduler;
	}



// Only a DLL on emulator for typhoon and earlier

GLDEF_C TInt E32Main()
	/**
	@return - Standard Epoc error code on exit
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

// Create a thread in the calling process
// Emulator typhoon and earlier

