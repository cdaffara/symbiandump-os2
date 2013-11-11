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
* Description:  ECom interface implementation definition
*
*/

/*
* %version: 23 %
*/

#ifndef WLANMGMTIMPL_H
#define WLANMGMTIMPL_H

#include "rwlmserver.h"
#include "genscanlist.h"
#include "wlanmgmtclient.h"
#include "wlanscaninfoimpl.h"

const TInt32 KRssMin = 0;
const TInt32 KRssMax = 100;
const TInt32 KRssHysteresisMin = 0;
const TInt32 KRssHysteresisMax = 100;

class CWlanScanRequest;
class CWlanAvailableIapsRequest;
class CProtectedSetupRequest;

/**
 * Implementation for MWlanMgmtInterface interface.
 *
 * This class uses services from RWLMServer class to implement
 * most of the required functionality.
 *
 * @lib wlanmgmtimpl.dll
 * @since Series 60 3.0
 */
class CWlanMgmtImpl : public CWlanMgmtClient, public MWLMNotify
    {
    public:  // Methods

       // Constructors and destructor
        
        /**
         * Static constructor.
         */
        static CWlanMgmtImpl* NewL();
        
        /**
         * Destructor.
         */
        virtual ~CWlanMgmtImpl();

       // New methods
        
        /**
         * Used to indicate completion of asynchronous scan request.
         *
         * @param aStatus Status of the request.
         */
        void ScanComplete(
            TInt aStatus );

        /**
         * Used to indicate completion of asynchronous GetAvailableIaps() request.
         *
         * @param aStatus Status of the request.
         */
        void AvailableIapsComplete(
            TInt aStatus );

       // Methods from base classes

        /**
         * (From MWlanMgmtInterface) Activate the notification service.
         * 
         * After the client has enabled the notification service, it can
         * receive asynchronous notifications from the server.
         *
         * @param aCallback The class that implements the callback interface.
         */
        virtual void ActivateNotificationsL(
            MWlanMgmtNotifications& aCallback );
        
        /**
         * (From MWlanMgmtInterface) Cancel the notification service.
         */
        virtual void CancelNotifications();

        /**
         * (From MWlanMgmtInterface) Perform a broadcast scan and return the detected WLAN networks.
         *
         * @param aResults Results of the scan.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetScanResults(
            CWlanScanInfo& aResults );

        /**
         * (From MWlanMgmtInterface) Perform a broadcast scan and return the detected WLAN networks.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aResults Results of the scan.
         */
        virtual void GetScanResults(
            TRequestStatus& aStatus,
            CWlanScanInfo& aResults );

        /**
         * (From MWlanMgmtInterface) Get the BSSID of the BSS currently connected to.
         *
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aBssid BSSID of the currently connected BSS.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionBssid(
            TWlanBssid& aBssid );

        /**
         * (From MWlanMgmtInterface) Get the SSID of the WLAN network currently connected to.
         *
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aSsid SSID of the currently connected network.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionSsid(
            TWlanSsid& aSsid );

        /**
         * (From MWlanMgmtInterface) Get the current Received Signal Strength Indicator (RSSI).
         *
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aSignalQuality Current RSSI.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionSignalQuality(
            TInt32& aSignalQuality );

        /**
         * (From MWlanMgmtInterface) Get the mode of the WLAN connection.
         *
         * @param aMode The current mode of the connection.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionMode(
            TWlanConnectionMode& aMode );

        /**
         * (From MWlanMgmtInterface) Get the currently used security mode of the WLAN connection.
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
            TWlanConnectionSecurityMode& aMode );

        /**
         * (From MWlanMgmtInterface) Get the available WLAN IAPs.
         *
         * @param aAvailableIaps Array of IAP IDs available.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetAvailableIaps(
            RArray<TUint>& aAvailableIaps );
 
        /**
         * (From MWlanMgmtInterface) Get the available WLAN IAPs.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.       
         * @param aAvailableIaps Array of IAP IDs available.
         */
        virtual void GetAvailableIaps(
            TRequestStatus& aStatus,
            RArray<TUint>& aAvailableIaps );

        /**
         * (From MWlanMgmtInterface)
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
         *                  seconds.
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
             RArray<TUint>& aAvailableIaps );
        
        /**
         * (From MWlanMgmtInterface) Notify the server about changed WLAN settings.
         */
        virtual void NotifyChangedSettings();
        
        /**
         * (From MWlanMgmtInterface)
         * Adds a bssid to the blacklist
         *
         * @param aBssid The BSSID of the accesspoint.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt AddBssidToBlacklist(
            const TWlanBssid& aBssid );

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
            const TInt32 aHysteresis );

        /**
         * (From MWlanMgmtInterface) 
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
            CWlanScanInfo& aResults );

        /**
         * (From MWlanMgmtInterface)
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
         *                  the scan results. The valid range is from 0 to 1200 seconds.
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
             CWlanScanInfo& aResults );
        
        /**
         * (From MWlanMgmtInterface)
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
            CArrayFixSeg<TWlanProtectedSetupCredentialAttribute>& aCredentials );

        /**
         * (From MWlanMgmtInterface)
         * Cancel an outstanding Protected Setup operation.
         */
        virtual void CancelProtectedSetup();

        /**
         * (From MWlanMgmtInterface)
         * Cancel an outstanding scan request.
         */
        virtual void CancelGetScanResults();

        /**
         * (From MWlanMgmtInterface)
         * Cancel an outstanding IAP availability request.
         */
        virtual void CancelGetAvailableIaps();

        /**
         * (From MWlanMgmtInterface)
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
            const CArrayFixFlat<TWlanSsid>& aSsidList );

        /**
         * (From MWlanMgmtInterface)
         * Remove any list of SSIDs attached to the given IAP ID.
         *
         * @param aIapId IAP ID the list is attached to.
         * @return KErrNone if the list was successfully removed, an error code otherwise.
         */
        virtual TInt RemoveIapSsidList(
            TUint aIapId );

        /**
         * (From MWlanMgmtInterface)
         * Get the currently used security mode of the WLAN connection.
         *
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aMode The security mode of the connection.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetExtendedConnectionSecurityMode(
            TWlanConnectionExtentedSecurityMode& aMode );        

        /**
         * (From MWlanMgmtInterface)
         * Activate the extended notification service.
         * 
         * After the client has enabled the notification service, it can
         * receive asynchronous notifications from the server.
         *
         * @param aCallback The class that implements the callback interface.
         * @param aCallbackInterfaceVersion The callback interface version implemented by
         *                                  the client.
         */
        virtual void ActivateExtendedNotificationsL(
            MWlanMgmtNotifications& aCallback,
            TUint aCallbackInterfaceVersion );

        /**
         * (From MWlanMgmtInterface)
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
            TWlanTrafficStreamStatus& aStreamStatus );

        /**
         * (From MWlanMgmtInterface)
         * Cancel an outstanding traffic stream creation request.
         */
        virtual void CancelCreateTrafficStream();

        /**
         * (From MWlanMgmtInterface)
         * Delete a virtual traffic stream.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aStreamId ID of the traffic stream to delete.
         */
        virtual void DeleteTrafficStream(
            TRequestStatus& aStatus,
            TUint aStreamId );

        /**
         * (From MWlanMgmtInterface)
         * Cancel an outstanding traffic stream deletion request.
         */
        virtual void CancelDeleteTrafficStream();

        /**
         * (From MWlanMgmtInterface)
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
            const TWlanBssid& aBssid );

        /**
         * (From MWlanMgmtInterface)
         * Cancel an outstanding directed roam request.
         */
        virtual void CancelDirectedRoam();

        /**
         * Used to indicate completion of asynchronous Protected Setup request.
         * @param aStatus Status of the request.
         */
        void ProtectedSetupComplete(
            TInt aStatus );

        /**
         * (From MWlanMgmtInterface)
         * Start aggressive background scanning.
         * @param aInterval Scan interval for aggressive mode (in seconds).
         *                  Valid range: 1-120 seconds.
         */
        virtual TInt StartAggressiveBgScan(
            TUint aInterval );

        /**
         * (From MWlanMgmtInterface)
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
            RArray<TWlanIapAvailabilityData>& aAvailableIaps );

	protected: // Methods

        /**
         * (From MWLMNotify) Connection status has changed.
         * 
         * @aParam aNewState specifies the new state
         */
        virtual void ConnectionStateChanged(
            TWlanConnectionState aNewState );
    
        /**
         * (From MWLMNotify) BSSID has changed (i.e. AP handover)
         * 
         * @aParam aNewBSSID specifies the new BSSID (access point name).
         */
        virtual void BSSIDChanged(
            TDesC8& aNewBSSID );

        /**
         * (From MWLMNotify) Connection has lost.
         */
        virtual void BSSLost();

        /**
         * (From MWLMNotify) Connection has regained.
         */
        virtual void BSSRegained();

        /**
         * (From MWLMNotify) New networks have been detected during scan.
         */
        virtual void NewNetworksDetected();

        /**
         * (From MWLMNotify) One or more networks have been lost since
         * the last scan.
         */
        virtual void OldNetworksLost();

        /**
         * (From MWLMNotify) The used transmit power has been changed.
         * 
         * @param aPower The transmit power in mW.
         */
        virtual void TransmitPowerChanged(
            TUint aPower );

        /**
         * (From MWLMNotify) Received signal strength has changed to another strength class.
         *
         * @param  specifies the rss class (normal/weak)
         * @param aRcpValue specifies the actual rss value 
         */
        virtual void RcpChanged(
            TWlanRCPLevel aRcpLevel,
            TUint aRcpValue );

        /**
         * (From MWLMNotify) The status of a virtual traffic stream has changed.
         *
         * @param aStreamId ID of the traffic stream.
         * @param aStreamStatus Status of the traffic stream.
         */
        virtual void TrafficStreamStatusChanged(
            TUint aStreamId,
            TWlanTrafficStreamStatus aStreamStatus );

    private: // Methods

        /**
         * C++ default constructor.
         */
        CWlanMgmtImpl();

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();
    
    private:  // Data

        // Interface to RWLMServer
        RWLMServer iServer;

        // Active Object for handling asynchronous scan requests
        CWlanScanRequest* iScanRequest;
       
        // Status of the client active object for scan. Not owned by this class.
        TRequestStatus* iPendingScanStatus;

        // Active Object for handling asynchronous GetAvailableIaps requests
        CWlanAvailableIapsRequest* iAvailableIapsRequest;
		
        // Status of the client active object for GetAvailableIaps(). Not owned by this class.
        TRequestStatus* iPendingAvailableIapsStatus;

		// Client notification callback. Not owned by this class.
		MWlanMgmtNotifications* iClientNotification;

        // Active Object for handling asynchronous Protected Setup requests
        CProtectedSetupRequest* iProtectedSetupRequest;
        
        // Status of the client active object for Protected Setup. Not owned by this class.
        TRequestStatus* iPendingProtectedSetupStatus;

        // How old cached results (in seconds) the client is willing to accept. Not used
        // in GetAvailableIaps and GetScanResults which have aCacheLifetime and aMaxDelay
        // parameters.
        TInt iCacheLifetime;

		// Maximum time (in seconds) the client is willing to wait for the scan results. Not
        // used in GetAvailableIaps and GetScanResults which have aCacheLifetime and aMaxDelay
        // parameters.
		TUint iMaxDelay;

    };

