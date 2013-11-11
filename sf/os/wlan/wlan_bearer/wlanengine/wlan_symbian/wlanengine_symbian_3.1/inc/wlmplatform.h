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
* Description:  Contains the class for handling platform-specific functionality.
*
*/

/*
* %version: 13 %
*/

#ifndef WLMPLATFORM_H
#define WLMPLATFORM_H

#include "wlmclientserver.h"
#include "wlandevicesettings.h"
#include "wlmsystemnotify.h"
#include "wlansystemtimehandler.h"
#include "am_platform_libraries.h"

class CWlmPlatformData;

/**
 * Possible values for scan types.
 */
enum TWLMScanType
    {
    EWLMScanNotAllowed,
    EWLMScanForcedPassive,
    EWLMScanForcedActive,
    EWLMScanAsRequested
    };

/**
* Callback interface used by the platform-specific functionality.
* These callbacks are implemented by the engine.
* @lib wlmplatform.dll
* @since Series 60 3.0
*/
class MWlmPlatformCallback
    {
    public: // Methods

        /**
        * Get the current connection state.
        * @return Current driver state.
        */
        virtual TWlanConnectionState GetCurrentState() = 0;

        /**
        * Send an indication to request data pipe disconnection.
        *
        * Calls ReleaseComplete() directly if data pipe isn't
        * connected.
        */
        virtual void ReleaseRequest() = 0;

        /**
        * Enable WLAN after it has been disabled by a
        * system mode change.
        */
        virtual void EnableWlan() = 0;

        /**
        * BT connection has been established.
        */
        virtual void BtConnectionEstablished() = 0;

        /**
        * BT connection has been disconnected.        
        */
        virtual void BtConnectionDisabled() = 0;        

        /**
        * System startup has been completed.
        */
        virtual void StartupComplete() = 0;
        
        /**
        * Emergency call was done during startup and now it has completed.
        * @param aStartupCompleted Is the startup already completed 
        */
        virtual void EmergencyCallComplete( TBool aStartupCompleted ) = 0;

        /**
         * User has changed the time and therefore WLAN region cache information
         * is not valid anymore.
         */
        virtual void SystemTimeChanged() = 0;

        /**
         * WLAN region cache is always cleared after defined time.
         */
        virtual void ClearRegionCache() = 0;
        
        /**
         * WLAN has been set ON.
         */
        virtual void WlanOn() = 0;
        
        /**
         * WLAN has been set OFF.
         */
        virtual void WlanOff() = 0;
        
    };

/**
* Class for handling platform specific functionality.
* @lib wlmplatform.dll
* @since Series 60 3.0
*/
NONSHARABLE_CLASS( CWlmPlatform ) : public CBase, public MWlmSystemNotify, public MWlanSystemTimeCallback
    {
    public:  // Methods

       // Constructors and destructor
        
        /**
        * Static constructor.
        * @param aCallback Reference to the platform callback object.
        * @return Pointer to created object.
        */
        IMPORT_C static CWlmPlatform* NewL(
            MWlmPlatformCallback& aCallback );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CWlmPlatform();

       // Methods from base classes

        /**
        * Do all initilizations that could not
        * be done in construction phase because they require callbacks.
        */
        IMPORT_C void Initialize();

        /**
        * Get the type of scan allowed.
        * @return Type of scan allowed.
        */
        IMPORT_C TWLMScanType GetScanType();

        /**
        * Check whether WLAN is disabled.
        * @return Whether WLAN is disabled.
        */
        IMPORT_C TBool IsWlanDisabled();

        /**
        * Get the current system mode.
        * @return Current system mode.
        */
        IMPORT_C TWlanSystemMode GetCurrentSystemMode();

        /**
        * Set the status of WLAN icon on cover display.
        * @param aStatus The WLAN icon status.
        * @return A Symbian error code.
        */
        IMPORT_C TInt SetIconState( TWlmIconStatus aStatus );

        /**
        * Returns MCC of the current operator.
        * @param aCountryCode MCC.
        */
        IMPORT_C void GetCurrentOperatorMccL( TUint& aCountryCode );
        
        /**
        * Publishes the MAC address to other applications.
        * @param aMacAddr The MAC address.
        * @return A Symbian error code.
        */
        IMPORT_C TInt PublishMacAddress( TMacAddress& aMacAddr );

        /**
         * Start timer to get indication when user has changed the time.
         */
        IMPORT_C void InitializeSystemTimeHandler();

        /**
         * Publishes WLAN background scan interval.
         * @param aInterval WLAN BG scan interval in seconds.
         */
        IMPORT_C void PublishBgScanInterval( TUint32& aInterval );
        
        /**
         * Get current WLAN on/off state.
         * @return WLAN on/off state.
         */
        IMPORT_C TWlanOnOffState GetWlanOnOffState();

    protected:  // Methods

       // Methods from base classes

        /**
        * (From MWlmSystemNotify) System Mode has changed.
        * @param aOldMode Old System Mode.
        * @param aNewMode New System Mode.
        */
        void SystemModeChanged(
            TWlanSystemMode aOldMode,
            TWlanSystemMode aNewMode );

        /**
        * (From MWlmSystemNotify) Emergency call has started, WLAN is forced to be enabled.
        */
        void EmergencyCallEstablished();
        
        /** 
        * (From MWlmSystemNotify) Emergency call has ended, WLAN is not forced to be enabled anymore.
        * @param aNewMode New System Mode
        */
        void EmergencyCallCompleted( TWlanSystemMode aNewMode );
        
        /**
        * (From MWlmSystemNotify) BT connection has been established.
        */
        void BtConnectionEstablished();

        /**
        * (From MWlmSystemNotify) BT connection has been disconnected.        
        */
        void BtConnectionDisabled();
        
        /**
         * (From MWlmSystemNotify) WLAN has been set ON.
         */
        void WlanOn();
        
        /**
         * (From MWlmSystemNotify) WLAN has been set OFF.
         */
        void WlanOff();
        
        /**
         * (From MWlanSystemTimeCallback) 
         * User has changed the time and therefore cached WLAN region is not valid anymore.
         */
        void OnSystemTimeChange();

        /**
         * (From MWlanSystemTimeCallback) 
         * WLAN region cache is forced to be cleared after defined time.
         */
        void OnCacheClearTimerExpiration();
                
    private: //Methods

        /**
        * C++ default constructor.
        * @param aCallback Pointer the platform callback object.
        */
        CWlmPlatform( MWlmPlatformCallback& aCallback );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private:   // Data

        /** Interface for callbacks to common functionality. */
        MWlmPlatformCallback& iCallback;

        /** Current states of various system mode settings. */
        TWlanSystemMode iCurrentSystemMode;
        
        /** Interface to PubSub/CenRep. */
        CWlmPlatformData* iPlatformData;

        /** 
         * Information whether WLAN connection existed before emergency call was made. 
         * If WLAN connection did not exist before emergency call and WLAN connection was 
         * established in Offline mode for the emergency call, then after the emergency call
         * the WLAN connection must be released. */
        TBool iWlanConnectedBeforeEmergencyCall;

        /** Interface to monitor system time changes. */
        CWlanSystemTimeHandler* iSystemTimeHandler;

        /** Interface to monitor when region cache needs to be cleared. */
        CWlanPeriodicCacheClearingHandler* iPeriodicCacheClearingHandler;     
                
    };

#endif // WLMPLATFORM_H
