/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the client side interface to HtiIPCommServer
*
*/


// INCLUDE FILES
#include <e32uid.h>
#include <HtiLogging.h>

#include "HtiIPCommServerCommon.h"
#include "HtiIPCommServerClient.h"

// CONSTANTS
const TUint KIPCommServerDefaultMessageSlots = 4; // Read, write, readCancel, writeCancel

//---------------------------------------------------------------------------
static TInt StartServer()
    {
    const TUidType serverUid( KNullUid, KNullUid, KIPCommServerUid3 );

    RProcess server;
    TInt r = server.Create( KIPCommServerName, KNullDesC, serverUid);
    if ( r != KErrNone )
        return r;

    TRequestStatus stat;
    server.Rendezvous( stat );
    if ( stat != KRequestPending )
        server.Kill(0);     // abort startup
    else
        server.Resume();    // logon OK - start the server

    User::WaitForRequest( stat );   //wait for start or death

    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    r = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();
    server.Close();
    return r;
    }

//---------------------------------------------------------------------------
EXPORT_C RHtiIPCommServer::RHtiIPCommServer()
    {
    }

//---------------------------------------------------------------------------
EXPORT_C TInt RHtiIPCommServer::Connect()
    {
    HTI_LOG_TEXT( "RHtiIPCommServer::Connect" );

    // Connect to the server, attempting to start it if necessary
    TInt retry=2;
    for (;;)
        {
        TInt r = CreateSession( KIPCommServerName,
                                Version(),
                                KIPCommServerDefaultMessageSlots );
        HTI_LOG_FORMAT( "CreateSession %d", r );
        if ( r != KErrNotFound && r != KErrServerTerminated )
            return r;

        if (--retry==0)
            return r;

        r = StartServer();
        HTI_LOG_FORMAT( "StartServer %d", r );
        if ( r != KErrNone && r != KErrAlreadyExists )
            return r;
        }
    }

//---------------------------------------------------------------------------
EXPORT_C void RHtiIPCommServer::Close()
    {
    HTI_LOG_FUNC_IN( "RHtiIPCommServer::Close" );

    // Use thread finder to find the server thread
    TFullName threadName;
    TFullName matchPattern;
    matchPattern.Append( _L( "*" ) );
    matchPattern.Append( KIPCommServerName );
    matchPattern.Append( _L( "*" ) );

    TFindThread threadFinder;
    threadFinder.Find( matchPattern );
    TInt err = threadFinder.Next( threadName );
    if ( err )
        {
        HTI_LOG_FORMAT( "RHtiIPCommServer::Close: Failed to find server thread: %d", err );
        return;
        }

    HTI_LOG_TEXT( "RHtiIPCommServer::Close: Found server thread:" );
    HTI_LOG_DES( threadName );

    RThread thread;
    err = thread.Open( threadName );
    if ( err )
        {
        HTI_LOG_FORMAT( "RHtiIPCommServer::Close: Failed to open server thread: %d", err );
        }
    else
        {
        // Close this session. Causes a call to ~CHtiIPCommServerSession which should
        // stop the active scheduler in IPCommServer
        RSessionBase::Close();

        // For clean server stop, wait for its death
        HTI_LOG_TEXT( "RHtiIPCommServer::Close: Waiting for server thread to die..." );
        TRequestStatus status;
        thread.Logon( status );
        User::WaitForRequest( status );
        HTI_LOG_TEXT( "RHtiIPCommServer::Close: Server thread dead." );
        }

    thread.Close();
    HTI_LOG_FUNC_OUT( "RHtiIPCommServer::Close" );
    }

//---------------------------------------------------------------------------
EXPORT_C TVersion RHtiIPCommServer::Version(void) const
    {
    return ( TVersion( KIPCommServerMajorVersionNumber,
                       KIPCommServerMinorVersionNumber,
                       KIPCommServerBuildVersionNumber ) );

    }

//---------------------------------------------------------------------------
EXPORT_C TInt RHtiIPCommServer::GetSendBufferSize() const
    {
    return KIPCommServerSendBufferMaxSize;
    }

//---------------------------------------------------------------------------
EXPORT_C TInt RHtiIPCommServer::GetReceiveBufferSize() const
    {
    return KIPCommServerReceiveBufferMaxSize;
    }

//---------------------------------------------------------------------------
EXPORT_C void RHtiIPCommServer::Receive(TDes8& aData,
                                        TRequestStatus& aStatus)
    {
    SendReceive( EIPCommServerRecv, TIpcArgs( &aData ), aStatus );
    }

//---------------------------------------------------------------------------
EXPORT_C void RHtiIPCommServer::Send(const TDesC8& aData,
                                        TRequestStatus& aStatus)
    {
    SendReceive( EIPCommServerSend, TIpcArgs( &aData ), aStatus );
    }

//---------------------------------------------------------------------------
EXPORT_C void RHtiIPCommServer::CancelReceive()
    {
    SendReceive( EIPCommServerCancelRecv, TIpcArgs( NULL ) );
    }

//---------------------------------------------------------------------------
EXPORT_C void RHtiIPCommServer::CancelSend()
    {
    SendReceive( EIPCommServerCancelSend, TIpcArgs( NULL ) );
    }
// End of the file
