/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main server
*
*/

/*
* %version: 59 %
*/

#ifndef WLMSERVER_H
#define WLMSERVER_H

#include <wdbifwlansettings.h>  // Used in SIAPSettings.

#include "abs_core_server_callback.h"
#include "abs_core_server.h"
#include "abs_wlan_eapol_callback.h"
#include "wlanscanproviderinterface.h"
#include "core_server.h"
#include "wlmnotification.h"
#include "wlandevicesettings.h"
#include "wlanscanresultcache.h"
#include "wlmplatform.h"
#include "umac_types.h" // TIndication
#include "wlanconversionutil.h"
#include "wlancontrolinterface.h"
#include "wlaneapolinterface.h"
#include "wlaneapolclient.h"
#include "wlangenericplugin.h"

class CWlmDriverIf;
class CWlanSsidListDb;
class CWlanTimerServices;
class CWlanTickTimer;

/**
 * Command Ids to be used un the asynchronous core service requests
 * External commands coming from clients get request id that is > KWlanExtCmdBase
 * Values 0-KWlanExtCmdBase are reserved for internal usage.
 */
const TUint32 KWlanIntCmdNull = 0;
const TUint32 KWlanIntCmdBackgroundScan = 1;
const TUint32 KWlanIntCmdEnableWlan = 2;
const TUint32 KWlanIntCmdDisableWlan = 3;
const TUint32 KWlanIntCmdSetPowerSave = 4;
const TUint32 KWlanExtCmdBase = 100;

/** Timer interval value for disabling background scan. */
const TInt KBackgroundScanIntervalNever = 0;

/** UID for WLAN Power Save Test Notifier */
const TUid KUidWlanPowerSaveTestNote = { 0x101F6D4F };

/** Multiplier for converting seconds into microseconds */
const TUint KWlanSecsToMicrosecsMultiplier( 1000000 );

