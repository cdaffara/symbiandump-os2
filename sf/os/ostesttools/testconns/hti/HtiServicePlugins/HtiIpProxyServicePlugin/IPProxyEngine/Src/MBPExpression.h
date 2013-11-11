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
* Description:  MExpression interface, defines basics for all the protocol
*                expressions
*
*/



#ifndef MBPEXPRESSION_H
#define MBPEXPRESSION_H

//  INCLUDES
#include <e32def.h>
#include <e32base.h>

class MExpressionObserver;

// CLASS DECLARATION

/**
*  MProtocol interface.
*/
NONSHARABLE_CLASS( MBPExpression )
    {

    public: // Abstract functions

        /**
        * Tries to handle the message, and informs if the msg does
        * not belong to it.
        *
        * @param aData Message to parse
        * @param aStartPos The index from where the message frame begins
        * @param aLength Length of the message
        * @return ETrue if the expression can handle the message,
        *         EFalse if not.
        */
        virtual TBool HandleRecievedMsgL(  TDes8& aData,
                                           TInt& aStartPos,
                                           TInt& aLength ) = 0;

        /** Virtual destructor
        *
        */
        virtual ~MBPExpression() {};
    };

#endif      // MBPEXPRESSION_H

// End of File
