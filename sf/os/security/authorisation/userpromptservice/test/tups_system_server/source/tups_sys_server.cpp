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


/**
 @file  
 @test
*/
 
#include "tups_sys_server.h"
#include "tups_clientapi_sysserver.h"
#include <bacline.h>
#include "f32file.h" 


/**
 * Constructor 
 */
EXPORT_C CUPSTestServer::CUPSTestServer() : CServer2(EPriorityStandard)
,	iSessionCount(0)
	{
	}

/**
 * Destructor 
 */
EXPORT_C CUPSTestServer::~CUPSTestServer()
	{
	iUpsSession.Close(); 
	}

/**
 * @param aName - Reference to the Server name
 * StartL + initiate server logging
 * Servers can StartL themselves or call this to gain server logging.
 */
EXPORT_C void CUPSTestServer::ConstructL(const TDesC& aName)
	 {
	 StartL(aName);
	 iServerName = aName;
	 User::LeaveIfError(iUpsSession.Connect()); 
  	 }

/**
 * Last one out switch off the lights
 * Stop the active sheduler and hence the server, if this is the last session
 */	 
void CUPSTestServer::SessionClosed()
	{
	iSessionCount--;
	if (iSessionCount == 0)
		CActiveScheduler::Stop();
	}

/**
 * @param RMessage - RMessage for the session open
 * Secure version
 */
EXPORT_C CSession2* CUPSTestServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& aMessage) const
	{
	CUPSTestSession* session = CUPSTestSession::NewL(this, aMessage);
	CONST_CAST(CUPSTestServer*,this)->iSessionCount++;
	return session;
	}

CUPSTestSession* CUPSTestSession::NewL(const CUPSTestServer *aServer, const RMessage2& aMessage)
{
	CUPSTestSession *self = new (ELeave) CUPSTestSession;
	CleanupStack::PushL(self);
	self->ConstructL(const_cast<CUPSTestServer *>(aServer), aMessage);
	CleanupStack::Pop(self);
	return self;
}

void CUPSTestSession::ConstructL(CUPSTestServer *aServer, const RMessage2& aMessage)
{
    RThread client;
    CleanupClosePushL(client);
    aMessage.ClientL(client);
	User::LeaveIfError(iUpsSubsession.Initialise(aServer->iUpsSession,client));
	CleanupStack::PopAndDestroy(&client);
}

/**
 * Constructor
 */
EXPORT_C CUPSTestSession::CUPSTestSession()
	{
	}

/**
 * Destructor
 */
EXPORT_C CUPSTestSession::~CUPSTestSession()
	{
	iUpsSubsession.Close();
	 
	CUPSTestServer* p=(CUPSTestServer*) Server();
	// Shuts Down the server if this is the last open session
	p->SessionClosed();
	}

/**
 * @param aMessage - Function and data for the session
 * Session was created by pure virtual CTestServer::NewSessionL()
 * Message Functions defined in TestExecuteClient.h
 * 
 * EOpenTestStep - Creates a new subsession
 * ERunTestStep - Executes the test step asynchronously
 * EAbortTestStep - Kill()'s the executing test step
 * ECloseTestStep - Free's the resource
 *
 * Secure and non-secure variants
 * There are two modes of operation:
 * Test step is opened with the shared data boolean set to FALSE:
 *		Create a new CStepControl instance and hence a new thread in its own heap
 *		Consecutive or Concurrent operation
 * Test step is opened with the shared data boolean set to TRUE:
 *		Create a CPersistentStepControl and keep reusing it, and its thread
 *		Consecutive operation only
 */

EXPORT_C void CUPSTestSession::ServiceL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
 		case EUpsTest_OpenSession:
			{
			aMessage.Complete(KErrNone);
			}
			break;

		case EUpsTest_UseTestService :
			{
			// load test data from first argument.
			TPckg<TUpsTestServiceRequestData> compoundRequest(iRequestData);
			aMessage.ReadL(0, compoundRequest);
			
		 	makeRequestToUPsL(aMessage);
			}
			break;
			
		case EUpsTest_GetServerSid :
			{
			TPckgBuf<TInt> sidBuf;
			RThread server;
			sidBuf() =server.SecureId();
			server.Close();
			
	 	    aMessage.WriteL(0,sidBuf);
			aMessage.Complete(KErrNone);
			}
			break;

		case EUpsTest_CloseSession :
			{ 
			aMessage.Complete(KErrNone);
			}
			break;

		default:
			break;
		}
	}

void CUPSTestSession::CTUPSTesterActiveComplete(CTUPSTesterActive *aTester)
{
	ASSERT(aTester == iUpsTestActiveObj);
	iUpsTestActiveObj = 0;
	// no much use to this, but it removes overnight build warnings.
	(void)aTester;
}

CUPSTestServer* CUPSTestServer::NewL(const TDesC& serverName)
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CUPSTestServer * server = new (ELeave) CUPSTestServer();
	CleanupStack::PushL(server);

	server->ConstructL(serverName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
/**
 * Main implementation
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
	CUPSTestServer* server = NULL;
	// Create the CTestServer derived server
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	TPtrC exeName = args->Arg(0);
	TParse FullName;
	FullName.Set(exeName, NULL, NULL); 
		
	TRAPD(err,server = CUPSTestServer::NewL(FullName.Name()));

	CleanupStack::PopAndDestroy(args);
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

EXPORT_C void CUPSTestSession::makeRequestToUPsL(const RMessage2& aMessage)
	{
	if(iUpsTestActiveObj)
	{
		User::Leave(KErrServerBusy);
	}

 	iUpsTestActiveObj = new (ELeave) CTUPSTesterActive(aMessage);

	iUpsTestActiveObj->doActiveUpsRequest(this);
	}
