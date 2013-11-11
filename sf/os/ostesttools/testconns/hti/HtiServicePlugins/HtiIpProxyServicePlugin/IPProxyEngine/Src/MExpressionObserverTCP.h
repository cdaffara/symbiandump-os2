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
* Description:  Callback functions for a TCP protocol expression observer.
*
*/



#ifndef MEXPRESSIONOBSERVERTCP_H
#define MEXPRESSIONOBSERVERTCP_H

//  INCLUDES
#include "MExpressionObserver.h"

#include <e32def.h>

// CLASS DECLARATION

/**
*  Events for TCP protocol expressions (eg. TCP_OPEN, ...).
*/
NONSHARABLE_CLASS( MExpressionObserverTCP ) :
            public MExpressionObserver
    {

    public: // Abstract functions

        virtual void OpenLocalTCPConnectionL( TUint aPort ) = 0;

        virtual void OpenListeningTCPConnectionL( TUint aPort ) = 0;

        virtual void CloseTCPConnectionL( TUint aPort ) = 0;

        virtual void CloseAllTCPConnections() = 0;
    };

#endif      // MEXPRESSIONOBSERVERTCP_H

// End of File
