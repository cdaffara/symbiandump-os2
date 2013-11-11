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
* Description:  Events for host connection
*
*/



#ifndef MHOSTCONNECTIONOBSERVER_H
#define MHOSTCONNECTIONOBSERVER_H

//  INCLUDES
#include <e32def.h>

// CLASS DECLARATION

/**
*  Events for host connection
*/
NONSHARABLE_CLASS( MHostConnectionObserver )
    {

    public: // Abstract functions

        /**
        * Connection to host established.
        */
        virtual void ConnectionEstablishedL() = 0;

        /**
        * Error when connecting to host.
        * @param aErrorCode Error code.
        */
        virtual void HostConnectionErrorL( TInt aErrorCode ) = 0;

        /**
        * Observer leaved in RunL.
        * @param aLeaveCode Leave code.
        */
        virtual void HostConnectionObserverLeaved( TInt aLeaveCode ) = 0;

    };

#endif      // MHOSTCONNECTIONOBSERVER_H

// End of File
