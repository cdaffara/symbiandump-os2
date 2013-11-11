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
* Description:  Host connection interface
*
*/



#ifndef MHOSTCONNECTION_H
#define MHOSTCONNECTION_H

//  INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MHostConnectionObserver;
class RSocket;

// CLASS DECLARATION

/**
*  Host connection interface
*/
NONSHARABLE_CLASS( MHostConnection )
    {

    public: // Abstract functions

        /**
        * Starts connecting procedure to host.
        */
        virtual void IssueConnectL() = 0;

        /**
        * Starts disconnection procedure.
        */
        virtual void IssueDisconnect() = 0;

        /**
        * Sets observer.
        * @param aObserver Pointer to observer.
        */
        virtual void SetObserver( MHostConnectionObserver* aObserver ) = 0;

        /**
        * @return Connection state.
        */
        virtual TBool IsConnected() = 0;

        /**
        * @return Pointer to current RSocket object. The ownership always
        * remains in this class.
        */
        virtual RSocket* Socket() = 0;

    };

#endif      // MHOSTCONNECTION_H

// End of File
