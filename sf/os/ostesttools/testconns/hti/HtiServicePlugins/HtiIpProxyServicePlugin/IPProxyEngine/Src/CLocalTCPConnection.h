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
* Description:  Local TCP connection for emulator testing
*
*/



#ifndef CLOCALTCPCONNECTION_H
#define CLOCALTCPCONNECTION_H

//  INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>


// DATA TYPES
enum TLocalTCPConnState
    {
    ELTCStateDisconnected = 0,
    ELTCStateConnecting,
    ELTCStateConnected,
    ELTCStateDisconnecting
    };


// FORWARD DECLARATIONS
class MLocalTCPConnectionObserver;

// CLASS DECLARATION

/**
*  Local TCP connection, which connects to local TCP port.
*
*/
NONSHARABLE_CLASS( CLocalTCPConnection ) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aObserver TCP connection observer.
        * @param aPort Port to connect to.
        * @return New CLocalTCPConnection instance.
        */
        static CLocalTCPConnection* NewL(
            MLocalTCPConnectionObserver* aObserver, TInt aPort );

        /**
        * Factory function.
        * @param aObserver TCP connection observer.
        * @param aPort Port to connect to.
        * @return New CLocalTCPConnection instance.
        */
        static CLocalTCPConnection* NewLC(
            MLocalTCPConnectionObserver* aObserver, TInt aPort );

        /**
        * Destructor.
        */
        ~CLocalTCPConnection();

    public: // New functions

        void IssueConnectL();
        void IssueDisconnect();
        void SetObserver( MLocalTCPConnectionObserver* aObserver );
        TBool IsConnected();
        RSocket* Socket();
        void SetSocketOwnership( TBool aOwns );

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

    public: // New methods

        /**
        * Returns the port number of this connection
        * @return Port number
        */
        TInt Port();

    private:

        /**
        * Default constructor.
        */
        CLocalTCPConnection( MLocalTCPConnectionObserver* aObserver,
                             TInt aPort );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:    // Data
        MLocalTCPConnectionObserver* iObserver;
        RSocketServ iSocketServer;
        RSocket* iSocket;
        TInetAddr iAddr;
        TLocalTCPConnState iState;
        TBool iOwnsSocket;
    };

#endif      // CLOCALTCPCONNECTION_H

// End of File
