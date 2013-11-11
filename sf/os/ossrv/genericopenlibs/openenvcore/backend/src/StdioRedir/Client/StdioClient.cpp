// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : stdioclient.cpp
// Part of     : stdio server
// stdio Redirection Server - client interface 
// 
//



#include "StdioClient.h"


// A version must be specified when creating a session with the server

const TUint KStdioServerMajorVersionNumber = 1;
const TUint KStdioServerMinorVersionNumber = 0;
const TUint KStdioServerBuildVersionNumber = 0;
const TInt  KBufSizeNotify = 256;

_LIT(KServerName,"stdioserver");
const TUid KServerUid3={0x10283075};

//-------------------------------------------------------------------------------
// Function Name : 	StartServer()
// Description   : 	Static function Start the server process. Simultaneous 
//					launching of two such processes should be detected when the 
//					second one attempts to create the server object, failing with
//					KErrAlreadyExists.
//-------------------------------------------------------------------------------	


 

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

//-------------------------------------------------------------------------------
// Function Name : 	RStdioSession::Connect()
// Description   : 	The Client process uses this exported function to connect 
//					to the server. If the server is not started then the server
//					stared by calling the above static function. The client process
//					tills it gets signal from the server.
//-------------------------------------------------------------------------------	


TInt RStdioSession::Connect()
	{
	TInt retry = 2; // A maximum of two iterations of the loop required
	for(;;)
		{
			TInt r = CreateSession(KServerName,TVersion(KStdioServerMajorVersionNumber,
										KStdioServerMinorVersionNumber,
										KStdioServerBuildVersionNumber), -1,
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
	
//-------------------------------------------------------------------------------
// Function Name : 	RStdioSession::Write()
// Description   : 	To implement write for the client process.
//					
//-------------------------------------------------------------------------------	

	
void RStdioSession::Write(TRequestStatus& aStatus, const TDesC8& aDes, TInt aLength)
	{
	SendReceive(EWrite, TIpcArgs(&aDes, aLength), aStatus);
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	RStdioSession::Read()
// Description   : To implement Read for the client process.
//					
//-------------------------------------------------------------------------------	
	
	
void RStdioSession::Read(TRequestStatus& aStatus, TDes8& aDes, TInt aLength)
	{
	SendReceive(ERead, TIpcArgs(&aDes, aLength), aStatus);
	}

//-------------------------------------------------------------------------------
// Function Name : 	RStdioSession::CheckMedia()
// Description   : 	Internal Function to check if the any Media is None. So that 
//					the read or write enters in a black hole.
//					
//-------------------------------------------------------------------------------	

TInt RStdioSession::CheckMedia(TDes8& aDes)
	{
	TRequestStatus status;
	SendReceive(ECheckMedia, TIpcArgs(&aDes), status);
	User::WaitForRequest(status);
	return status.Int();
	}


//-------------------------------------------------------------------------------
// Function Name : 	RStdioSession::NotifyActivity()
// Description   : 	To notify read request ready.
//					
//-------------------------------------------------------------------------------	
void RStdioSession::NotifyActivity(TInt aEvents, TRequestStatus& aStatus)
	{
	SendReceive(ENotifyActivity, TIpcArgs(aEvents, KBufSizeNotify), aStatus);
	}


//-------------------------------------------------------------------------------
// Function Name : 	RStdioSession::CancelNotify()
// Description   : 	To cancel notify read request ready.
//					
//-------------------------------------------------------------------------------	
void RStdioSession::CancelNotify()
	{
	SendReceive(ECancelNotify);
	}

//
//int RStdioSession::SetEcho(TBool aEcho)
//
//Request the stdio server to set the echo flag.
//
int RStdioSession::SetEcho(TBool aEcho)
	{
	TRequestStatus status;
	SendReceive(EEcho, TIpcArgs(aEcho), status);
	User::WaitForRequest(status);
	return status.Int();
	}

