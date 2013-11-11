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
// Implements the FLogger server process startup
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/commsdebugutility.h>
#include "comsdbgstd.h"

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



