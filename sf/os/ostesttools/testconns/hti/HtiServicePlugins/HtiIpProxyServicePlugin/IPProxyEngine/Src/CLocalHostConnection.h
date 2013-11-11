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



#ifndef CLOCALHOSTCONNECTION_H
#define CLOCALHOSTCONNECTION_H

//  INCLUDES
#include <e32base.h>
#include <es_sock.h>
//#include "MSocket.h"
#include "MHostConnection.h"
#include <in_sock.h>

// DATA TYPES
enum TLocalConnState
    {
    ELCStateDisconnected = 0,
    ELCStateConnecting,
    ELCStateConnected,
    ELCStateDisconnecting
    };


// FORWARD DECLARATIONS
class MHostConnectionObserver;

// CLASS DECLARATION

/**
*  Local TCP connection for emulator testing.
*  Due to problematic testing of bluetooth connections in emulator
*  this class is used to simulate the connection to local TCP port instead.
*  There must be running TCP port listener when connecting with this class.
*/
NONSHARABLE_CLASS( CLocalHostConnection ) : public CActive,
    public MHostConnection
    {
    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aSocketServer Opened Socket server session
        * @param aPort Port to connect.
        * @return New CLocalHostConnection instance.
        */
        static CLocalHostConnection* NewL(
            RSocketServ& aSocketServer, TInt aPort );

        /**
        * Factory function.
        * @param aSocketServer Opened Socket server session
        * @param aPort Port to connect.
        * @return New CLocalHostConnection instance.
        */
        static CLocalHostConnection* NewLC(
            RSocketServ& aSocketServer, TInt aPort );

        /**
        * Destructor.
        */
        ~CLocalHostConnection();

    public: // Functions from MHostConnection

        /**
        * Starts connecting procedure to host.
        */
        void IssueConnectL();

        /**
        * Starts disconnection procedure.
        */
        void IssueDisconnect();

        /**
        * Sets observer.
        * @param aObserver Pointer to observer.
        */
        void SetObserver( MHostConnectionObserver* aObserver );

        /**
        * @return Connection state.
        */
        TBool IsConnected();

        /**
        * @return Pointer to current RSocket object. The ownership always
        * remains in this class.
        */
        RSocket* Socket();

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
        CLocalHostConnection( RSocketServ& aSocketServer, TInt aPort );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:    // Data
        MHostConnectionObserver* iObserver;
        RSocketServ& iSocketServer;
        RSocket iClientSocket;
        TInetAddr iAddr;
        TLocalConnState iState;
    };

#endif      // CLOCALHOSTCONNECTION_H

// End of File
