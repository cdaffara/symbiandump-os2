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
* Name		: CTe_RegScriptCommandServer.cpp
* Author	  : Jason Zhou
*
*/



#include "te_regscriptcommandserver.h"
#include "accessstep.h"
#include "globalsharestep.h"
#include "starttimestep.h"
#include "checktimestep.h"
#include "checklogfilestep.h"
#include "checkprogramstep.h"
#include "checkfolderandfilestep.h"

_LIT(KServerName, "Te_RegScriptCommandServer");

CTe_RegScriptCommandServer::CTe_RegScriptCommandServer()
	{
	// No implementation required
	}

CTe_RegScriptCommandServer::~CTe_RegScriptCommandServer()
	{
	}

CTe_RegScriptCommandServer* CTe_RegScriptCommandServer::NewLC()
	{
	CTe_RegScriptCommandServer* self = new (ELeave) CTe_RegScriptCommandServer();
	CleanupStack::PushL(self);
	self->ConstructL(KServerName);
	return self;
	}

CTe_RegScriptCommandServer* CTe_RegScriptCommandServer::NewL()
	{
	CTe_RegScriptCommandServer* self = CTe_RegScriptCommandServer::NewLC();
	CleanupStack::Pop(); // self;
	return self;
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
	CTe_RegScriptCommandServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CTe_RegScriptCommandServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// Main Entry point
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

// __EDIT_ME__ - Use your own server class name
CTestStep* CTe_RegScriptCommandServer::CreateTestStep(const TDesC& aStepName)
/**
 * @aStepName - Provided along with script command
 * This is the name of the Test Step to be executed
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// __EDIT_ME__ - Create your own test steps here
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	if(aStepName == KGlobalShareStep1)
		{
		testStep = new CGlobalShareStep1();
		}
	
	else if(aStepName == KGlobalShareStep2)
		{
		testStep = new CGlobalShareStep2();
		}
		
	else if(aStepName == KAccessStep1)
		{
		testStep = new CAccessStep1();
		}
	
	else if(aStepName == KAccessStep2)
		{
		testStep = new CAccessStep2();
		}
	
	else if (aStepName==KStartTimeStep())
		{
		testStep = new (ELeave) CStartTimeStep(this);
		}

	else if (aStepName==KCheckTimeStep())
		{
		testStep = new (ELeave) CCheckTimeStep(this);
		}
	
	else if (aStepName==KCheckLogFileStep())
		{
		testStep = new (ELeave) CCheckLogFileStep();
		}
	
	else if (aStepName==KCheckProgramStep())
		{
		testStep = new (ELeave) CCheckProgramStep();
		}
	
	else if (aStepName==KCheckFolderAndFileStep())
		{
		testStep = new (ELeave) CCheckFolderAndFileStep();
		}
	
	return testStep;
	}
