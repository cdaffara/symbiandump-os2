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
* Description:  Events for socket writer
*
*/



#ifndef MSOCKETWRITEROBSERVER_H
#define MSOCKETWRITEROBSERVER_H

//  INCLUDES
#include <e32def.h>
#include <es_sock.h>


// CLASS DECLARATION

/**
*  Events for socket writer.
*/
NONSHARABLE_CLASS( MSocketWriterObserver )
    {

    public: // Abstract functions
        /**
        * Notifies that error has occurred.
        * @param aErrorCode Error code.
        */
        virtual void WriterErrorL( TInt aErrorCode ) = 0;

        /**
        * Notifies that all data has succesfully been written.
        */
        virtual void BufferUnderrunL() = 0;

        /**
        * Notifies that the observer has leaved while executing the callback
        * function.
        * @param aLeaveCode Leave code.
        */
        virtual void ObserverLeaved( TInt aLeaveCode ) = 0;
    };

#endif      // MSOCKETWRITEROBSERVER_H

// End of File
