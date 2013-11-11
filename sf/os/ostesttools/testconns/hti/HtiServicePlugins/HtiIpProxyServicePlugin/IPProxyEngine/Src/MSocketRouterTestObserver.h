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
* Description:  Observer for socket router for test purposes.
*                This is currently the only way to get the parsed frame back to
*                the unit test class.
*
*/



#ifndef MSOCKETROUTERTESTOBSERVER_H
#define MSOCKETROUTERTESTOBSERVER_H

//  INCLUDES
#include <e32def.h>


// FORWARD DECLARATIONS
class CSocket;

// CLASS DECLARATION

/**
*  Observer for socket router.
*/
NONSHARABLE_CLASS( MSocketRouterTestObserver )
    {
    public: // Abstract functions

        /**
        * Starting point of a frame has been found.
        */
        virtual void FrameStarted() = 0;

        /**
        * Forwards the parsed frame info to the observer
        */
        virtual void FrameParsedL(
            TUint aPort,
            const TDesC8& aData ) = 0;

        /**
        * Error occurred when parsing data.
        * @param aErrorCode Error code.
        * @param aReceivedData Whole frame that failed to be parsed.
        */
        virtual void ProtocolErrorL(
            TInt aErrorCode,
            const TDesC8& aReceivedData ) = 0;

        /**
        * Open a TCP connection to phone side port
        * @param aPort Port number where to connect
        */
        virtual void OpenLocalTCPConnectionL( TUint aPort ) = 0;

        /**
        * Close a TCP connection to phone side port
        * @param aPort Port number to close
        */
        virtual void CloseTCPConnection( TUint aPort ) = 0;

    };

#endif      // MSOCKETROUTERTESTOBSERVER_H

// End of File
