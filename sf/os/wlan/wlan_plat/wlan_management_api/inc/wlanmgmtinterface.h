/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECom interface definition for WLAN management services.
*
*/

/*
* %version: 18 %
*/

#ifndef WLANMGMTINTERFACE_H
#define WLANMGMTINTERFACE_H

// INCLUDES
#include "wlanmgmtcommon.h"
#include "wlanscaninfo.h"
#include "wlantrafficstreamparameters.h"

// CLASS DECLARATION
/**
 * ECom interface class for WLAN management services.
 *
 * This class contains the methods for managing WLAN connections
 * and querying the statuses of various connection variables.
 *
 * @since Series 60 3.0
 */
class MWlanMgmtInterface
    {
    public:

        /**
         * Activate the notification service.
         * 
         * After the client has enabled the notification service, it can
         * receive asynchronous notifications from the server.
         *
         * @param aCallback The class that implements the callback interface.
         * @note This method only activates notifications defined in callback interface v1.
         * @see ActivateExtendedNotificationsL
         */
        virtual void ActivateNotificationsL(
            MWlanMgmtNotifications& aCallback ) = 0;
        
        /**
         * Cancel the notification service.
         */
        virtual void CancelNotifications() = 0;

        /**
         * Perform a broadcast scan and return the detected WLAN networks.
         *
         * @param aResults Results of the scan.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetScanResults(
            CWlanScanInfo& aResults ) = 0;

        /**
         * Perform a broadcast scan and return the detected WLAN networks.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aResults Results of the scan.
         */
        virtual void GetScanResults(
            TRequestStatus& aStatus,
            CWlanScanInfo& aResults ) = 0;

        /**
         * Get the BSSID of the BSS currently connected to.
         *
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aBssid BSSID of the currently connected BSS.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionBssid(
            TWlanBssid& aBssid ) = 0;

        /**
         * Get the SSID of the WLAN network currently connected to.
         *
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aSsid SSID of the currently connected network.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionSsid(
            TWlanSsid& aSsid ) = 0;

        /**
         * Get the current Received Signal Strength Indicator (RSSI).
         *
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aSignalQuality Current RSSI.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionSignalQuality(
            TInt32& aSignalQuality ) = 0;

        /**
         * Get the mode of the WLAN connection.
         *
         * @param aMode The current mode of the connection.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionMode(
            TWlanConnectionMode& aMode ) = 0;

        /**
         * Get the currently used security mode of the WLAN connection.
         *
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aMode The security mode of the connection.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         * @deprecated This method is offered for backward compatibility reasons,
         *             GetExtendedConnectionSecurityMode() should be used instead.
         */
        virtual TInt GetConnectionSecurityMode(
            TWlanConnectionSecurityMode& aMode ) = 0;
        
        /**
         * Get the available WLAN IAPs.
         *
         * @param aAvailableIaps Array of IAP IDs available.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetAvailableIaps(
            RArray<TUint>& aAvailableIaps ) = 0;
        
        /**
         * Get the available WLAN IAPs.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.       
         * @param aAvailableIaps Array of IAP IDs available.
         */
        virtual void GetAvailableIaps(
            TRequestStatus& aStatus,
            RArray<TUint>& aAvailableIaps ) = 0;

        /**
         * Notify the server about changed WLAN settings.
         */
        virtual void NotifyChangedSettings() = 0;

        /**
         * Adds a bssid to the blacklist
         *
         * @param aBssid The BSSID of the accesspoint.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt AddBssidToBlacklist(
            const TWlanBssid& aBssid ) = 0;

        /**
         * Updates the RSS notification class boundaries.
         *
         * @param aRssLevelBoundary Specifies the RSS level when a signal level notification
         *                          should be given. This boundary is used when signal level
         *                          is getting worse (see next parameter).
         * @param aHysteresis Specifies the difference between RSS notification trigger levels
         *                    of declining and improving signal quality, i.e. since aRssLevelBoundary
         *                    specifies the level boundary for declining signal, the same boundary
         *                    for improving signal is ( aRssLevelBoundary - aHysteresis ).
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt UpdateRssNotificationBoundary( 
            const TInt32 aRssLevelBoundary,
            const TInt32 aHysteresis ) = 0;

        /**
         * Perform a direct scan for an SSID and return the detected WLAN networks.
         * If the SSID has zero length, a broadcast scan will be done.
         *
         * @param aSsid name of the WLAN network
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aResults Results of the scan.
         */
        virtual void GetScanResults(
            TWlanSsid& aSsid,
            TRequestStatus& aStatus,
            CWlanScanInfo& aResults ) = 0;
        
        /**
         * Start Protected Setup.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aSsid SSID of the network to configure.
         * @param aWpsPin PIN value to be used. "00000000" (string of eight zeros)
         *                if push button method is used.
         * @param aCredentials Results of a successful Protected Setup operation.
         * @sa \link psetup Protected Setup-specific error codes \endlink.
         */
        virtual void RunProtectedSetup(
            TRequestStatus& aStatus,
            const TWlanSsid& aSsid,
            const TWlanWpsPin& aWpsPin,
            CArrayFixSeg<TWlanProtectedSetupCredentialAttribute>& aCredentials ) = 0;

        /**
         * Cancel an outstanding Protected Setup operation.
         */
        virtual void CancelProtectedSetup() = 0;

        /**
         * Cancel an outstanding scan request.
         */
        virtual void CancelGetScanResults() = 0;

        /**
         * Cancel an outstanding IAP availability request.
         */
        virtual void CancelGetAvailableIaps() = 0;

        /**
         * Perform a direct scan for an SSID and return the detected WLAN networks.
         * If the SSID has zero length, a broadcast scan will be done.
         *
         * @param aCacheLifetime Defines how many seconds old cached results the client
         *                       is willing to accept. The valid is range is from 0 to
         *                       60 seconds. The value of -1 means the system default will
         *                       be used. The aCacheLifetime parameter has a meaning only
         *                       when the aMaxDelay parameter is zero.
         *                       Value will be changed to the actual value used by the
         *                       system.
         * @param aMaxDelay Maximum amount of seconds the client is willing to wait for
         *                  the scan results. The valid range is from 0 to 1200 seconds
         *                  or KWlanInfiniteScanDelay. KWlanInfiniteScanDelay
         *                  will never cause a scan, but the request will be
         *                  completed when any other broadcast scan request is completed.
         *                  Value will be changed to the actual value used by the system.
         * @param aSsid Name of the WLAN network.
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aResults Results of the scan.
         */
        virtual void GetScanResults(
            TInt& aCacheLifetime,
            TUint& aMaxDelay,
            TWlanSsid& aSsid,
            TRequestStatus& aStatus,
            CWlanScanInfo& aResults ) = 0;

        /**
         * Get the available WLAN IAPs.
         *
         * @param aCacheLifetime Defines how many seconds old cached results the client
         *                       is willing to accept. The valid is range is from 0 to
         *                       60 seconds. The value of -1 means the system default will
         *                       be used. The aCacheLifetime parameter has a meaning only
         *                       when the aMaxDelay parameter is zero.
         *                       Value will be changed to the actual value used by the
         *                       system.
         * @param aMaxDelay Maximum amount of seconds the client is willing to wait for
         *                  the availability results. The valid range is from 0 to 1200
         *                  seconds or KWlanInfiniteScanDelay. KWlanInfiniteScanDelay
         *                  will never cause a scan, but the request will be
         *                  completed when any other broadcast scan request is completed.
         *                  Value will be changed to the actual value used by the system.
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aAvailableIaps Array of IAP IDs available.
         */
        virtual void GetAvailableIaps(
            TInt& aCacheLifetime,
            TUint& aMaxDelay,
            TRequestStatus& aStatus,
            RArray<TUint>& aAvailableIaps ) = 0;

        /**
         * Add a list of SSIDs to the given IAP ID.
         *
         * The list of SSIDs is matched against the scan results during IAP availability
         * check and the corresponding IAP marked as available if a match is found.
         *
         * @param aIapId IAP ID the list is attached to.
         * @param aSsidList List of SSIDs. Any previous list attached will be overwritten.
         * @return KErrNone if the list was successfully added, an error code otherwise.
         */
        virtual TInt AddIapSsidList(
            TUint aIapId,
            const CArrayFixFlat<TWlanSsid>& aSsidList ) = 0;

        /**
         * Remove any list of SSIDs attached to the given IAP ID.
         *
         * @param aIapId IAP ID the list is attached to.
         * @return KErrNone if the list was successfully removed, an error code otherwise.
         */
        virtual TInt RemoveIapSsidList(
            TUint aIapId ) = 0;

        /**
         * Get the currently used security mode of the WLAN connection.
         *
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aMode The security mode of the connection.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetExtendedConnectionSecurityMode(
            TWlanConnectionExtentedSecurityMode& aMode ) = 0;

        /**
         * Activate the extended notification service.
         * 
         * After the client has enabled the notification service, it can
         * receive asynchronous notifications from the server.
         *
         * @param aCallback The class that implements the callback interface.
         * @param aCallbackInterfaceVersion The callback interface version implemented by
         *                                  the client. This MUST NOT be touched.
         */
        virtual void ActivateExtendedNotificationsL(
            MWlanMgmtNotifications& aCallback,
            TUint aCallbackInterfaceVersion = KWlanCallbackInterfaceVersion ) = 0;

        /**
         * Create a virtual traffic stream.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aStreamParameters Traffic stream parameters to use.
         * @param aStreamId Contains the ID assigned to this traffic stream
         *                  on successful completion.
         * @param aStreamStatus Contains the status of the traffic stream
         *                  on successful completion.
         */
        virtual void CreateTrafficStream(
            TRequestStatus& aStatus,
            const TWlanTrafficStreamParameters& aStreamParameters,
            TUint& aStreamId,
            TWlanTrafficStreamStatus& aStreamStatus ) = 0;

        /**
         * Cancel an outstanding traffic stream creation request.
         */
        virtual void CancelCreateTrafficStream() = 0;

        /**
         * Delete a virtual traffic stream.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aStreamId ID of the traffic stream to delete.
         */
        virtual void DeleteTrafficStream(
            TRequestStatus& aStatus,
            TUint aStreamId ) = 0;

        /**
         * Cancel an outstanding traffic stream deletion request.
         */
        virtual void CancelDeleteTrafficStream() = 0;

        /**
         * Initiate a roam to the given BSSID.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aBssid BSSID to roam to. If set to FF:FF:FF:FF:FF:FF address,
         *               search for a better BSS is initiated.
         */
        virtual void DirectedRoam(
            TRequestStatus& aStatus,
            const TWlanBssid& aBssid ) = 0;

        /**
         * Cancel an outstanding directed roam request.
         */
        virtual void CancelDirectedRoam() = 0;

        /**
         * Start aggressive background scanning.
         *
         * @param aInterval Scan interval (in seconds) for aggressive mode.
         */
        virtual TInt StartAggressiveBgScan(
            TUint aInterval ) = 0;

        /**
         * Get the available WLAN IAPs.
         *
         * @param aCacheLifetime Defines how many seconds old cached results the client
         *                       is willing to accept. The valid is range is from 0 to
         *                       60 seconds. The value of -1 means the system default will
         *                       be used. The aCacheLifetime parameter has a meaning only
         *                       when the aMaxDelay parameter is zero.
         *                       Value will be changed to the actual value used by the
         *                       system.
         * @param aMaxDelay Maximum amount of seconds the client is willing to wait for
         *                  the availability results. The valid range is from 0 to 1200
         *                  seconds or KWlanInfiniteScanDelay. KWlanInfiniteScanDelay
         *                  will never cause a scan, but the request will be
         *                  completed when any other broadcast scan request is completed.
         *                  Value will be changed to the actual value used by the system.
         * @param aFilteredResults Whether availability is filtered based on signal strength.
                                   ETrue if filtering is allowed, EFalse if not.
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aAvailableIaps Array of IAPs available.
         */
        virtual void GetAvailableIaps(
            TInt& aCacheLifetime,
            TUint& aMaxDelay,
            TBool aFilteredResults,
            TRequestStatus& aStatus,
            RArray<TWlanIapAvailabilityData>& aAvailableIaps ) = 0;
    };

#endif // WLANMGMTINTERFACE_H
            
// End of File
