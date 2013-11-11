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
* Description:  Observer for socket router
*
*/



#ifndef MSOCKETROUTEROBSERVER_H
#define MSOCKETROUTEROBSERVER_H

//  INCLUDES
#include <e32def.h>


// FORWARD DECLARATIONS
class MSocket;
class CSocket;

// CLASS DECLARATION

/**
*  Observer for socket router.
*/
NONSHARABLE_CLASS( MSocketRouterObserver )
    {
    public: // Abstract functions

        /**
        * Error occurred in socket.
        * @param aSocket MSocket object that caused error.
        * @param aErrorCode Error code.
        */
        virtual void SocketRouterErrorL(
            const MSocket* aSocket, TInt aErrorCode ) = 0;

        /**
        * Observer leaved in RunL.
        * @param aSocket CSocket object that caused leave.
        * @param aLeaveCode Leave code.
        */
        virtual void ObserverLeaved(
            const MSocket* aSocket, TInt aLeaveCode ) = 0;

        /**
        * Peer disconnected.
        * @param aSocket Socket that disconnected.
        */
        virtual void PeerDisconnectedL( const MSocket* aSocket ) = 0;

        /**
        * Host disconnected.
        * @param aSocket Socket that disconnected.
        */
        virtual void HostDisconnectedL( const MSocket* aSocket ) = 0;

        /**
        * Open a TCP connection to phone side port
        * @param aPort Port number where to connect
        */
        virtual void OpenLocalTCPConnectionL( TUint aPort ) = 0;

        /**
        * Open a listening TCP port on the phone side
        * @param aPort Port number where to start listening
        */
        virtual void OpenListeningTCPConnectionL( TUint aPort ) = 0;

        /**
        * Close a TCP connection to phone side port
        * @param aPort Port number to close
        */
        virtual void CloseTCPConnection( TUint aPort ) = 0;

        /**
        * Close all TCP connections from listening & connecting ports
        */
        virtual void CloseAllTCPConnections() = 0;
    };

#endif      // MSOCKETROUTEROBSERVER_H

// End of File