/**
* The server for WLAN services. Counterpart of RWLMServer.
*
* @lib wlmserversrv.lib
* @since Series 60 3.0
*/
NONSHARABLE_CLASS( CWlmServer ) : 
    public CServer2, 
    public abs_core_server_callback_c, 
    public MWlmPlatformCallback,
    public MWlanEapolCallbackInterface,
    public MWlanScanResultProvider
    {
    
    // Friend class for Unit Testing
    friend class UT_CWlmServer;
    
    public:  // Constructors and destructor

        /** 
         * map entry is used for queuing service requests
         */
        struct SRequestMapEntry
            {
            u32_t iRequestId;
            TUint iSessionId;
            RMessagePtr2 iMessage;
            TWLMCommands iFunction;
            TAny* iParam0;
            TAny* iParam1;
            TAny* iParam2;
            TAny* iParam3;
            TAny* iParam4;
            TUint* iTime;

            SRequestMapEntry() :
                iRequestId( 0 ),
                iSessionId( 0 ),
                iMessage(),
                iParam0( NULL ),
                iParam1( NULL ),
                iParam2( NULL ),
                iParam3( NULL ),
                iParam4( NULL ),
                iTime( NULL )
                { }
            };

        /**
         * Bit definitions for static WLAN features.
         */
        enum TWlanStaticFeature
            {
            /** Initialisation value. */
            EWlanStaticFeatureNone                = 0x00000000,
            /** Whether WAPI is enabled. */
            EWlanStaticFeatureWapi                = 0x00000001,
            /** Whether power save test is enabled. */
            EWlanStaticFeaturePowerSaveTest       = 0x00000002,
            /** Whether 802.11k is enabled. */
            EWlanStaticFeature802dot11k           = 0x00000004,
            /** Whether 802.11n is enabled. */
            EWlanStaticFeature802dot11n           = 0x00000008,
            };

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CWlmServer* NewL();
        
        /**
         * Destructor.
         */
        virtual ~CWlmServer();

    public: // New functions

        /** 
        * Initialises the server thread.
        *
        * The compilation macro is to ease to run the server as a
        * thread and as a own process.
        *
        * @since Series 60 3.0
        * @return Standard Symbian error.
        */
        IMPORT_C static TInt StartServerThread();
        
        /**
        * Get the type of scan allowed.
        * @return Type of scan allowed.
        */
        inline TWLMScanType GetScanType();

        /**
        * SetIconState sets the status of WLAN icon on cover display.
        * @param aStatus The WLAN icon status.
        * @return Symbian error code.
        */
        inline TInt SetIconState( TWlmIconStatus aStatus );
        
        /**
        * Find out the IAP to join to.
        * @param aLanServiceId  ID of the WLAN IAP settings to been joined.
        * @param aWlanSettings  (OUT) The WLAN IAP settings.
        * @param aSecondaryList (OUT) List of secondary SSIDs.
        */
        void GetIapSettingsL(
            const TUint32 aLanServiceId,
            SWLANSettings& aWlanSettings,
            RArray<TWlanSecondarySsid>& aSecondaryList );
            
        /**
        * Update the availability information.
        * @param aAvailability Availability information for the
        *                      Connection Monitor server.
        * @param aNetNetworksDetected Whether new networks have been detected.
        * @param aOldNetworksLost Whether one or more old networks have been lost.
        */
        inline void UpdateAvailability(
            TBool aAvailability,
            TBool aNewNetworksDetected = EFalse,
            TBool aOldNetworksLost = EFalse );
        
        /**
         * Notify changed PSM server mode
         *
         * @since Series 60 5.2
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        void NotifyChangedPsmSrvMode(
            TUint aSessionId,
            const RMessage2& aMessage );    
        
    public: // Functions from base classes

        /** 
        * From abs_core_server_callback_c Send a notification.
        * @param notification The notification to send.
        * @param data_length  Optional data length.
        * @param data         Pointer to optional data.
        */
        virtual void notify(
            core_notification_e notification,
            int_t data_length,
            const u8_t* data );

        /**
         * From abs_core_server_callback_c 
         * Notify about country information received from APs.
         * @param request_id Identification of the corresponding request.
         * @param found_region WLAN region information obtained from the AP's country information.
         * @param inconsistent_info Information whether different APs gave inconsistent country information.
         */
        virtual void store_ap_country_info(
        		u32_t request_id,
        		const core_wlan_region_e& found_region,
        	    bool_t inconsistent_info );        

        /**
        * From abs_core_server_callback_c 
        * Get the current regulatory domain.
        * @param request_id Identification of the corresponding request.
        * @return Current regulatory domain.
        * @param mcc_known Is the current mobile country code available
        */
        virtual void get_regulatory_domain(
            u32_t request_id,
            core_wlan_region_e& region,
            bool_t& mcc_known );
        
        /**
         * From abs_core_server_callback_c 
         * An asynchronous request from the adaptation layer has been completed.
         *
         * @since S60 v3.1
         * @param request_id Identification of the corresponding request.
         * @param status Completion status of the request.
         */
        virtual void request_complete(
            u32_t request_id,
            core_error_e status );

        /**
         * From abs_core_server_callback_c 
         * Cancel a pending asynchronous request.
         * 
         * @since S60 v3.1
         * @param request_id Identification of the corresponding request.
         */
        virtual void cancel_request(
            u32_t request_id );

        /**
         * From abs_core_server_callback_c 
         * Initialize interface to EAPOL.
         *
         * @since S60 v3.2
         * @param mode Operating mode to use.
         * @param partner Pointer to partner instance.
         * @return Status value of operation.
         */
        virtual u32_t load_eapol(
            core_eapol_operating_mode_e mode,
            abs_wlan_eapol_callback_c* const partner );
    
        /**
         * From abs_core_server_callback_c 
         * Shutdown EAPOL.
         *
         * @since S60 v3.2
         * @return Status value of operation.
         */
        virtual u32_t shutdown();
        
        /**
         * From abs_core_server_callback_c 
         * Configure EAPOL.
         *
         * @since S60 v3.2
         * @param header_offset Offset of EAP-header in packet_send.
         * @param MTU Maximum transfer unit (MTU).
         * @param trailer_length Length of trailer needed by lower levels..
         * @return Status value of operation.
         */
        virtual u32_t configure(
            const u32_t header_offset,
            const u32_t MTU,
            const u32_t trailer_length );

        /**
         * From abs_core_server_callback_c 
         * Send data to EAPOL.
         *
         * @since S60 v3.2
         * @param data Pointer to data to be send.
         * @param length Length of data to be send.
         * @return Status value of operation.
         */
        virtual u32_t process_data(
            const void * const data,
            const u32_t length );

        /**
        * From CServer2
        * @since Series 60 3.0
        * @param aVersion not used
        * @param aMessage not used
        * @return session object
        */
        virtual CSession2* NewSessionL(
            const TVersion& aVersion,
            const RMessage2& aMessage ) const;

        /**
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        virtual void NotifyAdd(
            CNotificationBase& aNotification );

        /**
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        virtual void NotifyRemove(
            CNotificationBase& aNotification );

        /**
         * Connect
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        virtual void Connect(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Cancel an outstanding connect request.
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        virtual void CancelConnect(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * GetCurrentBSSID
         * @since Series 60 3.0
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void GetCurrentBSSID(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * GetCurrentRCPI
         * @since Series 60 3.0
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void GetCurrentRCPI(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
        * @since Series 60 3.0
        * @param aSessionId ID identifying the session.        
        * @param aMessage containing the service request
        */
        virtual void GetCurrentSSID(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
        * GetCurrentSecurityMode
        * @since Series 60 3.0
        * @param aSessionId ID identifying the session.        
        * @param aMessage containing the service request
        */
        virtual void GetCurrentSecurityMode(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * GetCurrentSystemMode
         * @since Series 60 3.0
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void GetCurrentSystemMode(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Asks for connection state
         * @since Series 60 3.0
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void GetConnectionState(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Performs scan
         * @since Series 60 3.0
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void GetScanResult(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Cancel an outstanding scan request.
         * @since Series 60 3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void CancelGetScanResult(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Determines which IAPs are currently available
         * @since Series 60 3.1
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        void GetAvailableIaps(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Cancel an outstanding IAP availability request.
         * @since Series 60 3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        void CancelGetAvailableIaps(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * @since Series 60 3.0
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void ReleaseComplete(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        virtual TUint NotifySessionCreated();

        /**
        * @since Series 60 3.0
        * @param aSessionId ID identifying the session.
        */
        virtual void NotifySessionClosed( TUint aSessionId );

        /**
         * NotifyChangedSettings
         * @since Series 60 3.0
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void NotifyChangedSettings(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * AddBssidToRoguelist
         * @since Series 60 3.0
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void AddBssidToRoguelist(
            TUint aSessionId,
            const RMessage2& aMessage );
        
        /**
         * UpdateRcpNotificationBoundary
         * @since Series 60 3.0
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void UpdateRcpNotificationBoundary(
            TUint aSessionId,
            const RMessage2& aMessage );
        
        /**
         * ConfigureMulticastGroup
         * @since Series 60 3.0
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void ConfigureMulticastGroup(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Get packet statistics of the current connection.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        void GetPacketStatistics(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Clear packet statistics of the current connection.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */        
        void ClearPacketStatistics(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Get the current U-APSD settings.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        void GetUapsdSettings(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Set the U-APSD settings.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        void SetUapsdSettings(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Get the current power save settings.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        void GetPowerSaveSettings(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Set the power save settings.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        void SetPowerSaveSettings(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Request creation of a traffic stream.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        void CreateTrafficStream(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Cancel a pending traffic stream creation request.
         *
         * @since Series 60 3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void CancelCreateTrafficStream(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Request deletion of a traffic stream.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        void DeleteTrafficStream(
            TUint aSessionId,
            const RMessage2& aMessage );             

        /**
         * Cancel a pending traffic stream deletion request.
         *
         * @since Series 60 3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        virtual void CancelDeleteTrafficStream(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Get information about the current AP.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        void GetAccessPointInfo(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Get roam metrics of the current connection.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        void GetRoamMetrics(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Get a list of BSSIDs on the rogue list.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        void GetRogueList(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Get the current regulatory domain.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        void GetRegulatoryDomain(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Get the current power save mode.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        void GetPowerSaveMode(
            TUint aSessionId,
            const RMessage2& aMessage );
        
        /**
         * Run Protected Setup
         * @since Series 60 3.2
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        virtual void RunProtectedSetup(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Cancel Protected Setup
         * @since Series 60 3.2
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        virtual void CancelProtectedSetup(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Add a list of SSIDs to an IAP.
         *
         * @since Series 60 5.0.1
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        void AddIapSsidListL(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Remove any list of SSIDs attached to an IAP.
         *
         * @since Series 60 5.0.1
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        void RemoveIapSsidListL(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Set the power save mode.
         *
         * @since Series 60 5.0.1
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        void SetPowerSaveMode(
            TUint aSessionId,
            const RMessage2& aMessage );        

        /**
         * Get the current traffic status for access classes.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.
         * @param aMessage containing the service request
         */
        void GetAcTrafficStatus(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Initiate roam to the given BSSID.
         *
         * @since S60 v3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        void DirectedRoam(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * Cancel a pending directed roam request.
         *
         * @since Series 60 3.2
         * @param aSessionId ID identifying the session.         
         * @param aMessage containing the service request
         */
        void CancelDirectedRoam(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * StartAggressiveBgScan
         * Sets WLAN background scanning into more aggressive mode for a while.
         * 
         * @since S60 v5.2
         * @param aSessionId ID identifying the session.
         * @param aMessage Message containing the respective command.
         */
        void StartAggressiveBgScan(
            TUint aSessionId,
            const RMessage2& aMessage );

        /**
         * From MWlmPlatformCallback
         * Send an indication to request data pipe disconnection.
         *
         * Calls ReleaseComplete() directly if data pipe isn't
         * connected.
         */
        inline void ReleaseRequest();

        /**
         * From MWlmPlatformCallback
         * GetCurrentState requests the current connection state.
         * @since Series 60 3.0
         * @return connection state
         */
        virtual TWlanConnectionState GetCurrentState();

        /**
         * From MWlmPlatformCallback 
         * Enable WLAN functionality again
         * (after it has been disabled by a system mode change)
         * @since Series 60 3.0
         */
        inline void EnableWlan();

        /**
         * From MWlmPlatformCallback 
         * BT connection has been established.
         */
        void BtConnectionEstablished();
        
        /**
         * From MWlmPlatformCallback 
         * BT connection has been disconnected.        
         */
        void BtConnectionDisabled();

        /**
         * From MWlmPlatformCallback 
         * System startup has been completed.
         */
        void StartupComplete();

        /**
         * From MWlmPlatformCallback
         * Emergency call was done during startup and now it has completed.
         * @param aStartupCompleted Is the startup already completed 
         */
        void EmergencyCallComplete( TBool aStartupCompleted );

        /**
         * From MWlmPlatformCallback
         * User has changed the time and therefore cached WLAN region is not
         * valid anymore.
         */
        void SystemTimeChanged();

        /**
         * From MWlmPlatformCallback
         * WLAN region cache is always cleared after defined time.
         */
        void ClearRegionCache();
        
        /**
         * From MWlanEapolCallbackInterface
         * Callback interface to partner.
         *
         * @since S60 v3.2
         * @param aData Pointer to the data to be sent.
         * @param aLength Length of the data to be sent.
         * @return Return value is specified in interface specification.
         */
        TInt SendData(
            const void * const aData, 
            const TInt aLength );
        
        /**
         * From MWlanScanResultProvider
         * Issue scan request.
         *
         * @since S60 v5.2
         * @param aMaxDelay Maximum amount of seconds the client is willing to wait for
         *                  the availability results. The valid range is from 0 to 1200
         *                  seconds.
         */
        void Scan( const TUint& aMaxDelay );

        /**
         * From MWlanScanResultProvider
         * Cancel pending scan request.
         *
         * @since S60 v5.2
         */        
        void CancelScan();

    private:

        /**
        * Read WLAN specific settings from Commdb.
        * @param settings Settings are stored here.
        */
        void GetWlanSettingsL(
            CWlanDeviceSettings::SWlanDeviceSettings& aSettings );

        /**
         * Take the cached WLAN region into use if it is still valid.
         * @param region Cached WLAN region from CenRep.
         * @param timestamp Timestamp for the cached WLAN region in CenRep.
         */
         void SetCachedRegion(
             TUint32 region,
             TInt32 timestamp);
        
        /**
        * C++ default constructor.
        */
        CWlmServer();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
         * Method called by the framework when a pending core
         * request needs to be completed.
         *
         * @param aThisPtr Pointer to the server instance.
         * @return error code
         */       
        static TInt HandleCoreAsynchCb( TAny* aThisPtr );

        /**
         * Method called by the scan scheduling timer when it expires.
         * @param aThisPtr Pointer to the server instance.
         * @return error code
         */       
        static TInt ScanSchedulingTimerExpired( TAny* aThisPtr );

        /**
         * Method called by the scan scheduling timer when it expires.
         * @param aThisPtr Pointer to the server instance.
         * @return error code
         */
        static TInt ScanSchedulingTimerCanceled( TAny* aThisPtr );

        /**
         * A callback method that does absolutely nothing.
         * @param aThisPtr Pointer to the server instance.
         * @return error code
         */
        static TInt EmptyCb( TAny* aThisPtr );

        /**
         * Handles the conversion of IAP data list.
         * @param aCoreIapDataList Converted IAP data list.
         * @param aCoreIapSsidList Converted secondary SSID list.
         * @param aAmIapDataList IAP data list to be converted.
         * @return KErrNone if success, an error otherwise.
         */       
        TInt GetIapDataList(
            core_type_list_c<core_iap_data_s>& aCoreIapDataList,
            core_type_list_c<core_ssid_entry_s>& aCoreIapSsidList,
            const RArray<TWlanLimitedIapData>& aAmIapDataList );

        /**
         * Handles reading of lanservice table data related to service id.
         * @param aLanServiceId specifying the lan service
         * @param aLanSettings contains the results on completion
         * @return error code
         */       
        TInt GetLanSettings(
            TUint32 aLanServiceId, 
            SLanSettings& aLanSettings );

        /**
         * Update WLAN settings from commsdat and CenRep.
         *
         * @return KErrNone if success, an error code otherwise.
         */
        TInt UpdateWlanSettings();
        
        /**
         * Updates the background scan timing.
         * @param aNewInterval specifies the new background scan interval in seconds
         *        0 means background scan is off
         * @param aNextExpiration specifies when the background scan should be performed next time.
         */
        void UpdateBackgroundScan( 
            TUint32 aNewInterval,
            TUint32 aNextExpiration );

        /**
         * Updates the scan scheduling timer.
         * @param aScanTime specifies when the scan should be started in system ticks.
         * @param aTriggeringRequestId the id of that request which updates the timer.
         */        
        void UpdateScanSchedulingTimer( 
        	TUint aScantime,
        	TUint aTriggeringRequestId );

        /**
         * Finds the closest scan starting time from the pending scan requests in iPendingScanRequestMap.
         * @param aTriggeringRequestIndex index of the scan request that should be handled next.
         * @return if closest time was found.
         */
        TBool FindNextTimedScanSchedulingRequest( 
        	TUint& aTriggeringRequestIndex );        

        /**
         * Notify the server that a background scan has been done.
         */
        void NotifyBackgroundScanDone( 
            ScanList* aScanList, 
            core_type_list_c<core_iap_availability_data_s>& aIapAvailabilityData );

        /**
         * Check if this request can be completed.
         * @param aCheckedMapEntry pointer to the request to be checked
         * @param aCompletedMapEntry pointer to the request that was completed
         * @return whether the request can be completed by core.
         */
        TBool CanRequestBeCompleted(
            const SRequestMapEntry& aCheckedMapEntry,
            const SRequestMapEntry& aCompletedMapEntry ) const;
        
        /**
         * Handles completion routines of an internal request
         * @param aRequest request that should be completed.
         * @param aStatus status of the completed operation.
         * @param aCompletedWasTriggering value ETrue means that
         *        completed request is the same as the request which
         *        triggered the core operation 
         */
        void CompleteInternalRequest(
            const SRequestMapEntry& aRequest,
            core_error_e aStatus,
            TBool aCompletedWasTriggering = ETrue );

        /**
         * Handles completion routines of an external request
         * @param aRequest request that should be completed.
         * @param aStatus status of the completed operation.
         * @param aTriggerRequest pointer to the request that triggered core operation
         *        value NULL means that completed request is the same as the request which triggered 
         *        the core operation 
         */
        void CompleteExternalRequest(
            const SRequestMapEntry& aRequest,
            core_error_e aStatus,
            SRequestMapEntry* aTriggerRequest = NULL );

        /**
         * Compare two available network entries.
         * 
         * @param aFirst First entry to compare.
         * @param aSecond Second entry to compare.
         * @return ETrue if equal, EFalse otherwise.
         */
        static TBool IsAvailableNetworkEqual(
            const TWlanAvailableNetwork& aFirst,
            const TWlanAvailableNetwork& aSecond );

        /**
         * Get list of networks in the scan results.
         *
         * @param aScanList Scan results to be parsed.
         * @param aNetworkList List of networks is stored here.
         * @return Symbian error code.
         */
        TInt GetNetworkList(
            const ScanList& aScanList,
            RArray<TWlanAvailableNetwork>& aNetworkList );

        /**
         * Send availability notification updates and update the icon
         * if necessary.
         *
         * @param aNewNetworksDetected Whether new networks/IAPs have been detected.
         * @param aOldNetworksLost Whether old networks/IAPs have been lost.
         * @param aAnyNetworksDetected Whether any networks have been detected.
         */
        void UpdateAvailabilityInfo(
            TBool aNewNetworksDetected,
            TBool aOldNetworksLost,
            TBool aAnyNetworksDetected );

        /**
         * Cancel a pending request.
         *
         * @param aSessionId ID of the session.
         * @param aCommand Command to cancel.
         */
        void CancelExternalRequest(
            TUint aSessionId,
            TWLMCommands aCommand );

        /**
         * Check the given scan scheduling parameters and if the values are out of range
         * set new values. If aCacheLifetime value is -1, read the value from device settings.
         * @param aCacheLifetime how old cached results (in seconds) the client is willing to accept.
         * @param aMaxDelay maximum time (in seconds) the client is willing to wait for the scan results.
         */
        void CheckScanSchedulingParameters(
            TInt& aCacheLifetime,
            TUint& aMaxDelay );
        
        /**
         * Adds a background scan request to the scan scheduling queue.
         * @param aScanStartInterval when the background scan request should be handled.
         * @return was the request successfully made.
         */
        TInt BackgroundScanRequest(
        	TUint aScanStartInterval );
        
        /**
         * Check whether the phone is currently roaming.
         *
         * @return ETrue if the phone is currently roaming, EFalse otherwise.
         */
        inline TBool IsRoaming() const;

        /**
         * Check whether the given session is active.
         *
         * @param aEntry Session to be checked.
         * @return ETrue if session is active, EFalse if it has been closed.
         */
        inline TBool IsSessionActive(
            const SRequestMapEntry& aEntry ) const;

        /**
         * Calculate when the scanning should be started (= current moment in time + aDelay).
         * @param aDelay Number of seconds to add to current moment in time.
         * @return Calculated time.
         */
        inline TUint CalculateScanStartTime(
            const TInt aDelay ) const;

        /**
         * Find the index of the entry that is related to aRequestId, returns count of entries
         * in table if entry is not found.
         * @param aRequestId request id that is searched
         * @return TUint index to the iRequestMap entry that is related to given parameter
         */
        inline TUint FindRequestIndex(
            const TInt aRequestId ) const;

        /**
         * Check if the given request is pending scan request and part of scan scheduling
         * @param aIndex index to iRequestMap
         * @return TBool Whether request is a pending scan request
         */
        inline TBool IsPendingScanRequest(
            const TInt aIndex ) const;
        
        /**
         * Check if the given request is pending timed scan request and part of scan scheduling
         * @param aIndex index to iRequestMap
         * @return TBool Whether request is a pending scan request
         */
        inline TBool IsPendingTimedScanRequest(
        	const TInt aIndex ) const;
        
        /**
         * Check if the given request is first pending scan request and part of scan scheduling
         * @param aMapEntry entry in iRequestMap
         * @return TBool Whether request is a pending scan request
         */
        TBool IsOnlyTimedScanRequestInRequestMap(
        	const SRequestMapEntry& aMapEntry ) const;
        
        /**
         * Check if there are any SSID lists defined and update availability
         * results as needed.
         *
         * @param aMapEntry entry in iRequestMap 
         */
        void HandleSsidListAvailabilityL(
            const SRequestMapEntry& aMapEntry );
        
        /**
         * Store a key value to WLAN Central Repository.
         *
         * @param aKey Id of the WLAN Central Repository key.
         * @param aValue Value to be written to the WLAN Central Repository key.
         */
        void StoreWlanCenRepKeyValueL( const TUint32 aKey, const TInt aValue ) const;
        
        /**
         * Get a current IAP ID from Db.
         *
         * @param aLanServiceId Specifies the table row in wlansettings.
         * @param aCoreIapData Specifies the IAP data structure to be 
         * filled (i.e. IAP ID field to be filled for the structure).
         */
        TInt GetCurrentIapId( 
            const TUint aLanServiceId, 
            core_iap_data_s& aCoreIapData );
        
        /**
         * Store region and timestamp to CenRep.
         *
         * @param aRegion Specifies a region to be stored.
         * @param aTimestamp Specifies a timestamp to be stored.
         */
        void StoreRegionAndTimestamp( const TInt aRegion, const TInt aTimestamp );

        /**
         * Publish WLAN background scan interval.
         * 
         * @see MWlanScanResultProvider
         * @param aInterval WLAN BG scan interval in seconds.
         */
        void PublishBgScanInterval( TUint32& aInterval );

        /**
         * Callback method for indicating WLAN Server that WLAN has been
         * set ON.
         *
         * @see MWlmPlatformCallback
         */
         void WlanOn();
         
         /**
          * Callback method for indicating WLAN Server that WLAN has been
          * set OFF.
          *
          * @see MWlmPlatformCallback
          */
         void WlanOff();
         
         /**
          * Method for cancelling external requests by type.
          * 
          * @param aCommand Commands that need to be cancelled
          */
         void CancelExternalRequestsByType(
             const TWLMCommands aCommand );
         
         /**
          * Method for sending notifications to the subscribers
          * 
          * @param amNotification Notification to be sent.
          * @param aParams Notification parameters.
          */
         void SendNotification(
             TWlmNotify amNotification,
             TBuf8<KMaxNotificationLength>& aParams );

    private:    // Data
    
        /**
         * Pointer to core server
         */
        abs_core_server_c* iCoreServer;

        /**
         * Pointer to lower adaptation
         * NOTE: This should be just passed on to core server,
         * and let it handle communication towards this.
         * (i.e. upper and lower adaptation layers should not interact directly)
         */
        CWlmDriverIf* iDriverIf;

        /**
         * Bitmap of supported WLAN features.
         */
        TUint iSupportedFeatures;

        /**
         * Timer creating periodic expirations.
         * Used for starting scheduled scans
         */
        CWlanTickTimer* iScanSchedulingTimer;

        /**
         * Cache for scanresults etc.
         */
        CWlanScanResultCache* iCache;
        
        /**
         * Cache lifetime for scan results. This value is read from device settings.
         */
        TUint iConfiguredCacheLifetime;
         
        /** 
         * Handle to platform client that observes and updates system statuses
         */
        CWlmPlatform* iPlatform;
        
        /** 
         * List of clients who have subscribed notifications
         */
        RArray<CNotificationBase*> iNotificationArray;

        /**
         * Adaptation caches the connection state
         */
        TWlanConnectionState iConnectionState;

        /**
         * The previous connection state.
         */
        TWlanConnectionState iPrevConnectionState;

        /**
         * Whether the phone is currently roaming.
         */
        TBool iIsRoaming;
        
        /**
         * Adaptation caches the RCPI value.
         */        
        TUint iPrevRcpiValue;

        /**
         * Current regulatory domain
         */
        TWlanRegion iRegion;

        /**
         * Time of the last domain query
         */
        TTime iTimeofDomainQuery;

        /** 
         * Number of open client sessions
         */
        TUint iClientSessionCount;
        
        /**
         * RequestMap is an array containing pending service requests
         */
        RArray<SRequestMapEntry> iRequestMap;
    
        /** 
         * requestId counter ensures that consecutive service requests get
         * a unique identifier.
         */
        TUint32 iRequestIdCounter;

        /**
         * iSessionIdCounter is running counter for assigning an unique ID number
         * for new a session.
         */
        TUint32 iSessionIdCounter;

        /**
         * Asynchronous callback for completing core server requests.
         */        
        CAsyncCallBack* iCoreAsynchCb;
        
        /**
         * Request ID of the pending core server request.
         */
        TUint iCoreAsynchCbId;
        
        /**
         * Status of the pending core server request.
         */
        core_error_e iCoreAsynchCbStatus;

        /**
         * Whether system startup has been completed.
         */
        TBool iIsStartupComplete;        
        
        /**
         * EAPOL implementation instantiated via ECom plugin interface.
         */        
        CWlanEapolClient* iEapolClient;
        
        /**
         * EAPOL callback handler in core. Not owned by this pointer.
         */        
        abs_wlan_eapol_callback_c* iEapolHandler;

        /** 
         * The amount of microseconds per a system tick.
         */
        TInt iSystemTickPeriod;

        /**
         * Time in system ticks when the scan scheduling timer is set to expire.
         */
        TUint iScanSchedulingTimerExpiration;

        /**
         * Request id to that request in the iRequestMap which has set the scan scheduling timer.
         */
        TUint iRequestTriggeringScanning;
        
        /**
         * This flag is set when scan request is sent to Core and reset when Core has completed the request.
         * Scan request can be either GetScanResults, GetAvailableIaps or BackgroundScan
         */
        TBool iCoreHandlingScanRequest;
        
        /**
         * Power save mode requested from power save plugin. (Automatic/Light/Deep/None)
         */
        TWlanPowerSave iPowerSaveMode;
        
        /**
         * Power save enabled selected by user. (True/False)
         */
        TBool iPowerSaveEnabled;
        
        /**
         * Handler for Generic WLAN Plugins.
         */
        CGenericWlanPlugin iGenericWlanPlugin;
        
        /**
         * Storage for SSID lists.
         */
        CWlanSsidListDb* iSsidListDb;
        
        /**
         * Background scan provider.
         */
        MWlanBgScanProvider* iBgScanProvider;

        /**
         * Settings for background scan provider.
         */
        MWlanBgScanProvider::TWlanBgScanSettings iBgScanProviderSettings;
        
        /**
         * Timer services.
         */
        CWlanTimerServices* iTimerServices;
        
        /**
         * Flag indicating whether aggressive scanning has to be performed
         * after link loss.
         */   
        TBool iAggressiveScanningAfterLinkLoss;

        /**
         * WLAN device settings. 
         */
        CWlanDeviceSettings::SWlanDeviceSettings iDeviceSettings;

        /**
         * List of "weak" IAP IDs. 
         */
        RArray<TUint> iIapWeakList;
        
        /**
         * IAP ID of the connection.
         */
        TUint iConnectionIapId;
    };
    

#include "wlmserver.inl"

#endif // WLMSERVER_H
            
// End of File
