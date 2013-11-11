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
* Description:  TCP protocol specific callback functions
*
*/



#ifndef MPROTOCOLOBSERVERTCP_H
#define MPROTOCOLOBSERVERTCP_H

//  INCLUDES
#include <e32def.h>

#include "MProtocolObserver.h"

// CLASS DECLARATION

/**
*  Events for protocol.
*/
NONSHARABLE_CLASS( MProtocolObserverTCP ) : public MProtocolObserver
    {

    public: // Abstract functions

        /**
        * TCP Frame has been parsed.
        * @param aPort Port number parsed from received frame.
        * @param aData Data parsed from received frame.
        */
        virtual void TCPFrameParsedL(
            TUint aPort,
            const TDesC8& aData ) = 0;

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
        * Close a TCP connection
        * @param aPort Remote port number to close
        */
        virtual void CloseTCPConnectionL( TUint aPort ) = 0;

        /**
        * Close all TCP connections from listening & connecting ports
        */
        virtual void CloseAllTCPConnections() = 0;
    };

#endif      // MPROTOCOLOBSERVERTCP_H

// End of File
