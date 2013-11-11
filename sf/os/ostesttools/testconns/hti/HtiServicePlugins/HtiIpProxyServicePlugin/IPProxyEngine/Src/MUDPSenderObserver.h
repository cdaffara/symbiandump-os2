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
* Description:  Events for UDP Sender observer
*
*/



#ifndef MUDPSENDEROBSERVER_H
#define MUDPSENDEROBSERVER_H

//  INCLUDES
#include <e32def.h>
#include <es_sock.h>


// CLASS DECLARATION

/**
*  Events for socket writer.
*/
NONSHARABLE_CLASS( MUDPSenderObserver )
    {

    public: // Abstract functions
        /**
        * Notifies that error has occurred.
        * @param aErrorCode Error code.
        */
        virtual void UDPSenderErrorL( TInt aErrorCode ) = 0;

        /**
        * Notifies that the observer has leaved while executing the callback
        * function.
        * @param aLeaveCode Leave code.
        */
        virtual void UDPSenderLeavedL( TInt aLeaveCode ) = 0;
    };

#endif      // MUDPSENDEROBSERVER_H

// End of File
