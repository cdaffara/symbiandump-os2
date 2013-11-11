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
*              CHtiBtCommServer class.
*              CHtiBtCommServer handles Symbian server side operations
*              such as server starting and client session creation.
*
*/


// INCLUDE FILES
#include "HtiBtClientServerCommon.h"
#include "HtiBtCommServer.h"
#include "HtiBtCommServerSession.h"
#include "Logger.h"

#include <e32base.h>
#include <e32std.h>
#include <e32svr.h>
#include <e32uid.h>

// CONSTANTS
// For memory allocations
const TUint KBtCommServerHeapSizeMin       = 0x6000;
const TUint KBtCommServerHeapSizeMax       = 0x20000;
const TUint KBtCommServerStackSize         = 0x8000;


//*****************************************************************************
//
// Class CHtiBtCommServer
//
//*****************************************************************************

/*---------------------------------------------------------------------------*/
CHtiBtCommServer::CHtiBtCommServer( TInt aPriority )
    : CServer2( aPriority, ESharableSessions )
    {
    LOGFW(DebugLog(_L("CHtiBtCommServer: CHtiBtCommServer()")))
    __DECLARE_NAME(_S( "CHtiBtCommServer" ));
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServer::ConstructL()
    {
    LOGFW(DebugLog(_L("CHtiBtCommServer: ConstructL()")))
    }

/*---------------------------------------------------------------------------*/
CHtiBtCommServer* CHtiBtCommServer::NewL()
    {
    LOGFW(DebugLog(_L("CHtiBtCommServer: NewL()"));)

    CHtiBtCommServer *pS =
        new (ELeave) CHtiBtCommServer( EBtCommServerPriority );
    CleanupStack::PushL( pS );
    __ASSERT_ALWAYS( pS != NULL, PanicServer( ESvrCreateServer ) );

    pS->ConstructL();
    CleanupStack::Pop( pS );
    User::SetProcessCritical( User::ENotCritical );
    User::SetCritical( User::ENotCritical );
    LOGFW(InfoLog(_L("Server was started")))
    return pS;
    }

/*---------------------------------------------------------------------------*/
CHtiBtCommServer::~CHtiBtCommServer()
    {
    LOGFW(DebugLog(_L("CHtiBtCommServer: ~CHtiBtCommServer()")))
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServer::SessionFreed()
    {
    LOGFW(DebugLog(_L("CHtiBtCommServer: SessionFreed(): Stopping active scheduler"));)
    iSession = NULL; // iSession is owned by server framework, not deleted here
    CActiveScheduler::Stop();
    LOGFW(DebugLog(_L("CHtiBtCommServer: SessionFreed(): Done"));)
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServer::SessionCreated( CHtiBtCommServerSession* aSession )
    {
    LOGFW(DebugLog(_L("CHtiBtCommServer: SessionCreated()"));)
    iSession = aSession;
    }

/*---------------------------------------------------------------------------*/
CSession2* CHtiBtCommServer::NewSessionL( const TVersion &aVersion,
    const RMessage2& aMessage ) const
    {
    LOGFW(DebugLog(_L("CHtiBtCommServer: NewSessionL() - IPC V2"));)
    aMessage.IsNull();
    if ( iSession )
        User::Leave( KErrAlreadyExists ); // Allow only one session

    // Check that server is the right version
    TVersion ver( KBtCommServerMajorVersionNumber,
                  KBtCommServerMinorVersionNumber,
                  KBtCommServerBuildVersionNumber );
    if ( !User::QueryVersionSupported( ver, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
    return CHtiBtCommServerSession::NewL( (CHtiBtCommServer*)this );
    }

/*---------------------------------------------------------------------------*/
GLDEF_C TInt CHtiBtCommServer::ThreadFunction( TAny* anArg )
    {
    LOGFW(_L("CHtiBtCommServer: ThreadFunction(): Starting"));

    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New();

    // Convert argument into semaphore reference
    RSemaphore& semaphore = *(RSemaphore*)anArg;

    // Start scheduler...
    LOGFW(DebugLog(_L("CHtiBtCommServer: ThreadFunction(): Installing active scheduler"));)
    CActiveScheduler *pA = new CActiveScheduler;
    __ASSERT_ALWAYS( pA != NULL, PanicServer( EMainSchedulerError ) );
    CActiveScheduler::Install( pA );

    CHtiBtCommServer* pS = NULL;
    TRAPD(err,
        // ...and server
        LOGFW(DebugLog(_L("CHtiBtCommServer: ThreadFunction(): Creating server instance"));)
        pS = CHtiBtCommServer::NewL();
        )

    if ( err != KErrNone )
        {
        LOGFW(DebugLog(_L("CHtiBtCommServer: ThreadFunction(): Failed creating server instance"));)
        }

    LOGFW(DebugLog(_L("CHtiBtCommServer: ThreadFunction(): Starting server"));)
    __ASSERT_ALWAYS( pS->Start(KBtCommServerName) == KErrNone,
        PanicServer( ESvrStartServer ) ); // Make first request pending,

    // Signal that server has started
    LOGFW(DebugLog(_L("CHtiBtCommServer: ThreadFunction(): Signalling client: server is up and running"));)
    semaphore.Signal();

    LOGFW(DebugLog(_L("CHtiBtCommServer: ThreadFunction(): Waiting for server's death"));)
    // Start receiving requests from clients
    CActiveScheduler::Start();
    LOGFW(DebugLog(_L("CHtiBtCommServer: ThreadFunction(): Server was stopped"));)
    LOGFW(InfoLog(_L("Server was stopped")))
    delete pS;
    LOGFW(DebugLog(_L("CHtiBtCommServer: ThreadFunction(): Server was deleted"));)

    // Finished
    delete pA;
    pA = NULL;

     // Destroy clean-up stack
    delete cleanup;
    cleanup = NULL;

    __UHEAP_MARKEND;
    return KErrNone;
    }

/*---------------------------------------------------------------------------*/
GLDEF_C void PanicServer( TBtCommServerPanic aPanic )
    {
    LOGFW(DebugLog(_L("CHtiBtCommServer: PanicServer()"));)
    _LIT( KTxtServerPanic, "BtCommServer panic" );
    User::Panic( KTxtServerPanic, aPanic );
    }

/*---------------------------------------------------------------------------*/
EXPORT_C TInt StartThread()
    {
//    LOGFW(_L("CHtiBtCommServer: StartThread()"));
    TInt res = KErrNone;

    // Create server - if one of this name does not already exist
    TFindServer findBtCommServer( KBtCommServerName );
    TFullName name;
    if ( findBtCommServer.Next( name ) != KErrNone ) // Server doesn't exist
        {
         // Create a semaphore to know when thread initialization has finished
        RSemaphore semaphore;
        semaphore.CreateLocal(0);

//        LOGFW(_L("CHtiBtCommServer: Created Semaphore...\n"));

        // Create new server thread and thread's main function
        RThread thread;
        res = thread.Create( KBtCommServerName,
                             CHtiBtCommServer::ThreadFunction,
                             KBtCommServerStackSize,
                             KBtCommServerHeapSizeMin,
                             KBtCommServerHeapSizeMax,
                             &semaphore );


        if ( res == KErrNone ) // Thread created ok - now start it going
            {
//            LOGFW(_L("CHtiBtCommServer: StartThread() - Create OK"));

            thread.SetPriority( EPriorityNormal );
//            TRequestStatus stat1;
//            thread.Logon(stat1);

            thread.Resume();  // Start it going

            semaphore.Wait(); // Wait until it's initialized

            thread.Close();   // No longer interest in the other thread
            }
        else // Thread not created ok
            {
            // No further interest in it
//            LOGFW(_L("CHtiBtCommServer: StartThread() - Create FAIL"));
            thread.Close();
            }
        semaphore.Close();
        }
    return res;
    }

/*---------------------------------------------------------------------------*/
GLDEF_C TInt E32Main()
    {
    return KErrNone;
    }

// End of the file
