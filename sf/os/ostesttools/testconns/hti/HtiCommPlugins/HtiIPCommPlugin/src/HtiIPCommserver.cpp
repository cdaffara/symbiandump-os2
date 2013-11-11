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
* Description: This file contains the implementation of the
*              CHtiIPCommServer class.
*              CHtiIPCommServer handles Symbian server side operations
*              such as server starting and client session creation.
*
*/


// INCLUDE FILES
#include "HtiIPCommServerCommon.h"
#include "HtiIPCommServer.h"
#include "HtiIPCommServerSession.h"
#include "HtiIPCommLogging.h"

CHtiIPCommServer::CHtiIPCommServer()
    :CServer2( EPriorityUserInput, EUnsharableSessions ),
    iConnectionManager( NULL ),
    iRunning( EFalse ),
    iNrOfSessions( 0 )
    {
    // NOTE: Server needs to have highest priority of all our active
    // objects to be able to service client requests while other active
    // objects are running.
    //
    // This also affects closing of sessions. If priorities
    // are the same ~CHtiIPCommServerSession does not get called before active
    // object completes.
    }

CHtiIPCommServer::~CHtiIPCommServer()
    {
    delete iConnectionManager;
    }

CHtiIPCommServer* CHtiIPCommServer::NewLC()
    {
    CHtiIPCommServer* self=new(ELeave) CHtiIPCommServer;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CHtiIPCommServer::ConstructL()
    {
    iConnectionManager = CHtiConnectionManager::NewL( this );
    StartL( KIPCommServerName );
    }

CSession2* CHtiIPCommServer::NewSessionL( const TVersion&, const RMessage2&) const
    {
    // There should be only one session
    if ( iNrOfSessions )
        User::Leave( KErrAlreadyExists );

    return new(ELeave) CHtiIPCommServerSession( (CHtiIPCommServer*)this );
    }

void CHtiIPCommServer::SessionOpened()
    {
    iNrOfSessions++;
    }

void CHtiIPCommServer::SessionClosed()
    {
    // Die if no more sessions
    if ( --iNrOfSessions == 0 )
        {
        HTI_LOG_TEXT( "HtiIPCommServer no more sessions - dying..." );
        CloseServer();
        }
    }

void CHtiIPCommServer::CloseServer()
    {
    // Can be called from CHtiIPCommServer::SessionClosed and
    // CHtiConnectionManager::TimerExpiredL
    // So this can actually be called twice if it is called from
    // TimerExpiredL - hence the iRunning variable :)
    if ( iRunning )
        {
        iRunning = EFalse;
        CActiveScheduler::Stop();
        HTI_LOG_TEXT( "CActiveScheduler::Stop() called" );
        }
    }

static void RunServerL()
    {
    // naming the server thread after the server helps to debug panics

    User::LeaveIfError( RThread::RenameMe( KIPCommServerName ) );

    // create and install the active scheduler we need
    CActiveScheduler* as = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( as );
    CActiveScheduler::Install( as );

    // create the server (leave it on the cleanup stack)
    CHtiIPCommServer* server = CHtiIPCommServer::NewLC();

    // Rendezvous with the client (RHtiIPCommServer::Connect)
    RProcess::Rendezvous(KErrNone);

    server->iRunning = ETrue;
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(2);     // server, as
    }


// Server process entry-point
TInt E32Main()
    {

    HTI_LOG_TEXT( "*** Starting HtiIPCommServer ***" );

    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt r = KErrNoMemory;

    if ( cleanup )
        {
        TRAP( r, RunServerL() );
        delete cleanup;
        }

    __UHEAP_MARKEND;

    HTI_LOG_TEXT( "*** HtiIPCommServer died! ***" );

    return r;
    }
