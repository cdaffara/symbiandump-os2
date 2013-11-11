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
* Description:  Events for local TCP connections
*
*/



#ifndef MLOCALTCPCONNECTIONOBSERVER_H
#define MLOCALTCPCONNECTIONOBSERVER_H

//  INCLUDES
#include <e32def.h>

// CLASS DECLARATION

/**
*  Events for host connection
*/
NONSHARABLE_CLASS( MLocalTCPConnectionObserver )
    {

    public: // Abstract functions

        /**
        * Connection to host established.
        * @param aPort Port that was connected.
        */
        virtual void LocalTCPConnectionEstablishedL( TUint aPort ) = 0;


        /**
        * Error when connecting to host.
        * @param aPort Port that reported error.
        * @param aErrorCode Error code.
        */
        virtual void LocalTCPConnectionErrorL( TInt aPort, TInt aErrorCode ) = 0;


        /**
        * Observer leaved in RunL.
        * @param aPort Port that caused the leave.
        * @param aLeaveCode Leave code.
        */
        virtual void LocalTCPConnectionObserverLeaved( TInt aPort, TInt aLeaveCode ) = 0;

    };

#endif      // MLOCALTCPCONNECTIONOBSERVER_H

// End of File
