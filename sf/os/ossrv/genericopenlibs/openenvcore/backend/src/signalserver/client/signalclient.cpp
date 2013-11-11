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
// Name        : signalclient.cpp
// Part of     : signal server
// 
//

#include "signalclient.h"
#include "signalcommon.h"
#include <e32std.h>

// A version must be specified when creating a session with the server

const TUint KIpcServerMajorVersionNumber = 1;
const TUint KIpcServerMinorVersionNumber = 0;
const TUint KIpcServerBuildVersionNumber = 0;

_LIT(KServerName,"SignalServer");
const TUid KServerUid3={0x2001F3C8};

static TInt StartServer()
    {
    const TUidType serverUid(KNullUid,KNullUid,KServerUid3);
    RProcess server;
    TInt r = server.Create(KServerName,KNullDesC);
    if ( r != KErrNone )
    	{
    	return r;
    	}
    TRequestStatus stat;
    server.Rendezvous(stat);
    if ( stat != KRequestPending )
    	{
       	server.Kill(0);     // abort startup
    	}
    else
       	{
       	server.Resume();    // logon OK - start the server
       	}
    User::WaitForRequest(stat);     // wait for start or death

    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone

    r = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();
    server.Close();
    return r;
    }

TInt RSignalSession::Connect()
	{
	TInt retry = 2; // A maximum of two iterations of the loop required
	for(;;)
		{
			TInt r = CreateSession(KServerName,TVersion(KIpcServerMajorVersionNumber,
										KIpcServerMinorVersionNumber,
										KIpcServerBuildVersionNumber), -1,
										EIpcSession_Sharable);

			if ( (KErrNotFound != r) && (KErrServerTerminated != r) )
				{
				return r;
				}
			if ( --retry == 0 )
				{
				return r;
				}
			r = StartServer();
			if ( (KErrNone != r) && (KErrAlreadyExists != r) )
				{
				return r;
				}
		}
	}

TInt RSignalSession::OnDemandConnection()
	{
	TInt err = KErrNone;
	//On Demand connection
	if ( !iIsConnected )
		{
		Lock();
		if ( !iIsConnected )
			{
			err = Connect ();
			if ( KErrNone == err)
				{
				iIsConnected = ETrue;
				}
			}
		UnLock();
		}
	return err;
	}

TInt RSignalSession::SendSignal(TInt aPid,TInt aSignal)
	{
	TInt err = KErrNone;
	if (KErrNone != OnDemandConnection())
			{
			return KErrGeneral;
			}
	err = SendReceive(ESendSignal, TIpcArgs(aPid,aSignal));
	return err;
	}

TInt RSignalSession::SendSignalValue(TInt aPid,TInt aSignal,TInt aValue)
	{
	TInt err = KErrNone;
	if (KErrNone != OnDemandConnection())
			{
			return KErrGeneral;
			}
	err = SendReceive(ESendSignalValue, TIpcArgs(aPid,aSignal,aValue));
	return err;
	}

TInt RSignalSession::SendPipe(RPipe& aPipe)
	{
	TInt err = KErrNone;
	if (KErrNone != OnDemandConnection())
			{
			return KErrGeneral;
			}
	err = SendReceive(ESendPipe, TIpcArgs(aPipe));
	return err;
	}
