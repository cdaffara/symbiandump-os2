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
* Description:  Callback functions for TCP protocol observer
*
*/



#ifndef MPROTOCOLOBSERVER_H
#define MPROTOCOLOBSERVER_H

//  INCLUDES
#include <e32def.h>

// CLASS DECLARATION

/**
*  Events for protocol.
*/
NONSHARABLE_CLASS( MProtocolObserver )
    {

    public: // Abstract functions

        /**
        * Frame has started.
        */
        virtual void FrameStarted() = 0;

        /**
        * Error occurred when parsing data.
        * @param aErrorCode Error code.
        * @param aReceivedData Whole frame that failed to be parsed.
        */
        virtual void ProtocolErrorL(
            TInt aErrorCode,
            const TDesC8& aReceivedData ) = 0;

    };

#endif      // MPROTOCOLOBSERVER_H

// End of File
