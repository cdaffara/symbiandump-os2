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
* Description:  Server side session in HtiIPCommServer.
*
*/


#ifndef CHTIIPCOMMSERVERSESSION_H
#define CHTIIPCOMMSERVERSESSION_H

//  INCLUDES

#include <e32std.h>
#include <e32base.h>

#include "HtiIPCommServer.h"
#include "HtiConnectionManager.h"

// FORWARD DECLARATIONS
class CHtiIPCommServer;


// CLASS DECLARATION
class CHtiIPCommServerSession : public CSession2
    {
public:
    /**
    * Constructor
    *
    * @param aServer Pointer to server instance
    */
    CHtiIPCommServerSession( CHtiIPCommServer* aServer );

private:

    /**
    * Destructor
    */
    ~CHtiIPCommServerSession();

    /**
    * Receives all server command and directs them to
    * their handlers.
    *
    * @param aMessage Message to process
    */
    void ServiceL( const RMessage2& aMessage );

    /**
    * Receives data from socket
    *
    * @param aMessage Message containing a descriptor where data is received
    */
    void HandleReceiveRequestL( const RMessage2& aMessage );

    /**
    * Sends data to socket
    *
    * @param aMessage Message containing a descriptor of the data to send
    */
    void HandleSendRequestL( const RMessage2& aMessage );

    /**
    * Cancels pending Receive request on server.
    */
    void HandleCancelReceiveRequestL( const RMessage2& aMessage );

    /**
    * Cancels pending Send request on server.
    */
    void HandleCancelSendRequestL( const RMessage2& aMessage );

private:
    /**
    * Server pointer for calling callbacks functions
    * and connection manager
    */
    CHtiIPCommServer* iServer;
    };

#endif // CHTIIPCOMMSERVERSESSION_H

// End of File
