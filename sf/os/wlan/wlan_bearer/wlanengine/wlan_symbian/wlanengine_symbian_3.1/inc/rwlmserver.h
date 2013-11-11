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
* Description:  Client side interface to WLAN Services
*
*/

/*
* %version: 28 %
*/

#ifndef RWLMSERVER_H
#define RWLMSERVER_H

#include <e32base.h>
#include <wlancontrolinterface.h>
#include <wlantrafficstreamparameters.h>
#include "genscanlist.h"
#include "wlmnotify.h"
#include "wlancontrolinterface.h"

class CWlmScanRequest;

/**
* Application interface.
* This class hides inter-thread communication between client
* and server. Hides also asynchronous notification services.
* @since Series 60 3.0
* @lib wlmserver.lib
*/
NONSHARABLE_CLASS( RWLMServer ) : public RSessionBase, public MWLMNotify
    {
    friend class CWlmScanRequest;

    public: // Methods.

        /**
         * Structure for storing buffers for a traffic stream create request.
         */
        struct TWlmTsCreateRequest
            {
            TWlmTsCreateRequest(
                const TWlanTrafficStreamParameters& aStreamParameters,
                TUint& aStreamId,
                TWlanTrafficStreamStatus& aStreamStatus ) :
                iTsParameterPckg( aStreamParameters ),
                iTsIdPckg( aStreamId ),
                iTsStatusPckg( aStreamStatus ) {};

            /** Buffer for storing TSPEC parameters. */
            TPckgBuf<TWlanTrafficStreamParameters> iTsParameterPckg;

            /** Buffer for storing traffic stream ID. */
            TPckg<TUint> iTsIdPckg;

            /** Buffer for storing traffic stream status. */
            TPckg<TWlanTrafficStreamStatus> iTsStatusPckg;
            };

    // New methods

        /** 
         * Cancel pending notification message from server.
         * CWLMNotify class uses this method to cancel pending message at server 
         * side.
         */
        void CancelRequest();

        /**
         * Close the connection; release the handle to the server.
         */
        IMPORT_C void Close();

        /** 
         * Set a message to pend notification.
         * @param aReturnValue Status of the calling active object.
         * @param aReturnData  (OUT) Data returned by the notification.
         * @param aNotificationMask Bitmask of active notifications.
         */
        void WaitForNotification(
            TRequestStatus& aReturnValue,
            TDes8& aReturnData,
            TUint32 aNotificationMask );

        /** 
         * Client activates the notification service.
         * With this service client can receive asynchronous notifications from
         * the server.
         * @param aCallback Client's callback interface for notifications.
         * @param aNotificationMask Bitmask of notifications to activate.
         * @see TWLMResponses
         */
        IMPORT_C void ActivateNotifiesL(
            MWLMNotify& aCallback,
            TUint32 aNotificationMask );

        /** 
         * Client deactivates the notification service.
         * This is the client's interface to stop receiveing notifications from the
         * server.
         */
        IMPORT_C void CancelNotifies();

        /**
         * Client makes the connection to the server.        
         * @return General Symbian error code.
         */
        IMPORT_C TInt Connect();

        /**
         * Version information.
         * @return Version information.
         */
        TVersion Version() const;

        /**
         * Get connection state of WLAN.
         * @return Current state of WLAN connection.
         */
        IMPORT_C TWlanConnectionState GetConnectionState();

        /**
         * Get current connections RCPI.
         * @param aRCPI (OUT) Current signal strength of the connection, 0 if no WLAN connection.
         * @return General Symbian error code.
         */
        IMPORT_C TInt GetConnectionRCPI(
            TInt32& aRCPI );

        /**
         * Get scan results.
         * @param aResults (OUT) Results of the last scan.
         * @param aSsid SSID to be scanned.
         * @param aCacheLifetime how old cached results (in seconds) the client is willing to accept.
         * @param aMaxDelay maximum time (in seconds) the client is willing to wait for the scan results.
         * @return General Symbian error code.
         */
         IMPORT_C TInt GetScanResults(
             ScanList& aResults,
             const TDesC8& aSsid,
             TInt& aCacheLifetime,
             TUint& aMaxDelay );

         /**
          * Get scan results.
          * @param aResults (OUT) Results of the last scan.
          * @param aSsid SSID to be scanned.
          * @param aCacheLifetime how old cached results (in seconds) the client is willing to accept.
          * @param aMaxDelay maximum time (in seconds) the client is willing to wait for the scan results.
          * @param aStatus Status of the calling active object.
          */
         IMPORT_C void GetScanResults(
             ScanList& aResults,
             const TDesC8& aSsid,
             TInt& aCacheLifetime,
             TUint& aMaxDelay,
             TRequestStatus& aStatus );
         
        /**
         * Cancel an outstanding scan request.
         */
        IMPORT_C void CancelGetScanResults();

        /** 
         * Join to a network using the settings from "IAP specific WLAN table". 
         *
         * @param aStatus    Status of the calling active object.
         * @param aId        Index to the "IAP specific WLAN table".
         * @param aOverrides Override parameters to joining the network.
         */
        IMPORT_C void Join(
            TRequestStatus& aStatus,
            const TUint32 aId,
            TWLMOverrideSettings* aOverrides = NULL );

        /**
         * Cancel an outstanding join request.
         */
        IMPORT_C void CancelJoin();

        /**
         * Reset the network connection.
         * @return General Symbian error code.
         */
        IMPORT_C TInt Release();

        /**
        * Get the BSSID of the current connection.
        * @param aBssid The BSSID of the current connection.
        *               If no connection a zero address is returned.
        * @return General Symbian error code.
        */
        IMPORT_C TInt GetBSSID(
            TMacAddress& aBssid );

        /**
        * Get the name of the current network.
        * @param aNwName (OUT) The name of the current network.
        * @return General Symbian error code.
        */
        IMPORT_C TInt GetNetworkName(
            TDes& aNwName );

        /**
        * Get current security mode.
        * @param aMode (OUT) The current security mode.
        * @return General Symbian error code.
        */
        IMPORT_C TInt GetSecurityMode(
            TWlanSecurity& aMode );

        /**
        * Get current system mode.
        * @param aMode (OUT) The current system mode.
        * @return General Symbian error code.
        */
        IMPORT_C TInt GetSystemMode(
            TWlanSystemMode& aMode );

        /**
        * Get available WLAN IAPs.
        * @param aAvailableIaps Array of IAP IDs available.
        * @param aCacheLifetime how old cached results (in seconds) the client is willing to accept.
        * @param aMaxDelay maximum time (in seconds) the client is willing to wait for the scan results.
        * @param aFilteredResults Whether availability is filtered based on signal strength.
        * @return General Symbian error code.
        */
        IMPORT_C TInt GetAvailableIaps(
            TWlmAvailableIaps& aAvailableIaps,
            TInt& aCacheLifetime,
            TUint& aMaxDelay,
            TBool aFilteredResults );

        /**
         * Get available WLAN IAPs.
         * @param aAvailableIaps Array of IAP IDs available.
         * @param aCacheLifetime how old cached results (in seconds) the client is willing to accept.
         * @param aMaxDelay maximum time (in seconds) the client is willing to wait for the scan results.
         * @param aFilteredResults Whether availability is filtered based on signal strength.
         * @param aStatus Status of the calling active object.
         *
         * In order to call this method, an instance of TWlmAvailableIaps must
         * packaged into a TPckg<TWlmAvailableIaps> package buffer. This buffer
         * can be passed to the called method as a descriptor.
         *
         * Example:
         * TPckg<TWlmAvailableIaps> availableIapsBuf( availableIaps );
         * GetAvailableIaps( availableIapsBuf, cacheLifetime, maxDelay, status );        
         */
         IMPORT_C void GetAvailableIaps(
             TDes8& aAvailableIaps,
             TDes8& aCacheLifetime,
             TDes8& aMaxDelay,
             TBool aFilteredResults,
             TRequestStatus& aStatus );
        
        /**
         * Cancel an outstanding IAP availability request.
         */
        IMPORT_C void CancelGetAvailableIaps();

        /**
        * Notify the server about changed WLAN settings.
        */
        IMPORT_C void NotifyChangedSettings();

        /**
        * Adds a BSSID to rogue list
        * @param aBssid a BSSID of an accesspoint.
        * @return Error code.
        */
        IMPORT_C TInt AddBssidToRoguelist(
            const TMacAddress& aBssid );

        /**
        * Updates the RCP notification class boundaries.
        * @param aRcpLevelBoundary specifies the RCP level when notification
        *     should be given. This boundary is used when signal level is
        *     getting worse (see next parameter).
        * @param aHysteresis specifies the difference between RCP notification
        *     trigger levels of declining and improving signal quality.
        *     I.e. since aRcpLevelBoundary specifies the level boundary for
        *     declining signal, the same boundary for imrpoving signal is
        *     ( aRcpLevelBoundary - aHysteresis ).
        * @return Error value
        */
        IMPORT_C TInt UpdateRcpNotificationBoundary(
            const TInt32 aRcpLevelBoundary,
            const TInt32 aHysteresis );

        /**
         * ConfigureMulticast
         * @param aCommand specifies either KSoIp6JoinGroup or
         *        KSoIp6LeaveGroup
         * @param aMulticastAddress contains the MAC address
         * @return Error code.
         */
        IMPORT_C TInt ConfigureMulticast(
            TUint aCommand,
            const TMacAddress& aMulticastAddress );

        /**
         * Get packet statistics of the current connection.
         *
         * @since S60 v3.2
         * @param aStatistics Packet statistics of the current connection.
         * @return KErrNone if statistics were read successfully, an error otherwise.
         */
        IMPORT_C TInt GetPacketStatistics(
            TWlanPacketStatistics& aStatistics );

        /**
         * Clear packet statistics of the current connection.
         *
         * @since S60 v3.2
         */        
        IMPORT_C void ClearPacketStatistics();

        /**
         * Get the current U-APSD settings.
         *
         * @since S60 v3.2
         * @param aSettings Current U-APSD settings.
         * @return KErrNone if settings were read successfully, an error otherwise.
         */
        IMPORT_C TInt GetUapsdSettings(
            TWlanUapsdSettings& aSettings );

        /**
         * Set the U-APSD settings.
         *
         * @since S60 v3.2
         * @param aSettings Current U-APSD settings to be set.
         * @return KErrNone if settings were set successfully, an error otherwise.
         */
        IMPORT_C TInt SetUapsdSettings(
            const TWlanUapsdSettings& aSettings );        

        /**
         * Get the current power save settings.
         *
         * @since S60 v3.2
         * @param aSettings power save settings.
         * @return KErrNone if settings were read successfully, an error otherwise.
         */
        IMPORT_C TInt GetPowerSaveSettings(
            TWlanPowerSaveSettings& aSettings );

        /**
         * Set the power save settings.
         *
         * @since S60 v3.2
         * @param aSettings Current power save settings to be set.
         * @return KErrNone if settings were set successfully, an error otherwise.
         */      
        IMPORT_C TInt SetPowerSaveSettings(
            const TWlanPowerSaveSettings& aSettings );

        /**
         * Run Protected Setup.
         * @param aStatus  Status of the calling active object.
         * @param aSsid SSID of the network to configure.
         * @param aWpsPin PIN value to be used. "00000000" (string of eight zeros)
         *                if push button method is used.
         * @param Results of a successful Protected Setup operation.
         */
        IMPORT_C void RunProtectedSetup(
            TRequestStatus& aStatus,
            const TWlanSsid& aSsid,
            const TWlanWpsPin& aWspPin,  
            TDes8& aCredentials );

        /**
         * Cancel an outstanding Protected Setup request.
         */
        IMPORT_C void CancelProtectedSetup();

        /**
         * Create a virtual traffic stream.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aStreamParameters Traffic stream parameters to use.
         * @param aIsAutomaticStream Whether the stream was created automatically
         *                           by WLAN NIF.
         * @param aStreamId Contains the ID assigned to this traffic stream
         *                  on successful completion.
         * @param aStreamStatus Contains the status of the traffic stream
         *                  on successful completion.
         */
        IMPORT_C void CreateTrafficStream(
            TRequestStatus& aStatus,
            const TWlanTrafficStreamParameters& aStreamParameters,
            TBool aIsAutomaticStream,
            TUint& aStreamId,
            TWlanTrafficStreamStatus& aStreamStatus );

        /**
         * Cancel an outstanding traffic stream creation request.
         *
         * @since S60 v3.2
         */
        IMPORT_C void CancelCreateTrafficStream();

        /**
         * Delete a virtual traffic stream.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aStreamId ID of the traffic stream to delete.
         */
        IMPORT_C void DeleteTrafficStream(
            TRequestStatus& aStatus,
            TUint aStreamId );
        /**
         * Cancel an outstanding traffic stream deletion request.
         *
         * @since S60 v3.2
         */
        IMPORT_C void CancelDeleteTrafficStream();

        /**
         * Get information about the current AP.
         *
         * @since S60 v3.2
         * @param aInfo Information about the current AP.
         * @return KErrNone if information is available, an error otherwise.
         */
        IMPORT_C TInt GetAccessPointInfo(
            TWlanAccessPointInfo& aInfo );

        /**
         * Get roam metrics of the current connection.
         *
         * @since S60 v3.2
         * @param aRoamMetrics Roam metrics of the current connection.
         * @return KErrNone if settings were read successfully, an error otherwise.
         */
        IMPORT_C TInt GetRoamMetrics(
            TWlanRoamMetrics& aRoamMetrics );

        /**
         * Return a list of BSSIDs on the rogue list.
         *
         * @since S60 v3.2
         * @param aRogueList List of BSSIDs on the rogue list.
         * @return KErrNone if list were read successfully, an error otherwise.
         */
        IMPORT_C TInt GetRogueList(
            CArrayFixSeg<TWlanBssid>& aRogueList );
        
        /**
         * Get the current regulatory domain.
         *
         * @since S60 v3.2
         * @param aRegion current region.
         * @return KErrNone if value was read successfully, an error otherwise.
         */
        IMPORT_C TInt GetRegulatoryDomain(
            TWlanRegion& aRegion );
        
        /**
         * Get the current power save mode.
         *
         * @since S60 v3.2
         * @param aPowerSaveMode current power save mode.
         * @return KErrNone if value was read successfully, an error otherwise.
         */
        IMPORT_C TInt GetPowerSaveMode(
            TWlanPowerSave& aPowerSaveMode );

        /**
         * Add a list of SSIDs to the given IAP ID.
         *
         * The list of SSIDs is matched against the scan results during IAP availability
         * check and the corresponding IAP marked as available if a match is found.
         *
         * @since S60 v5.0.1
         * @param aIapId IAP ID the list is attached to.
         * @param aSsidList List of SSIDs. Any previous list attached will be overwritten.
         * @return KErrNone if the list was successfully added, an error code otherwise.
         */
        IMPORT_C TInt AddIapSsidList(
            TUint aIapId,
            const CArrayFixFlat<TWlanSsid>& aSsidList );

        /**
         * Remove any list of SSIDs attached to the given IAP ID.
         *
         * @since S60 v5.0.1
         * @param aIapId IAP ID the list is attached to.
         * @return KErrNone if the list was successfully removed, an error code otherwise.
         */
        IMPORT_C TInt RemoveIapSsidList(
            TUint aIapId );  

        /**
         * Set the power save mode.
         *
         * @since S60 v5.0.1
         * @param aMode Power save mode to be set.
         * @return KErrNone if the mode was set successfully, an error otherwise.
         */       
        IMPORT_C TInt SetPowerSaveMode(
            TWlanPowerSaveMode aMode );

        /**
         * Notify the server about PSM server mode change.
         *
         * @since S60 v5.2
         * @param aMode new PSM server mode.
         * @return KErrNone if the mode was set successfully, an error otherwise.
         */
        IMPORT_C TInt NotifyChangedPsmSrvMode(
            TInt aMode ); 

        /**
         * Get the current traffic status for access classes.
         * 
         * @since S60 v5.0.1
         * @param aArray Traffic status for access classes.
         * @return KErrNone if information is available, an error otherwise. 
         */
        IMPORT_C TInt GetAcTrafficStatus(
            TWlmAcTrafficStatusArray& aArray );

        /**
         * Initiate a roam to the given BSSID.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aBssid BSSID to roam to. If set to FF:FF:FF:FF:FF:FF address,
         *               search for a better BSS is initiated.
         */
        IMPORT_C void DirectedRoam(
            TRequestStatus& aStatus,
            const TMacAddress& aBssid );

        /**
         * Cancel an outstanding directed roam request.
         */
        IMPORT_C void CancelDirectedRoam();
        
        /**
         * Start aggressive background scanning.
         * 
         * Sends a blind request (i.e. request is sent without waiting for
         * reply) to WLAN Engine to enter aggressive BG scan mode. WLAN
         * Engine decides how long it stays in the aggressive mode. The
         * client can monitor BG scan interval changes via WLAN Info P&S.
         * The request is valid only when WLAN is disconnected, if the request
         * is sent when WLAN is connected, it is ignored. If the interval
         * given as a parameter is out of valid range, it is automatically
         * corrected to closest valid value. In case of multiple simultaneous
         * requests, the one with highest scan frequency is served.
         * 
         * @since S60 v5.2
         * @param aInterval Desired background scan interval in seconds.
         *                  Valid range is from 1 to 120 seconds.
         * @return KErrNone if sending succeeded, an error otherwise
         */
        IMPORT_C TInt StartAggressiveBgScan(
            TUint aInterval );

    protected: // Methods

        /**
        * Start the server during Connect() if the server is not loaded.
        * @return error code
        */
        TInt StartServer();

    private: // Members

        /** Notification service. Exists only if its activated. */
        CWLMNotify* iNotify;

        /** Class for handling scan requests. */
        CWlmScanRequest* iScanRequest;

        /** Parameters for traffic stream create request. */  
        TWlmTsCreateRequest* iTsCreateRequest;

        /** Buffer for storing override settings. */
        TPckgBuf<TWLMOverrideSettings> iOverridePckg;

        /** Buffer for storing BSSID to roam to. */
        TPckgBuf<TMacAddress> iRoamBssidPckg; 

        /** Buffer for storing SSID for Protected setup. */
        TPckgBuf<TWlanSsid> iWlanSsidPckg; 

        /** Buffer for storing pincode for Protected setup. */
        TPckgBuf<TWlanWpsPin> iWlanWpsPinPckg; 

    };

#endif // RWLMSERVER_H
