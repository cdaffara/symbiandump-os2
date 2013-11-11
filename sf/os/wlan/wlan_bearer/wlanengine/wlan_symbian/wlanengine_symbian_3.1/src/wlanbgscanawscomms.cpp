/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements BgScan AWS communication.
*
*/

/*
* %version: 4 %
*/

#include <e32base.h>
#include <e32cmn.h>
#include <e32std.h>

#include "awsinterface.h"
#include "wlanbgscanawscommsinterface.h"
#include "awsenginebase.h"
#include "wlanbgscancommandlistener.h"
#include "wlanbgscancommand.h"
#include "wlanbgscanawscomms.h"
#include "wlanbgscan.h"
#include "am_debug.h"

/**
 * Maximum heap size for AWS thread.
 */
const TInt KMaxHeapSize     = 0x10000;

/**
 * BgScan version communicated to AWS.
 */
const TUint KBgScanVersion  = 1;

/**
 * Index of the first item in an array.
 */
const TInt KFirstItemIndex = 0;

MWlanBgScanAwsComms* CWlanBgScanAwsCommsFactory::InstanceL( CWlanBgScan& aBgScan )
    {
    DEBUG( "CWlanBgScanAwsCommsFactory::Instance()" );
    return CWlanBgScanAwsComms::NewL( aBgScan );
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWlanBgScanAwsComms::CWlanBgScanAwsComms
// ---------------------------------------------------------------------------
//
CWlanBgScanAwsComms::CWlanBgScanAwsComms( CWlanBgScan& aBgScan ) :
    CActive( CActive::EPriorityStandard ),
    iBgScan( aBgScan ),
    iAws( NULL ),
    iAwsImplUid( 0 ),
    iCommandHandler( NULL ),
    iAwsVersion( 0 ),
    iPendingCommand( EAwsCommandMax )
    {
    DEBUG( "CWlanBgScanAwsComms::CWlanBgScanAwsComms()" );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanAwsComms::~CWlanBgScanAwsComms()
// ---------------------------------------------------------------------------
//
CWlanBgScanAwsComms::~CWlanBgScanAwsComms()
    {
    DEBUG( "CWlanBgScanAwsComms::~CWlanBgScanAwsComms()" );

    delete iAws;
    iAws = NULL;

    delete iCommandHandler;
    iCommandHandler = NULL;

    iAwsMsgQueue.Close();
    Cancel();
    iWlanEngineThread.Close();
    }

// ---------------------------------------------------------------------------
// CWlanBgScanAwsComms::NewL
// ---------------------------------------------------------------------------
//
CWlanBgScanAwsComms* CWlanBgScanAwsComms::NewL( CWlanBgScan& aBgScan )
    {
    DEBUG( "CWlanBgScanAwsComms::NewL()" );
    CWlanBgScanAwsComms* self = new ( ELeave ) CWlanBgScanAwsComms( aBgScan );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// ---------------------------------------------------------------------------
// CWlanBgScanAwsComms::ConstructL
// ---------------------------------------------------------------------------
//
void CWlanBgScanAwsComms::ConstructL()
    {
    DEBUG( "CWlanBgScanAwsComms::ConstructL()" );
    
    // Get handle to current (WLAN Engine) thread so that
    // the handle can be later used for completing the
    // request status from AWS thread.
    TThreadId id = RThread().Id();
    TInt err = iWlanEngineThread.Open( id );
    if( err != KErrNone )
        {
        DEBUG( "CWlanBgScanAwsComms::ConstructL() - Opening own thread handle failed" );
        User::Leave( KErrBadHandle );
        }
        
    // create handler for incoming messages from AWS
    iCommandHandler = CWlanBgScanCommand::NewL( *this );
    
    CActiveScheduler::Add( this );
    
    DEBUG( "CWlanBgScanAwsComms::ConstructL() - Starting AWS thread" );
    
    // leaves if no AWS present in system
    StartAwsThreadL();
            
    DEBUG( "CWlanBgScanAwsComms::ConstructL() - done" );    
    }

// ---------------------------------------------------------------------------
// CWlanBgScanAwsComms::CleanupEComArray
// ---------------------------------------------------------------------------
//
void CWlanBgScanAwsComms::CleanupEComArray(TAny* aArray)
    {
    DEBUG( "CWlanBgScanAwsComms::CleanupEComArray()" );
    
    ASSERT( aArray );
    
    RImplInfoPtrArray *implInfoArray = static_cast<RImplInfoPtrArray*> ( aArray );
    implInfoArray->ResetAndDestroy();
    implInfoArray->Close();
    }

// ---------------------------------------------------------------------------
// CWlanBgScanAwsComms::StartAwsThreadL
//
// This method is executing in the context of the WLAN Engine thread.
// ---------------------------------------------------------------------------
//
void CWlanBgScanAwsComms::StartAwsThreadL()
    {
    DEBUG( "CWlanBgScanAwsComms::StartAwsThreadL()" );
    
    RImplInfoPtrArray awsImplArray;
    TCleanupItem awsImplArrayCleanup( CleanupEComArray, &awsImplArray );
    CleanupStack::PushL( awsImplArrayCleanup );
            
    CAwsEngineBase::ListImplementationsL( awsImplArray );
    
    if( awsImplArray.Count() == 0 )
        {
        DEBUG( "CWlanBgScanAwsComms::StartAwsThreadL() - no AWS implementation found" );
        User::Leave( KErrNotFound );
        }
    
    // first found AWS implementation will be taken into use
    CImplementationInformation* awsImplInfo = static_cast<CImplementationInformation*>( awsImplArray[KFirstItemIndex] );
    iAwsImplUid = awsImplInfo->ImplementationUid().iUid;

    DEBUG( "CWlanBgScanAwsComms::InstantiateAwsPluginL() - trying to instantiate AWS implementation:" );
    DEBUG1( "CWlanBgScanAwsComms::InstantiateAwsPluginL() - ImplementationUid:  0x%08X", awsImplInfo->ImplementationUid().iUid );
#ifdef _DEBUG
    TBuf8<KPrintLineLength> buf8;
    buf8.Copy( awsImplInfo->DisplayName() );
#endif
    DEBUG1S( "CWlanBgScanAwsComms::InstantiateAwsPluginL() - DisplayName:        ", buf8.Length(), buf8.Ptr() );
    DEBUG1(  "CWlanBgScanAwsComms::InstantiateAwsPluginL() - Version:            %i", awsImplInfo->Version() );
    DEBUG1S( "CWlanBgScanAwsComms::InstantiateAwsPluginL() - DataType:           ", awsImplInfo->DataType().Length(), awsImplInfo->DataType().Ptr() );
    DEBUG1S( "CWlanBgScanAwsComms::InstantiateAwsPluginL() - OpaqueData:         ", awsImplInfo->OpaqueData().Length(), awsImplInfo->OpaqueData().Ptr() );
    DEBUG1(  "CWlanBgScanAwsComms::InstantiateAwsPluginL() - RomOnly:            %i", awsImplInfo->RomOnly() );
    DEBUG1(  "CWlanBgScanAwsComms::InstantiateAwsPluginL() - RomBased:           %i", awsImplInfo->RomBased() );
    DEBUG1(  "CWlanBgScanAwsComms::InstantiateAwsPluginL() - VendorId:           0x%08X", awsImplInfo->VendorId().iId );
                
    DEBUG( "CWlanBgScanAwsComms::StartAwsThreadL() - setting request status to pending" );
    iStatus = KRequestPending;
    DEBUG( "CWlanBgScanAwsComms::StartAwsThreadL() - setting pending command to EAwsStartupCompleted" );
    iPendingCommand = EAwsStartupCompleted;
    DEBUG( "CWlanBgScanAwsComms::StartAwsThreadL() - setting ao active" );
    SetActive();
    
    DEBUG( "CWlanBgScanAwsComms::StartAwsThreadL() - creating AWS thread" );
    RThread thread;
    TInt err = thread.Create( awsImplInfo->DisplayName(),
                                AwsThreadEntryPoint,
                                KDefaultStackSize,
                                KMinHeapSize,
                                KMaxHeapSize, 
                                reinterpret_cast<TAny*>( this ) );
    
    CleanupStack::PopAndDestroy( &awsImplArray ); //this causes a call to CleanupEComArray
        
    if( err != KErrNone)
        {
        DEBUG1( "CWlanBgScanAwsComms::StartAwsThreadL() - error: thread creation failed with error %i", err );
        User::Leave( err );
        }

    DEBUG( "CWlanBgScanAwsComms::StartAwsThreadL() - resuming AWS thread" );
    thread.Resume();
    thread.Close();

    DEBUG( "CWlanBgScanAwsComms::StartAwsThreadL() - done" );    
    }

// -----------------------------------------------------------------------------
// CWlanBgScanAwsComms::AwsThreadEntryPoint
//
// This method is executing in the context of the AWS thread.
// -----------------------------------------------------------------------------
//
TInt CWlanBgScanAwsComms::AwsThreadEntryPoint( TAny* aThisPtr )
    {
    DEBUG( "CWlanBgScanAwsComms::AwsThreadEntryPoint()" );
    
    CWlanBgScanAwsComms* self = static_cast<CWlanBgScanAwsComms*>( aThisPtr );
    
    // create cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if ( cleanup == NULL )
        {
        DEBUG("CWlanBgScanAwsComms::AwsThreadEntryPoint() - error: Cleanup stack creation failed");
        User::Exit( KErrNoMemory );
        }
    
    __UHEAP_MARK;
    
    TRAPD( err, self->InstantiateAndRunAwsPluginL() );
    if ( err != KErrNone )
        {
        DEBUG1( "CWlanBgScanAwsComms::AwsThreadEntryPoint() - error: AWS instantiation leaved, completing request to WLAN Engine thread with code %i", err );
        TRequestStatus *status = &self->iStatus;
        self->iWlanEngineThread.RequestComplete( status, err );
        }
    
    __UHEAP_MARKEND;

    delete cleanup;
    cleanup = NULL;
    DEBUG( "CWlanBgScanAwsComms::AwsThreadEntryPoint() - AWS thread exiting" );
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CWlanBgScanAwsComms::InstantiateAndRunAwsPluginL
//
// This method is executing in the context of the AWS thread.
// -----------------------------------------------------------------------------
//
void CWlanBgScanAwsComms::InstantiateAndRunAwsPluginL()
    {
    DEBUG( "CWlanBgScanAwsComms::InstantiateAndRunAwsPluginL()" );
    
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );
    
    DEBUG( "CWlanBgScanAwsComms::InstantiateAndRunAwsPluginL() - active scheduler installed" );
    CAwsEngineBase::TAwsEngineConstructionParameters params = { this, KBgScanVersion, iAwsVersion };

    TRequestStatus *status = &iStatus;
    
    DEBUG( "CWlanBgScanAwsComms::InstantiateAndRunAwsPluginL() - calling CAwsEngineBase::NewL()" );
    iAws = CAwsEngineBase::NewL( iAwsImplUid, &params ); // iAws deallocated in destructor

    DEBUG1( "CWlanBgScanAwsComms::InstantiateAndRunAwsPluginL() - AWS version %u instantiated", iAwsVersion );
    
    DEBUG( "CWlanBgScanAwsComms::InstantiateAndRunAwsPluginL() - completing request to WLAN Engine thread" );
    iWlanEngineThread.RequestComplete( status, KErrNone );
    
    DEBUG( "CWlanBgScanAwsComms::InstantiateAndRunAwsPluginL() - starting active scheduler - AWS is now in control of this thread" );
    CActiveScheduler::Start();

    // Thread execution will stay in CActiveScheduler::Start() until active scheduler is stopped
    DEBUG("CWlanBgScanAwsComms::InstantiateAndRunAwsPluginL() - active scheduler stopped" );

    // clean up
    CleanupStack::PopAndDestroy( scheduler );
    
    DEBUG( "CWlanBgScanAwsComms::InstantiateAndRunAwsPluginL() - done" );
    }

// ---------------------------------------------------------------------------
// From class MAwsBgScanProvider.
// CWlanBgScanAwsComms::SetInterval
//
// This method is executing in the context of the AWS thread.
// ---------------------------------------------------------------------------
//
void CWlanBgScanAwsComms::SetInterval( TUint32 aNewInterval, TRequestStatus& aStatus )
    {
    DEBUG1( "CWlanBgScanAwsComms::SetInterval() - new interval %u", aNewInterval );
        
    iCommandHandler->CommandQueue( CWlanBgScanCommand::ESetInterval, aNewInterval, aStatus );
    
    DEBUG( "CWlanBgScanAwsComms::SetInterval() - returning" );    
    }

// ---------------------------------------------------------------------------
// CWlanBgScanAwsComms::SendOrQueueAwsCommand
//
// This method is executing in the context of the WLAN Engine thread.
// ---------------------------------------------------------------------------
//
void CWlanBgScanAwsComms::SendOrQueueAwsCommand( TAwsMessage& aMessage )
    {
    DEBUG1( "CWlanBgScanAwsComms::SendOrQueueAwsCommand( command %u )", aMessage.iCmd );
    
    // if a request is pending, queue the new command
    if( iStatus.Int() == KRequestPending )
        {
        DEBUG( "CWlanBgScanAwsComms::SendOrQueueAwsCommand() - request pending -> queue new command" );

        // store command to queue
        TInt err = iAwsMsgQueue.Append( aMessage );
        if( KErrNone == err )
            {
            DEBUG( "CWlanBgScanAwsComms::SendOrQueueAwsCommand() - command queued successfully" );
            }
        else
            {
            DEBUG( "CWlanBgScanAwsComms::SendOrQueueAwsCommand() - command queueing failed" );
            }
        return;
        }

    SendAwsCommand( aMessage );
    
    DEBUG( "CWlanBgScanAwsComms::SendOrQueueAwsCommand() - done" );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanAwsComms::SendAwsCommand
//
// This method is executing in the context of the WLAN Engine thread.
// ---------------------------------------------------------------------------
//
void CWlanBgScanAwsComms::SendAwsCommand( TAwsMessage& aMessage )
    {
    DEBUG1( "CWlanBgScanAwsComms::SendAwsCommand( command %u )", aMessage.iCmd );

    if( !iAws )
        {
        DEBUG( "CWlanBgScanAwsComms::SendAwsCommand() - error: no AWS present!" );
        ASSERT( 0 );
        return;
        }
    
    DEBUG( "CWlanBgScanAwsComms::SendAwsCommand() - sending command" );

    switch( aMessage.iCmd )
        {
        case EStart:
            {
            iPendingCommand = EStart;
            iAws->Start( iStatus );
            SetActive();
            DEBUG( "CWlanBgScanAwsComms::SendAwsCommand() - EStart command sent" );
            break;
            }
        case EStop:
            {
            iPendingCommand = EStop;
            iAws->Stop( iStatus );
            SetActive();
            DEBUG( "CWlanBgScanAwsComms::SendAwsCommand() - EStop command sent" );
            break;
            }
        case ESetPowerSaveMode:
            {
            iPendingCommand = ESetPowerSaveMode;
            iAws->SetPowerSaveMode( static_cast<TAwsPsMode>( aMessage.iParameter ), iStatus );
            SetActive();
            DEBUG( "CWlanBgScanAwsComms::SendAwsCommand() - ESetPowerSaveMode command sent" );
            break;
            }
        default:
            {
            iPendingCommand = EAwsCommandMax;
            DEBUG1( "CWlanBgScanAwsComms::SendAwsCommand() - unknown command %u", aMessage.iCmd );
            ASSERT( 0 );
            }
        }
    DEBUG( "CWlanBgScanAwsComms::SendAwsCommand() - done" );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanAwsComms::RunL
//
// This method is executing in the context of the WLAN Engine thread.
// ---------------------------------------------------------------------------
//
void CWlanBgScanAwsComms::RunL()
    {   
    DEBUG2( "CWlanBgScanAwsComms::RunL() - command: %u, completion status: %d", iPendingCommand, iStatus.Int() );
    
    switch ( iPendingCommand )
        {
        case EAwsStartupCompleted:
            {
            DEBUG( "CWlanBgScanAwsComms::RunL() - AWS startup has been completed" );
            iBgScan.AwsStartupComplete( iStatus.Int() );
            break;
            }
        default:
            {
            DEBUG( "CWlanBgScanAwsComms::RunL() - AWS command has been completed" );
            iBgScan.AwsCommandComplete( iPendingCommand, iStatus.Int() );
            break;
            }
        }
    
    TAwsMessage cmd = { EAwsCommandMax, NULL };

    // if there are more commands, send the next one
    if( iAwsMsgQueue.Count() )
        {
        cmd = static_cast<TAwsMessage> ( iAwsMsgQueue[KFirstItemIndex] );
        iAwsMsgQueue.Remove( KFirstItemIndex );
        
        SendAwsCommand( cmd );
        }
    }

// ---------------------------------------------------------------------------
// CWlanBgScanAwsComms::DoCancel
// ---------------------------------------------------------------------------
//
void CWlanBgScanAwsComms::DoCancel()
    {
    DEBUG( "CWlanBgScanAwsComms::DoCancel()" );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanAwsComms::RunError
// ---------------------------------------------------------------------------
//
TInt CWlanBgScanAwsComms::RunError( TInt aError )
    {
    DEBUG1( "CWlanBgScanAwsComms::RunError( %d )", aError );
    return aError;
    }

// ---------------------------------------------------------------------------
// CWlanBgScanAwsComms::DoSetInterval
// ---------------------------------------------------------------------------
//
void CWlanBgScanAwsComms::DoSetInterval( TUint32 aNewInterval )
    {
    DEBUG1( "CWlanBgScanAwsComms::DoSetInterval( aNewInterval: %u )", aNewInterval );
    
    iBgScan.DoSetInterval( aNewInterval );
    }

