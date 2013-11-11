// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the FLogger server process startup and stubs all functionality.
// 
//

/**
 @file
 @internalComponent
*/

#include "comsdbgstub.h"



/********************************************************************************/
//                      Stub Functionality from Flogsvr.cpp



CFileLoggerServer* CFileLoggerServer::NewL()
	{

	CFileLoggerServer* r=new(ELeave) CFileLoggerServer();
	CleanupStack::PushL(r);
	r->ConstructL();
	r->StartL(KFLoggerServerName);
	CleanupStack::Pop();
	return r;
	}

CFileLoggerServer::CFileLoggerServer()
	: CServer2(EPriorityNormal,ESharableSessions)
	{}

void CFileLoggerServer::ConstructL()
/**
 *
  @note Constructs the secondary thread, passing through a pointer to the log queue.
  Constructs the file parser object and parses the file, attempting to copy the
  file from the ROM if it is not found on C drive.
  Constructs the time beat.
  Performs the first UpdateMedia to ensure a media is set.
  Stubbed version does very little.
 */
	{
	}


CFileLoggerServer::~CFileLoggerServer()
	{
	}


CSession2* CFileLoggerServer::NewSessionL(const TVersion &aVersion ,const RMessage2& /*aMessage*/) const
/**
 * Create a new server session. Check that client is using current or older interface and make a new session.
 * @note Called by kernel after RFileLogger::DoConnect().
 */
	{
	if (aVersion.iMajor == 1) {}
	return CFileLogSession::NewL();
	}



TInt CFileLoggerServer::RunError(TInt aError)
/**
 * Leave has occured in CFileLogSession::ServiceL.
 * Usually this is because the appending of the message to the queue has failed
 * due to the queue being filled.
 */
	{
	return CServer2::RunError(aError);
	}

//
// CFileLogSession class definition
//

CFileLogSession* CFileLogSession::NewL()
/**
 * Construct new server end of session.
 * @note Only called from CFileLoggerServer::NewSessionL()
 */
	{

	CFileLogSession* self = new(ELeave) CFileLogSession();
	return self;
	}

CFileLogSession::CFileLogSession()
	{}


CFileLogSession::~CFileLogSession()
	{
	}

void CFileLogSession::ServiceL(const RMessage2& aMessage)
/**
 Processes message from client-side (RFileLogger)
 @note Most messages result in logs being added to the queue. If
 synchronous logging is on, this function will wait until the queue is then emptied.
 Slimline Stub version
 */
	{
	aMessage.Complete(KErrNone);
	}



/////////////////////////////////////////////////////////////////


/********************************************************************************/
//                      Stub Functionality from Flogthrd.cpp



const TInt KFLoggerServerUid=0x10004ee0;

_LIT(KFLoggerServerExecutable,"COMSDBGSVR");           ///< Filename of flogger server executable


// only needed for EKA1:
// END EKA1



EXPORT_C TInt FLogger::Start()
/**
 * Start the FLOGGER server - called by a hopeful client when they first connect.
 * @return TInt of (KErrorNone) if the server startup was successful, otherwise
 *         an error code.
 */
	{

	TRequestStatus stat;


	TInt ret;
// Different approaches for EKA1/EKA2:
	//
	// Target and EKA2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	ret = server.Create(KFLoggerServerExecutable,KNullDesC,TUidType(KNullUid,KNullUid,TUid::Uid(KFLoggerServerUid)));
// END EKA1/EKA2
	
	if (ret!=KErrNone)
		{
		return ret;
		}

	server.SetPriority(EPriorityHigh);

	server.Rendezvous(stat);

	server.Resume();
	
	// setting the server process priority is really just a formality since
	// we set the server's thread priorities to absolute values later anyway.

	server.Close();

	User::WaitForRequest(stat);

	return stat.Int();
	}

EXPORT_C TInt FLogger::Run()
/**
 * Start the active scheduler and create the server. This is called from the DLL entry code.
 * @return TInt of (KErrorNone) if the server startup was successful, otherwise
 *         an error code.
@internalComponent
 */
	{

	__UHEAP_MARK;
	
	RThread self;
	self.SetPriority(EPriorityAbsoluteHigh); // was EPriorityMore
	self.Close();
	
	TInt ret = KErrNone;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup==NULL)
		ret=KErrNoMemory;

	CActiveScheduler* scheduler = NULL;
	CFileLoggerServer* server = NULL;
	if (ret==KErrNone)
		{
		scheduler = new CActiveScheduler;
		if (scheduler == NULL)
			ret = KErrNoMemory;
		else
			{
			CActiveScheduler::Install(scheduler);
			TRAP(ret, server = CFileLoggerServer::NewL());
			}
		}

	RProcess::Rendezvous(ret);

	if (ret==KErrNone)
		CActiveScheduler::Start();

	delete server;
	delete scheduler;
	delete cleanup;

	__UHEAP_MARKEND;

	return ret;
	}