/**
* Class for handling asynchronous scan requests.
*/
class CWlanScanRequest : public CActive 
    {
    public:  // Methods

       // Constructors and destructor

        /**
         * C++ default constructor.
         * @param aCallback Callback interface to CWlanMgmtImpl.
         * @param aServer Interface to RWLMServer. 
         * @param aScanInfo The scan results are stored here.
         * @param aSsid name of the network
         * @param aCacheLifetime how old cached results (in seconds) the client is willing to accept.
         * @param aMaxDelay maximum time (in seconds) the client is willing to wait for the scan results.
         */
        CWlanScanRequest( 
            CWlanMgmtImpl& aCallback, 
            RWLMServer& aServer, 
            CWlanScanInfoImpl& aScanInfo,
            const TWlanSsid& aSsid,
            TInt& aCacheLifetime,
            TUint& aMaxDelay );

        /**
         * Destructor.
         */
        virtual ~CWlanScanRequest();
        
       // New methods

        /**
         * Issues a new request and indicates the active scheduler
         * of an outstanding request.
         */
        void IssueRequest();

    protected:  // Methods

       // Methods from base classes

        /**
         * (From CActive) Called when the request has been completed.
         */       
        virtual void RunL();

        /**
         * (From CActive) Called by the framework if RunL leaves.
         * @param aError The error code RunL leaved with.
         * @return One of the system-wide error codes.
         */
        virtual TInt RunError(
            TInt aError );

        /**
         * (From CActive) Called by the framework as part of AO's Cancel().
         */
        virtual void DoCancel();

    private:   // Data

        // Callback interface to CWlanMgmtImpl
        CWlanMgmtImpl& iCallback;

        // Interface to RWLMServer
        RWLMServer& iServer;

        // Handle to the scan list.
        CWlanScanInfoImpl& iScanInfo;

        // SSID to scan for.
        TWlanSsid iSsid;

        // How old cached results (in seconds) the client is willing to accept
        TInt& iCacheLifetime;
		
        // Maximum time (in seconds) the client is willing to wait for the scan results.
        TUint& iMaxDelay;

    };

