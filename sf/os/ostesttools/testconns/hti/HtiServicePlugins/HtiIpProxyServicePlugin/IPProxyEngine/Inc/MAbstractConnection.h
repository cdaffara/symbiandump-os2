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



#ifndef MABSTRACTCONNETION_H
#define MABSTRACTCONNETION_H

//  INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MSocket;
class MHostConnection;

// CLASS DECLARATION

/**
*  Host connection interface
*/
NONSHARABLE_CLASS( MAbstractConnection )
    {

    public: // Abstract functions

        /**
        * Get pointer to current socket instance.
        * @return Pointer to current RSocket object. The ownership always
        *  remains in the class which derives from this interface.
        */
        virtual MSocket* GetSocket() = 0;

        /**
        * Get pointer to current host connection instance.
        * @return Pointer to current RSocket object. The ownership always
        *  remains in the class which derives from this interface.
        */
        virtual MHostConnection* GetHostConnection() = 0;
    };

#endif      // MABSTRACTCONNETION_H

// End of File
