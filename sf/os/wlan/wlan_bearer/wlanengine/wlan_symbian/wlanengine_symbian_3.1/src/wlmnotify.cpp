/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Notification class
*
*/

/*
* %version: 16 %
*/

#include "rwlmserver.h"
#include "am_debug.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWLMNotify::ConstructL
// ---------------------------------------------------------
//
void CWLMNotify::ConstructL()
    {
    DEBUG( "CWLMNotify::ConstructL()" );
    CActiveScheduler::Add( this );
    WaitForNotifications();
    }

// ---------------------------------------------------------
// CWLMNotify::CWLMNotify
// ---------------------------------------------------------
//
CWLMNotify::CWLMNotify(
    MWLMNotify& aCallback,
    RWLMServer& aServer,
    TUint32 aNotificationMask ):
    CActive( CActive::EPriorityStandard ), 
    iCallback( &aCallback ),
    iServer( aServer ),
    iDataPckg(),
    iCancelRequested( EFalse ),
    iNotificationMask( aNotificationMask )
    {
    DEBUG( "CWLMNotify::CWLMNotify()" );
    }

// ---------------------------------------------------------
// CWLMNotify::~CWLMNotify
// ---------------------------------------------------------
//
CWLMNotify::~CWLMNotify()
    {
    DEBUG( "CWLMNotify::~CWLMNotify()" );
    Cancel();
    iCallback = NULL;
    }

// ---------------------------------------------------------
// CWLMNotify::DoCancel
// This is been called when active object should be 
// cancelled. This is called by CActive::Cancel().
// ---------------------------------------------------------
//
void CWLMNotify::DoCancel()
    {
    DEBUG( "CWLMNotify::DoCancel()" );
    iServer.CancelRequest();
    }

