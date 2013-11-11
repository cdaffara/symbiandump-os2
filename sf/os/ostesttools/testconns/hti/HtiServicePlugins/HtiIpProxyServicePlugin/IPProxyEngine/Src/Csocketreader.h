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
* Description:  Socket reader
*
*/



#ifndef CSOCKETREADER_H
#define CSOCKETREADER_H

//  INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>


// CONSTANTS
const TInt KReadBufferSize = 8*1024;  //bytes

// FORWARD DECLARATIONS
class MSocketReaderObserver;
class RSocket;
class CDesC8Array;

// CLASS DECLARATION

/**
*  Socket reader. ActiveObject that reads data from opened RSocket.
*/
NONSHARABLE_CLASS( CSocketReader ) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aSocket Opened RSocket session from where data is read.
        * @param aUDPRemotePort Peer ends port.
        * @return New CSocketReader instance.
        */
        static CSocketReader* NewL( RSocket& aSocket,
            TInt aUDPRemotePort = -1 );

        /**
        * Factory function.
        * @param aSocket Opened RSocket session from where data is read.
        * @param aUDPRemotePort Peer ends port.
        * @return New CSocketReader instance.
        */
        static CSocketReader* NewLC( RSocket& aSocket,
            TInt aUDPRemotePort = -1 );

        /**
        * Destructor.
        */
        ~CSocketReader();

    public: // New functions

        /**
        * Sets observer.
        * @param aObserver Pointer to observer.
        */
        void SetObserver( MSocketReaderObserver* aObserver );

        /**
        * Starts reading the socket.
        */
        void Start();

    private:
        /**
        * Starts reading the socket.
        */
        void IssueRead();

    protected: // Functions from base classes

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
        */
        CSocketReader( RSocket& aSocket, TInt aUDPRemotePort = -1 );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:    // Data
        TSockXfrLength iReceivedDataLength;
        TBuf8<KReadBufferSize> iReadBuffer;
        MSocketReaderObserver* iObserver;
        RSocket& iSocket;
        const TInt iUDPRemotePort;
        TInetAddr iUDPRemoteAddr;
    };

#endif      // CSOCKETREADER_H

// End of File
