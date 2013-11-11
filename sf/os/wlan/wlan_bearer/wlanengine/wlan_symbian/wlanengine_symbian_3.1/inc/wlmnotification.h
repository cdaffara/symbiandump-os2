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
* Description:  notification class
*
*/


#ifndef WLMNOTIFICATION_H
#define WLMNOTIFICATION_H

// INCLUDES
#include "wlmsession.h"
#include "am_platform_libraries.h"

class CWlmSession;

/**
* Base class for all notification types.
*
* Basically this only determines the interface how Server is forwarding 
* notifications to Sessions or WAL.
*
* @lib wlmserversrv.lib
* @since Series 60 3.0
*/
class CNotificationBase : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CNotificationBase() {};

       // New methods

    public: // New functions

        /**
         * AddNotification
         * @since Series 60 3.0
         * @param aNotification identifier
         * @param aData content of the notification
         */
        virtual void AddNotification(
            TUint aNotification,
            TDes8& aData ) = 0;            
    };

/**
* Forwards notifications to a session.
*
* @lib wlmserversrv.dll
* @since Series 60 3.0
*/
NONSHARABLE_CLASS( CSessionNotification ) : public CNotificationBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * 
        * @param aSession Handle to the session.
        * @param aNotificationMask Notifications enabled for the session.
        * @return Pointer to the created instance, NULL if unable to create an instance.
        */
        static CSessionNotification* NewL(
            CWlmSession& aSession,
            TUint32 aNotificationMask );

        /**
        * Destructor.
        */
        virtual ~CSessionNotification();

    public: // Functions from base classes

        /**
         * AddNotification
         * @since Series 60 3.0
         * @param aNotification identifier
         * @param aData content of the notification
         */
        void AddNotification(
            TUint aNotification,
            TDes8& aData );

    private: // Methods

        /**
        * C++ default constructor.
        * 
        * @param aSession Handle to the session.
        * @param aNotificationMask Notifications enabled for the session.
        */
        CSessionNotification(
            CWlmSession& aSession,
            TUint32 aNotificationMask );

    private:    // Data

        /** Interface to session to forward notifications. */
        CWlmSession& iSession;

        /** Bitmask of active notifications. */
        TUint32 iNotificationMask;
    };

#endif // WLMNOTIFICATION_H
            
// End of File
