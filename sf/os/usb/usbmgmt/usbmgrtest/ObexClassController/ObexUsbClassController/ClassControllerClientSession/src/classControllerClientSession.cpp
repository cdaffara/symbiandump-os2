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
*
*/

#include "../../public/clientServerShared.h"
#include "classControllerClientSession.h"


/**
 * Function used by RClassControllerClient::Connect member function.
 * It exists to create a new process which contains the server side code.
 *
 * @return Value of error code created
 */
static TInt StartServer()
	{
	const TUidType serverUid(KNullUid, KNullUid, KObexCCUid);
	RProcess server;
	TInt err = server.Create(KServerExe, KNullDesC, serverUid);
	if ( err != KErrNone )
		{
		return err;
		}

	TRequestStatus stat;
	server.Rendezvous(stat);

	if ( stat != KRequestPending )
		{
		server.Kill(0); 	// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}

	User::WaitForRequest(stat); 	// wait for start or death

	// we can't use the 'exit reason' if the server paniced as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	err = (server.ExitType() == EExitPanic) ? KErrServerTerminated : stat.Int();
	
	server.Close();

	return err;
	}



/**
 * Function that creates a session with the server and then returns the appropriate error code.
 * Then calls StartServer to start the process with the server side code.
 * 
 * @return Value of error code produced
 */

EXPORT_C TInt RClassControllerClient::Connect()
	{
	TInt retry = 2;

	FOREVER
		{
		//Create Session, With server name from shared header 
		//And lowest version number of the server with which this client is compatible. 
		//Number of message slots available, -1 for meessages from global free pool

		TInt err = CreateSession(KServerName, TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber), KMessageSlots);

		if ((err != KErrNotFound) && (err != KErrServerTerminated))
			{
			return err;
			}

		if (--retry == 0)
			{
			return err;
			}

		err = StartServer();

		if ((err != KErrNone) && (err != KErrAlreadyExists))
			{
			return err;
			}
		}
	}

/**
 * Function that sends a message to the server to initiate the start
 * of an Obex Service. On completion, TRequestStatus contains error code.
 *
 * @return Value of TRequestStatus 
 */
EXPORT_C TInt RClassControllerClient::StartService()
	{
	return (SendReceive(EStartClassContServer));
	}

/**
 * Function that sends a message to the server to initiate the stop
 * of an Obex Service. On completion, TRequestStatus contains error code.
 *
 * @return Value of TRequestStatus 
 */
EXPORT_C TInt RClassControllerClient::StopService()
	{
	return (SendReceive(EStopClassContServer));
	}


