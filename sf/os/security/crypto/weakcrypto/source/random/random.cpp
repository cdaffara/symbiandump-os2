/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* (c) 1999-2003 Symbian Ltd
*
*/


/**
 @file
*/

#include <e32std.h>
#include <random.h>
#include <hash.h>
#include <e32math.h>
#include "randsvr.h"
#include "randcliserv.h"

_LIT(KRandomServerImg,"z:\\system\\libs\\randsvr.exe");		// DLL/EXE name
_LIT(KRandomServerConnect, "Randsvr connect");
_LIT(KRandomServerGet, "Randsvr get");

const TUid KServerUid3={0x100066dc};

extern "C" {
EXPORT_C void RAND_bytes(unsigned char* buf,int bytes)
	{
	TPtr8 ptr(buf,bytes,bytes);
	buf[0]++;
	TRandom::Random(ptr);
	}
}

EXPORT_C CRandom::CRandom(void)
	{
	}

EXPORT_C CSystemRandom* CSystemRandom::NewL(void)
	{
	CSystemRandom* self = new(ELeave)CSystemRandom();
	return self;
	}

EXPORT_C CSystemRandom* CSystemRandom::NewLC(void)
	{
	CSystemRandom* self = NewL();
	CleanupStack::PushL(self);
	return self;
	}

void CSystemRandom::GenerateBytesL(TDes8& aDest)
	{
	TRandom::RandomL(aDest);
	}

CSystemRandom::CSystemRandom(void)
	{
	}

EXPORT_C void TRandom::Random(TDes8& aDestination)
	{
	RRandomSession rs;
	TRAPD(ret,rs.ConnectL());
	if (ret)
		{
		User::Panic(KRandomServerConnect, ret);
		}
	TInt err=rs.GetRandom(aDestination);
	if (err)
		{
		User::Panic(KRandomServerGet, err);
		}
	rs.Close();
	}

EXPORT_C void TRandom::RandomL(TDes8& aDestination)
	{
	RRandomSession rs;
	TRAPD(ret,rs.ConnectL());
	User::LeaveIfError(ret);
	CleanupClosePushL(rs);

	TInt err=rs.GetRandom(aDestination);
	User::LeaveIfError(err);

	CleanupStack::PopAndDestroy(); // rs
	}

EXPORT_C RRandomSession::RRandomSession(void)
	{
	}

static TInt StartServer()
// Borrowed from AndrewT's server startup code.
// Start the server process/thread which lives in an EPOCEXE object
//
	{
	
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);

	//
	// EPOC and EKA2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt r=server.Create(KRandomServerImg, KNullDesC, serverUid);

	if (r!=KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;

	}

EXPORT_C void RRandomSession::ConnectL(void)
	{
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KRandomServerName,TVersion(0,0,0),1);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			   User::Leave(r);
		if (--retry==0)
			User::Leave(r);
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			User::Leave(r);
		}
	}

EXPORT_C TInt RRandomSession::GetRandom(TDes8& aDestination)
	{
	if (aDestination.Length()<KRandomBlockSize)
		{
		return SendReceive(CRandomSession::KRandomRequest,
		                   TIpcArgs(&aDestination, aDestination.Length()));
		}
	else
		{
		TInt i;
		TInt err=KErrNone;
		TInt length=aDestination.Length();
		for (i=0;(i+KRandomBlockSize)<length;i+=KRandomBlockSize)
			{
			TPtr8 buffer(&aDestination[i],KRandomBlockSize,KRandomBlockSize);
			err=SendReceive(CRandomSession::KRandomRequest,
			 				TIpcArgs(&buffer, KRandomBlockSize));
			if (err)
				{
				return err;
				}
			}
		TPtr8 buffer(&aDestination[i],length%KRandomBlockSize,KRandomBlockSize);
		err=SendReceive(CRandomSession::KRandomRequest,
						TIpcArgs(&buffer, length-i));
		return err;
		}
	}

