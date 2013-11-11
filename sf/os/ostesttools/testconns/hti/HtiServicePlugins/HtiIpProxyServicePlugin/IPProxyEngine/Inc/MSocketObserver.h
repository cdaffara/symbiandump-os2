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
* Description:  Events for socket
*
*/



#ifndef MSOCKETOBSERVER_H
#define MSOCKETOBSERVER_H

//  INCLUDES
#include <e32def.h>
#include <es_sock.h>


// FORWARD DECLARATIONS
class MSocket;
class CSocket;

// CLASS DECLARATION

/**
*  Events for socket.
*/
NONSHARABLE_CLASS( MSocketObserver )
    {
    // Abstract functions
    public:
        /**
        * Data has been received.
        * @param aSocket MSocket where the data received from.
        * @param aData Received data.
        */
        virtual void DataReceivedL(
            const MSocket* aSocket, const TDesC8& aData ) = 0;

        /**
        * Error occurred in socket.
        * @param aSocket MSocket object that caused error.
        * @param aErrorCode Error code.
        */
        virtual void ErrorL( const MSocket* aSocket, TInt aErrorCode ) = 0;

        /**
        * Socket disconnected.
        * @param aSocket Socket that disconnected.
        */
        virtual void DisconnectedL( const MSocket* aSocket ) = 0;

        /**
        * Notifies that the observer has leaved while executing the callback
        * function.
        * @param aSocket Socket that caused the leave.
        * @param aLeaveCode Leave code.
        */
        virtual void ObserverLeaved(
            const MSocket* aSocket, TInt aLeaveCode ) = 0;
    };

#endif      // MSOCKETOBSERVER_H

// End of File
