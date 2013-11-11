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
* Description:  IPProxy TCP protocol expression for closing phone side TCP
*                connection.
*
*/



#ifndef CBPEXPRTCPCLOSE_H
#define CBPEXPRTCPCLOSE_H

//  INCLUDES
#include <e32base.h>

#include "MBPExpression.h"

// CONSTANTS


// FORWARD DECLARATIONS

class MExpressionObserverTCP;

// CLASS DECLARATION

/**
*  TCP protocol expression for closing a new TCP connection from PC-side
*  to phone side port
*/
NONSHARABLE_CLASS( CExprTCPClose ) : public CBase, public MBPExpression
    {
    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aObserver Pointer to observer.
        * @return New CBPExpression instance.
        */
        static CExprTCPClose* NewL( MExpressionObserverTCP* aObserver );

        /**
        * Factory function.
        * @param aObserver Pointer to observer.
        * @return New CBPExpression instance.
        */
        static CExprTCPClose* NewLC( MExpressionObserverTCP* aObserver );

        /**
        * Destructor.
        */
        ~CExprTCPClose();

    public: // from MBPExpression

        TBool HandleRecievedMsgL( TDes8& aData, TInt& aStartPos, TInt& aLength );

    public: // new functions

        TInt TryParsingL( TDes8& aData, TInt& aLength );

    private:

        /**
        * Default constructor.
        * @param aObserver Pointer to observer.
        */
        CExprTCPClose( MExpressionObserverTCP* aObserver );

    private:    // Data

        MExpressionObserverTCP* iObserver;
    };

#endif      // CBPEXPRTCPCLOSE_H

// End of File
