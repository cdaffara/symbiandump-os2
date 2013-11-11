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
* Description:  MSocketWriter interface
*
*/



#ifndef MSOCKETWRITER_H
#define MSOCKETWRITER_H

//  INCLUDES
#include <e32def.h>


// CLASS DECLARATION

/**
*  MSocketWriter interface.
*/
NONSHARABLE_CLASS( MSocketWriter )
    {

    public: // Abstract functions

        /**
        * Writes data to socket.
        * @param aData Data to be written.
        */
        virtual void WriteL( const TDesC8& aData ) = 0;

    };

#endif      // MSOCKETWRITER_H

// End of File
