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
* sisregistry - client registry session interface implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#include <s32mem.h>
#include "clientserver.h"
#include "rtaclient.h"
#include <iniparser.h>

using namespace ReferenceTestAgent;

TInt RRtaClient::StartRTAServer()
	{
	const TUidType serverUid(KNullUid, KNullUid, KServerUid3);
	RProcess server;
	TInt err = server.Create(KRtaImg, KNullDesC, serverUid);
	if (err != KErrNone)
		{
		return err;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat != KRequestPending)
		{
		server.Kill(0);		// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	err = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return err;
	}

RRtaClient::RRtaClient()
	{
	}
	
TInt RRtaClient::Connect()
//
// Connect to the server, attempting to start it if necessary
//
	{
	TInt retry=2;
	for (;;)
		{
		TInt err = CreateSession(KRtaServerName, TVersion(0, 0, 0), 1);
		if (err != KErrNotFound && err != KErrServerTerminated)
			{
			return err;
			}
		if (--retry==0)
			{
			return err;
			}
		err = StartRTAServer();
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			return err;
			}
		}
	}

TBool RRtaClient::IsDrmCapabilityEnforced() const
	{
	_LIT(KIniFile, "c:\\rta\\settings.ini");
	_LIT(KDrmCapabilityEnforced, "DrmCapabilityCheckEnforced");
	TBool enforced = ETrue;	
	TInt value = 1;
	CIniData* iniData = NULL;
	TRAPD(err, iniData = CIniData::NewL(KIniFile()));
	if(err == KErrNone)
		{
		if(iniData->FindVar(KDrmCapabilityEnforced(), value))
			{
			if(value == 0)
				{
				enforced = EFalse;
				}
			}
		delete iniData;
		}
	return enforced;
	}
