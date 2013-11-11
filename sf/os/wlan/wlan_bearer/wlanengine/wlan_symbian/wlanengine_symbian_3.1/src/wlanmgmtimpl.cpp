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
* Description:  Implementation of wlan management API
*
*/

/*
* %version: 34 %
*/

// INCLUDE FILES
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "gendebug.h"
#include "wlanmgmtimpl.h"
#include "wlanscaninfoimpl.h"

// LOCAL CONSTANTS
/**
* Pairs ECom implementation UIDs with a pointer to the instantiation 
* method for that implementation. Required for all ECom implementation
* collections.
*/
//lint --e{611}
const TImplementationProxy ImplementationTable[] =
    {
        {{0x101f8eff}, reinterpret_cast<TProxyNewLPtr>( CWlanMgmtImpl::NewL )},
        {{0x101f8f01}, reinterpret_cast<TProxyNewLPtr>( CWlanScanInfoImpl::NewL )}
    };

/** The default values for scan scheduling parameters. */
const TInt KWlanMgmtDefaultCacheLifetime = -1;
const TUint KWlanMgmtDefaultMaxDelay = 0;

/**
 * A mapping table between WLAN Management callback API versions
 * and internal notifications.
 */
const TUint KWlanMgmtCallbackInterfaceMap[KWlanCallbackInterfaceVersion] =
    { KWlmDefaultNotificationsV1,
      KWlmDefaultNotificationsV2 };

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWlanMgmtImpl::CWlanMgmtImpl() : 
    iScanRequest( NULL ),
    iPendingScanStatus( NULL ),
    iAvailableIapsRequest( NULL ),
    iPendingAvailableIapsStatus( NULL ),
	iClientNotification( NULL ),
	iProtectedSetupRequest( NULL ),
	iPendingProtectedSetupStatus( NULL ),
	iCacheLifetime( 0 ),
	iMaxDelay( 0 )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::CWlanMgmtImpl()" ) ) );
    }

// Symbian 2nd phase constructor can leave.
void CWlanMgmtImpl::ConstructL()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::ConstructL()" ) ) );

    User::LeaveIfError( iServer.Connect() );
    }

// Static constructor.
CWlanMgmtImpl* CWlanMgmtImpl::NewL()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::NewL()" ) ) );

    CWlanMgmtImpl* self = new (ELeave) CWlanMgmtImpl;
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CWlanMgmtImpl::~CWlanMgmtImpl()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::~CWlanMgmtImpl()" ) ) );

    CancelGetAvailableIaps();
    CancelGetScanResults();
    CancelProtectedSetup();
    iServer.Close();
    delete iScanRequest;
    delete iAvailableIapsRequest;
    delete iProtectedSetupRequest;
    iPendingScanStatus = NULL;
    iPendingAvailableIapsStatus = NULL;
    iClientNotification = NULL;
    iPendingProtectedSetupStatus = NULL;
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::ActivateNotificationsL
// ---------------------------------------------------------
//
void CWlanMgmtImpl::ActivateNotificationsL(
    MWlanMgmtNotifications& aCallback )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::ActivateNotificationsL()" ) ) );
	iClientNotification = &aCallback;
	iServer.ActivateNotifiesL( *this, KWlmDefaultNotificationsV1 );
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::CancelNotifications
// ---------------------------------------------------------
//
void CWlanMgmtImpl::CancelNotifications()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::CancelNotifications()" ) ) );
	iServer.CancelNotifies();
	iClientNotification = NULL;
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetScanResults
// ---------------------------------------------------------
//
void CWlanMgmtImpl::GetScanResults(
    TRequestStatus& aStatus,
    CWlanScanInfo& aResults )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetScanResults()" ) ) );
        
    aStatus = KRequestPending;
    iPendingScanStatus = &aStatus;

    // Because CWlanScanInfo has been initialized through ECom
    // interface, the object is actually CWlanScanInfoImpl.
    CWlanScanInfoImpl* impl = reinterpret_cast<CWlanScanInfoImpl*>(&aResults);

    TWlanSsid tmp;
    iCacheLifetime = KWlanMgmtDefaultCacheLifetime;
    iMaxDelay = KWlanMgmtDefaultMaxDelay;
    
    iScanRequest = new CWlanScanRequest( *this, iServer, *impl, tmp, iCacheLifetime, iMaxDelay );
	if ( !iScanRequest )
		{
		User::RequestComplete( iPendingScanStatus, KErrNoMemory );
		iPendingScanStatus = NULL;
		return;
		}

    iScanRequest->IssueRequest();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetScanResults
// ---------------------------------------------------------
//
void CWlanMgmtImpl::GetScanResults(
    TWlanSsid& aSsid,
    TRequestStatus& aStatus,
    CWlanScanInfo& aResults )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetScanResults() (directed scan)" ) ) );

    aStatus = KRequestPending;
    iPendingScanStatus = &aStatus;

    iCacheLifetime = KWlanMgmtDefaultCacheLifetime;
    iMaxDelay = KWlanMgmtDefaultMaxDelay;

    // Because CWlanScanInfo has been initialized through ECom
    // interface, the object is actually CWlanScanInfoImpl.
    CWlanScanInfoImpl* impl = reinterpret_cast<CWlanScanInfoImpl*>(&aResults);

    iScanRequest = new CWlanScanRequest( *this, iServer, *impl, aSsid, iCacheLifetime, iMaxDelay );
	if ( !iScanRequest )
		{
		User::RequestComplete( iPendingScanStatus, KErrNoMemory );
		iPendingScanStatus = NULL;
		return;
		}

    iScanRequest->IssueRequest();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetScanResults
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::GetScanResults(
    CWlanScanInfo& aResults )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetScanResults() (2)" ) ) );

    // Because CWlanScanInfo has been initialized through ECom
    // interface, the object is actually CWlanScanInfoImpl.
    CWlanScanInfoImpl* impl = reinterpret_cast<CWlanScanInfoImpl*>(&aResults);

    TWlanSsid tmp;
    iCacheLifetime = KWlanMgmtDefaultCacheLifetime;
    iMaxDelay = KWlanMgmtDefaultMaxDelay;
    
    TInt ret = iServer.GetScanResults( impl->GetInternalList(), tmp, iCacheLifetime, iMaxDelay );
    if ( ret != KErrNone )
        {
        TraceDump( INFO_LEVEL, ( _L( "GetScanResults() returned with %d" ), ret ) );
        return ret;
        }

    return impl->UpdateResults();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetScanResults
// ---------------------------------------------------------
//
void CWlanMgmtImpl::GetScanResults(
    TInt& aCacheLifetime,
    TUint& aMaxDelay,
    TWlanSsid& aSsid,
    TRequestStatus& aStatus,
    CWlanScanInfo& aResults )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetScanResults() (scan scheduling)" ) ) );

    aStatus = KRequestPending;
    iPendingScanStatus = &aStatus;

    // Because CWlanScanInfo has been initialized through ECom
    // interface, the object is actually CWlanScanInfoImpl.
    CWlanScanInfoImpl* impl = reinterpret_cast<CWlanScanInfoImpl*>(&aResults);

	iScanRequest = new CWlanScanRequest( *this, iServer, *impl, aSsid, aCacheLifetime, aMaxDelay );
	if ( !iScanRequest )
		{
		User::RequestComplete( iPendingScanStatus, KErrNoMemory );
		iPendingScanStatus = NULL;
		return;
		}

    iScanRequest->IssueRequest();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetConnectionBssid
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::GetConnectionBssid(
    TWlanBssid& aBssid )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetConnectionBssid()" ) ) );

	TMacAddress bssid;
	TInt ret = iServer.GetBSSID( bssid );
	if ( ret != KErrNone )
		{
		return ret;
		}

	aBssid.Copy( &bssid.iMacAddress[0], KMacAddressLength ); 
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetConnectionSsid
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::GetConnectionSsid(
    TWlanSsid& aSsid )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetConnectionSsid()" ) ) );

    TBuf16<KWlanMaxSsidLength> ssid;
	TInt ret = iServer.GetNetworkName( ssid );
	if ( ret != KErrNone )
		{
		return ret;
		}
	
	aSsid.Copy( ssid );
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetConnectionSignalQuality
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::GetConnectionSignalQuality(
    TInt32& aSignalQuality )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetConnectionSignalQuality()" ) ) );
    
    TInt32 rcpi = 0;
    iServer.GetConnectionRCPI( rcpi );
    // RCPI -> RSSI CONVERSION
    // Note: conversion may round the result by 0.5 units
    aSignalQuality = ( 110 - ( rcpi / 2 ) );
    TraceDump( INFO_LEVEL, ( _L( " - (RCP, RSS) == (%u, %u)"), rcpi, aSignalQuality ) );
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetConnectionState
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::GetConnectionMode(
    TWlanConnectionMode& aMode )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetConnectionMode()" ) ) );

	TWlanConnectionState state = iServer.GetConnectionState();
	switch ( state )
		{
		case EWlanStateInfrastructure:
			aMode = EWlanConnectionModeInfrastructure;
			break;
		case EWlanStateSearchingAP:
			aMode = EWlanConnectionModeSearching;
			break;
		case EWlanStateIBSS:
			aMode = EWlanConnectionModeAdhoc;
			break;
		case EWlanStateSecureInfra:
			aMode = EWlanConnectionModeSecureInfra;
			break;
		default:
			aMode = EWlanConnectionModeNotConnected;
			break;
        }
	
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetConnectionSecurityMode
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::GetConnectionSecurityMode(
    TWlanConnectionSecurityMode& aMode )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetConnectionSecurityMode()" ) ) );

	TWlanSecurity mode( EWlanSecurityOpen );
	TInt ret = iServer.GetSecurityMode( mode );
	if ( ret != KErrNone )
		{
		return ret;
		}

	switch ( mode )
		{
		case EWlanSecurityWepOpen: // Fallthrough on purpose
		case EWlanSecurityWepShared:
			aMode = EWlanConnectionSecurityWep;
			break;
		case EWlanSecurity802d1x:  // Fallthrough on purpose
		case EWlanSecurityWapi:    // Fallthrough on purpose
		case EWlanSecurityWapiPsk:
			aMode = EWlanConnectionSecurity802d1x;
			break;
		case EWlanSecurityWpa: // Fallthrough on purpose
		case EWlanSecurityWpa2:
			aMode = EWlanConnectionSecurityWpa;
			break;
		case EWlanSecurityWpaPsk: // Fallthrough on purpose
		case EWlanSecurityWpa2Psk:
			aMode = EWlanConnectionSecurityWpaPsk;
			break;
		default:
			aMode = EWlanConnectionSecurityOpen;
			break;
        }

    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetAvailableIaps
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::GetAvailableIaps(
    RArray<TUint>& aAvailableIaps )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetAvailableIaps()" ) ) );
    
    TWlmAvailableIaps iaps;
    aAvailableIaps.Reset();

    iCacheLifetime = KWlanMgmtDefaultCacheLifetime;
    iMaxDelay = KWlanMgmtDefaultMaxDelay;

	TInt ret = iServer.GetAvailableIaps( iaps, iCacheLifetime, iMaxDelay, ETrue );
	if ( ret != KErrNone )
		{
		return ret;
		}   
    for ( TUint idx( 0 ); idx < iaps.count; ++idx )
        {
        aAvailableIaps.Append( iaps.iaps[idx].iapId );
        }

    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetAvailableIaps
// ---------------------------------------------------------
//
void CWlanMgmtImpl::GetAvailableIaps(
    TRequestStatus& aStatus,
    RArray<TUint>& aAvailableIaps )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetAvailableIaps() (async)" ) ) );
    
    aAvailableIaps.Reset();
    
    aStatus = KRequestPending;
    iPendingAvailableIapsStatus = &aStatus;    

    iCacheLifetime = KWlanMgmtDefaultCacheLifetime;
    iMaxDelay = KWlanMgmtDefaultMaxDelay;

	iAvailableIapsRequest = new CWlanAvailableIapsRequest( *this, iServer, aAvailableIaps, iCacheLifetime, iMaxDelay );
	if ( !iAvailableIapsRequest )
		{
		User::RequestComplete( iPendingAvailableIapsStatus, KErrNoMemory );
		iPendingAvailableIapsStatus = NULL;
		return;
		}
    iAvailableIapsRequest->IssueRequest();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetAvailableIaps
// ---------------------------------------------------------
//
void CWlanMgmtImpl::GetAvailableIaps(
	TInt& aCacheLifetime,
	TUint& aMaxDelay,
	TRequestStatus& aStatus,
    RArray<TUint>& aAvailableIaps )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetAvailableIaps() (async)" ) ) );
    
    aAvailableIaps.Reset();
    
    aStatus = KRequestPending;
    iPendingAvailableIapsStatus = &aStatus;    
    
	iAvailableIapsRequest = new CWlanAvailableIapsRequest( *this, iServer, aAvailableIaps, aCacheLifetime, aMaxDelay );
	if ( !iAvailableIapsRequest )
		{
		User::RequestComplete( iPendingAvailableIapsStatus, KErrNoMemory );
		iPendingAvailableIapsStatus = NULL;
		return;
		}
    iAvailableIapsRequest->IssueRequest();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::NotifyChangedSettings