// ---------------------------------------------------------
// CWLMNotify::NewL
// ---------------------------------------------------------
//
CWLMNotify* CWLMNotify::NewL(
    MWLMNotify& aCallback,
    RWLMServer& aServer,
    TUint32 aNotificationMask )
    {
    DEBUG( "CWLMNotify::NewL()" );
    CWLMNotify* self = new (ELeave) CWLMNotify( aCallback, aServer, aNotificationMask );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWLMNotify::RunL
// This is called when a response is received to a 
// asynchronous message. This method dispatches 
// notifications to client's interface.
// ---------------------------------------------------------
//
void CWLMNotify::RunL()
    {
    DEBUG1( "CWLMNotify::RunL() - status == %d", iStatus.Int() );

    if( iStatus == KRequestPending )
        {
        DEBUG( "CWLMNotify::RunL() - request still pending!" );
        return;
        }

    if( iStatus == KErrServerTerminated )
        {
        DEBUG( "CWLMNotify::RunL() - server terminated" );
        return;
        }

    switch( iStatus.Int() )
        {
        case EWlmNotifyConnectionStateChanged:
            {
            iCallback->ConnectionStateChanged(
                static_cast<TWlanConnectionState>( iDataPckg().data[0] ));
            break;
            }
        case EWlmNotifyBssidChanged:
            {
            iCallback->BSSIDChanged( iDataPckg().data );
            break;
            }
        case EWlmNotifyBssLost:
            {
            iCallback->BSSLost();
            break;
            }
        case EWlmNotifyBssRegained:
            {
            iCallback->BSSRegained();
            break;
            }
        case EWlmNotifyNewNetworksDetected:
        	{
        	iCallback->NewNetworksDetected();
        	break;
        	}
        case EWlmNotifyOldNetworksLost:
        	{
        	iCallback->OldNetworksLost();
        	break;
        	}
        case EWlmNotifyTransmitPowerChanged:
            {
            TUint power( 0 );
            // safer to copy than cast: alignment might not be correct
            Mem::Copy( &power, iDataPckg().data.Ptr(), sizeof( power ) );
            DEBUG1( "CWLMNotify::RunL() - transmit power changed to %u", power );
            iCallback->TransmitPowerChanged( power );
            break;
            }
        case EWlmNotifyNotificationsCancelled:
            {
            DEBUG( "CWLMNotify::RunL() - notifications cancelled" );
            return; // Do NOT call WaitForNotifications()
            }
        case EWlmNotifyRcpChanged:
            {
            TWlanRCPLevel rcpLevel = static_cast<TWlanRCPLevel>( iDataPckg().data[0] );
            TUint rcp = static_cast<TUint>( iDataPckg().data[1] );
            DEBUG2( "CWLMNotify::RunL() - EWlmNotifyRcpChanged %d, %d", rcpLevel, rcp );
            iCallback->RcpChanged( rcpLevel, rcp );
            break;
            }
        case EWlmNotifyTrafficStreamStatusChanged:
            {
            TUint id( 0 );
            // safer to copy than cast: alignment might not be correct
            Mem::Copy( &id, iDataPckg().data.Ptr(), sizeof( id ) );
            TWlanTrafficStreamStatus status =
                static_cast<TWlanTrafficStreamStatus>( iDataPckg().data[4] );
            DEBUG2( "CWLMNotify::RunL() - status of traffic stream %u changed to %u", id, status );
            iCallback->TrafficStreamStatusChanged( id, status );
            break;
            }
        case EWlmNotifyAccessPointInfoChanged:
            {
            TWlanAccessPointInfo info;
            Mem::Copy( &info, iDataPckg().data.Ptr(), sizeof( info ) );

            DEBUG5( "CWLMNotify::RunL() - AP info changed (Voice = %u, Video = %u, BestEffort = %u, Background = %u, WPX = %u)",
                info.isAcRequiredForVoice,
                info.isAcRequiredForVideo,
                info.isAcRequiredForBestEffort,
                info.isAcRequiredForBackground,
                info.isWpx );

            iCallback->AccessPointInfoChanged( info );
            break;
            }
        case EWlmNotifyAcTrafficModeChanged:
            {
            TWlmAccessClass accessClass(
                static_cast<TWlmAccessClass>( iDataPckg().data[0] ) );
            TWlmAcTrafficMode mode(
                static_cast<TWlmAcTrafficMode>( iDataPckg().data[1] ) );

            DEBUG2( "CWLMNotify::RunL() - traffic mode of access class (%u) changed to %u",
                accessClass, mode );

            iCallback->AccessClassTrafficModeChanged(
                accessClass,
                mode );
            break;
            }
        case EWlmNotifyAcTrafficStatusChanged:
            {
            TWlmAccessClass accessClass(
                static_cast<TWlmAccessClass>( iDataPckg().data[0] ) );
            TWlmAcTrafficStatus status(
                static_cast<TWlmAcTrafficStatus>( iDataPckg().data[1] ) );

            DEBUG2( "CWLMNotify::RunL() - traffic status of access class (%u) changed to %u",
                accessClass, status );

            iCallback->AccessClassTrafficStatusChanged(
                accessClass,
                status );
            break;
            }
        default:
            DEBUG1( "CWLMNotify::RunL() - unknown notification: %d", iStatus.Int() );
        }
        
    // Make a new notification request if allowed
    if( !iCancelRequested )
        {
        WaitForNotifications();
        }
    else
        {
        DEBUG("CWLMNotify::RunL() - cancellation has been requested -> cancelling request.");
        iServer.CancelRequest();
        }
    }

// ---------------------------------------------------------
// CWLMNotify::WaitForNotifications
// The loop goes: WaitForNotifications->RunL->
// WaitForNotifications->...
// ---------------------------------------------------------
//
void CWLMNotify::WaitForNotifications()
    {
    DEBUG( "CWLMNotify::WaitForNotifications()" );
    iServer.WaitForNotification( iStatus, iDataPckg, iNotificationMask );    
    SetActive();
    }

// ---------------------------------------------------------
// CWLMNotify::Activate
// ---------------------------------------------------------
//
void CWLMNotify::Activate(
    MWLMNotify& aCallback,
    TUint32 aNotificationMask )
    {
    DEBUG( "CWLMNotify::Activate()" );
    iCallback = &aCallback;
    iCancelRequested = EFalse;
    iNotificationMask = aNotificationMask;
    WaitForNotifications();
    }

// ---------------------------------------------------------
// CWLMNotify::SetCancelled
// ---------------------------------------------------------
//
void CWLMNotify::SetCancelled()
    {
    iCancelRequested = ETrue;
    }
