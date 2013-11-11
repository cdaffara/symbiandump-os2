/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Listens TCP port
*
*/



#ifndef CTCPPORTLISTENER_H
#define CTCPPORTLISTENER_H

//  INCLUDES
#include <e32base.h>
#include <es_sock.h>


// FORWARD DECLARATIONS
class MTCPPortListenerObserver;

// CLASS DECLARATION

/**
*  ActiveObject which listens a TCP port.
*/
NONSHARABLE_CLASS( CTCPPortListener ) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aPort TCP port to listen.
        * @param aObserver Pointer to observer.
        */
        static CTCPPortListener* NewL( TInt aPort,
                                       MTCPPortListenerObserver* aObserver );

        /**
        * Factory function.
        * @param aPort TCP port to listen.
        * @param aObserver Pointer to observer.
        */
        static CTCPPortListener* NewLC( TInt aPort,
                                        MTCPPortListenerObserver* aObserver );

        /**
        * Destructor.
        */
        ~CTCPPortListener();

    public: // New functions

        /**
        * Starts listening the port.
        */
        void IssueListen();

        /**
        * Retrieve current port number for this listener
        * @return Port number for this listening socket
        */
        TInt Port() const;

    public: // Functions from base classes

        /**
        * From CActive. Pending request has been completed.
        */
        void RunL();

        /**
        * From CActive. Pending request has been cancelled.
        */
        void DoCancel();

        /**
        * From CActive. RunL has leaved.
        */
        TInt RunError( TInt aError );

    private:

        /**
        * Default constructor.
        * @param aPort TCP port to listen.
        * @param aObserver Pointer to observer.
        */
        CTCPPortListener( TInt aPort,
                          MTCPPortListenerObserver* aObserver );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:    // Data
        MTCPPortListenerObserver* iObserver;
        TInt iPort;
        RSocketServ iSocketServer;
        RSocket iListeningSocket;
        RSocket* iAcceptedSocket;
    };

#endif      // CTCPPORTLISTENER_H

// End of File