// ---------------------------------------------------------
//
void CWlanMgmtImpl::NotifyChangedSettings()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::NotifyChangedSettings()" ) ) );

    iServer.NotifyChangedSettings();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::AddBssidToBlacklist
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::AddBssidToBlacklist(
    const TWlanBssid& aBssid )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::AddBssidToBlacklist()" ) ) );
    TMacAddress bssid;
    Mem::Copy( &bssid.iMacAddress[0], aBssid.Ptr(), sizeof( TMacAddress ) );
    return iServer.AddBssidToRoguelist( bssid );
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::UpdateRssNotificationBoundary
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::UpdateRssNotificationBoundary( 
    const TInt32 aRssLevelBoundary,
    const TInt32 aHysteresis )
    {
    // Do some basic checks:
    if( aRssLevelBoundary <= KRssMin || aRssLevelBoundary > KRssMax )
        {
        return KErrArgument;
        }
    if( aHysteresis <= KRssHysteresisMin || aHysteresis > KRssHysteresisMax)
        {
        return KErrArgument;
        }

    // RSSI -> RCPI Conversion
    TInt32 rcpLevelBoundary = ( 110 - aRssLevelBoundary ) * 2;
    // Hysteresis Conversion
    TInt32 hysteresis = ( ( 110 - ( aRssLevelBoundary - aHysteresis ) ) * 2 ) - rcpLevelBoundary;

    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::UpdateRssNotificationBoundary()" ) ) );
    TraceDump( INFO_LEVEL, ( _L( "RSS(%d, %d) -> RCP(%d, %d)" ),
        aRssLevelBoundary, aHysteresis,
        rcpLevelBoundary, hysteresis ) );

    return iServer.UpdateRcpNotificationBoundary( rcpLevelBoundary, hysteresis );
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::RunProtectedSetup
// ---------------------------------------------------------
//
void CWlanMgmtImpl::RunProtectedSetup(
    TRequestStatus& aStatus,
    const TWlanSsid& aSsid,
    const TWlanWpsPin& aWpsPin,
    CArrayFixSeg<TWlanProtectedSetupCredentialAttribute>& aCredentials )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::RunProtectedSetup()" ) ) );

    aCredentials.Reset();

    aStatus = KRequestPending;
    iPendingProtectedSetupStatus = &aStatus;

    iProtectedSetupRequest = new CProtectedSetupRequest( *this, iServer, aSsid, aWpsPin, aCredentials );
    if ( !iProtectedSetupRequest )
        {
        User::RequestComplete( iPendingProtectedSetupStatus, KErrNoMemory );
        iPendingProtectedSetupStatus = NULL;
        return;
        }

    iProtectedSetupRequest->IssueRequest();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::CancelProtectedSetup
// ---------------------------------------------------------
//
void CWlanMgmtImpl::CancelProtectedSetup()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::CancelProtectedSetup()" ) ) );

    if( iProtectedSetupRequest &&
        iProtectedSetupRequest->IsActive() )
        {
        iProtectedSetupRequest->Cancel();
        ProtectedSetupComplete( KErrCancel );
        }
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::CancelGetScanResults
// ---------------------------------------------------------
//
void CWlanMgmtImpl::CancelGetScanResults()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::CancelGetScanResults()" ) ) );

    if( iScanRequest &&
        iScanRequest->IsActive() )
        {
        iScanRequest->Cancel();
        ScanComplete( KErrCancel );
        }
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::CancelGetAvailableIaps
// ---------------------------------------------------------
//
void CWlanMgmtImpl::CancelGetAvailableIaps()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::CancelGetAvailableIaps()" ) ) );

    if( iAvailableIapsRequest &&
        iAvailableIapsRequest->IsActive() )
        {
        iAvailableIapsRequest->Cancel();
        AvailableIapsComplete( KErrCancel );
        }
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::AddIapSsidList
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::AddIapSsidList(
    TUint aIapId,
    const CArrayFixFlat<TWlanSsid>& aSsidList )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::AddIapSsidList()" ) ) );
    
    return iServer.AddIapSsidList( aIapId, aSsidList );
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::RemoveIapSsidList
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::RemoveIapSsidList(
    TUint aIapId )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::RemoveIapSsidList()" ) ) );

    return iServer.RemoveIapSsidList( aIapId );    
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetExtendedConnectionSecurityMode
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::GetExtendedConnectionSecurityMode(
    TWlanConnectionExtentedSecurityMode& aMode )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetExtendedConnectionSecurityMode()" ) ) );

    TWlanSecurity mode( EWlanSecurityOpen );
    TInt ret = iServer.GetSecurityMode( mode );
    if ( ret != KErrNone )
        {
        return ret;
        }

    switch ( mode )
        {
        case EWlanSecurityWepOpen:
            aMode = EWlanConnectionExtentedSecurityModeWepOpen;
            break;
        case EWlanSecurityWepShared:
            aMode = EWlanConnectionExtentedSecurityModeWepShared;
            break;
        case EWlanSecurity802d1x:
            aMode = EWlanConnectionExtentedSecurityMode802d1x;
            break;
        case EWlanSecurityWpa:
            aMode = EWlanConnectionExtentedSecurityModeWpa;
            break;
        case EWlanSecurityWpa2:
            aMode = EWlanConnectionExtentedSecurityModeWpa2;
            break;
        case EWlanSecurityWpaPsk:
            aMode = EWlanConnectionExtentedSecurityModeWpaPsk;
            break;
        case EWlanSecurityWpa2Psk:
            aMode = EWlanConnectionExtentedSecurityModeWpa2Psk;
            break;
        case EWlanSecurityWapi:
            aMode = EWlanConnectionExtentedSecurityModeWapi;
            break;
        case EWlanSecurityWapiPsk:
            aMode = EWlanConnectionExtentedSecurityModeWapiPsk;
            break;
        default:
            aMode = EWlanConnectionExtentedSecurityModeOpen;
            break;
        }

    return KErrNone;    
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::ActivateExtendedNotificationsL
// ---------------------------------------------------------
//
void CWlanMgmtImpl::ActivateExtendedNotificationsL(
    MWlanMgmtNotifications& aCallback,
    TUint aCallbackInterfaceVersion )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::ActivateExtendedNotificationsL()" ) ) );
    iClientNotification = &aCallback;

    if( !aCallbackInterfaceVersion ||
        aCallbackInterfaceVersion > KWlanCallbackInterfaceVersion )
        {
        aCallbackInterfaceVersion = KWlanCallbackInterfaceVersion;
        }

    iServer.ActivateNotifiesL(
        *this,
        KWlanMgmtCallbackInterfaceMap[aCallbackInterfaceVersion-1] );
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::ActivateExtendedNotificationsL
// ---------------------------------------------------------
//
void CWlanMgmtImpl::CreateTrafficStream(
    TRequestStatus& aStatus,
    const TWlanTrafficStreamParameters& aStreamParameters,
    TUint& aStreamId,
    TWlanTrafficStreamStatus& aStreamStatus )
    {
    iServer.CreateTrafficStream(
        aStatus,
        aStreamParameters,
        EFalse,
        aStreamId,
        aStreamStatus );        
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::CancelCreateTrafficStream
// ---------------------------------------------------------
//
void CWlanMgmtImpl::CancelCreateTrafficStream()
    {
    iServer.CancelCreateTrafficStream();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::DeleteTrafficStream
// ---------------------------------------------------------
//
void CWlanMgmtImpl::DeleteTrafficStream(
    TRequestStatus& aStatus,
    TUint aStreamId )
    {
    iServer.DeleteTrafficStream(
        aStatus,
        aStreamId );
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::CancelDeleteTrafficStream
// ---------------------------------------------------------
//
void CWlanMgmtImpl::CancelDeleteTrafficStream()
    {
    iServer.CancelDeleteTrafficStream();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::DirectedRoam
// ---------------------------------------------------------
//
void CWlanMgmtImpl::DirectedRoam(
    TRequestStatus& aStatus,
    const TWlanBssid& aBssid )
    {
    TMacAddress bssid;
    Mem::Copy( &bssid.iMacAddress[0], aBssid.Ptr(), sizeof( TMacAddress ) );   
    iServer.DirectedRoam(
        aStatus,
        bssid );
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::CancelDirectedRoam
// ---------------------------------------------------------
//
void CWlanMgmtImpl::CancelDirectedRoam()
    {
    iServer.CancelDirectedRoam();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::ProtectedSetupComplete
// ---------------------------------------------------------
//
void CWlanMgmtImpl::ProtectedSetupComplete(
    TInt aStatus )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::ProtectedSetupComplete()" ) ) );     
    User::RequestComplete( iPendingProtectedSetupStatus, aStatus );
    delete iProtectedSetupRequest;
    iProtectedSetupRequest = NULL;
    iPendingProtectedSetupStatus = NULL;
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::StartAggressiveBgScan
// ---------------------------------------------------------
//
TInt CWlanMgmtImpl::StartAggressiveBgScan(
    TUint aInterval )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::StartAggressiveBgScan()" ) ) );
    return iServer.StartAggressiveBgScan( aInterval );
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::GetAvailableIaps
// ---------------------------------------------------------
//
void CWlanMgmtImpl::GetAvailableIaps(
    TInt& aCacheLifetime,
    TUint& aMaxDelay,
    TBool aFilteredResults,
    TRequestStatus& aStatus,
    RArray<TWlanIapAvailabilityData>& aAvailableIaps )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::GetAvailableIaps() (async)" ) ) );
    
    aAvailableIaps.Reset();
    
    aStatus = KRequestPending;
    iPendingAvailableIapsStatus = &aStatus;    
    iAvailableIapsRequest = new CWlanAvailableIapsRequest( *this, iServer, aAvailableIaps, aCacheLifetime, aMaxDelay, aFilteredResults );
    if ( !iAvailableIapsRequest )
        {
        User::RequestComplete( iPendingAvailableIapsStatus, KErrNoMemory );
        iPendingAvailableIapsStatus = NULL;
        return;
        }
    iAvailableIapsRequest->IssueRequest();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::ScanComplete
// ---------------------------------------------------------
//
void CWlanMgmtImpl::ScanComplete(
    TInt aStatus )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::ScanComplete()" ) ) );    	

    User::RequestComplete( iPendingScanStatus, aStatus );
	delete iScanRequest;
	iScanRequest = NULL;
    iPendingScanStatus = NULL;
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::AvailableIapsComplete
// ---------------------------------------------------------
//
void CWlanMgmtImpl::AvailableIapsComplete(
    TInt aStatus )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::AvailableIapsComplete()" ) ) );    	

    User::RequestComplete( iPendingAvailableIapsStatus, aStatus );
	delete iAvailableIapsRequest;    
	iAvailableIapsRequest = NULL;   
    iPendingAvailableIapsStatus = NULL;
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::ConnectionStateChanged
// ---------------------------------------------------------
//
void CWlanMgmtImpl::ConnectionStateChanged(
    TWlanConnectionState aNewState )
	{
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::ConnectionStateChanged()" ) ) );    	
	
	TWlanConnectionMode mode( EWlanConnectionModeNotConnected );

	switch ( aNewState )
		{
		case EWlanStateInfrastructure:
			mode = EWlanConnectionModeInfrastructure;
			break;
		case EWlanStateSearchingAP:
			mode = EWlanConnectionModeSearching;
			break;
		case EWlanStateIBSS:
			mode = EWlanConnectionModeAdhoc;
			break;
		case EWlanStateSecureInfra:
			mode = EWlanConnectionModeSecureInfra;
			break;
		default:
			break;
        }

	iClientNotification->ConnectionStateChanged( mode );
	}
    
// ---------------------------------------------------------
// CWlanMgmtImpl::BSSIDChanged
// ---------------------------------------------------------
//
void CWlanMgmtImpl::BSSIDChanged(
    TDesC8& aNewBSSID )
	{
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::BSSIDChanged()" ) ) );    	
	
	TWlanBssid bssid;
	bssid.Copy( aNewBSSID );
	iClientNotification->BssidChanged( bssid );
	}

// ---------------------------------------------------------
// CWlanMgmtImpl::BSSLost
// ---------------------------------------------------------
//
void CWlanMgmtImpl::BSSLost()
	{
	TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::BSSLost()" ) ) );
	
	iClientNotification->BssLost();
	}

// ---------------------------------------------------------
// CWlanMgmtImpl::BSSRegained
// ---------------------------------------------------------
//
void CWlanMgmtImpl::BSSRegained()
	{
	TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::BSSRegained()" ) ) );
	
	iClientNotification->BssRegained();
	}

// ---------------------------------------------------------
// CWlanMgmtImpl::NewNetworksDetected
// ---------------------------------------------------------
//
void CWlanMgmtImpl::NewNetworksDetected()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::NewNetworksDetected()" ) ) );
    
    iClientNotification->NewNetworksDetected();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::OldNetworksLost
// ---------------------------------------------------------
//    
void CWlanMgmtImpl::OldNetworksLost()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::OldNetworksLost()" ) ) );
    
    iClientNotification->OldNetworksLost();
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::TransmitPowerChanged
// ---------------------------------------------------------
//
void CWlanMgmtImpl::TransmitPowerChanged(
    TUint aPower )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::TransmitPowerChanged()" ) ) );
    
    iClientNotification->TransmitPowerChanged( aPower );
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::RssChanged
// ---------------------------------------------------------
//
void CWlanMgmtImpl::RcpChanged(
    TWlanRCPLevel aRcpLevel,
    TUint aRcpValue )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanMgmtImpl::RcpChanged: %u, %u"), aRcpLevel, aRcpValue ) );
    
    // Convert TWlanRCPLevel -> TWlanRssClass
    // (These types must be kept in synch)
    TWlanRssClass rssClass = EWlanRssClassNormal;
    switch( aRcpLevel )
        {
        case EWlanRcpNormal:
            rssClass = EWlanRssClassNormal;
            break;
        case EWlanRcpWeak:
            rssClass = EWlanRssClassWeak;
            break;
        }
    
    // Convert RCPI -> RSSI...
    // Note: conversion may round the result by 0.5 units
    TUint rssValue = 110 - ( aRcpValue / 2 );
    
    TraceDump( INFO_LEVEL, ( _L( " - Converted RSS values: %u, %u"), rssClass, rssValue ) );
    
    iClientNotification->RssChanged( rssClass, rssValue );
    }

// ---------------------------------------------------------
// CWlanMgmtImpl::TrafficStreamStatusChanged
// ---------------------------------------------------------
//
void CWlanMgmtImpl::TrafficStreamStatusChanged(
    TUint aStreamId,
    TWlanTrafficStreamStatus aStreamStatus )
    {
    iClientNotification->TrafficStreamStatusChanged(
        aStreamId, aStreamStatus );
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWlanScanRequest::CWlanScanRequest( 
    CWlanMgmtImpl& aCallback, 
    RWLMServer& aServer, 
    CWlanScanInfoImpl& aScanInfo ,
    const TWlanSsid& aSsid,
    TInt& aCacheLifetime,
    TUint& aMaxDelay ) :
    CActive( CActive::EPriorityStandard ),
    iCallback( aCallback ),
    iServer( aServer ),
    iScanInfo( aScanInfo ),
    iSsid( aSsid ),
    iCacheLifetime( aCacheLifetime ),
    iMaxDelay( aMaxDelay )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanScanRequest::CWlanScanRequest()" ) ) );
    CActiveScheduler::Add( this );
    }

// Destructor
CWlanScanRequest::~CWlanScanRequest()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanScanRequest::~CWlanScanRequest()" ) ) );
    Cancel();
    }

// ---------------------------------------------------------
// CWlanScanRequest::IssueRequest
// ---------------------------------------------------------
//
void CWlanScanRequest::IssueRequest()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanScanRequest::IssueRequest()" ) ) );
    iServer.GetScanResults( iScanInfo.GetInternalList(), iSsid, iCacheLifetime, iMaxDelay, iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CWlanScanRequest::RunL
// ---------------------------------------------------------
//
void CWlanScanRequest::RunL()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanScanRequest::RunL()" ) ) );
    TInt ret = iStatus.Int();
    if ( ret == KErrNone )
        {
        ret = iScanInfo.UpdateResults();
        }       

    iCallback.ScanComplete( ret );
    }

// ---------------------------------------------------------
// CWlanScanRequest::RunError
// ---------------------------------------------------------
//
TInt CWlanScanRequest::RunError(
    TInt /* aError */ )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanScanRequest::RunError()" ) ) );

    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanScanRequest::DoCancel
// ---------------------------------------------------------
//
void CWlanScanRequest::DoCancel()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanScanRequest::DoCancel()" ) ) );
    
    iServer.CancelGetScanResults();
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWlanAvailableIapsRequest::CWlanAvailableIapsRequest(
    CWlanMgmtImpl& aCallback, 
    RWLMServer& aServer,
    RArray<TUint>& aAvailableIaps,
    TInt& aCacheLifetime,
    TUint& aMaxDelay ) :
    CActive( CActive::EPriorityStandard ),
    iCallback( aCallback ),
    iServer( aServer ),
    iPendingAvailableIapIds( &aAvailableIaps ),
    iPendingAvailableIaps( NULL ),
    iAvailableIapsBuf( iAvailableIaps ),
    iCacheLifetimeBuf( aCacheLifetime ),
    iMaxDelayBuf( aMaxDelay ),
    iFilteredResults( ETrue )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanAvailableIapsRequest::CWlanAvailableIapsRequest()" ) ) );
    CActiveScheduler::Add( this );
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWlanAvailableIapsRequest::CWlanAvailableIapsRequest(
    CWlanMgmtImpl& aCallback, 
    RWLMServer& aServer,
    RArray<TWlanIapAvailabilityData>& aAvailableIaps,
    TInt& aCacheLifetime,
    TUint& aMaxDelay,
    TBool aFilteredResults ) :
    CActive( CActive::EPriorityStandard ),
    iCallback( aCallback ),
    iServer( aServer ),
    iPendingAvailableIapIds( NULL ),
    iPendingAvailableIaps( &aAvailableIaps ),
    iAvailableIapsBuf( iAvailableIaps ),
    iCacheLifetimeBuf( aCacheLifetime ),
    iMaxDelayBuf( aMaxDelay ),
    iFilteredResults( aFilteredResults )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanAvailableIapsRequest::CWlanAvailableIapsRequest()" ) ) );
    CActiveScheduler::Add( this );
    }

// Destructor
CWlanAvailableIapsRequest::~CWlanAvailableIapsRequest()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanAvailableIapsRequest::CWlanAvailableIapsRequest()" ) ) );
    Cancel();
    iPendingAvailableIapIds = NULL;
    iPendingAvailableIaps = NULL;
    }

// ---------------------------------------------------------
// CWlanAvailableIapsRequest::IssueRequest
// ---------------------------------------------------------
//
void CWlanAvailableIapsRequest::IssueRequest()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanAvailableIapsRequest::IssueRequest()" ) ) );
    iServer.GetAvailableIaps( iAvailableIapsBuf, iCacheLifetimeBuf, iMaxDelayBuf, iFilteredResults, iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CWlanAvailableIapsRequest::RunL
// ---------------------------------------------------------
//
void CWlanAvailableIapsRequest::RunL()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanAvailableIapsRequest::RunL()" ) ) );
    if( iPendingAvailableIapIds )
        {
        for ( TUint idx( 0 ); idx < iAvailableIaps.count; ++idx )
            {
            iPendingAvailableIapIds->Append( iAvailableIaps.iaps[idx].iapId );
            }
        }
    else if( iPendingAvailableIaps )
        {
        for ( TUint idx( 0 ); idx < iAvailableIaps.count; ++idx )
            {
            TWlanIapAvailabilityData tmp;
            tmp.iIapId = iAvailableIaps.iaps[idx].iapId;
            // RCPI -> RSSI CONVERSION
            // Note: conversion may round the result by 0.5 units
            tmp.iRssi = ( 110 - ( iAvailableIaps.iaps[idx].rcpi / 2 ) ); 
            iPendingAvailableIaps->Append( tmp );
            }        
        }
    iCallback.AvailableIapsComplete( iStatus.Int() );
    }

// ---------------------------------------------------------
// CWlanAvailableIapsRequest::RunError
// ---------------------------------------------------------
//
TInt CWlanAvailableIapsRequest::RunError(
    TInt /* aError */ )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanAvailableIapsRequest::RunError()" ) ) );

    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanAvailableIapsRequest::DoCancel
// ---------------------------------------------------------
//
void CWlanAvailableIapsRequest::DoCancel()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanAvailableIapsRequest::DoCancel()" ) ) );

    iServer.CancelGetAvailableIaps();
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CProtectedSetupRequest::CProtectedSetupRequest( 
    CWlanMgmtImpl& aCallback, 
    RWLMServer& aServer, 
    const TWlanSsid& aSsid,
    const TWlanWpsPin& aWpsPin, 
    CArrayFixSeg<TWlanProtectedSetupCredentialAttribute>& aCredentials ) :
    CActive( CActive::EPriorityStandard ),
    iCallback( aCallback ),
    iServer( aServer ),
    iSsid( aSsid ),
    iWpsPin( aWpsPin ),
    iCredentials ( aCredentials ),
    iCredentialsBuf( iCredentialsStorage )
    {
    TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::CProtectedSetupRequest()" ) ) );
    CActiveScheduler::Add( this );
    }

// Destructor
CProtectedSetupRequest::~CProtectedSetupRequest()
    {
    TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::~CProtectedSetupRequest()" ) ) );

    Cancel();
    }

// ---------------------------------------------------------
// CProtectedSetupRequest::IssueRequest
// ---------------------------------------------------------
//
void CProtectedSetupRequest::IssueRequest()
    {
    TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::IssueRequest()" ) ) );
    iServer.RunProtectedSetup( iStatus, iSsid, iWpsPin, iCredentialsBuf );
    SetActive();
    }

// ---------------------------------------------------------
// CProtectedSetupRequest::RunL
// ---------------------------------------------------------
//
void CProtectedSetupRequest::RunL()
    {
    TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL()" ) ) );
    
    for ( TUint idx( 0 ); idx < iCredentialsStorage.count; ++idx )
        {
        TRAP_IGNORE( iCredentials.AppendL(
            iCredentialsStorage.credentials[idx] ) )
        }

#ifdef _DEBUG
    TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials.Count() is %u"  ),
        iCredentials.Count() ) );
    for ( TInt idx( 0 ); idx < iCredentials.Count(); ++idx )
        {
        if ( iCredentials[idx].iOperatingMode == EWlanOperatingModeAdhoc )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iOperatingMode = EWlanOperatingModeAdhoc" ),
                idx + 1 ) );
            }
        else if ( iCredentials[idx].iOperatingMode == EWlanOperatingModeInfrastructure )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iOperatingMode = EWlanOperatingModeInfrastructure" ),
                idx + 1 ) );
            }
        else
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iOperatingMode = unknown" ),
                idx + 1 ) );           
            }

        if ( iCredentials[idx].iAuthenticationMode == EWlanAuthenticationModeOpen )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iAuthenticationMode = EWlanAuthenticationModeOpen" ),
                idx + 1 ) );
            }
        else if ( iCredentials[idx].iAuthenticationMode == EWlanAuthenticationModeShared )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iAuthenticationMode = EWlanAuthenticationModeShared" ),
                idx + 1 ) );
            }
        else
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iAuthenticationMode = unknown" ),
                idx + 1 ) );
            }

        if ( iCredentials[idx].iSecurityMode == EWlanIapSecurityModeAllowUnsecure )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iSecurityMode = EWlanIapSecurityModeAllowUnsecure" ),
                idx + 1 ) );            
            }
        else if ( iCredentials[idx].iSecurityMode == EWlanIapSecurityModeWep )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iSecurityMode = EWlanIapSecurityModeWep" ),
                idx + 1 ) );                       
            }
        else if ( iCredentials[idx].iSecurityMode == EWlanIapSecurityMode802d1x )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iSecurityMode = EWlanIapSecurityMode802d1x" ),
                idx + 1 ) );                        
            }
        else if ( iCredentials[idx].iSecurityMode == EWlanIapSecurityModeWpa )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iSecurityMode = EWlanIapSecurityModeWpa" ),
                idx + 1 ) );                                    
            }
        else if ( iCredentials[idx].iSecurityMode == EWlanIapSecurityModeWpa2Only )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iSecurityMode = EWlanIapSecurityModeWpa2Only" ),
                idx + 1 ) );                        
            }
        else
            {            
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iSecurityMode = unknown" ),
                idx + 1 ) );                        
            }
        
        TBuf<KWlanMaxSsidLength> ssid;
        ssid.Copy( iCredentials[idx].iSsid );
        TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iSsid = %S" ),
            idx + 1, &ssid ) );
        
        TBuf<KWlanWepKeyMaxLength> wepKey;
        
        wepKey.Zero();
        for ( TInt length( 0 ); length < iCredentials[idx].iWepKey1.Length(); ++length )
            {
            wepKey.AppendFormat( _L( "%02X" ), iCredentials[idx].iWepKey1[length] );
            }
        if ( !iCredentials[idx].iWepKey1.Length() )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepKey1 = <undefined>" ),
                idx + 1 ) );           
            }
        else
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepKey1 = 0x%S" ),
                idx + 1, &wepKey ) );
            }

        wepKey.Zero();
        for ( TInt length( 0 ); length < iCredentials[idx].iWepKey2.Length(); ++length )
            {
            wepKey.AppendFormat( _L( "%02X" ), iCredentials[idx].iWepKey2[length] );
            }
        if ( !iCredentials[idx].iWepKey2.Length() )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepKey2 = <undefined>" ),
                idx + 1 ) );           
            }
        else
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepKey2 = 0x%S" ),
                idx + 1, &wepKey ) );
            }

        wepKey.Zero();
        for ( TInt length( 0 ); length < iCredentials[idx].iWepKey3.Length(); ++length )
            {
            wepKey.AppendFormat( _L( "%02X" ), iCredentials[idx].iWepKey3[length] );
            }
        if ( !iCredentials[idx].iWepKey3.Length() )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepKey3 = <undefined>" ),
                idx + 1 ) );           
            }
        else
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepKey3 = 0x%S" ),
                idx + 1, &wepKey ) );
            }

        wepKey.Zero();
        for ( TInt length( 0 ); length < iCredentials[idx].iWepKey4.Length(); ++length )
            {
            wepKey.AppendFormat( _L( "%02X" ), iCredentials[idx].iWepKey4[length] );
            }
        if ( !iCredentials[idx].iWepKey4.Length() )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepKey4 = <undefined>" ),
                idx + 1 ) );           
            }
        else
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepKey4 = 0x%S" ),
                idx + 1, &wepKey ) );
            }

        if ( iCredentials[idx].iWepDefaultKey == EWlanDefaultWepKey1 )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepDefaultKey = EWlanDefaultWepKey1" ),
                idx + 1 ) );            
            }
        else if ( iCredentials[idx].iWepDefaultKey == EWlanDefaultWepKey2 )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepDefaultKey = EWlanDefaultWepKey2" ),
                idx + 1 ) );                       
            }
        else if ( iCredentials[idx].iWepDefaultKey == EWlanDefaultWepKey3 )            
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepDefaultKey = EWlanDefaultWepKey3" ),
                idx + 1 ) );                        
            }
        else if ( iCredentials[idx].iWepDefaultKey == EWlanDefaultWepKey4 )
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepDefaultKey = EWlanDefaultWepKey4" ),
                idx + 1 ) );                        
            }
        else
            {
            TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWepDefaultKey = unknown" ),
                idx + 1 ) );
            }

        TBuf<KWlanWpaPskMaxLength> presharedKey;
        presharedKey.Copy( iCredentials[idx].iWpaPreSharedKey );
        TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunL() - iCredentials[%u].iWpaPreSharedKey = %S" ),
            idx + 1, &presharedKey ) );        
        }
#endif // _DEBUG        

    iCallback.ProtectedSetupComplete( iStatus.Int() );
    }

// ---------------------------------------------------------
// CProtectedSetupRequest::RunError
// ---------------------------------------------------------
//
TInt CProtectedSetupRequest::RunError(
    TInt /* aError */ )
    {
    TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::RunError()" ) ) );

    return KErrNone;
    }

// ---------------------------------------------------------
// CProtectedSetupRequest::DoCancel
// ---------------------------------------------------------
//
void CProtectedSetupRequest::DoCancel()
    {
    TraceDump( INFO_LEVEL, ( _L( "CProtectedSetupRequest::DoCancel()" ) ) );

    iServer.CancelProtectedSetup();
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// Returns an instance of the proxy table.
// Returns: KErrNone
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

//  End of File  