/**
* Class for handling asynchronous GetAvailableIaps() requests.
*/
class CWlanAvailableIapsRequest : public CActive 
    {
    public:  // Methods

       // Constructors and destructor

        /**
         * C++ default constructor.
         * @param aCallback Callback interface to CWlanMgmtImpl.
         * @param aServer Interface to RWLMServer. 
         * @param aAvailableIaps Available IAPs are stored here.
         * @param aCacheLifetime how old cached results (in seconds) the client is willing to accept.
         * @param aMaxDelay maximum time the client is willing to wait for the scan results.
         */
        CWlanAvailableIapsRequest(
            CWlanMgmtImpl& aCallback,
            RWLMServer& aServer,
            RArray<TUint>& aAvailableIaps,
            TInt& aCacheLifetime,
            TUint& aMaxDelay );

        /**
         * C++ default constructor.
         * @param aCallback Callback interface to CWlanMgmtImpl.
         * @param aServer Interface to RWLMServer. 
         * @param aAvailableIaps Available IAPs are stored here.
         * @param aCacheLifetime how old cached results (in seconds) the client is willing to accept.
         * @param aMaxDelay maximum time the client is willing to wait for the scan results.
         * @param aFilteredResults Whether availability is filtered based on signal strength.
         */
        CWlanAvailableIapsRequest(
            CWlanMgmtImpl& aCallback,
            RWLMServer& aServer,
            RArray<TWlanIapAvailabilityData>& aAvailableIaps,
            TInt& aCacheLifetime,
            TUint& aMaxDelay,
            TBool aFilteredResults );

        /**
         * Destructor.
         */
        virtual ~CWlanAvailableIapsRequest();
        
       // New methods

        /**
         * Issues a new request and indicates the active scheduler
         * of an outstanding request.
         */
        void IssueRequest();

    protected:  // Methods

       // Methods from base classes

        /**
         * (From CActive) Called when the request has been completed.
         */       
        virtual void RunL();

        /**
         * (From CActive) Called by the framework if RunL leaves.
         * @param aError The error code RunL leaved with.
         * @return One of the system-wide error codes.
         */
        virtual TInt RunError(
            TInt aError );

        /**
         * (From CActive) Called by the framework as part of AO's Cancel().
         */
        virtual void DoCancel();

    private:   // Data

        // Callback interface to CWlanMgmtImpl
        CWlanMgmtImpl& iCallback;

        // Interface to RWLMServer
        RWLMServer& iServer;

        // Handle to client storage for available IAP IDs for legacy methods.
        // Not owned by this pointer.
        RArray<TUint>* iPendingAvailableIapIds;

        // Handle to client storage for available IAPs.
        // Not owned by this pointer.
        RArray<TWlanIapAvailabilityData>* iPendingAvailableIaps;

		// Storage for available IAPs
		TWlmAvailableIaps iAvailableIaps;

		// The package buffer needed for the asynchronous request
		TPckg<TWlmAvailableIaps> iAvailableIapsBuf;
		
		// The package buffer needed for the asynchronous request
		TPckg<TInt> iCacheLifetimeBuf;
		
		// The package buffer needed for the asynchronous request
		TPckg<TUint> iMaxDelayBuf;
		
		// Whether availability is filtered based on signal strength.
		TBool iFilteredResults;
    };


