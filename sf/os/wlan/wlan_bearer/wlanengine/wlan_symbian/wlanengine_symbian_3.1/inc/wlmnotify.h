/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Notification service for clients
*
*/

/*
* %version: 13 %
*/

#ifndef WLMNOTIFY_H
#define WLMNOTIFY_H

#include "wlmclientserver.h"
#include "wlancontrolinterface.h"

class RWLMServer;

/**
 * MWLMNotify specifies a callback interface for notification services.
 * These virtual methods should be implemented by the client if it
 * needs any notifications.
 * @lib wlmserver.lib
 * @since 3.0
 */
class MWLMNotify
    {
    public:
        /**
         * Connection status has changed.
         *
         * @aParam aNewState specifies the new state
         */
        virtual void ConnectionStateChanged(
            TWlanConnectionState /*aNewState*/ ){};

        /**
         * BSSID has changed (i.e. AP handover).
         *
         * @aParam aNewBSSID specifies the new BSSID (access point name).
         */
        virtual void BSSIDChanged(
            TDesC8& /*aNewBSSID*/ ){};

        /**
         * Connection has lost.
         */
        virtual void BSSLost(){};

        /**
         * Connection has regained.
         */
        virtual void BSSRegained(){};

        /**
         * New networks have been detected during scan.
         */
        virtual void NewNetworksDetected(){};

        /**
         * One or more networks have been lost since the last scan.
         */
        virtual void OldNetworksLost(){};

        /**
         * The used transmit power has been changed.
         *
         * @param aPower The transmit power in mW.
         */
        virtual void TransmitPowerChanged(
            TUint /*aPower*/ ){};

        /**
         * Received signal strength has changed to another strength class.
         *
         * @param aRcpLevel specifies the rss class (normal/weak)
         * @param aRcpValue specifies the actual rcp value 
         */
        virtual void RcpChanged(
            TWlanRCPLevel /*aRcpLevel*/,
            TUint /*aRcpValue*/ ){};

        /**
         * Information about the current access point after a connect/roam.
         *
         * @param aInfo Information about the current access point.
         */
        virtual void AccessPointInfoChanged(
            const TWlanAccessPointInfo& /* aInfo */ ){};

        /**
         * The status of a virtual traffic stream has changed.
         *
         * @param aStreamId ID of the traffic stream.
         * @param aStreamStatus Status of the traffic stream.
         */
        virtual void TrafficStreamStatusChanged(
            TUint /* aStreamId */,
            TWlanTrafficStreamStatus /* aStreamStatus */ ) {};

        /**
         * The traffic mode of an access class has changed.
         *
         * @param aAccessClass Access class.
         * @param aMode Traffic mode of the access class.
         */
        virtual void AccessClassTrafficModeChanged(
            TWlmAccessClass /* aAccessClass */,
            TWlmAcTrafficMode /* aMode */ ){};

        /**
         * The traffic status of an access class has changed.
         *
         * @param aAccessClass Access class.
         * @param aStatus Traffic status of the access class.
         */
        virtual void AccessClassTrafficStatusChanged(
            TWlmAccessClass /* aAccessClass */,
            TWlmAcTrafficStatus /* aStatus */ ){};

    };

/**
* CWLMNotify offers the notification service.
* Active object that waits notifications from server.
* @lib wlmserver.lib
* @since 3.0
*/
NONSHARABLE_CLASS( CWLMNotify ) : public CActive
    {
    public: // Methods

        // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CWLMNotify();
        
        /**
        * Create and activate notification service.
        * @note This activates the notification service already.
        * @param aCallback Callback interface to forward notifications to
        *                  user process.
        * @param aServer   Interface to send requests to server.
        * @param aNotificationMask Bitmask of notifications to activate.
        * @return Pointer to a new constructed CWLMNotify object.
        */
        static CWLMNotify* NewL(
            MWLMNotify& aCallback,
            RWLMServer& aServer,
            TUint32 aNotificationMask );

        /**
        * Change the callback interface
        * and activate notifications
        * @param aCallback New callback interface.
        * @param aNotificationMask Bitmask of notifications to activate.
        */
        void Activate(
            MWLMNotify& aCallback,
            TUint32 aNotificationMask );
        
        /**
         * Sets the requests cancelled flag so that no new notification requests
         * are done once current notification is complete.
         */
        void SetCancelled();

    protected: // from CActive
        /**
        * (From CActive) Receive notification.
        */
        void RunL();
        
        /**
        * (From CActive) This is called by Cancel() of CActive framework.
        */
        void DoCancel();

    private:
        /** 
        * Constructor.
        * @param aCallback Callback interface to forward notifications to
        *                  application.
        * @param aServer   Interface to send requests to server.
        * @param aNotificationMask Bitmask of notifications to activate.
        */
        CWLMNotify(
            MWLMNotify& aCallback,
            RWLMServer& aServer,
            TUint32 aNotificationMask );

        /**
        * Second phase construction.
        */
        void ConstructL();
        
        /**
        * Notification query loop.
        */
        void WaitForNotifications();

    private:    // Members.
        /**
         * Callback interface to the client. Not owned by this class.
         */
        MWLMNotify* iCallback;
        /**
         * Interface to send messages to the server
         */
        RWLMServer& iServer;
        /**
         * Data package for return data
         */
        TPckgBuf<TWlmNotifyData> iDataPckg;
        /**
         * Cancel has been requested
         */
        TBool iCancelRequested;
        /**
         * Bitmask of active notifications.
         */
        TUint32 iNotificationMask;
   };

#endif // WLMNOTIFY_H
