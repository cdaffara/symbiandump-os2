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
* Description:  Callback functions for UDP protocol
*
*/



#ifndef MPROTOCOLOBSERVERUDP_H
#define MPROTOCOLOBSERVERUDP_H

//  INCLUDES
#include "MProtocolObserver.h"

#include <e32def.h>

// CLASS DECLARATION

/**
*  Events for protocol.
*/
NONSHARABLE_CLASS( MProtocolObserverUDP ) : public MProtocolObserver
    {

    public: // Abstract functions

        /**
        * Frame has been parsed.
        * @param aPort Port number parsed from received frame.
        * @param aData Data parsed from received frame.
        */
        virtual void UDPFrameParsedL(
            TUint aPort,
            const TDesC8& aData ) = 0;

    };

#endif      // MPROTOCOLOBSERVERUDP_H

// End of File
