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
* fstokenserver.cpp
*
*/


#include "fsserver.h"
#include "fstokenutil.h"
#include "cfskeystoreserver.h"
#include "CKeyStoreSession.h"
#include "filecertstore.h"
#include "CCertStoreSession.h"
#include "CFSCertAppsServer.h"
#include "CCertAppsSession.h"
#include "FSResources.h"
#include "FSDialog.h"
#include "tokenserverdebug.h"
#include "fstokenservername.h"

// CTokenServerSession /////////////////////////////////////////////////////////

CTokenServerSession::CTokenServerSession()
	{
	}

inline CTokenServer& CTokenServerSession::Server()
	{
	return static_cast<CTokenServer&>(const_cast<CServer2&>(*CSession2::Server()));
	}

void CTokenServerSession::CreateL()
	{
	Server().AddSession();
	}

CTokenServerSession::~CTokenServerSession()
	{
	Server().DropSession();
	}

/**
 * Handle a client request.  Leaving is handled by
 * CTokenServerSession::ServiceError() which reports the error code to the
 * client.
 */
void CTokenServerSession::ServiceL(const RMessage2& aMessage)
	{
#ifdef _DEBUG
	// OOM testing
	switch (aMessage.Function())
		{
		case EStartOOMTest:
			TokenServerDebug::StartOOMTest();
			aMessage.Complete(KErrNone);
			return;
			
		case EIncHeapFailPoint:
			TokenServerDebug::IncHeapFailPoint();
			aMessage.Complete(KErrNone);
			return;
			
		case EResetHeapFail:
			TokenServerDebug::ResetHeapFail();
			aMessage.Complete(KErrNone);
			return;

		case EAllocCount:
			aMessage.Complete(User::CountAllocCells());
			return;
		}	
#endif
	
	DoServiceL(aMessage);
	}

/**
 * Handle an error from ServiceL() A bad descriptor error implies a badly
 * programmed client, so panic it - otherwise report the error to the client.
 */
void CTokenServerSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	if (aError==KErrBadDescriptor)
		{
		PanicClient(aMessage, EPanicBadDescriptor);
		}
	
	CSession2::ServiceError(aMessage, aError);
	}

// CTokenServer ////////////////////////////////////////////////////////////////

inline CTokenServer::CTokenServer() :
	CServer2(0, ESharableSessions)
	{
	}

CServer2* CTokenServer::NewLC()
	{
	CTokenServer* self=new(ELeave) CTokenServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTokenServer::~CTokenServer()
	{
	FSResources::Cleanup();
	FSDialog::Cleanup();

	delete iKeyStoreServer;
	delete iCertStoreServer;
	delete iCertAppsServer;
	}

/**	2nd phase construction - ensure the timer and server objects are running. */
void CTokenServer::ConstructL()
	{
	FSResources::InitialiseL();
	FSDialog::InitialiseL();
	
	TPtrC serverName(KFSTokenServerName());
		// Naming the server thread after the server helps to debug panics
#ifdef __WINS__
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	serverName.Set(KFSNewTokenServerName());
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#endif // __WINS__
		
	StartL(serverName);
	
	// Ensure that the server still exits even if the 1st client fails to connect
	iShutdown.ConstructL();
	iShutdown.Start();
	}

/** A new session is being created - cancel the shutdown timer if it was running. */
void CTokenServer::AddSession()
	{
	++iSessionCount;
	iShutdown.Cancel();
	}

/** A session is being destroyed - start the shutdown timer if it is the last session. */
void CTokenServer::DropSession()
	{
	if (--iSessionCount==0)
		{
		iShutdown.Start();
		}
	}

/** Lazily create key store server object. */
CFSKeyStoreServer& CTokenServer::KeyStoreServerL() const
	{
	if (!iKeyStoreServer)
		{
		iKeyStoreServer = CFSKeyStoreServer::NewL();
		}
	
	return *iKeyStoreServer;
	}

/** Lazily create cert store server object. */
CFSCertStoreServer& CTokenServer::CertStoreServerL() const
	{
	if (!iCertStoreServer)
		{
		iCertStoreServer = CFSCertStoreServer::NewL();
		}
	
	return *iCertStoreServer;
	}

/** Lazily create cert apps server object. */
CFSCertAppsServer& CTokenServer::CertAppsServerL() const
	{
	if (!iCertAppsServer)
		{
		iCertAppsServer = CFSCertAppsServer::NewL();
		}
	
	return *iCertAppsServer;
	}

/** Create a new client session. */
CSession2* CTokenServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	// The token the client wants to talk to is encoded in the major version number
	ETokenEnum token = static_cast<ETokenEnum>(aVersion.iMajor);	

	// The minor version number represents the version of the protocol
	if (aVersion.iMinor != KFSProtolVersion)
		{
		User::Leave(KErrNotSupported);
		}

	CTokenServerSession* result = NULL;
	
	switch (token)
		{
		case EFileKeyStore:
			result = KeyStoreServerL().CreateSessionL();
			break;

		case EFileCertStore:
			result = CertStoreServerL().CreateSessionL();
			break;
			
		case EFileCertApps:
			result = CertAppsServerL().CreateSessionL();
			break;
			
		default:
			User::Leave(KErrNotSupported);
			break;
		}

	return result;
	}

// CShutdown ///////////////////////////////////////////////////////////////////

inline CShutdown::CShutdown() :
	CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}

inline void CShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

inline void CShutdown::Start()
	{
	After(KServerShutdownDelay);
	}

/** Initiate server exit when the timer expires. */
void CShutdown::RunL()
	{
	CActiveScheduler::Stop();
	}

// Server startup //////////////////////////////////////////////////////////////

/**
 * Perform all server initialisation, in particular creation of the scheduler
 * and server and then run the scheduler.
 */
static void RunServerL()
	{
	TPtrC serverName(KFSTokenServerName());
	// Naming the server thread after the server helps to debug panics
#ifdef __WINS__
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	serverName.Set(KFSNewTokenServerName());
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#endif // __WINS__
	
	User::LeaveIfError(User::RenameThread(serverName));
	
	// Create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	
	// Create the server and leave it on the cleanup stack
	CTokenServer::NewLC();
	
	// Before starting the server, notify client that initialisation is
	// complete.
	// (note that WINS on EKA1 uses threads since it lacks process emulation)
	RProcess::Rendezvous(KErrNone);

	// Ready to run
	CActiveScheduler::Start();
	
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

/** Server process entry point. */
TInt E32Main()
	{
#ifdef _DEBUG
	TokenServerDebug::HeapCheckStart();
#endif

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}

#ifdef _DEBUG
	TokenServerDebug::HeapCheckEnd();
#endif
	return r;
	}

// Only for wins on EKA1 - WINS loads a DLL and starts a new thread
// by calling WinsMain which does the "server" startup
