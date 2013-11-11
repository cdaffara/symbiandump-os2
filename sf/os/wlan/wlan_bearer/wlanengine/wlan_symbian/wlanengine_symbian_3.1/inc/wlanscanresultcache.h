/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Cache for scan results
*
*/

/*
* %version: 13 %
*/

#ifndef WLANSCANRESULTCACHE_H
#define WLANSCANRESULTCACHE_H

#include <e32base.h>
#include "genscanlist.h"
#include "wlmclientserver.h"
#include "genscaninfo.h"
#include "core_type_list.h"

/**
* Structure for storing information about an available network.
*/
struct TWlanAvailableNetwork
    {
    TBuf8<KMaxSSIDLength> ssid;
    EConnectionMode networkType;
    WlanSecurityMode securityMode;
    TUint8 rcpi;
    };

/**
 * CWlanScanResultCache
 * The class caches latest scan results for a specific time period.
 * Also caches the latest list of available IAPs (IAP list has no expiry time)
 * @lib wlmserversrv.lib
 * @since Series 60 3.1
 */
NONSHARABLE_CLASS( CWlanScanResultCache ) : public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CWlanScanResultCache* NewL();
    
    /**
     * Destructor.
     */
    virtual ~CWlanScanResultCache();

    /**
     * UpdateScanList updates the cached list of scan results
     * @param aScanList contains the new scan results
     * @return errorcode
     */
    void UpdateScanList(
        ScanList* aScanList );
    
    /**
     * Returns the latest scanresults if they are fresh enough
     * @param aCacheLifetime how old results (in seconds) are considered valid. 
     * @return pointer to ScanList or NULL if they are not available 
     * or they are too old
     */
    ScanList* GetScanList(
        TUint aCacheLifetime );
    
    /**
     * Updates the list of available networks (IAPs and SSIDs)
     * @param aIapAvailabilityList List of available IAPs.
     * @param aNetworkList List of available networks.
     * @param aNewIapsAvailable is set to ETrue on completion if
     *        new networks or IAPs were detected since the last update.
     * @param aOldIapsLost is set to ETrue on completion if
     *        networks or IAPs have been lost since the last update.
     */
    void UpdateAvailableNetworksList(
        core_type_list_c<core_iap_availability_data_s>& aIapAvailabilityList,
        RArray<TWlanAvailableNetwork>& aNetworkList,
        TBool& aNewIapsAvailable,
        TBool& aOldIapsLost );
    
    /**
     * Returns the latest list of available IAPs if they are fresh enough.
     *
     * @param aIapList Latest list of WLAN IAPs from commsdat.
     * @param aCacheLifetime how old results (in seconds) are considered valid.
     * @return Pointer to list of available IAPs or NULL if they are not available
     * or they are too old.
     */
    RArray<TWlmAvailabilityData>* AvailableIaps(
        RArray<TWlanLimitedIapData>& aIapList,
        TUint aCacheLifetime );

    /**
     * Returns the latest list of WLAN IAPs from commsdat.
     *
     * @param aIapList List of WLAN IAPs.
     * @return KErrNone if successful, an error code otherwise.
     */       
    TInt GetIapDataList(
        RArray<TWlanLimitedIapData>& aIapList );

    /**
     * Return the cached list of WLAN IAPs.
     * 
     * @return The cached list of WLAN IAPs.
     */
    const RArray<TWlanLimitedIapData>& CachedIapDataList() const;

    /**
     * Mark currently cached IAP availability results as invalid.
     */
    void InvalidateAvailabilityCache();

private: // Functions

    /**
     * C++ default constructor.
     */
    CWlanScanResultCache();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Method for determining whether two networks are equal.
     * @param aFirst First network.
     * @param aSecond Second network.
     * @return Result of comparison.
     */
    static TBool IsNetworkEqual(
        const TWlanAvailableNetwork& aFirst,
        const TWlanAvailableNetwork& aSecond );

    /**
     * Method for determining whether two IAPs are equal.
     * @param aFirst First IAP.
     * @param aSecond Second IAP.
     * @return Result of comparison.
     */
    static TBool IsIapEqual(
        const TWlmAvailabilityData& aFirst,
        const TWlmAvailabilityData& aSecond );

    /**
     * Check whether the given IAP list is equal with the given IAP data list.
     *
     * @param aIapList IAP list to compare.
     * @param aIapDataList IAP data list to compare against.
     * @return ETrue is the lists are equal, EFalse otherwise.
     */
    static TBool IsIapListEqual(
        const RArray<TWlanLimitedIapData>& aFirst,
        const RArray<TWlanLimitedIapData>& aSecond );

    /**
     * Update the list of IAPs from the given IAP data list.
     *
     * @param aIapDataList IAP data list to update from.
     */
    void UpdateIapList(
        const RArray<TWlanLimitedIapData>& aIapDataList );

private: // Data
        
    /** Latest ScanList */
    ScanList* iScanList;

    /** Scanlist timestamp */
    TTime iScanListTimeStamp;

    /** IapList timestamp */
    TTime iIapListTimeStamp;

    /** Latest list of IAPs. */
    RArray<TWlanLimitedIapData> iIapList;

    /** Latest list of available IAPs */
    RArray<TWlmAvailabilityData> iAvailableIapList;

    /** Latest list of available networks */
    RArray<TWlanAvailableNetwork> iAvailableNetworkList;

    };

#endif // WLANSCANRESULTCACHE_H
