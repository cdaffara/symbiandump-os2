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
* Description:  Implementation of interface for WLAN services.
*
*/

/*
* %version: 31 %
*/

#include <in_sock.h>
#include "wlandevicesettings.h"
#include "rwlmserver.h"
#include "wlmscanrequest.h"
#include "am_debug.h"

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::ActivateNotifiesL(
    MWLMNotify& aCallback,
    TUint32 aNotificationMask )
    {
    DEBUG( "RWLMServer::ActivateNotifiesL()" );
    if( iNotify != NULL )
        {
        iNotify->Activate( aCallback, aNotificationMask );
        }
    else
        {
        iNotify = CWLMNotify::NewL( aCallback, *this, aNotificationMask );        
        }
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::CancelNotifies()
    {
    DEBUG( "RWLMServer::CancelNotifies()" );
    if( iNotify == NULL )
        {
        return;
        }
    if( iNotify->IsActive() )
        {
        // Notifications activated, request is pending
        iNotify->Cancel();
        }
    else
        {
        // Notifications activated, currently executing a notification
        //(i.e. in RunL of iNotify)
        iNotify->SetCancelled();
        }
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
void RWLMServer::CancelRequest()
    {
    DEBUG( "RWLMServer::CancelRequest()" );

    // Check is notify service activated.
    if( iNotify != NULL )
        {
        // Send synchronous request to the data server
        DEBUG( "RWLMServer::CancelRequest() Cancelling..." );
        SendReceive( ECancelNotifications, TIpcArgs() );
        DEBUG( "Done!" );
        }
    else
        {
        DEBUG( "RWLMServer::CancelRequest() Notification service was not started." );
        }
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::Close()
    {
    DEBUG( "RWLMServer::Close()" );
    delete iNotify;
    iNotify = NULL;
    delete iScanRequest;
    iScanRequest = NULL;
    delete iTsCreateRequest;
    iTsCreateRequest = NULL;
    RHandleBase::Close();
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::Connect()
    {
    DEBUG( "RWLMServer::Connect()" );

    // Check that server is running
    TInt err = StartServer();
    if( err != KErrNone )
        {
        DEBUG1( "RWLMServer::Connect() - unable to start server (%d)",
            err );

        return err;
        }

    // Create a new session
    err = CreateSession( KWLMDataServerName, Version() );
    if( err != KErrNone )
        {
        DEBUG1( "RWLMServer::Connect() - unable to create a session (%d)",
            err );

        return err;
        }

    iNotify = NULL;        
    iScanRequest = NULL;
    iTsCreateRequest = NULL;
    TRAP( err, iScanRequest = CWlmScanRequest::NewL( *this ) );
    if ( err != KErrNone )
        {
        DEBUG1( "RWLMServer::Connect() - unable to create CWlmScanRequest (%d)",
            err );
        RHandleBase::Close();

        return err;
        }

    return KErrNone;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetBSSID( TMacAddress& aBssid )
    {
    DEBUG( "RWLMServer::GetBSSID()" );

    TMacPckg mac;
    mac.data = KZeroMacAddr;
    TPckg<TMacPckg> inPckg( mac );
    TIpcArgs params( &inPckg );

    TInt ret = SendReceive( EGetMacAddress, params );
    if( ret == KErrNone )
        {
        Mem::Copy( &aBssid, &mac.data, sizeof( aBssid ) );
        }
    return ret;
    }


// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetNetworkName( TDes& aNwName )
    {
    DEBUG( "RWLMServer::GetNetworkName()" );

    // Get SSID from the server.
    TSSID ssid = { 0 };
    TPckg<TSSID> inPckg( ssid );
    TIpcArgs params( &inPckg );

    TInt ret = SendReceive( EGetNetworkName, params );
    if( ret == KErrNone )
        {
        // Convert it to text.    
        TBuf8<KMaxSsidStrLength> buf;
        buf.Format( _L8( "%s" ), ssid.ssid );
        buf.SetLength(
            ssid.ssidLength < static_cast<TUint>( aNwName.MaxLength() ) ? 
            ssid.ssidLength : 
            aNwName.MaxLength() );
        aNwName.Copy( buf );
        }
    return ret;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TWlanConnectionState RWLMServer::GetConnectionState()
    {
    DEBUG( "RWLMServer::GetConnectionState()" );

    // Call the server
    TWlanConnectionState connectionState( EWlanStateNotConnected );
    TPckg<TWlanConnectionState> outPckg( connectionState );
    TIpcArgs params( &outPckg );
    
    TInt ret = SendReceive( EGetCurrentState, params );
    if( ret != KErrNone )
        {
        return EConnectionStateUnknown;
        }
    else
        {
        return connectionState;
        }
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetConnectionRCPI( TInt32& aRCPI )
    {
    DEBUG( "RWLMServer::GetConnectionRCPI()" );

    TPckg<TInt32> inPckg( aRCPI );
    TIpcArgs params( &inPckg );
    return SendReceive( EGetCurrentRSSI, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetSecurityMode( TWlanSecurity& aMode )
    {
    DEBUG( "RWLMServer::GetSecurityMode()" );

    TPckg<TWlanSecurity> inPckg( aMode );
    TIpcArgs params( &inPckg );
    return SendReceive( EGetSecurityMode, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetSystemMode( TWlanSystemMode& aMode )
    {
    DEBUG( "RWLMServer::GetSystemMode()" );

    TPckg<TWlanSystemMode> inPckg( aMode );
    TIpcArgs params( &inPckg );
    return SendReceive( EGetSystemMode, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetScanResults(
    ScanList& aResults,
    const TDesC8& aSsid,
    TInt& aCacheLifetime,
    TUint& aMaxDelay ) 
    {
    DEBUG( "RWLMServer::GetScanResults( aResults, aSsid, aCacheLifetime, aMaxDelay )" );

    return iScanRequest->IssueRequest(
        aResults,
        aSsid,
        aCacheLifetime,
        aMaxDelay,
        NULL );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::GetScanResults(
    ScanList& aResults,
    const TDesC8& aSsid,
    TInt& aCacheLifetime,
    TUint& aMaxDelay,    
    TRequestStatus& aStatus )
    {
    DEBUG2( "RWLMServer::GetScanResults( aResults, aSsid, aCacheLifetime(%d), aMaxDelay(%u), aStatus )",
            aCacheLifetime, aMaxDelay );

    iScanRequest->IssueRequest(
        aResults,
        aSsid,
        aCacheLifetime,
        aMaxDelay,
        &aStatus );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::CancelGetScanResults()
    {
    DEBUG( "RWLMServer::CancelGetScanResults()" );

    if ( iScanRequest->IsActive() )
        {
        iScanRequest->Cancel();

        // Complete the pending client request.
        TRequestStatus* status = iScanRequest->ClientStatus();        
        User::RequestComplete( status, KErrCancel );
        }
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::Join(
    TRequestStatus& aStatus, 
    const TUint32 aId, 
    TWLMOverrideSettings* aOverrides )
    {
    DEBUG( "RWLMServer::Join()" );

    // Use the override settings if necessary
    if( aOverrides )
        {
        iOverridePckg = *aOverrides;
        }
    else
        {
        // override settings not used
        
        TWLMOverrideSettings overrides;
        overrides.settingsMask = EOverrideNoneMask;
        iOverridePckg = overrides;
        }

    aStatus = KRequestPending;
    TIpcArgs params( reinterpret_cast<TAny*>( aId ), &iOverridePckg );

    // Send the command
    SendReceive( EJoinByProfileId, params, aStatus );    
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::CancelJoin()
    {
    DEBUG( "RWLMServer::CancelJoin()" );

    // Send the command
    SendReceive( ECancelJoin );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::Release()
    {
    DEBUG( "RWLMServer::Release()" );
    return SendReceive( EReset, TIpcArgs() );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetAvailableIaps(
    TWlmAvailableIaps& aAvailableIaps,
    TInt& aCacheLifetime,
    TUint& aMaxDelay,
    TBool aFilteredResults )
    {
    DEBUG3( "RWLMServer::GetAvailableIaps( aAvailableIaps, aCacheLifetime(%d), aMaxDelay(%u), aFilteredResult(%u) )",
        aCacheLifetime, aMaxDelay, aFilteredResults );

    TRequestStatus status;
    TPckg<TWlmAvailableIaps> inPckg( aAvailableIaps );
    TPckg<TInt> inPckg2( aCacheLifetime );
    TPckg<TUint> inPckg3( aMaxDelay );
    GetAvailableIaps( inPckg, inPckg2, inPckg3, aFilteredResults, status );
    User::WaitForRequest( status );
    DEBUG1( "RWLMServer::GetAvailableIaps() - Server returned status %d", status.Int() );    

    return status.Int();
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::GetAvailableIaps( 
    TDes8& aAvailableIaps,
    TDes8& aCacheLifetime,
    TDes8& aMaxDelay,
    TBool aFilteredResults,
    TRequestStatus& aStatus )
    {
    DEBUG1( "RWLMServer::GetAvailableIaps( aAvailableIaps, aCacheLifetime, aMaxDelay, aFilteredResult(%u), aStatus )",
        aFilteredResults );

    aStatus = KRequestPending;
    TIpcArgs params( &aAvailableIaps, &aCacheLifetime, &aMaxDelay, aFilteredResults );
    SendReceive( EGetAvailableIaps, params, aStatus );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::CancelGetAvailableIaps()
    {
    DEBUG( "RWLMServer::CancelGetAvailableIaps()" );

    // Send the command
    SendReceive( ECancelGetAvailableIaps );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::NotifyChangedSettings()
    {
    DEBUG( "RWLMServer::NotifyChangedSettings()" );
    SendReceive( ENotifyChangedSettings, TIpcArgs() );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::AddBssidToRoguelist( const TMacAddress& aBssid )
    {
    DEBUG( "RWLMServer::AddBssidToRoguelist()" );

    TMacPckg mac;
    Mem::Copy( &mac.data, &aBssid, sizeof( aBssid ) );
    TPckg<TMacPckg> inPckg( mac );
    TIpcArgs params( &inPckg );
    return SendReceive( EAddBssidToRoguelist, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::UpdateRcpNotificationBoundary(
    const TInt32 aRcpLevelBoundary,
    const TInt32 aHysteresis )
    {
    DEBUG2( "RWLMServer::UpdateRcpNotificationBoundary(): %d, %d", aRcpLevelBoundary, aHysteresis );
    TIpcArgs params( aRcpLevelBoundary, aHysteresis );
    return SendReceive( EUpdateRcpNotificationBoundaries, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::ConfigureMulticast(
    TUint aCommand,
    const TMacAddress& aMulticastAddress )
    {
    DEBUG1( "RWLMServer::ConfigureMulticast(): %d", aCommand );
    DEBUG_BUFFER( KMacAddressLength, aMulticastAddress.iMacAddress );

    TMacPckg mac;
    Mem::Copy( &mac.data, &aMulticastAddress, sizeof( aMulticastAddress ) );
    TPckg<TMacPckg> inPckg( mac );
    TIpcArgs params( &inPckg, aCommand );
    return SendReceive( EConfigureMulticastGroup, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetPacketStatistics(
    TWlanPacketStatistics& aStatistics )
    {
    DEBUG( "RWLMServer::GetPacketStatistics()" );

    TPckg<TWlanPacketStatistics> inPckg( aStatistics );
    TIpcArgs params( &inPckg );
    return SendReceive( EGetPacketStatistics, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//      
EXPORT_C void RWLMServer::ClearPacketStatistics()
    {
    DEBUG( "RWLMServer::ClearPacketStatistics()" );

    SendReceive( EClearPacketStatistics, TIpcArgs() );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetUapsdSettings(
    TWlanUapsdSettings& aSettings )
    {
    DEBUG( "RWLMServer::GetUapsdSettings()" );
    
    TPckg<TWlanUapsdSettings> inPckg( aSettings );
    TIpcArgs params( &inPckg );
    return SendReceive( EGetUapsdSettings, params );    
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::SetUapsdSettings(
    const TWlanUapsdSettings& aSettings )
    {
    DEBUG( "RWLMServer::SetUapsdSettings()" );

    TPckg<const TWlanUapsdSettings> inPckg( aSettings );
    TIpcArgs params( &inPckg );
    return SendReceive( ESetUapsdSettings, params );    
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetPowerSaveSettings(
    TWlanPowerSaveSettings& aSettings )
    {
    DEBUG( "RWLMServer::GetPowerSaveSettings()" );
   
    TPckg<TWlanPowerSaveSettings> inPckg( aSettings );
    TIpcArgs params( &inPckg );
    return SendReceive( EGetPowerSaveSettings, params );    
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::SetPowerSaveSettings(
    const TWlanPowerSaveSettings& aSettings )
    {
    DEBUG( "RWLMServer::SetPowerSaveSettings()" );

    TPckg<const TWlanPowerSaveSettings> inPckg( aSettings );
    TIpcArgs params( &inPckg );
    return SendReceive( ESetPowerSaveSettings, params );   
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::RunProtectedSetup(
    TRequestStatus& aStatus,
    const TWlanSsid& aSsid,
    const TWlanWpsPin& aWspPin,  
    TDes8& aCredentials )
    {
    DEBUG( "RWLMServer::RunProtectedSetup(TRequestStatus)" );

    aStatus = KRequestPending;
    
    iWlanSsidPckg = aSsid;
    iWlanWpsPinPckg = aWspPin;
    
    TIpcArgs params( &iWlanSsidPckg, &iWlanWpsPinPckg, &aCredentials );

    // Send the command
    SendReceive( ERunProtectedSetup, params, aStatus );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::CancelProtectedSetup()
    {
    DEBUG( "RWLMServer::CancelProtectedSetup()" );

    // Send the command
    SendReceive( ECancelProtectedSetup );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::CreateTrafficStream(
    TRequestStatus& aStatus,
    const TWlanTrafficStreamParameters& aStreamParameters,
    TBool aIsAutomaticStream,
    TUint& aStreamId,
    TWlanTrafficStreamStatus& aStreamStatus )    
    {
    DEBUG( "RWLMServer::CreateTrafficStream()" );

    delete iTsCreateRequest;
    iTsCreateRequest = NULL;
    
    iTsCreateRequest = new TWlmTsCreateRequest(
        aStreamParameters,
        aStreamId,
        aStreamStatus );
    if( !iTsCreateRequest )
        {
        DEBUG( "RWLMServer::CreateTrafficStream() - unable to allocate TWlmTsCreateRequest" );

        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNoMemory );

        return;
        }

    TIpcArgs params(
        &iTsCreateRequest->iTsParameterPckg,
        reinterpret_cast<TAny*>( aIsAutomaticStream ),
        &iTsCreateRequest->iTsIdPckg,
        &iTsCreateRequest->iTsStatusPckg );

    // Send the command
    SendReceive( ECreateTrafficStream, params, aStatus );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::CancelCreateTrafficStream()
    {
    DEBUG( "RWLMServer::CancelCreateTrafficStream()" );

    // Send the command
    SendReceive( ECancelCreateTrafficStream );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::DeleteTrafficStream(
    TRequestStatus& aStatus,
    TUint aStreamId )
    {
    DEBUG( "RWLMServer::DeleteTrafficStream()" );

    TIpcArgs params(
        reinterpret_cast<TAny*>( aStreamId ) );

    // Send the command
    SendReceive( EDeleteTrafficStream, params, aStatus );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::CancelDeleteTrafficStream()
    {
    DEBUG( "RWLMServer::CancelDeleteTrafficStream()" );

    // Send the command
    SendReceive( ECancelDeleteTrafficStream );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetAccessPointInfo(
    TWlanAccessPointInfo& aInfo )
    {
    DEBUG( "RWLMServer::GetAccessPointInfo()" );

    TPckg<TWlanAccessPointInfo> inPckg( aInfo );
    TIpcArgs params( &inPckg );
    return SendReceive( EGetAccessPointInfo, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetRoamMetrics(
    TWlanRoamMetrics& aRoamMetrics )
    {
    DEBUG( "RWLMServer::GetRoamMetrics()" );

    TPckg<TWlanRoamMetrics> inPckg( aRoamMetrics );
    TIpcArgs params( &inPckg );
    return SendReceive( EGetRoamMetrics, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetRogueList(
    CArrayFixSeg<TWlanBssid>& aRogueList )
    {
    DEBUG( "RWLMServer::GetRogueList()" );
    
    aRogueList.Reset();
    TWlmRogueList rogueListStorage;
    TPckg<TWlmRogueList> rogueListBuf( rogueListStorage );
    
    TIpcArgs params( &rogueListBuf );
    TInt ret = SendReceive( EGetRogueList, params );
    
    for ( TUint idx( 0 ); idx < rogueListStorage.count; ++idx )
        {
        TRAP_IGNORE( aRogueList.AppendL(
            rogueListStorage.list[idx] ) )
        }
    return ret;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetRegulatoryDomain(
    TWlanRegion& aRegion )
    {
    DEBUG( "RWLMServer::GetRegulatoryDomain()" );

    TPckg<TWlanRegion> inPckg( aRegion );
    TIpcArgs params( &inPckg );
    return SendReceive( EGetRegulatoryDomain, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetPowerSaveMode(
    TWlanPowerSave& aPowerSaveMode )
    {
    DEBUG( "RWLMServer::GetPowerSaveMode()" );

    TPckg<TWlanPowerSave> inPckg( aPowerSaveMode );
    TIpcArgs params( &inPckg );
    return SendReceive( EGetPowerSaveMode, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::AddIapSsidList(
    TUint aIapId,
    const CArrayFixFlat<TWlanSsid>& aSsidList )
    {
    DEBUG( "RWLMServer::AddIapSsidList()" );

    TUint size(
        aSsidList.Count() * aSsidList.Length() );
    TIpcArgs params(
        reinterpret_cast<TAny*>( aIapId ),
        aSsidList.Count(),
        NULL );

    if( size )
        {    
        TPtr8 ssidListPtr(
            reinterpret_cast<TUint8*>(
                const_cast<TWlanSsid*>( aSsidList.Back( 0 ) ) ), size, size );
        params.Set( 2, &ssidListPtr );
        }

    // Send the command
    return SendReceive( EAddIapSsidList, params );    
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::RemoveIapSsidList(
    TUint aIapId )
    {
    DEBUG( "RWLMServer::RemoveIapSsidList()" );

    TIpcArgs params(
        reinterpret_cast<TAny*>( aIapId ) );

    // Send the command
    return SendReceive( ERemoveIapSsidList, params );    
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::SetPowerSaveMode(
    TWlanPowerSaveMode aMode )
    {
    DEBUG( "RWLMServer::SetPowerSaveMode()" );

    TIpcArgs params(
        reinterpret_cast<TAny*>( aMode ) );

    // Send the command
    return SendReceive( ESetPowerSaveMode, params );     
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::NotifyChangedPsmSrvMode(
        TInt aMode )
    {
    DEBUG( "RWLMServer::NotifyChangedPsmSrvMode()" );

    TIpcArgs params(
        reinterpret_cast<TAny*>( aMode ) );

    // Send the command
    return SendReceive( ENotifyChangedPsmSrvMode, params );     
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::GetAcTrafficStatus(
    TWlmAcTrafficStatusArray& aArray )
    {
    DEBUG( "RWLMServer::GetAcTrafficStatus()" );

    TPckg<TWlmAcTrafficStatusArray> inPckg( aArray ); 
    TIpcArgs params( &inPckg );
    return SendReceive( EGetAcTrafficStatus, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::DirectedRoam(
    TRequestStatus& aStatus,
    const TMacAddress& aBssid )
    {
    DEBUG( "RWLMServer::DirectedRoam()" );

    iRoamBssidPckg = aBssid;
    TIpcArgs params(
        &iRoamBssidPckg );

    // Send the command
    SendReceive( EDirectedRoam, params, aStatus );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C void RWLMServer::CancelDirectedRoam()
    {
    DEBUG( "RWLMServer::CancelDirectedRoam()" );

    // Send the command
    SendReceive( ECancelDirectedRoam );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
EXPORT_C TInt RWLMServer::StartAggressiveBgScan(
    TUint aInterval )
    {
    DEBUG1( "RWLMServer::StartAggressiveBgScan( %u )",
        aInterval );
    
    TIpcArgs params( aInterval );
    
    // Send the command as a "blind" request
    return Send( EStartAggressiveBgScan, params );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
TInt RWLMServer::StartServer()
    {
    DEBUG( "RWLMServer::StartServer()" );

    RProcess server;
    RSemaphore globStartSignal;
    TFindServer findWlanServer( KWLMDataServerName );
    TFullName name;

    // Does the server already exist?
    if ( findWlanServer.Next( name ) == KErrNone )
        {
        DEBUG( "RWLMServer::StartServer() Server does exist!" );
        return KErrNone;
        }

    DEBUG( "RWLMServer::StartServer() Server is not loaded -- load it." );

    // Create semaphore to allow server to signal when it starts
    TInt err = globStartSignal.CreateGlobal( KWLMServerSemaphore, 0 );
    if ( err != KErrNone )
        {
        DEBUG( "RWLMServer::StartServer() semafore is in use..." );
        // If semaphore already exists, open it and wait
        err = globStartSignal.OpenGlobal( KWLMServerSemaphore,EOwnerProcess );
        if ( err == KErrNone )
            {
            DEBUG( "Waiting release of the semafore..." );
            globStartSignal.Wait();
            return KErrNone;
            }
        else
            {
            DEBUG( "RWLMServer::StartServer() Opening semafore failed." );
            return err;
            }
        }

    DEBUG( "RWLMServer::StartServer() Creating server process." );

    // Create the server process
    TBufC<1> cmdline;

    err = server.Create( KWLMServerExe, cmdline, EOwnerThread );
    if ( err == KErrNone )
        {
        DEBUG( "RWLMServer::StartServer() set priority..." );
        server.SetPriority( EPriorityHigh );
        }
    else
        {
        DEBUG1( "RWLMServer::StartServer() Server creation failed (%d).", err );
        return err;
        }

    // Resume server process
    server.Resume();
    server.Close();

    // Wait for server process to initialise
    globStartSignal.Wait();

    // Delete semaphore
    globStartSignal.Close();

    // Ensure that server is really there
    DEBUG( "RWLMServer::StartServer() Ensure that server is really there..." );
    findWlanServer.Find( KWLMDataServerName );
    TInt r = findWlanServer.Next( name );
    DEBUG1( "RWLMServer::StartServer() server search returns %d", r );
    return r;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
TVersion RWLMServer::Version() const
    {
    DEBUG( "RWLMServer::Version()" );
    return( TVersion(
        KWLMServMajorVersionNumber,
        KWLMServMinorVersionNumber,
        KWLMServBuildVersionNumber ) );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
void RWLMServer::WaitForNotification(
    TRequestStatus& aReturnValue,
    TDes8& aReturnData,
    TUint32 aNotificationMask )
    {
    DEBUG( "RWLMServer::WaitForNotification()" );
    TIpcArgs params( &aReturnData, reinterpret_cast<TAny*>( aNotificationMask ) );
    SendReceive( EOrderNotifications, params, aReturnValue );
    }
