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
* Description:  Callback functions for a UDP protocol expression observer.
*
*/



#ifndef MEXPRESSIONOBSERVERUDP_H
#define MEXPRESSIONOBSERVERUDP_H

//  INCLUDES
#include "MExpressionObserver.h"

#include <e32def.h>

// CLASS DECLARATION

/**
*  Events for UDP protocol expressions (eg. UDP_OPEN, ...).
*/
NONSHARABLE_CLASS( MExpressionObserverUDP ) :
            public MExpressionObserver
    {

    public: // Abstract functions

        virtual void OpenLocalUDPConnectionL( TUint aPort ) = 0;

        virtual void CloseUDPConnection( TUint aPort ) = 0;

    };

#endif      // MEXPRESSIONOBSERVERUDP_H

// End of File
