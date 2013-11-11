/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* Example file/test code to demonstrate how to develop a TestExecute Server
* Developers should take this project as a template and substitute their own
* code at the __EDIT_ME__ tags
* for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
* in the process of the client. The client initialises the server by calling the
* one and only ordinal.
*
*/



/**
 @file CTEFUnitServer.cpp
*/
#include "ctefunitserver.h"
#include "ctestcommand.h"
#include "tefunit.h"

_LIT(KTEFUnitStepNamePrefix, "tefunit.");
GLREF_D const TTestName ServerName();
GLREF_D CTestSuite* CreateTestSuiteL();
GLREF_D CTestStep* CreateTEFTestStep(const TDesC& aStepName, CTEFUnitServer& aServer);

// __EDIT_ME__ - Use your own server class name
CTEFUnitServer* CTEFUnitServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	// __EDIT_ME__ new your server class here
	CTEFUnitServer * server = new (ELeave) CTEFUnitServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.

	//server->StartL(ServerName());
	server->ConstructL(ServerName());
	server->iSuite = CreateTestSuiteL();
	CleanupStack::Pop(server);
	return server;
	}

CTEFUnitServer::~CTEFUnitServer()
	{
	delete iSuite;
	iSuite = NULL;
	}

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
	// __EDIT_ME__ Your server name
	CTEFUnitServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CTEFUnitServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	server = NULL;
	delete sched;
//	sched = NULL:
	}


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
	cleanup = NULL;
	return KErrNone;
    }

// Create a thread in the calling process

// __EDIT_ME__ - Use your own server class name
CTestStep* CTEFUnitServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	TBuf<9> stepNamePrefix;
	if (aStepName.Length() > KMinTEFUnitStepNameSize)
		{
		stepNamePrefix.Copy(aStepName.Mid(0,KMinTEFUnitStepNameSize));
		stepNamePrefix.LowerCase();
		}

	// If StepName prefix with TEFUnit. Create a CTestCommand here with the aStepName passed in
		//   note: aStepName will actually be the path of the the test
		//         in the suite hirarchy
	// Else Call Global CreateTEFTestStep() which proceeds with legacy Test Step creation
		// Refer Example TEFUnitTE test component for implementation
		// CreateTEFTestStep() should return NULL if TEF steps are not used
	if (stepNamePrefix.Length() && stepNamePrefix == KTEFUnitStepNamePrefix)
		{
		TTestPath aStepPath(aStepName.Mid(KMinTEFUnitStepNameSize));
		testStep = new CTestCommand( aStepPath, iSuite );
		}
	else
		{
		testStep = CreateTEFTestStep(aStepName, *this);
		}

	return testStep;
	}

