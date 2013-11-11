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
* Description:  Bluetooth serial client.
*
*/


#ifndef __BTSERIALCLIENT_H__
#define __BTSERIALCLIENT_H__

// INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include <bluetooth/btpowercontrol.h>
#include <bt_sock.h>
#include <btextnotifiers.h>
#include <btsdp.h>

#include "SocketObserver.h"

// FORWARD DECLARATIONS
class CMessageServiceSearcher;
class CSocketsReader;
class CSocketsWriter;

class MBtSerialClientObserver
    {
public:
    virtual void ConnectedToServer(TInt aError) = 0;
    virtual void DisconnectedFromServer() = 0;
    virtual void DataFromServer(const TDesC8& aData) = 0;
    virtual void AllBufferedDataSent() = 0;
    };

/**
* CBtSerialClient
* Connects and sends messages to a remote machine using bluetooth
*/
class CBtSerialClient : public CActive,
                        public MSocketObserver
    {
    public: // Constructors and destructor

        /**
        * NewL()
        * Construct a CBtSerialClient
        * @param aObserver the observer for this BT serial client
        * @return a pointer to the created instance of CBtSerialClient
        */
        IMPORT_C static CBtSerialClient* NewL( MBtSerialClientObserver& aObserver );

        /**
        * ~CBtSerialClient()
        * Destroy the object and release all memory objects.
        * Close any open sockets.
        */
        IMPORT_C virtual ~CBtSerialClient();

    public:     // New functions

        /**
        * ConnectL()
        * Connect to remote device. Query user for a device.
        */
        IMPORT_C void ConnectL();

        /**
        * ConnectL()
        * Connect to remote device.
        */
        IMPORT_C void ConnectL(const TBTDevAddr aBTServerDevice, const TInt aPort );

        /**
        * ConnectL()
        * Connect to remote device by device name.
        */
        IMPORT_C void ConnectL( const TDesC& aBTServerDeviceName, const TInt aPort );

        /**
        * DisconnectL()
        * Disconnect from remote machine
        */
        IMPORT_C void Disconnect();

        /**
        * IsConnecting()
        * @return ETrue if the client is establishing a
        * connection to the server.
        */
        IMPORT_C TBool Connecting();

        /**
        * Connected()
        * @return ETrue if the client is fully connected to the server.
        */
        IMPORT_C TBool Connected();

        /**
        * ServerAddressL()
        * @return address of connected server. Leaves with KErrNotReady, if
        * not connected.
        */
        TBTDevAddr ServerAddressL();

        /**
        * Add data to outgoing buffer and start sending it to client.
        *
        * Leaves with KErrOverflow, if the outgoing buffer cannot
        * be added all of the aData.
        * Leaves with KErrNotReady, if client is not connected.
        * When all data in internal buffer has been sent, observer
        * is notified (AllBufferedDataSent)
        */
        IMPORT_C void SendL(const TDesC8& aData);

        /**
        * Issue read operation. Will complete asyncronously.
        *
        * Leaves with KErrNotReady, if client is not connected.
        * Notifies observer, when some data has been read. Caller
        * is responsible for calling this method again to receive
        * more data.
        */
        IMPORT_C void ReadAsyncL();

        /**
        * Query free size of outgoing buffer.
        */
        IMPORT_C TInt FreeSpaceInSendBuffer();

        /**
        * Query max size of outgoing buffer.
        */
        IMPORT_C TInt SendBufferMaxSize();

        /**
        * Query the port of the service we are connected with
        * @return Port number or KErrDisconnected if not connected to a service
        */
        IMPORT_C TInt ServicePort();

    protected:    // from CActive

        /**
        * DoCancel()
        * Cancel any outstanding requests
        */
        void DoCancel();

        /**
        * RunL()
        * Respond to an event
        */
        void RunL();

    protected: // from MSocketObserver

        void ReportError( TErrorType aErrorType, TInt aErrorCode );
        void NewData( const TDesC8& aData );
        void AllBufferedDataSent();

    private:

        /**
        * ConnectToServerL
        * Connects to the service
        */
        void ConnectToServerL();

        /**
        * DisconnectFromServer()
        * Disconnects from the service
        */
        void DisconnectFromServer();

        /**
        * CBtSerialClient()
        * Constructs this object
        */
        CBtSerialClient( MBtSerialClientObserver& aObserver );

        /**
        * ConstructL()
        * Performs second phase construction of this object
        */
        void ConstructL();

    private:    // data

        /**
        * TState
        * The state of the active object, determines behaviour within
        * the RunL method.
        * EWaitingToGetDevice waiting for the user to select a device
        * EGettingDevice searching for a device
        * EGettingService searching for a service
        * EGettingConnection connecting to a service on a remote machine
        * EConnected connected to a service on a remote machine
        */
        enum TState
            {
            EWaitingToGetDevice,      // phase 1 of Connect
            EGettingDevice,           // phase 2 of Connect
            EGettingService,          // phase 3 of Connect
            EGettingConnection,       // phase 4 of Connect
            EConnected, // sending and receiving data
            EDisconnecting
            };

        MBtSerialClientObserver& iObserver;

        /** iState the current state of the client */
        TState iState;

        /**
        * iServiceSearcher searches for service this
        * client can connect to.
        * Owned by CBtSerialClient
        */
        CMessageServiceSearcher* iServiceSearcher;

        /**
        * iCurrentServiceIndex the index number of the service we are
        * currently connecting/connected to
        */
        TInt iCurrentServiceIndex;

        /** iSocketServer a connection to the socket server */
        RSocketServ iSocketServer;

        /** iSocket a socket to connect with */
        RSocket iSocket;

        /** iServiceClass the service class UUID to search for */
        TUUID iServiceClass;

        TBTDevAddr iBTServerDevice;

        CSocketsReader* iSocketReader;
        CSocketsWriter* iSocketWriter;

        RBTPowerControl iPowerControl;
    };

#endif // __BTSERIALCLIENT_H__

// End of File
