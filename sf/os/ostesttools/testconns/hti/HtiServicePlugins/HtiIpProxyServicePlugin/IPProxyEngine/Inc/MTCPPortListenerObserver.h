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
* Description:  Events for TCP port listener
*
*/



#ifndef MTCPPORTLISTENEROBSERVER_H
#define MTCPPORTLISTENEROBSERVER_H

//  INCLUDES
#include <e32def.h>
#include <es_sock.h>


// CLASS DECLARATION

/**
*  Events for TCP port listener.
*/
NONSHARABLE_CLASS( MTCPPortListenerObserver )
    {

    public: // Abstract functions

        /**
        * Notifies that TCP port listener has accepted an incoming
        * TPC connection.
        * @param aSocket RSocket object that has an opened subsession
        * to Socket server. This means that the object who implements
        * this event has the ownership and is responsible in closing the
        * subsession too!
        */
        virtual void ConnectionAcceptedL( RSocket* aSocket ) = 0;

        /**
        * Notifies that error has occurred.
        * @param aErrorCode Error code.
        */
        virtual void ErrorL( TInt aErrorCode ) = 0;

        /**
        * Notifies that the observer has leaved while executing the callback
        * function.
        * @param aLeaveCode Leave code.
        */
        virtual void ObserverLeaved( TInt aLeaveCode ) = 0;
    };

#endif      // MTCPPORTLISTENEROBSERVER_H

// End of File
