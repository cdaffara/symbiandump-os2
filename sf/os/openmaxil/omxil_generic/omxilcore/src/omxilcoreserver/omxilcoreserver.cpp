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
//

#include "log.h"
#include "omxilcoreserver.h"
#include "omxilcoreserversession.h"
#include "omxilcoreclientserver.h"
#include "omxilcore.h"
#include <ecom/ecom.h>


/**
 * Constructor
 */
COmxILCoreServer::COmxILCoreServer()
:  CServer2(EPriorityStandard)
	{
    DEBUG_PRINTF(_L8("COmxILCoreServer::COmxILCoreServer"));
	Cancel();
	}

/**
 * Destructor.
 */
COmxILCoreServer::~COmxILCoreServer()
	{
    DEBUG_PRINTF(_L8("COmxILCoreServer::~COmxILCoreServer"));
	delete ipOmxILCore;
	delete ipStopCallback;
	}

/**
 * Constructs, and returns a pointer to, a new COmxILCoreServer object.
 * Leaves on failure.
 * @return COmxILCoreServer* A pointer to newly created utlitly object.
 */
COmxILCoreServer* COmxILCoreServer::NewL()
	{
    DEBUG_PRINTF(_L8("COmxILCoreServer::NewL"));
	COmxILCoreServer* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

/**
 * Constructs, leaves object on the cleanup stack, and returns a pointer
 * to, a new OMX IL Core Server object.
 * Leaves on failure.
 * @return COmxILCoreServer* A pointer to newly created utlitly object.
 */
COmxILCoreServer* COmxILCoreServer::NewLC()
	{
    DEBUG_PRINTF(_L8("COmxILCoreServer::NewLC"));
	COmxILCoreServer* self = new(ELeave) COmxILCoreServer;
	CleanupStack::PushL(self);
	self->ConstructL();

	return self;
	}

/**
   Symbian 2nd phase constructor.
 */
void COmxILCoreServer::ConstructL()
	{
    DEBUG_PRINTF(_L8("COmxILCoreServer::ConstructL"));

	// Create the OpenMAX IL Core
	ipOmxILCore = new (ELeave) COmxILCore;

	// Create the server termination callback object...
	TCallBack callbackStop (COmxILCoreServer::StopServer, this);
	ipStopCallback =
		new (ELeave) CAsyncCallBack (callbackStop, CActive::EPriorityLow);

	}

/**
   From CServer2. Creates a server-side client session object.  Creates a new
  session.  This function may leave with one of the system-wide error codes.

  @param const TVersion& aVersion The version number of the session.

  @param const RMessage2& aMessage

  @return A pointer to the new session.
 */
CSession2* COmxILCoreServer::NewSessionL(const TVersion& aVersion,
										 const RMessage2& aMessage) const
	{

	RThread clientThread;
	aMessage.Client(clientThread);
	RProcess clientProcess;
	clientThread.Process(clientProcess);
	RProcess thisProcess;

    DEBUG_PRINTF3(_L8("COmxILCoreServer::NewSessionL : this process [%d] - client process [%d]"), thisProcess.Id().operator TUint(), clientProcess.Id().operator TUint());

	// Only allow sessions from clients running in the server process
	if (thisProcess.Id() != clientProcess.Id())
		{
		clientThread.Close();
		clientProcess.Close();
		thisProcess.Close();
		User::Leave(KErrAccessDenied);
		}

	clientThread.Close();
	clientProcess.Close();
	thisProcess.Close();

	if(!User::QueryVersionSupported(TVersion(KOmxILCoreServerVersion,
											 KOmxILCoreServerMinorVersionNumber,
											 KOmxILCoreServerBuildVersionNumber),
									aVersion))
		{
		User::Leave(KErrNotSupported);
		}

	if (iDeinitInProgress)
		{
		User::Leave(KErrNotReady);
		}

	COmxILCoreServerSession* omxilcoreSession =
		COmxILCoreServerSession::NewL(*ipOmxILCore,
									  const_cast<COmxILCoreServer&>(*this));

	return omxilcoreSession;
	}

TInt COmxILCoreServer::StopServer(TAny* aPtr)
	{
    DEBUG_PRINTF(_L8("COmxILCoreServer::StopServer"));
	COmxILCoreServer* self =
		static_cast<COmxILCoreServer*> (aPtr);
	self->DoStopServer();
	return KErrNone;

	}

void COmxILCoreServer::DoStopServer()
	{
    DEBUG_PRINTF(_L8("COmxILCoreServer::DoStopServer"));
	CActiveScheduler::Stop();
	}


void COmxILCoreServer::SetDeinitInProgress()
	{
    DEBUG_PRINTF(_L8("COmxILCoreServer::SetDeinitInProgress"));

	iDeinitInProgress = ETrue;

	}

void COmxILCoreServer::SessionOpened()
	{
    DEBUG_PRINTF(_L8("COmxILCoreServer::SessionOpened"));

	iSessionCounter++;

	}


void COmxILCoreServer::SessionClosed()
	{
    DEBUG_PRINTF(_L8("COmxILCoreServer::SessionClosed"));

	iSessionCounter--;

	if (iSessionCounter == 0 && iDeinitInProgress)
		{
		DEBUG_PRINTF(_L8("COmxILCoreServer::SessionClosed : iSessionCounter = 0 and DeinitInProgress = TRUE"));
		ipStopCallback->Call();
		}

	}


TInt COmxILCoreServer::ThreadFunctionL(TAny* aServerHandle)
	{
	// create an active scheduler and server
	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);

	//Install the active scheduler
	CActiveScheduler::Install(sched);

	COmxILCoreServer* server = COmxILCoreServer::NewL();
	CleanupStack::PushL(server);

	// Start the server
    TInt err = server->Start(KNullDesC);
	if (err != KErrNone)
		{
		return err;
		}

	RServer2 serverHandle = server->Server();
	User::LeaveIfError(serverHandle.Duplicate(RThread(), EOwnerProcess));
	// Return the handle of the server to function which created us.
	*reinterpret_cast<TUint32 *>(aServerHandle) = serverHandle.Handle();
	
	// Let everyone know that we are ready to
	// deal with requests.
	RThread::Rendezvous(KErrNone);

	// And start fielding requests from client(s).
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2, sched); // sched, server

	return KErrNone;
	}

