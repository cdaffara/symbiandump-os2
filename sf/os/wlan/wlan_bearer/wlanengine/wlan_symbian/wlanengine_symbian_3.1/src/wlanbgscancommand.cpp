/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements command queue for BgScan.
*
*/

/*
* %version: 4 %
*/

#include <s32mem.h>
#include "wlanbgscancommand.h" 
#include "wlanbgscancommandlistener.h"
#include "am_debug.h"

// Number of message slots in BgScan message queue, which is used for
// passing commands from AWS to BgScan
const TInt KNumberOfSlots( 5 );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor.
//
// Runs in: WLAN Engine thread context.
// ---------------------------------------------------------------------------
//
CWlanBgScanCommand::CWlanBgScanCommand( MWlanBgScanCommandListener& aListener ) : 
    CActive( CActive::EPriorityStandard ),
    iListener( aListener )
    {
    DEBUG( "CWlanBgScanCommand::CWlanBgScanCommand()" );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
//
// Runs in: WLAN Engine thread context.
// ---------------------------------------------------------------------------
//
void CWlanBgScanCommand::ConstructL()
    {
    DEBUG( "CWlanBgScanCommand::ConstructL()" );
    CActiveScheduler::Add( this );
    
    // create message queue for passing messages from AWS to BgScan
    TInt err = iMsgQueue.CreateLocal( KNumberOfSlots );
    
    if( err != KErrNone )
        {
        DEBUG1( "CWlanBgScanCommand::ConstructL() - message queue creation failed, err: %d",
                err );
        User::Leave( err );
        }
    
    // subscribe notification about when new messages land on the queue
    iMsgQueue.NotifyDataAvailable( iStatus );
    
    SetActive();
    
    DEBUG( "CWlanBgScanCommand::ConstructL() - returning" );
    }

// ---------------------------------------------------------------------------
// Static constructor.
//
// Runs in: WLAN Engine thread context.
// ---------------------------------------------------------------------------
//
CWlanBgScanCommand* CWlanBgScanCommand::NewL(
    MWlanBgScanCommandListener& aListener )
    {
    DEBUG( "CWlanBgScanCommand::NewL()" );
    CWlanBgScanCommand* self = new ( ELeave ) CWlanBgScanCommand( aListener );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
//
// Runs in: WLAN Engine thread context.
// ---------------------------------------------------------------------------
//
CWlanBgScanCommand::~CWlanBgScanCommand()
    {
    DEBUG( "CWlanBgScanCommand::~CWlanBgScanCommand()" );
    Cancel();
    }

// ---------------------------------------------------------------------------
// Do Cancel.
// Cancels any pending command and completes client's request on that part.
//
// Runs in: WLAN Engine thread context.
// ---------------------------------------------------------------------------
//
void CWlanBgScanCommand::DoCancel()
    {
    DEBUG( "CWlanBgScanCommand::DoCancel()" );
    
    TWlanBgScanMsg msg;
    
    // Retrieve messages from message queue as long as there are any
    // and cancel them
    while( iMsgQueue.Receive( msg ) == KErrNone )
         {
         // Complete client's request with KErrCancel
         msg.iClientThread.RequestComplete( msg.iReportStatusPtr, KErrCancel );
                     
         // Destroy the handle to the client thread
         msg.iClientThread.Close();
         }
    
    DEBUG( "CWlanBgScanCommand::DoCancel() - cancelling data available notification" );
    iMsgQueue.CancelDataAvailable();
    DEBUG( "CWlanBgScanCommand::DoCancel() - done" );
    }

// ---------------------------------------------------------------------------
// Command Queue.
// This method completes BgScan command AO thereby signalling WLAN Engine 
// thread that it is eligible for runnning i.e. the command itself is processed 
// in WLAN Engine thread context.
//
// Runs in: AWS thread context.
// ---------------------------------------------------------------------------
//
void CWlanBgScanCommand::CommandQueue(
    TWlanBgScanCommand aCommand,     // IN: Command to be processed
    TUint32 aParameter,              // IN: Command parameter
    TRequestStatus& aReportStatus )  // IN: Client's request status that needs
                                     //     to be completed when the command
                                     //     has been processed.
    {
    DEBUG( "CWlanBgScanCommand::CommandQueue()" );
    
    TRequestStatus* statusPtr = &aReportStatus;
    TWlanBgScanMsg msg;
    
    // Set client's (BgScan) request status to request pending
    *statusPtr = KRequestPending;
    
    // NOTE! About which class has to be used when completing
    // the request statuses of both WLAN thread and AWS thread.
    // RThread class is always used for completing statuses in other
    // threads and User class for completing statuses in running thread.
    // Using incorrect class's RequestComplete method changes the status
    // of the active object BUT it does not increment the thread's
    // semaphore and hence the thread does not go to "ready to
    // be run" state. Consequently, the active object would be
    // run only when some other trigger causes the thread's 
    // semaphore to be incremented and hence making thread
    // eligible to be run.
    
    // Get handle to client thread (AWS thread) so that
    // the handle can be later used for completing the client's
    // request status
    TThreadId id = RThread().Id();
    TInt err = msg.iClientThread.Open( id );
    if( err != KErrNone )
        {
        DEBUG( "CWlanBgScanCommand::CommandQueue() - Opening client thread handle failed" );
        // Complete request with error code
        // Note! User class is used for completing the status here
        // as we are completing the active object in the same thread
        // (WLAN Engine thread) that is running this method.
        User::RequestComplete( statusPtr, KErrBadHandle );
        return;
        }
    
    // Store command so that it will be processed when RunL is called.
    msg.iCommand = aCommand;
    // Store parameter so that it will be processed when RunL is called.
    msg.iParameter = aParameter;
    // Store handle to report status which is completed when command
    // has been processed (i.e. RunL called)
    msg.iReportStatusPtr = statusPtr;    
    
    // Put message to message queue, so that the command
    // itself will be processed in WLAN Engine thread context
    err = iMsgQueue.Send( msg );
    if( err != KErrNone ) // Sending failed
        {
        // Complete request with error code
        DEBUG( "CWlanBgScanCommand::CommandQueue() - command sending failed due to BgScan message queue full" );
        msg.iClientThread.Close();
        User::RequestComplete( statusPtr, KErrServerBusy );
        }

    DEBUG( "CWlanBgScanCommand::CommandQueue() - command sent to BgScan message queue" );
    
    }

// ---------------------------------------------------------------------------
// RunL.
// Executes the command that was requested with CommandQueue method.
//
// Runs in: WLAN Engine thread context.
// ---------------------------------------------------------------------------
//
void CWlanBgScanCommand::RunL()
    {
    DEBUG( "CWlanBgScanCommand::RunL()" );
    ProcessCommands();
    }

// ---------------------------------------------------------------------------
// Process Commands.
// Reads commands (sent by AWS) from BgScan message queue and
// processes them.
//
// Runs in: WLAN Engine thread context.
// ---------------------------------------------------------------------------
//
void CWlanBgScanCommand::ProcessCommands()
    {
    DEBUG( "CWlanBgScanCommand::ProcessCommands()" );
    
    TWlanBgScanMsg msg;
        
    // retrieve a message from queue and process it
    TInt rx_err = iMsgQueue.Receive( msg );
    
    switch( rx_err )
        {
        case KErrNone:
            {
            TInt err( KErrNone );
            
            switch( msg.iCommand )
                {
                case ESetInterval:
                    DEBUG( "CWlanBgScanCommand::ProcessCommands() - calling DoSetInterval" );
                    iListener.DoSetInterval( msg.iParameter );
                    DEBUG( "CWlanBgScanCommand::ProcessCommands() - DoSetInterval returned" );
                    break;
                default: // Unknown command
                    DEBUG1( "CWlanBgScanCommand::ProcessCommands() - unknown command (%d)", msg.iCommand );
                    err = KErrNotSupported;
                    break;
                }
            
            // complete client's request
            msg.iClientThread.RequestComplete( msg.iReportStatusPtr, err );
                
            // destroy the handle to the client thread
            msg.iClientThread.Close();

            break;
            }
        case KErrUnderflow:
            {
            DEBUG( "CWlanBgScanCommand::ProcessCommands() - signalled for pending message but queue was empty");
            iMsgQueue.CancelDataAvailable();
            DEBUG( "CWlanBgScanCommand::ProcessCommands() - DataAvailable notification cancelled");
            break;
            }
        default:
            {
            DEBUG1( "CWlanBgScanCommand::ProcessCommands() - unrecognised error code %d", rx_err );
            iMsgQueue.CancelDataAvailable();
            DEBUG( "CWlanBgScanCommand::ProcessCommands() - DataAvailable notification cancelled");
            ASSERT( 0 );
            break;
            }
        }

    // Subscribe notification about when new messages land on the queue
    iMsgQueue.NotifyDataAvailable( iStatus );
    
    SetActive();
    DEBUG( "CWlanBgScanCommand::ProcessCommands() - done");
    }

// ---------------------------------------------------------------------------
// Error in RunL.
//
// Runs in: WLAN Engine thread context.
// ---------------------------------------------------------------------------
//
TInt CWlanBgScanCommand::RunError( TInt aError )
    {
    if( KErrNone != aError )
        {
        DEBUG1( "CWlanBgScanCommand::RunError() - RunError %d", aError );
        }
    
    return KErrNone;
    }
