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
* Description:  Implementation of ECOM plug-in service interface providing
*                the IP-Proxy service.
*
*/


#ifndef HTIIPPROXYSERVICEPLUGIN_H__
#define HTIIPPROXYSERVICEPLUGIN_H__

// INCLUDES
#include <HtiServicePluginInterface.h>
#include "MSocket.h"
#include "MHostConnection.h"
#include "MAbstractConnection.h"

// FORWARD DECLARATIONS
class MSocketObserver;
class MIPProxyEngine;
class CDesC8Array;
class MHostConnectionObserver;

// CLASS DECLARATION
/**
* IP-Proxy plugin implementation
*/
class CHtiIpProxyServicePlugin :
    public CHTIServicePluginInterface,
    public MAbstractConnection,
    public MSocket,
    public MHostConnection
    {
    public:
        static CHtiIpProxyServicePlugin* NewL();

    // Interface implementation

        /**
        * Called by HTI-framework to initialize this plugin
        */
        void InitL();

        /**
        * Return busy state status
        *
        * @return ETrue when busy otherwise EFalse
        */
        TBool IsBusy();

        /**
        * Process either control or data message depending on aPriority
        *
        * @param aMessage message with commands or data
        * @param aPriority indicates type of aMessage
        */
        void ProcessMessageL( const TDesC8& aMessage,
                              THtiMessagePriority aPriority );

        /**
        * Available memory notification from HTI-framework
        *
        * @param aAvailableMemory amount of available memory for message dispatching
        */
        void NotifyMemoryChange( TInt aAvailableMemory );

    public:     // From MAbstractConnection
        /**
        * Get pointer to current socket instance.
        *
        * @return Pointer to current RSocket object. The ownership always
        *  remains in the class which derives from this interface.
        */
        virtual MSocket* GetSocket();

        /**
        * Get pointer to current host connection instance.
        *
        * @return Pointer to current RSocket object. The ownership always
        *  remains in the class which derives from this interface.
        */
        virtual MHostConnection* GetHostConnection();

    public:     // From MSocket
        /**
        * @return RSocket object
        */
        virtual RSocket* GetRSocket();

        /**
        * @return Local TCP port.
        */
        virtual TUint LocalPort() const;

        /**
        * @return Remote TCP port.
        */
        virtual TUint RemotePort() const;

        /**
        * Sets observer.
        *
        * @param aObserver Pointer to observer.
        */
        virtual void SetObserver( MSocketObserver* aObserver );

        /**
        * Sets the ownership mode of RSocket object. If set ETrue
        * the socket will be closed and RSocket object will be deleted
        * in the destruction.
        *
        * @param aHasOwnership Ownership mode.
        */
        virtual void SetSocketOwnershipMode( TBool aHasOwnership );

        /**
        * Starts receiving data.
        */
        virtual void IssueRead();

        /**
        * Cancels all pending requests.
        */
        virtual void Cancel();

        /*
        * Returns the protocol information of the socket
        */
        virtual void SocketInfo( TProtocolDesc& aDesc ) const;

        /*
        * Is this a UDP socket
        */
        virtual TBool IsUDP() const;

    public:     // From MSocketWriter
        /**
        * Writes data to socket.
        * @param aData Data to be written.
        */
        virtual void WriteL( const TDesC8& aData );

    public:     // From MHostConnection
        /**
        * Starts connecting procedure to host.
        */
        virtual void IssueConnectL();

        /**
        * Starts disconnection procedure.
        */
        virtual void IssueDisconnect();

        /**
        * Sets observer.
        *
        * @param aObserver Pointer to observer.
        */
        virtual void SetObserver( MHostConnectionObserver* aObserver );

        /**
        * @return Connection state.
        */
        virtual TBool IsConnected();

        /**
        * @return Pointer to current RSocket object. The ownership always
        *
        * remains in this class.
        */
        virtual RSocket* Socket();


    protected:
        CHtiIpProxyServicePlugin();
        virtual ~CHtiIpProxyServicePlugin();

        void ConstructL();

    private:
        MSocketObserver* iSocketObserver;
        MHostConnectionObserver* iHostObserver;
        MIPProxyEngine* iProxyEngine;
        CDesC8Array* iOutgoingArray;
        TBool iBusy;
        TBool iConnected;
    };

#endif // HTIIPPROXYSERVICEPLUGIN_H__

// End of File
