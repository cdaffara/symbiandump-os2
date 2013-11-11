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
* Description:  Server side session in HtiBtCommServer.
*
*/


#ifndef CHTIBTCOMMSERVERSESSION_H
#define CHTIBTCOMMSERVERSESSION_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "BtSerialClient.h" // MBtSerialClientObserver

// FORWARD DECLARATIONS

class CHtiBtCommServer;

// CLASS DECLARATION

/**
* This class represents a client session in the server.
*
* Server owns one instance of session (meaning only one client session
* is allowed for this server).
* Session allows only one outstanding read request and write request at a time.
* Read request and write request may be outstanding simultaneously.
*/
class CHtiBtCommServerSession :
    public CSession2,
    public MBtSerialClientObserver
    {
public:

   /**
    * Two phase constructor.
    *
    * @param aServer Pointer to the server instance
    */
    static CHtiBtCommServerSession* NewL( CHtiBtCommServer* aServer );

private: // Construction

   /**
    * C++ default constructor.
    *
    * @param aClient
    * @param aServer
    */
    CHtiBtCommServerSession( CHtiBtCommServer* aServer );

    /**
     * Second phase construction.
     */
    void ConstructL();

   /**
    * Destructor.
    */
    ~CHtiBtCommServerSession();

private: // From MBtSerialClientObserver

    void ConnectedToServer(TInt aError);
    void DisconnectedFromServer();
    void DataFromServer(const TDesC8& aData);
    void AllBufferedDataSent();

private:

    /**
    * Handle client request. This is invoced by client server framework.
    * @param aMessage Message object describing client request
    */
    virtual void ServiceL(const RMessage2 &aMessage);

    /**
    * Determine client request type and delegate handling accordingly.
    * @param aMessage Message object describing client request
    */
    void DispatchMessageL(const RMessage2 &aMessage);

    void HandleConnectRequestL(const RMessage2& aMessage);

    /**
    * Handle client's send request.
    * Copies data from client address space to internal send buffer and starts
    * sending it.
    * @param aMessage Message object describing client request
    */
    void HandleSendRequestL(const RMessage2& aMessage);

    /**
    * Handle client's read request.
    * Wait for incoming data. Once received, copy its data to client's
    * address space and complete client's request.
    * @param aMessage Message object describing client request
    */
    void HandleReadRequestL(const RMessage2& aMessage);

    /**
    * Handle client's cancel requests.
    * Cancel either read or write operation depending on client request.
    * @param aMessage Message object describing client request
    */
    void HandleCancelRequestL(const RMessage2& aMessage);

    /**
     * Handle port number request
     * Write the port number of the currently connected service. If not
     * connected writes KErrDisconnected.
     */
    void HandlePortNumberRequestL(const RMessage2& aMessage);

private: // Helpers

    void PanicClient(TInt aPanic) const;
    void ResetAll(TInt aCompletionCode);
    void TryCompleteReadRequest();

private: // Data

    CHtiBtCommServer *iBtCommServer; // referenced

    RMessage2 iReadRequest;
    RMessage2 iWriteRequest;
    RMessage2 iConnectRequest;
    TBool iWriteRequestComplete; // ETrue: no previous pending write request
    TBool iReadRequestComplete; // ETrue: no previous pending read request
    TBool iConnectRequestComplete;

    CBtSerialClient* iBtClient;

    HBufC8* iIncomingDataBuf; // Coming from remote host
    HBufC8* iSendBuffer;      // Sent to remote host

    };

#endif // CHTIBTCOMMSERVERSESSION_H

// End of File
