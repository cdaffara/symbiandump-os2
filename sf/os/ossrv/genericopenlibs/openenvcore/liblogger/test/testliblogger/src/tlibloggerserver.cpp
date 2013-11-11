// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tlibloggerserver.cpp
// 
//


#include <c32comm.h>

#if defined (__WINS__)
#define PDD_NAME		_L("ECDRV")
#else
#define PDD_NAME		_L("EUART1")
#define PDD2_NAME		_L("EUART2")
#define PDD3_NAME		_L("EUART3")
#define PDD4_NAME		_L("EUART4")
#endif

#define LDD_NAME		_L("ECOMM")

/**
 * @file
 *
 * Pipe test server implementation
 */
#include "tliblogger.h"
#include "tlibloggerserver.h"


_LIT(KServerName, "tliblogger");
_LIT(KTxtExampleCode,"Binary Compatibility - 1");
_LIT(KTxtPressAnyKey," [press any key]");


 LOCAL_D CConsoleBase* console; // write all your messages to this


CLibLoggerTestServer * CLibLoggerTestServer ::NewL()
	{
	CLibLoggerTestServer  *server = new(ELeave) CLibLoggerTestServer ();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

static void InitCommsL()
    {
    TInt ret = User::LoadPhysicalDevice(PDD_NAME);
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);

#ifndef __WINS__
    ret = User::LoadPhysicalDevice(PDD2_NAME);
    ret = User::LoadPhysicalDevice(PDD3_NAME);
    ret = User::LoadPhysicalDevice(PDD4_NAME);
#endif

    ret = User::LoadLogicalDevice(LDD_NAME);
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);
    ret = StartC32();
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);
    }

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	//InitCommsL();
	
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CLibLoggerTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CLibLoggerTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


void ConstrutEnvL()
{
	console=Console::NewL(KTxtExampleCode,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); // get and ignore character
	CleanupStack::PopAndDestroy(); // close console
}
#ifdef EKA1
#ifdef __WINS__

// @anchor E32Dll
GLDEF_C TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}

#else  // __ARMI__

// @anchor E32Main
TInt E32Main()
	{
	return ConstrutEnvL(NULL);
	}
#endif
#else
IMPORT_C TInt WinsMain();
EXPORT_C TInt WinsMain()
	{
	return reinterpret_cast<TInt>(&ConstrutEnvL);
	}
/**
 * Server entry point
 * @return Standard Epoc error code on exit
 */
TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL) 
		{
		return KErrNoMemory;  
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

#endif

CTestStep* CLibLoggerTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == KLibloggerTest)
		{
		testStep = new CTestLibLogger(aStepName);
		}
	return testStep;
	}



