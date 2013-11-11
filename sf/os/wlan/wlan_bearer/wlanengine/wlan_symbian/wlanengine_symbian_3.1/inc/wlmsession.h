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
* Description:  Session class is required by the Client/Server architecture
*
*/


#ifndef WLMSESSION_H
#define WLMSESSION_H

#include <e32base.h>
#include "wlmclientserver.h"

class CWlmServer;
class CSessionNotification;

/**
 * Server side counterpart for client's RWlmServer class.
 * Each client connection has its own session in server.
 *
 * @lib wlmserversrv.lib
 * @since Series 60 3.0
 */
NONSHARABLE_CLASS(CWlmSession) : public CSession2
    {

    struct TNotification
        {
        TInt id;
        TBuf8<KMaxNotificationLength> data;
        };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CWlmSession* NewL(
            CWlmServer& aServer );

        /**
        * Destructor.
        */
        virtual ~CWlmSession();

    public: // New functions
        
        /**
        * AddNotification
        * @since Series 60 3.0
        * @param ?arg1 ?description
        */
        void AddNotification(
            TInt aNotification,
            const TDesC8& aData );

    public: // Functions from base classes

        /**
        * From CSession2
        * ServiceL is called by client/server infrastructure,
        * and it takes care of one service request
        * @since Series 60 3.0
        * @param aMessage containing the service request
        */
        virtual void ServiceL(
            const RMessage2& aMessage );

    private:

        /**
        * C++ default constructor.
        */
        CWlmSession(
            CWlmServer& aServer );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
 
        /**
        * ?member_description.
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        void HandleNotification();

        /**
        * ?member_description.
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        void HandleOrderNotifications(
            const RMessage2& aMessage );
        
        /**
        * ?member_description.
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        void HandleCancelNotifications(
            const RMessage2& aMessage );

    private:    // Data
    
        /** ID of the session. */
        TUint iId;
    
        /** Handle to the WLAN server. */           
        CWlmServer& iWlmServer;
    
        /** List of pending (not sent) notifications. */
        RArray<TNotification> iPendingNotifications;
        
        /** The request from the client pending for a notification. */
        RMessagePtr2 iPendingNotificationRequest;

        /** Is there a pending request from the client. */
        TBool iIsNotificationRequestPending;

        /** Handle to remove notification. */
        CSessionNotification* iNotificationHandle;
    };

#endif // WLMSESSION_H
            
// End of File