/**
   Create the thread that will act as the server.

*/
TInt COmxILCoreServer::ThreadFunction(TAny* aServerHandle)
	{
    DEBUG_PRINTF(_L8("COmxILCoreServer::ThreadFunction"));
	// get clean-up stack
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}

	TRAPD( err, ThreadFunctionL(aServerHandle) );

	REComSession::FinalClose();

	delete cleanup;
    DEBUG_PRINTF(_L8("COmxILCoreServer::ThreadFunction: returning"));
	return err;
	}

/**
   Create the thread that will act as the server.  This function is exported
   from the DLL and called by the client.

*/
EXPORT_C TInt StartOmxILCoreServer(TUint32 *aServerHandle)
	{
    DEBUG_PRINTF(_L8("StartOmxILCoreServer"));

	TInt res = KErrNone;
	RThread serverThread;

	// Create the thread for the server.
    res = serverThread.Create(KNullDesC,
							  COmxILCoreServer::ThreadFunction,
							  KOmxILCoreServerStackSize,
							  &User::Heap(),
							  aServerHandle);

	if (res==KErrNone)
		{
		DEBUG_PRINTF(_L8("StartOmxILCoreServer : Thread created"));
		TRequestStatus rendezvousStatus;

		serverThread.SetPriority(EPriorityNormal);

		// Synchronise with the server
		serverThread.Rendezvous(rendezvousStatus);
		if (rendezvousStatus != KRequestPending)
			{
			serverThread.Kill(KErrDied);
			}
		else
			{
			serverThread.Resume();
			}

		User::WaitForRequest(rendezvousStatus);
		if(rendezvousStatus != KErrNone)
			{
			res = rendezvousStatus.Int();
			}

		}
	else
		{
		// The thread could not be created..
		DEBUG_PRINTF2(_L8("StartOmxILCoreServer : Thread Creation error [%d]"), res);
		res = KErrGeneral;
		}

	serverThread.Close();

	return res;

	}

// End of file
