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
* Description:  Callback functions for a protocol expression observer
*
*/



#ifndef MEXPRESSIONOBSERVER_H
#define MEXPRESSIONOBSERVER_H

//  INCLUDES
#include <e32def.h>

// CLASS DECLARATION

/**
*  Events for protocol.
*/
NONSHARABLE_CLASS( MExpressionObserver )
    {

    public: // Abstract functions

        /**
        * Frame has started.
        */
        virtual void FrameStarted() = 0;

        /**
        * Frame has been parsed.
        * @param aPort Port number parsed from received frame.
        * @param aData Data parsed from received frame.
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

    };

#endif      // MEXPRESSIONOBSERVER_H

// End of File
