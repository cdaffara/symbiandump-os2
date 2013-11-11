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
* Description:  Main class for IPProxyEngine
*
*/



#ifndef CIPPROXYENGINE_H
#define CIPPROXYENGINE_H

//  INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include "MIPProxyEngine.h"
#include "Mhostconnectionobserver.h"
#include "MLocalTCPConnectionObserver.h"
#include "MTCPPortListenerObserver.h"
#include "MSocketRouterObserver.h"


// FORWARD DECLARATIONS
class MHostConnection;
class CTCPPortListener;
class CSocketRouter;
class MIPProxyEngineObserver;
class CLocalTCPConnection;
class MHostConnection;
class MAbstractConnection;

// CLASS DECLARATION

/**
*  Main class for IPProxyEngine.
*/
NONSHARABLE_CLASS( CIPProxyEngine ) : public CActive,
    public MIPProxyEngine,
    public MTCPPortListenerObserver,
    public MHostConnectionObserver,
    public MLocalTCPConnectionObserver,
    public MSocketRouterObserver
    {
    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aConnection pointer to host connection.
        */
        static CIPProxyEngine* NewL( MAbstractConnection* aConnection );

        /**
        * Factory function.
        * @param aConnection pointer to host connection.
        */
        static CIPProxyEngine* NewLC( MAbstractConnection* aConnection );

        /**
        * Destructor.
        */
        ~CIPProxyEngine();


    protected: // Functions from base classes

        /**
        * From CActive. Pending request has been completed.
        */
        void RunL();

        /**
        * From CActive. Pending request has been cancelled.
        */
        void DoCancel();


    public:     // Functions from base classes

        /**
        * From MIPProxyEngine.
        * Set observer of IPProxyEngine.
        * @param aObserver Pointer to observer.
        */
        void SetObserver( MIPProxyEngineObserver* aObserver );

        /**
        * From MIPProxyEngine.
        * Add port to listen.
        * @param aPort Port to listen.
        */
        void AddPeerListeningPortL( TInt aPort );

        /**
        * From MIPProxyEngine.
        * Starts listening peers.
        */
        void StartListening();

        /**
        * From MIPProxyEngine.
        * Stops listening peers.
        */
        void StopListening();

        /**
        * From MIPProxyEngine.
        * Disconnects all peers and the host connection.
        */
        void DisconnectAllConnections();

    protected:  // Functions from MTCPPortListenerObserver

        void ConnectionAcceptedL( RSocket* aSocket );
        void ErrorL( TInt aErrorCode );
        void ObserverLeaved( TInt aLeaveCode );

    protected:  // Functions from MHostConnectionObserver

        void ConnectionEstablishedL();
        void HostConnectionErrorL( TInt aErrorCode );
        void HostConnectionObserverLeaved( TInt aLeaveCode );

    protected:  // Functions from MLocalTCPConnectionObserver

        void LocalTCPConnectionEstablishedL( TUint aPort );
        void LocalTCPConnectionErrorL( TInt aPort, TInt aErrorCode );
        void LocalTCPConnectionObserverLeaved( TInt aPort, TInt aLeaveCode );

    protected:  // Functions from MSocketRouterObserver

        void SocketRouterErrorL( const MSocket* aSocket, TInt aErrorCode );
        void ObserverLeaved( const MSocket* aSocket, TInt aLeaveCode );
        void PeerDisconnectedL( const MSocket* aSocket );
        void HostDisconnectedL( const MSocket* aSocket );
        void OpenLocalTCPConnectionL( TUint aPort );
        void OpenListeningTCPConnectionL( TUint aPort );
        void CloseTCPConnection( TUint aPort );
        void CloseAllTCPConnections();
        void AssureConnectionL();

    protected:

        /**
        * Default constructor.
        */
        CIPProxyEngine( MAbstractConnection* aConnection );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // New functions

        /**
        * Helper function to find the correct connection from the local
        * TCP connection -list.
        * @param aPort The remote port of the connection to find
        * @return index of the local conn in the iLocalConnArray
        */
        TInt FindLocalTCPConn( TUint aPort );

    private:    // Owned data

        RSocketServ iSocketServ;
        CArrayPtr<CTCPPortListener>* iPeerListenerArray;
        CArrayPtr<CLocalTCPConnection>* iLocalConnArray;
        TBool iListening;
        CSocketRouter* iSocketRouter;
        RTimer iTimer;

    private:    // Not owned data
        MAbstractConnection* iAbstractConnection;
        MHostConnection* iHostConnection;
        MIPProxyEngineObserver* iObserver;
    };

#endif      // CIPPROXYENGINE_H

// End of File