/**
* Class for handling asynchronous Protected Setup requests.
*/
class CProtectedSetupRequest : public CActive 
    {
    public:  // Methods

       // Constructors and destructor

        /**
         * C++ default constructor.
         * @param aCallback Callback interface to CWlanMgmtImpl.
         * @param aServer Interface to RWLMServer. 
         * @param aId Service ID of network which user has selected to be configured.
         * @param aCredentials Results of a successful Protected Setup operation.
         */
        CProtectedSetupRequest(
            CWlanMgmtImpl& aCallback,
            RWLMServer& aServer,
            const TWlanSsid& aSsid,
            const TWlanWpsPin& aWpsPin,
            CArrayFixSeg<TWlanProtectedSetupCredentialAttribute>& aCredentials );

        /**
         * Destructor.
         */
        virtual ~CProtectedSetupRequest();
        
       // New methods

        /**
         * Issues a new request and indicates the active scheduler
         * of an outstanding request.
         */
        void IssueRequest();

    protected:  // Methods

       // Methods from base classes

        /**
         * (From CActive) Called when the request has been completed.
         */       
        virtual void RunL();

        /**
         * (From CActive) Called by the framework if RunL leaves.
         * @param aError The error code RunL leaved with.
         * @return One of the system-wide error codes.
         */
        virtual TInt RunError(
            TInt aError );

        /**
         * (From CActive) Called by the framework as part of AO's Cancel().
         */
        virtual void DoCancel();

    private:   // Data

        // Callback interface to CWlanMgmtImpl
        CWlanMgmtImpl& iCallback;

        // Interface to RWLMServer
        RWLMServer& iServer;

        // SSID of the network to configure.
        TWlanSsid iSsid;

        // PIN value to be used. 
        TWlanWpsPin iWpsPin;

        // Results of a successful Protected Setup operation.
        CArrayFixSeg<TWlanProtectedSetupCredentialAttribute>& iCredentials;

		// Storage for received credentials.
		TWlmProtectedSetupCredentials iCredentialsStorage;

        // The package buffer needed for the asynchronous request.
        TPckg<TWlmProtectedSetupCredentials> iCredentialsBuf;
    };

#endif // WLANMGMTIMPL_H
