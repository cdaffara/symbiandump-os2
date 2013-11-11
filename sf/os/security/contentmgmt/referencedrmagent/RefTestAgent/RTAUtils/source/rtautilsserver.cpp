/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "rtautilsserver.h"
#include "rtautilsstep.h"
#include "archivestep.h"
#include "rightsDbStep.h"
#include "datastep.h"
_LIT(KServerName,"rtautils");

CRTAUtilsServer* CRTAUtilsServer::NewL()
	{
	CRTAUtilsServer * server = new (ELeave) CRTAUtilsServer();
	CleanupStack::PushL(server);
	User::LeaveIfError(server->iFs.Connect());

	// Make the file session sharable
	User::LeaveIfError(server->iFs.ShareProtected());

	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/*
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
	CRTAUtilsServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CRTAUtilsServer::NewL());
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
/*
 * return standard error code on exit
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

CTestStep* CRTAUtilsServer::CreateTestStep(const TDesC& aStepName)
/*
 * return a CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	if(aStepName == KCreateDrmArchive)
		testStep = new CCreateDrmArchive(*this);
	else if(aStepName == KOpenCorruptedFile)
		testStep = new COpenCorruptedFile(*this);
	else if(aStepName == KContainerUniqueIdBdrChk)
		testStep = new CContainerUniqueIdBdrChk(*this);
	else if(aStepName == KContentNameBdrChk)
		testStep = new CContentNameBdrChk(*this);
	else if(aStepName == KContentUniqueIdBdrChk)
		testStep = new CContentUniqueIdBdrChk(*this);

	else if(aStepName == KContainerNoUniqueId)
		testStep = new CContainerNoUniqueId(*this);
	else if(aStepName == KContentNoUniqueId)
		testStep = new CContentNoUniqueId(*this);
	else if(aStepName == KContainerEmptyUniqueId)
		testStep = new CContainerEmptyUniqueId(*this);
	else if(aStepName == KContentEmptyUniqueId)
		testStep = new CContentEmptyUniqueId(*this);

	else if(aStepName == KDumpDrmArchive)
		testStep = new CDumpDrmArchive(*this);
	else if(aStepName == KImportDrmArchive)
		testStep = new CImportDrmArchive(*this);
	else if(aStepName == KClearRightsDb)
		testStep = new CClearRightsDb(*this);
	else if(aStepName == KDumpRightsDb)
		testStep = new CDumpRightsDb(*this);
	else if(aStepName == KLoadOldDb)
		testStep = new CLoadOldDb(*this);
	
	else if(aStepName == KMultipartContent)
		testStep = new CMultipartContent(*this);
	else if(aStepName == KBadFileHandleCheck)
			testStep = new CBadFileHandleCheck(*this);
	
	return testStep;
	}
