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
* Description:  Client side interface to HtiBtCommServer
*
*/


#ifndef __HTIBTCOMMINTERFACE_H__
#define __HTIBTCOMMINTERFACE_H__

//  INCLUDES
#include <e32test.h>
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Remote interface to HtiBtCommServer.
 * HtiBtCommServer runs in separate thread, but always within the
 * client process.
 *
 * The thread is started and session to server created, when the
 * Connect is called for the first time.
 *
 * Server allows only one connection (session) at a time.
 *
 * Library: htibtcomminterface.lib
 */
class RHtiBtCommInterface : public RSessionBase
    {
    public: // Constructor and destructor

        /**
        * C++ default constructor.
        */
        IMPORT_C RHtiBtCommInterface();

    public:
        /**
        * Connects and creates session to BtCommServer. Additionally,
        * connects to remote BT device. This may take a while
        * and may require user interaction (device selection).
        *
        * If the server is not running within this process, it is
        * started in a new thread. Note that server allows only one
        * connection (session).
        * @return Error code
        */
        IMPORT_C TInt Connect( TDesC8& aDeviceNameOrAddress, TInt aPort );

        /**
        * Close the session to server and wait for servers death.
        * Will disconnect the server from remote BT host, if connected.
        */
        IMPORT_C void Close();

        /**
        * Returns server version
        * @return Version
        */
        IMPORT_C TVersion Version() const;

        /**
         *  Return required buffer max size for Send operation.
         */
        IMPORT_C TInt GetSendBufferSize() const;

        /**
         *  Return required buffer max size for Receive operation.
         */
        IMPORT_C TInt GetReceiveBufferSize() const;

        /**
        * Receives data from BtCommServer.
        * Note: The aData buffer size must equal to GetReceiveBufferSize().
        * (server will write 1 to GetReceiveBufferSize bytes to the buffer).
        * @param aData Descriptor where data is received
        * @param aStatus Asynchronous request status
        */
        IMPORT_C void Receive( TDes8& aData,
                           TRequestStatus& aStatus );

        /**
        * Sends data to BtCommServer.
        * Note: The aData buffer size must equal to GetSendBufferSize().
        * (server will read as much as there is data in aData. There must
        * never be more data than GetSendBufferSize. This is important to notice,
        * if HBufC is used, because its actual max size can be greater than
        * initially suggested in its construction)
        *
        * @param aData Data to be sent
        * @param aStatus Asynchronous request status
        */
        IMPORT_C void Send( const TDesC8& aData,
                           TRequestStatus& aStatus );


        /**
        * Cancels pending Receive request on server.
        */
        IMPORT_C void CancelReceive();

        /**
        * Cancels pending Send request on server.
        */
        IMPORT_C void CancelSend();

        /**
        * Gets the port number of the remote service where the server is
        * connected.
        * @return Port number or KErrDisconnected if server is not connected
        */
         IMPORT_C TInt GetPortNumber() const;

    private:

        /**
        * During Connect(), connect the server
        * to remote BT host service.
        */
        TInt RHtiBtCommInterface::ConnectBt( TDesC8& aDeviceNameOrAddress,
                                             TInt aPort );

};

#endif // __HTIBTCOMMINTERFACE_H__

// End of File
