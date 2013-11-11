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
* Description:  Interface class to CenRep and PubSub
*
*/

/*
* %version: 14 %
*/

#ifndef WLMPLATFORMDATA_H
#define WLMPLATFORMDATA_H

#include <e32property.h>
#include "802dot11.h"
#include "wlmsystemnotify.h"
#include "wlmplatformsubscriber.h"
#include "wlaninternalpskeys.h"

/**
* Interface class to PubSub and CenRep.
*/
NONSHARABLE_CLASS( CWlmPlatformData ) :
    public CBase, 
    public MWlmPlatformResponder
    {
    public: // Methods

       // Constructors and destructor

        /**
        * Static constructor.
        * @param aCallback Callback for notifications..
        * @return Pointer to create object.
        */
        static CWlmPlatformData* NewL( MWlmSystemNotify& aCallback );

        /**
        * Destructor.
        */
        virtual ~CWlmPlatformData();

       // Methods from base classses

        /**
        * Sets the status of WLAN icon on cover display.
        * @param aStatus The WLAN icon status.
        * @return A Symbian error code.
        */
        virtual TInt SetIconState(
            TWlmIconStatus aStatus );

        /**
        * Publishes the MAC address to other applications.
        * @param aMacAddr The MAC address.
        * @return A Symbian error code.
        */
        virtual TInt PublishMacAddress(
            TMacAddress& aMacAddr );

        /**
        * Returns MCC of the current operator.
        * @param aCountryCode MCC.
        */
        virtual void GetCurrentOperatorMccL(
            TUint& aCountryCode );

        /**
        * Forces the system to re-read all system statuses and
        * indicate all changed statuses via notification callbacks.
        */
        virtual void UpdateSystemStatuses();

        /**
        * From MWlmPlatformResponder The subscribed property has changed.
        * @since Series 60 3.0
        * @param aCategory Category of the changed property.
        * @param aKey Key of the changed property.
        */
        virtual void HandlePropertyChangedL(
            const TUid& aCategory,
            const TUint aKey );

        /**
         * Publishes WLAN background scanning interval via P&S.
         * @since S60 v5.2
         * @param aInterval WLAN BG scan interval in seconds.
         * @return Symbian error code
         */
        TInt PublishBgScanInterval(
            TUint32& aInterval );
            
        /**
         * Method for reading WLAN on/off state.
         * @return WLAN on/off state
         */
        TWlanOnOffState GetWlanOnOffState();

    private:   // Methods

        /**
        * C++ default constructor.
        * @param aCallback Callback for notifications..
        */
        CWlmPlatformData( MWlmSystemNotify& aCallback );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();
        
        /**
         * Notifies WLAN on/off observer of the changes
         */
        void NotifyWlanOnOffObserver();
        
        /**
         * Method for publishing WLAN on/off state.
         * @param aWlanState WLAN state to be published.
         */
        void PublishWlanOnOff( TPSWlanOnOff aWlanState );

    private:  // Definitions
        
        /**
         * Last WLAN on/off state notified to the observer.
         */
        enum TWlanNotifiedState
            {
            EWlanNotifiedNone,  // Initial value, Observer not yet notified anything
            EWlanNotifiedOff,   // Observer notified that WLAN is set OFF
            EWlanNotifiedOn     // Observer notified that WLAN is set ON
            };

    private:  // Data
        
        /** Subscriber for watching KPSGlobalSystemState via P&S. */
        CWlmPlatformSubscriber* iPropertySystemState;

        /** Subscriber for watching KPropertyKeyBluetoothGetPHYCount via P&S. */
        CWlmPlatformSubscriber* iBtConnections;

        /** Subscriber for watching KCTSYEmergencyCallInfo via P&S. */
        CWlmPlatformSubscriber* iEmergencyCall;

        /** Subscriber for watching WLAN master switch via CenRep. */
        CWlmPlatformSubscriber* iWlanOnOff;
        
        /** Subscriber for watching WLAN force disable switch via CenRep. */
        CWlmPlatformSubscriber* iWlanForceDisable;

        /** Handle to KPropertyWlanMacAddress property via P&S. */
        RProperty iPsMacAddress;
        
        /** Handle to KPropertyWlanIndicator property via P&S. */
        RProperty iPsIndicator;
        
        /** Handle to KPropertyWlanBgScanInterval property via P&S. */
        RProperty iPsBgScanInterval;

        /** Handle to KPropertyWlanOnOffState property via P&S. */
        RProperty iPsOnOffState;

        /** Callback for notifications. */
        MWlmSystemNotify& iCallback;

        /** Stores the current System Mode */
        TWlanSystemMode iSystemMode;

        /** Stores the number of BT connections. */
        TUint iBtConnectionCount;

        /** The currently shown icon */
        TWlmIconStatus iCurrentIcon;

        /** Whether system startup has been completed. */
        TBool iIsStartupComplete;

        /** Whether system is in offline. */
        TBool iIsInOffline;

        /** Whether Emergency Call is active. */
        TBool iIsEmergencyCall;
        
        /** Last WLAN on/off state notified to the observer. */
        TWlanNotifiedState iNotifiedWlanState;
                
    };

#endif // WLMPLATFORMDATA_H
