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
* Description:  MSocket interface
*
*/



#ifndef MSOCKET_H
#define MSOCKET_H

//  INCLUDES
#include <es_sock.h>
#include "MSocketWriter.h"

// FORWARD DECLARATIONS
class RSocket;
class MSocketObserver;

// CLASS DECLARATION

/**
*  MSocket interface.
*/
NONSHARABLE_CLASS( MSocket ) : public MSocketWriter
    {
    public:
        virtual ~MSocket() {};

    public: // Abstract functions

        /**
        * @return RSocket object
        */
        virtual RSocket* GetRSocket() = 0;

        /**
        * @return Local TCP port.
        */
        virtual TUint LocalPort() const = 0;

        /**
        * @return Remote TCP port.
        */
        virtual TUint RemotePort() const = 0;

        /**
        * Sets observer.
        * @param aObserver Pointer to observer.
        */
        virtual void SetObserver( MSocketObserver* aObserver ) = 0;

        /**
        * Sets the ownership mode of RSocket object. If set ETrue
        * the socket will be closed and RSocket object will be deleted
        * in the destruction.
        * @param aHasOwnership Ownership mode.
        */
        virtual void SetSocketOwnershipMode( TBool aHasOwnership ) = 0;

        /**
        * Starts receiving data.
        */
        virtual void IssueRead() = 0;

        /**
        * Cancels all pending requests.
        */
        virtual void Cancel() = 0;

        /*
        * Returns the protocol information of the socket
        */
        virtual void SocketInfo( TProtocolDesc& aDesc) const = 0;

        /*
        * Is this a UDP socket
        */
        virtual TBool IsUDP() const = 0;

    public: // From MSocketWriter
        /**
        * Issues writing data to socket.
        * @param aData Data to be written.
        */
        virtual void WriteL( const TDesC8& aData ) = 0;
    };

#endif      // MSOCKET_H

// End of File
