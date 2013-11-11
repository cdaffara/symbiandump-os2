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
* Description:  Main interface for IPProxyEngine
*
*/



#ifndef IPPROXYENGINE_H
#define IPPROXYENGINE_H

//  INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MIPProxyEngineObserver;

// CLASS DECLARATION

/**
*  Main interface for IPProxyEngine.
*/
class MIPProxyEngine
    {
    public: // Abstract functions
        /**
        * Virtual destructor makes it possible to delete the object via this
        * interface.
        */
        virtual ~MIPProxyEngine() {};

        /**
        * Set observer of IPProxyEngine.
        * @param aObserver Pointer to observer.
        */
        virtual void SetObserver( MIPProxyEngineObserver* aObserver ) = 0;

        /**
        * Add port to listen.
        * @param aPort Port to listen.
        */
        virtual void AddPeerListeningPortL( TInt aPort ) = 0;

        /**
        * Starts listening peers.
        */
        virtual void StartListening() = 0;

        /**
        * Stops listening peers.
        */
        virtual void StopListening() = 0;

        /**
        * Disconnects all peers and the host connection.
        */
        virtual void DisconnectAllConnections() = 0;
    };

#endif      // IPPROXYENGINE_H

// End of File
