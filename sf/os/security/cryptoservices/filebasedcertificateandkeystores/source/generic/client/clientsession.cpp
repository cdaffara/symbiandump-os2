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


#include "clientsession.h"
#include <e32std.h>
#include <e32uid.h>
#include "clientutils.h"
#include "fstokenservername.h"

#ifdef SYMBIAN_AUTH_SERVER

#ifdef __WINS__
#include <u32hal.h>
#endif //__WINS__

#include <e32svr.h>
#endif // SYMBIAN_AUTH_SERVER

//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	
//	Tokentype session class for file based certificate store
//	Connects and passes messages to the file store tokentype server
//	Coded specifically for file store token type
//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	

_LIT(KFSTokenServerImg,"fstokenserver");
#ifdef SYMBIAN_AUTH_SERVER
#ifdef __WINS__
_LIT(KFSNewTokenServerImg,"fstokenserver_useauth");
#endif // __WINS__
#endif // SYMBIAN_AUTH_SERVER

RFileStoreClientSession::RFileStoreClientSession()
{}

TInt RFileStoreClientSession::SendRequest(TFSTokenMessages aRequest, const TIpcArgs& aArgs) const
{
	return SendReceive(aRequest, aArgs);
}

void RFileStoreClientSession::SendAsyncRequest(TFSTokenMessages aRequest, const TIpcArgs& aArgs, TRequestStatus* aStatus) const
{
	__ASSERT_ALWAYS(aStatus, FSTokenPanic(EBadArgument));

	if (aStatus)
	{
		*aStatus = KRequestPending;
		SendReceive(aRequest, aArgs, *aStatus);
	}
}


//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	
//	Client-server startup code
//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	

static TInt StartServer();	//	Forward declaration
//
// Connect to the server, attempting to start it if necessary
//

#ifdef SYMBIAN_AUTH_SERVER
#ifdef __WINS__

static bool UseAuthServer(void);

#endif // SYMBIAN_AUTH_SERVER
#endif // __WINS__

TInt RFileStoreClientSession::Connect(ETokenEnum aToken)
	{
	// The version is made up of three pieces of information:
	// 1. iMajor - The token we want to talk to
	// 2. iMinor - The protocol version number
	// 3. iBuild - unused
	TVersion version(aToken, KFSProtolVersion, 0);
	
	TInt retry=2;
	for (;;)
		{
		TInt err = KErrNone;
#ifdef SYMBIAN_AUTH_SERVER
#ifdef __WINS__
		if(UseAuthServer())
			{
			err = CreateSession(KFSNewTokenServerName, version, 1);
			}
		else
#endif // __WINS__
#endif // SYMBIAN_AUTH_SERVER
			{
			err = CreateSession(KFSTokenServerName, version, 1);
			}
		
		if (err!=KErrNotFound && err!=KErrServerTerminated)
			return err;
		if (--retry==0)
			return err;
		err=StartServer();
		if (err!=KErrNone && err!=KErrAlreadyExists)
			return err;
		}
	}

#ifdef SYMBIAN_AUTH_SERVER
#ifdef __WINS__

/*static*/ bool UseAuthServer(void)
	{
	bool useAuthServer = false;
	
	TUint32 useAuth = 0;
	// For the emulator allow the constant to be patched via epoc.ini
	UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty,
	(TAny*)"KKeyStoreUseAuthServer", &useAuth); // read emulator property (if present)
	if(useAuth)
		{
		useAuthServer = true;
		}
	return useAuthServer; 
	}
#endif // __WINS__
#endif // SYMBIAN_AUTH_SERVER

TInt StartServer()
	{
	// Server startup is different for WINS in EKA1 mode ONLY (lack of process
	// emulation - we load the library in this instance
	const TUidType serverUid(KNullUid, KNullUid, KUidFSTokenServer);

	RProcess server;
	TInt error = KErrNone;
	
#ifdef SYMBIAN_AUTH_SERVER
#ifdef __WINS__
	if(UseAuthServer())
		{
		error = server.Create(KFSNewTokenServerImg, KNullDesC, serverUid);
		}
	else
#endif // __WINS__
#endif // SYMBIAN_AUTH_SERVER
		{
		error = server.Create(KFSTokenServerImg, KNullDesC, serverUid);
		}
	
	if (error != KErrNone)
		{
		return error;
		}

	// Synchronise with the process to make sure it hasn't died straight away
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat != KRequestPending)
		{
		// logon failed - server is not yet running, so cannot have terminated
		server.Kill(0);				// Abort startup
		}
	else
		{
		// logon OK - start the server
		server.Resume();
		}

	// Wait to synchronise with server - if it dies in the meantime, it
	// also gets completed
	User::WaitForRequest(stat);	

	// We can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	error = (server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return (error);
	}

