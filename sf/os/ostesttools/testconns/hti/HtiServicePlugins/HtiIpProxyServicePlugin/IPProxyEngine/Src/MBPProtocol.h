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
* Description:  MBPProtocol interface, defines basics for all the protocols
*
*/



#ifndef MBPPROTOCOL_H
#define MBPPROTOCOL_H

//  INCLUDES
#include <e32def.h>
#include <e32base.h>

#include "MSocketWriter.h"


// CLASS DECLARATION

/**
*  MBPProtocol interface.
*/
NONSHARABLE_CLASS( MBPProtocol )
    {

    public: // Abstract functions

        /**
        * Tries to handle the message, responds if it can do it
        *
        * @param aData Data to be interpreted
        * @param aStartPos The index from where the message frame begins
        * @param aLength Length of the message
        * @return ETrue if protocol can handle the msg, otherwise EFalse
        */
        virtual TBool HandleReceivedDataL(  TDes8& aData,
                                            TInt& aStartPos,
                                            TInt& aLength ) = 0;

        /**
        * Packs data to frame and sends the frame.
        * @param aSocketWriter Object that executes the writing.
        * @param aPeerPort Peer's TCP port.
        * @param aOriginalPort Original TCP port.
        * @param aData Data to be sent.
        */
        virtual void WriteFrameL( MSocketWriter& aSocketWriter,
            TUint aPeerPort, TUint aOriginalPort, const TDesC8& aData ) const = 0;

        /** Virtual destructor
        *
        */
        virtual ~MBPProtocol() {};
    };

#endif      // MBPPROTOCOL_H

// End of File
