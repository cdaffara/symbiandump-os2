/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Session class is required by the client/server framework
*
*/

/*
* %version: 24 %
*/

// INCLUDE FILES
#include "wlmsession.h"
#include "genscanlist.h"
#include "wlmserver.h"
#include "am_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWlmSession::CWlmSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWlmSession::CWlmSession(
    CWlmServer& aServer ) :
    iId( 0 ),
    iWlmServer( aServer ),
    iIsNotificationRequestPending( EFalse ),
    iNotificationHandle( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CWlmSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWlmSession::ConstructL()
    {
    iPendingNotifications.Reset();
    iId = iWlmServer.NotifySessionCreated();
    }

// -----------------------------------------------------------------------------
// CWlmSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWlmSession* CWlmSession::NewL(
    CWlmServer& aServer )
    {
    CWlmSession* self = new( ELeave ) CWlmSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CWlmSession::~CWlmSession()
    {
    iPendingNotifications.Close();

    if( iNotificationHandle )
        {
        iWlmServer.NotifyRemove( *iNotificationHandle );
        delete iNotificationHandle;
        }
    iWlmServer.NotifySessionClosed( iId );
    }

// ---------------------------------------------------------
// CWlmSession::ServiceL
// ---------------------------------------------------------
//
void CWlmSession::ServiceL(
    const RMessage2& aMessage )
    {    
    DEBUG1( "CWlmSession: Function %d received", aMessage.Function() );
#ifdef _DEBUG    
    TInt totalAllocated = 0;
    TInt cellCount = User::AllocSize( totalAllocated );
    DEBUG2("WLMSERVER MEMORYINFO: mem allocated == %d, cellcount == %d",
        totalAllocated, cellCount );
#endif // _DEBUG

    TInt cmd = aMessage.Function();
    switch ( cmd )
        {
        case EOrderNotifications:
            HandleOrderNotifications( aMessage );
            break;
        // --------------------------------------------------------------------
        case ECancelNotifications:
            HandleCancelNotifications( aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetScanResults:
            iWlmServer.GetScanResult( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ECancelGetScanResults:
            iWlmServer.CancelGetScanResult( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EJoinByProfileId:
            // Joining/creating network requires capabilities
            aMessage.HasCapabilityL( ECapabilityNetworkServices );
            iWlmServer.Connect( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ECancelJoin:
            iWlmServer.CancelConnect( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EReset:
            // Releasing network connections requires capabilities
            aMessage.HasCapabilityL( ECapabilityNetworkServices );
            iWlmServer.ReleaseComplete( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetAvailableIaps:
            iWlmServer.GetAvailableIaps( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ECancelGetAvailableIaps:
            iWlmServer.CancelGetAvailableIaps( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetCurrentState:
            iWlmServer.GetConnectionState( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetCurrentRSSI:
            iWlmServer.GetCurrentRCPI( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetMacAddress:
            iWlmServer.GetCurrentBSSID( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetNetworkName:
            iWlmServer.GetCurrentSSID( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetSecurityMode:
            iWlmServer.GetCurrentSecurityMode( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetSystemMode:
            iWlmServer.GetCurrentSystemMode( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ENotifyChangedSettings:
            iWlmServer.NotifyChangedSettings( iId, aMessage );
            break;        
        // --------------------------------------------------------------------
        case EAddBssidToRoguelist:
            iWlmServer.AddBssidToRoguelist( iId, aMessage );
            break;        
        // --------------------------------------------------------------------
        case EUpdateRcpNotificationBoundaries:
            iWlmServer.UpdateRcpNotificationBoundary( iId, aMessage );
            break;        
        // --------------------------------------------------------------------
        case EConfigureMulticastGroup:
            iWlmServer.ConfigureMulticastGroup( iId, aMessage );
            break;        
        // --------------------------------------------------------------------
        case EGetPacketStatistics:
            iWlmServer.GetPacketStatistics( iId, aMessage );
            break;        
        // --------------------------------------------------------------------
        case EClearPacketStatistics:
            iWlmServer.ClearPacketStatistics( iId, aMessage );
            break;        
        // --------------------------------------------------------------------
        case EGetUapsdSettings:
            iWlmServer.GetUapsdSettings( iId, aMessage );
            break;        
        // --------------------------------------------------------------------
        case ESetUapsdSettings:
            iWlmServer.SetUapsdSettings( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetPowerSaveSettings:
            iWlmServer.GetPowerSaveSettings( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ESetPowerSaveSettings:
            iWlmServer.SetPowerSaveSettings( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ERunProtectedSetup:
            iWlmServer.RunProtectedSetup( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ECancelProtectedSetup:
            iWlmServer.CancelProtectedSetup( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ECreateTrafficStream:
            iWlmServer.CreateTrafficStream( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ECancelCreateTrafficStream:
            iWlmServer.CancelCreateTrafficStream( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EDeleteTrafficStream:
            iWlmServer.DeleteTrafficStream( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ECancelDeleteTrafficStream:
            iWlmServer.CancelDeleteTrafficStream( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetAccessPointInfo:
            iWlmServer.GetAccessPointInfo( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetRoamMetrics:
            iWlmServer.GetRoamMetrics( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetRogueList:
            iWlmServer.GetRogueList( iId, aMessage );
            break;
            // --------------------------------------------------------------------
        case EGetRegulatoryDomain:
            iWlmServer.GetRegulatoryDomain( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetPowerSaveMode:
            iWlmServer.GetPowerSaveMode( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EAddIapSsidList:
            iWlmServer.AddIapSsidListL( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ERemoveIapSsidList:
            iWlmServer.RemoveIapSsidListL( iId, aMessage );
            break;      
        // --------------------------------------------------------------------
        case ESetPowerSaveMode:
            iWlmServer.SetPowerSaveMode( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ENotifyChangedPsmSrvMode:
            iWlmServer.NotifyChangedPsmSrvMode( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EGetAcTrafficStatus:
            iWlmServer.GetAcTrafficStatus( iId, aMessage );
            break;                
        // --------------------------------------------------------------------
        case EDirectedRoam:
            iWlmServer.DirectedRoam( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case ECancelDirectedRoam:
            iWlmServer.CancelDirectedRoam( iId, aMessage );
            break;
        // --------------------------------------------------------------------
        case EStartAggressiveBgScan:
            iWlmServer.StartAggressiveBgScan( iId, aMessage );
            break;        
        // --------------------------------------------------------------------
        default:
            aMessage.Complete( KErrNotSupported );
        }
    }

// ---------------------------------------------------------
// CWlmSession::HandleOrderNotifications
// ---------------------------------------------------------
//
void CWlmSession::HandleOrderNotifications(
    const RMessage2& aMessage )
    {
    iPendingNotificationRequest = aMessage;
    iIsNotificationRequestPending = ETrue;
    if( !iNotificationHandle ) // == NULL i.e. is not activated
        {
        TRAPD( err, iNotificationHandle =
            CSessionNotification::NewL( *this, aMessage.Int1() ) );
        if ( err == KErrNone )
            {
            iWlmServer.NotifyAdd( *iNotificationHandle );
            }
        else
            {
            iIsNotificationRequestPending = EFalse;
            aMessage.Complete( err );
            return;
            }
        }
    HandleNotification(); // check is there any unsent notifications
    }

// ---------------------------------------------------------
// CWlmSession::HandleCancelNotifications
// ---------------------------------------------------------
//
void CWlmSession::HandleCancelNotifications(
    const RMessage2& aMessage )
    {
    if( iIsNotificationRequestPending )
        {
        iPendingNotificationRequest.Complete( EWlmNotifyNotificationsCancelled );
        iIsNotificationRequestPending = EFalse;
        iPendingNotifications.Reset();
        }
    if( iNotificationHandle )
        {
        iWlmServer.NotifyRemove( *iNotificationHandle );
        delete iNotificationHandle;
        iNotificationHandle = NULL;
        }
    aMessage.Complete( KErrNone );
    }
        
// ---------------------------------------------------------
// CWlmSession::AddNotification
// ---------------------------------------------------------
//
void CWlmSession::AddNotification(
    TInt aNotification,
    const TDesC8& aData )
    {
    DEBUG( "CWlmSession::AddNotification" );

    TNotification notif;
    notif.id = aNotification;
    notif.data = aData;

    iPendingNotifications.Append( notif );
    HandleNotification(); // check is there client waiting for notification
    }

// ---------------------------------------------------------
// CWlmSession::HandleNotification
// ---------------------------------------------------------
//
void CWlmSession::HandleNotification()
    {
    DEBUG( "CWlmSession::HandleNotification" );

    // Check is there message to wait notification and 
    // notification that is not sent.
    if ( iIsNotificationRequestPending && iPendingNotifications.Count() != 0 )
        {
        DEBUG( "CWlmSession::HandleNotification - sending response..." );
        iIsNotificationRequestPending = EFalse;

        TPckgBuf<TWlmNotifyData> pckg;
        pckg().data = iPendingNotifications[0].data;
        TInt ret( iPendingNotificationRequest.Write( 0, pckg ) );
        if ( ret != KErrNone )
            {
            iPendingNotificationRequest.Complete( ret );
            return;
            }
        iPendingNotificationRequest.Complete( iPendingNotifications[0].id );
        iPendingNotifications.Remove( 0 );
        }
    }

//  End of File  
