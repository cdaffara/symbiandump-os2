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
* Example Code
* No changes necessary to support User Prompt Service
*
*/


#include <e32cmn.h>
#include "tmsgclient.h"

static TInt StartServer()
	{
	const TUidType serverUid(KNullUid,KNullUid,KMsgServerUid3);
	RProcess server;
	TInt r=server.Create(KMsgServerImg,KNullDesC,serverUid);
	if (r!=KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

EXPORT_C TInt RMsgSession::Connect()
	{
	TInt retry=2;
	for (;;)
		{
		TVersion version(KMsgServerVersionMajor, KMsgServerVersionMinor, KMsgServerVersionBuild);
		TInt r=CreateSession(KMsgServerName, version);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}

EXPORT_C void RMsgSession::SendMsg(const TDesC& aTo, const TDesC& aBody, TRequestStatus& aStatus) const
/**
 Sends a message
 @param aTo		The recipient of the message.
 @param	aBody	The message body.	
 @param aStatus	The server completes this request object when it has finished handling the request. 
 */
	{
	aStatus = KRequestPending;
	SendReceive(ESendMsg, TIpcArgs(&aTo, &aBody), aStatus);
	}

EXPORT_C void RMsgSession::CancelSendMsg() const
/**
 Cancels an active call to SendMsg
 */
	{
	SendReceive(ECancelSendMsg);
	}
