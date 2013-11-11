/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Factory class for timers
*
*/

/*
* %version: 13 %
*/

#include "wlanscanresultcache.h"
#include "am_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWlanScanResultCache::NewL
// -----------------------------------------------------------------------------
//
CWlanScanResultCache* CWlanScanResultCache::NewL()
    {
    DEBUG( "CWlanScanResultCache::NewL()" );
    CWlanScanResultCache* self = new(ELeave)CWlanScanResultCache();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::ConstructL
// -----------------------------------------------------------------------------
//
void CWlanScanResultCache::ConstructL()
    {
    DEBUG( "CWlanScanResultCache::ConstructL()" );

    iScanList = new(ELeave) ScanList();
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::CWlanScanResultCache
// -----------------------------------------------------------------------------
//
CWlanScanResultCache::CWlanScanResultCache() :
    iScanList( NULL ),
    iScanListTimeStamp( 0 ),
    iIapListTimeStamp( 0 )
    {
    DEBUG( "CWlanScanResultCache::CWlanScanResultCache()" );
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::~CWlanScanResultCache
// -----------------------------------------------------------------------------
//
CWlanScanResultCache::~CWlanScanResultCache()
    {
    DEBUG( "CWlanScanResultCache::~CWlanScanResultCache()" );

    delete iScanList;
    iIapList.Close(); 
    iAvailableIapList.Close();
    iAvailableNetworkList.Close();
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::UpdateScanList
// -----------------------------------------------------------------------------
//
void CWlanScanResultCache::UpdateScanList(
    ScanList* aScanList )
    {
    DEBUG( "CWlanScanResultCache::UpdateScanList()" );

    delete iScanList;
    iScanList = aScanList; // take ownership
    iScanListTimeStamp.HomeTime();
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::GetScanList
// -----------------------------------------------------------------------------
//
ScanList* CWlanScanResultCache::GetScanList(
	TUint aCacheLifetime )
    {
    DEBUG( "CWlanScanResultCache::GetScanList()" ); 

    TTime currentTime(0);
    currentTime.HomeTime();
    TTimeIntervalSeconds difference(0);

    TInt overflow = currentTime.SecondsFrom( iScanListTimeStamp, difference );
    if( difference.Int() > aCacheLifetime || difference.Int() < 0 || overflow )
        {
        DEBUG2( "CWlanScanResultCache::GetScanList() - results are too old, difference is %d, overflow is %d",
            difference.Int(), overflow );
        return NULL;
        }
    else
        {
        DEBUG1( "CWlanScanResultCache::GetScanList() - old results are still valid, difference is %u",
            difference.Int() );
        return iScanList;
        }
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::UpdateAvailableNetworksList
// -----------------------------------------------------------------------------
//
void CWlanScanResultCache::UpdateAvailableNetworksList(
    core_type_list_c<core_iap_availability_data_s>& aIapAvailabilityList,
    RArray<TWlanAvailableNetwork>& aNetworkList,
    TBool& aNewIapsAvailable,
    TBool& aOldIapsLost )
    {
    DEBUG( "CWlanScanResultCache::UpdateAvailableNetworksList()" ); 

    DEBUG1( "CWlanScanResultCache::UpdateAvailableNetworksList() - %u IAP(s) available",
        aIapAvailabilityList.count() );
    DEBUG1( "CWlanScanResultCache::UpdateAvailableNetworksList() - %u IAP(s) previously available",
        iAvailableIapList.Count() ); 
    DEBUG1( "CWlanScanResultCache::UpdateAvailableNetworksList() - %u networks(s) available",
        aNetworkList.Count() );
    DEBUG1( "CWlanScanResultCache::UpdateAvailableNetworksList() - %u networks(s) previously available",
        iAvailableNetworkList.Count() );

    const TInt oldIapCount( iAvailableIapList.Count() );
    const TInt newNetworkCount( aNetworkList.Count() );
    const TInt oldNetworkCount( iAvailableNetworkList.Count() );
    aNewIapsAvailable = EFalse;
    aOldIapsLost = EFalse;
    TIdentityRelation<TWlanAvailableNetwork> isNetworkEqual( CWlanScanResultCache::IsNetworkEqual );
    TIdentityRelation<TWlmAvailabilityData> isIapEqual( CWlanScanResultCache::IsIapEqual );

    // Iterate through previously available IAPs to find lost IAPs.    
    TInt idx( 0 );
    //while( idx < oldIapCount && !aOldIapsLost )
    while( idx < oldIapCount )
        {
        const core_iap_availability_data_s* newIap = aIapAvailabilityList.first();

        while( newIap )
            {
            if( newIap->id == iAvailableIapList[idx].iapId )
                {
                break;
                }

            newIap = aIapAvailabilityList.next();
            }

        if( !newIap )
            {
            DEBUG1( "CWlanScanResultCache::UpdateAvailableNetworksList() - old IAP %u has been lost",
                iAvailableIapList[idx] );
            aOldIapsLost = ETrue;
            }

        ++idx;
        }

    // Iterate through available IAPs to find new IAPs.    
    const core_iap_availability_data_s* newIap = aIapAvailabilityList.first();
    //while( newId && !aNewIapsAvailable )
    while( newIap )
        {
        TWlmAvailabilityData tmp;
        tmp.iapId = newIap->id;
        tmp.rcpi = newIap->rcpi;
        if ( iAvailableIapList.Find( tmp, isIapEqual ) == KErrNotFound )
            {
            DEBUG1( "CWlanScanResultCache::UpdateAvailableNetworksList() - new IAP %u has been detected",
                newIap->id );
            aNewIapsAvailable = ETrue;
            }
        
        newIap = aIapAvailabilityList.next();
        }

    // Iterate through previously available networks to find lost networks.
    idx = 0;
    //while ( idx < oldNetworkCount && !aOldIapsLost )
    while ( idx < oldNetworkCount )
        {
        if ( aNetworkList.Find( iAvailableNetworkList[idx], isNetworkEqual ) == KErrNotFound )
            {
            DEBUG1S( "CWlanScanResultCache::UpdateAvailableNetworksList() - old network has been lost, SSID ",
                iAvailableNetworkList[idx].ssid.Length(), iAvailableNetworkList[idx].ssid.Ptr() );
            aOldIapsLost = ETrue;
            }
        ++idx;
        }

    // Iterate through available networks to find new networks.        
    idx = 0;
    //while( idx < newNetworkCount && !aNewIapsAvailable )
    while( idx < newNetworkCount )
        {
        if ( iAvailableNetworkList.Find( aNetworkList[idx], isNetworkEqual ) == KErrNotFound )
            {
            DEBUG1S( "CWlanScanResultCache::UpdateAvailableNetworksList() - new network has been detected, SSID ",
                aNetworkList[idx].ssid.Length(), aNetworkList[idx].ssid.Ptr() );            
            aNewIapsAvailable = ETrue;
            }
        ++idx;
        }

    // Update the list
    iAvailableIapList.Reset();
    iAvailableNetworkList.Reset();
    iIapListTimeStamp.HomeTime();
        
    newIap = aIapAvailabilityList.first();
    while( newIap )
        {
        TWlmAvailabilityData tmp;
        tmp.iapId = newIap->id;
        tmp.rcpi = newIap->rcpi;
        iAvailableIapList.Append( tmp );
        newIap = aIapAvailabilityList.next();
        }
        
    idx = 0;
    while( idx < newNetworkCount )
        {
        iAvailableNetworkList.Append( aNetworkList[idx] );
        ++idx;
        }
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::AvailableIaps
// -----------------------------------------------------------------------------
//
RArray<TWlmAvailabilityData>* CWlanScanResultCache::AvailableIaps(
    RArray<TWlanLimitedIapData>& aIapList,
    TUint aCacheLifetime )
    {
    DEBUG( "CWlanScanResultCache::AvailableIaps()" );

    /**
     * Take a copy of the previous IAP list since GetIapDataList() will overwrite
     * the member variable.
     */
    RArray<TWlanLimitedIapData> iapList;
    for( TInt idx( 0 ); idx < iIapList.Count(); ++idx )
        {
        iapList.Append( iIapList[idx] );
        }

    TInt ret = GetIapDataList( aIapList );
    if ( ret != KErrNone )
        {
        DEBUG( "CWlanScanResultCache::AvailableIaps() - unable to read list of IAPs, assuming no cache" );
        iapList.Close();

        return NULL;
        }

    TTime currentTime(0);
    currentTime.HomeTime();
    TTimeIntervalSeconds difference(0);

    TInt overflow = currentTime.SecondsFrom( iIapListTimeStamp, difference );
    if( difference.Int() > aCacheLifetime || difference.Int() < 0 || overflow )
        {
        DEBUG2( "CWlanScanResultCache::AvailableIaps() - results are too old, difference is %d, overflow is %d",
            difference.Int(), overflow );
        iapList.Close();

        return NULL;
        }

    /**
     * Check whether the list of IAPs has change since last GetIapDataList().
     */
    if ( !IsIapListEqual( aIapList, iapList ) )
        {
        DEBUG( "CWlanScanResultCache::AvailableIaps() - cache time is still valid but IAPs have changed" );
        iapList.Close();

        return NULL;
        }

    DEBUG1( "CWlanScanResultCache::AvailableIaps() - old results are still valid, difference is %u",
        difference.Int() );
    iapList.Close();

    return &iAvailableIapList;
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::GetIapDataList
// -----------------------------------------------------------------------------
//
TInt CWlanScanResultCache::GetIapDataList(
    RArray<TWlanLimitedIapData>& aIapList )
    {
    DEBUG( "CWlanScanResultCache::GetIapDataList()" );

    aIapList.Reset();

    /**
     * Read all WLAN IAPs from commsdat.
     */
    CWLanSettings wlanSettings;    
    TInt ret = wlanSettings.Connect();
    if ( ret != KErrNone )
        {
        DEBUG1( "CWlanScanResultCache::GetIapDataList() - CWLanSettings::Connect() failed (%d)",
            ret );

        return ret;
        }

    RArray<SWlanIAPId> wlanIapIds;
    TRAP( ret, wlanSettings.GetIAPWlanServicesL( wlanIapIds ) );
    if( ret != KErrNone )
        {
        DEBUG1( "CWlanScanResultCache::GetIapDataList() - CWLanSettings::GetIAPWlanServicesL() failed (%d)",
            ret );
        wlanIapIds.Close();
        wlanSettings.Disconnect();

        return ret;
        }

    /**
     * Retrieve IAP data for each IAP id.
     */
    DEBUG1( "CWlanScanResultCache::GetIapDataList() - reading %u WLAN IAPs",
        wlanIapIds.Count() );

    for( TInt i( 0 ); i < wlanIapIds.Count(); i++ )
        {
        TWlanLimitedIapData iap;
        iap.iapId = wlanIapIds[i].iIAPId;
        iap.serviceId = wlanIapIds[i].iWLANRecordId;

        DEBUG1( "CWlanScanResultCache::GetIapDataList() - reading IAP %u",
            wlanIapIds[i].iIAPId );

        SWLANSettings iapData;
        ret = wlanSettings.GetWlanSettings(
            wlanIapIds[i].iWLANRecordId,
            iapData );
        if( ret != KErrNone )
            {
            DEBUG2( "CWlanScanResultCache::GetIapDataList() - reading of IAP %u failed (%d), ignoring",
                wlanIapIds[i].iIAPId, ret );
            }
        else if( iapData.SSID.Length() ) // filter out EasyWlan IAP
            {
            iap.ssid.Copy( iapData.SSID );
            iap.usedSsid.Copy( iapData.UsedSSID );
            iap.networkType = static_cast<EConnectionMode>( iapData.ConnectionMode );
            iap.securityMode = static_cast<EWlanSecurityMode>( iapData.SecurityMode );
            iap.isPskEnabled = iapData.EnableWpaPsk;
            iap.isHidden = iapData.ScanSSID;

            aIapList.Append( iap );
            }
        }

    UpdateIapList( aIapList );
    wlanIapIds.Close();
    wlanSettings.Disconnect();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::IsNetworkEqual
// -----------------------------------------------------------------------------
//
TBool CWlanScanResultCache::IsNetworkEqual(
    const TWlanAvailableNetwork& aFirst,
    const TWlanAvailableNetwork& aSecond )
    {
    if ( aFirst.ssid != aSecond.ssid ||
         aFirst.networkType != aSecond.networkType ||
         aFirst.securityMode != aSecond.securityMode )
        {
        return EFalse;
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::IsIapEqual
// -----------------------------------------------------------------------------
//
TBool CWlanScanResultCache::IsIapEqual(
    const TWlmAvailabilityData& aFirst,
    const TWlmAvailabilityData& aSecond )
    {
    if ( aFirst.iapId != aSecond.iapId )
        {
        return EFalse;
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::IsIapListEqual
// -----------------------------------------------------------------------------
//
TBool CWlanScanResultCache::IsIapListEqual(
    const RArray<TWlanLimitedIapData>& aFirst,
    const RArray<TWlanLimitedIapData>& aSecond )
    {
    /**
     * If the amount of IAPs is different, it's an obvious fault.
     */
    if ( aFirst.Count() != aSecond.Count() )
        {
        DEBUG2( "CWlanScanResultCache::GetIapDataList() - false, amount of IAPs is different (%u vs %u)",
            aFirst.Count(), aSecond.Count() );

        return EFalse;
        }

    /**
     * Some of the settings might have changed.
     */
    for( TInt i( 0 ); i < aFirst.Count(); ++i )
        {
        TBool isFound( EFalse );
        for ( TInt j( 0 ); j < aSecond.Count() && !isFound; ++j )
            {
            if ( aFirst[i].iapId == aSecond[j].iapId )
                {
                isFound = ETrue;
                if ( aFirst[i].ssid != aSecond[j].ssid )
                    {
                    DEBUG1( "CWlanScanResultCache::GetIapDataList() - false, IAP %u has mismatching SSID",
                        aFirst[i].iapId );

                    return EFalse;
                    }
                else if ( aFirst[i].networkType != aSecond[j].networkType )
                    {
                    DEBUG1( "CWlanScanResultCache::GetIapDataList() - false, IAP %u has mismatching network mode",
                        aFirst[i].iapId );

                    return EFalse;
                    }
                else if ( aFirst[i].securityMode != aSecond[j].securityMode ||
                          aFirst[i].isPskEnabled != aSecond[j].isPskEnabled )
                    {
                    DEBUG1( "CWlanScanResultCache::GetIapDataList() - false, IAP %u has mismatching security mode",
                        aFirst[i].iapId );

                    return EFalse;
                    }
                else if ( aFirst[i].isHidden != aSecond[j].isHidden )
                    {
                    DEBUG1( "CWlanScanResultCache::GetIapDataList() - false, IAP %u has mismatching hidden setting",
                        aFirst[i].iapId );

                    return EFalse;
                    }
                }
            }
        if ( !isFound )
            {
            DEBUG1( "CWlanScanResultCache::GetIapDataList() - false, IAP %u not found in previous list",
                aFirst[i].iapId );

            return EFalse;
            }
        }

    DEBUG( "CWlanScanResultCache::GetIapDataList() - true, IAP lists are equal" );

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::UpdateIapList
// -----------------------------------------------------------------------------
//  
void CWlanScanResultCache::UpdateIapList(
    const RArray<TWlanLimitedIapData>& aIapDataList )
    {
    iIapList.Reset();
    TUint32 arraySize( 0 );

    for( TInt idx( 0 ); idx < aIapDataList.Count(); ++idx )
        {
        arraySize += sizeof( aIapDataList[idx] );
        iIapList.Append( aIapDataList[idx] );
        }
        
    DEBUG1( "CWlanScanResultCache::UpdateIapList() - total size of IAP list is %u bytes",
        arraySize );
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::CachedIapDataList
// -----------------------------------------------------------------------------
//
const RArray<TWlanLimitedIapData>& CWlanScanResultCache::CachedIapDataList() const
    {
    return iIapList;
    }

// -----------------------------------------------------------------------------
// CWlanScanResultCache::InvalidateAvailabilityCache
// -----------------------------------------------------------------------------
//
void CWlanScanResultCache::InvalidateAvailabilityCache()
    {
    iIapListTimeStamp = 0;
    }
