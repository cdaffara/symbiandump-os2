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
* Description:  General purpose socket implementetion
*
*/



#ifndef CSOCKET_H
#define CSOCKET_H

//  INCLUDES
#include <e32base.h>
#include "MSocketWriterObserver.h"
#include "MSocketReaderObserver.h"
#include "MSocket.h"

// FORWARD DECLARATIONS
class CSocketWriter;
class CSocketReader;

// CLASS DECLARATION

/**
*  General purpose socket implementetion.
*  With this class the client can write and receive data
*  to and from the opened RSocket object.
*/
NONSHARABLE_CLASS( CSocket ) : public CBase,
    public MSocketWriterObserver,
    public MSocketReaderObserver,
    public MSocket
    {
    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aSocket Opened RSocket instance that is used to read and write
        * data.
        * @param aUDPRemotePort Remote UDB port.

        */
        static CSocket* NewL( RSocket* aSocket, TInt aUDPRemotePort = -1 );

        /**
        * Factory function.
        * @param aSocket Opened RSocket instance that is used to read and write
        * data.
        * @param aUDPRemotePort Remote UDB port.
        */
        static CSocket* NewLC( RSocket* aSocket, TInt aUDPRemotePort = -1  );

        /**
        * Destructor.
        */
        virtual ~CSocket();

    public: // New functions

        /**
        * @return RSocket object
        */
        RSocket* GetRSocket();

        /**
        * @return Local TCP port.
        */
        TUint LocalPort() const;

        /**
        * @return Remote TCP port.
        */
        TUint RemotePort() const;

        /**
        * Sets observer.
        * @param aObserver Pointer to observer.
        */
        void SetObserver( MSocketObserver* aObserver );

        /**
        * Sets the ownership mode of RSocket object. If set ETrue
        * the socket will be closed and RSocket object will be deleted
        * in the destruction.
        * @param aHasOwnership Ownership mode.
        */
        void SetSocketOwnershipMode( TBool aHasOwnership );

        /**
        * Starts receiving data.
        */
        void IssueRead();

        /**
        * Cancels all pending requests.
        */
        void Cancel();

        /*
        * Returns the protocol information of the socket
        */
        void SocketInfo( TProtocolDesc& aDesc) const;

        /*
        * Is this a UDP socket
        */
        TBool IsUDP() const;

    public: // From MSocketWriter
        /**
        * Issues writing data to socket.
        * @param aData Data to be written.
        */
        void WriteL( const TDesC8& aData );

    protected:  // From MSocketReaderObserver
        /**
        * From MSocketReaderObserver.
        * Data has been received.
        * @param aData Received data.
        */
        void DataReceivedL( const TDesC8& aData );

        /**
        * From MSocketReaderObserver.
        * Notifies that error has occurred.
        * @param aErrorCode Error code.
        */
        void ReaderErrorL( TInt aErrorCode );

        /**
        * From MSocketWriterObserver.
        * Notifies that error has occurred.
        * @param aErrorCode Error code.
        */
        void WriterErrorL( TInt aErrorCode );

        /**
        * From MSocketWriterObserver.
        * Notifies that all data has succesfully been written.
        */
        void BufferUnderrunL();

        /**
        * From MSocketReaderObserver and MSocketWriterObserver.
        * Notifies that the observer has leaved while executing the callback
        * function.
        * @param aLeaveCode Leave code.
        */
        void ObserverLeaved( TInt aLeaveCode );


    private:

        /**
        * Default constructor.
        */
        CSocket( RSocket* aSocket, TInt aUDPRemotePort = -1  );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:    // Data
         RSocket* iSocket;
         CSocketWriter* iSocketWriter;
         CSocketReader* iSocketReader;
         MSocketObserver* iObserver;
         TBool iHasOwnership;
         const TInt iUDPRemotePort;
    };

#endif      // CSOCKET_H

// End of File
