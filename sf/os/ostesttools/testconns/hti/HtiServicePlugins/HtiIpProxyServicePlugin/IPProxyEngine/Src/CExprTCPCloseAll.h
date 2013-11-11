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
* Description:  IP-Proxy TCP protocol expression for closing all phone side
*                sockets.
*
*/



#ifndef CBPEXPRTCPCLOSEALL_H
#define CBPEXPRTCPCLOSEALL_H

//  INCLUDES
#include <e32base.h>

#include "MBPExpression.h"

// CONSTANTS


// FORWARD DECLARATIONS

class MExpressionObserverTCP;

// CLASS DECLARATION

/**
*  TCP protocol expression for closing all socket ports
*/
NONSHARABLE_CLASS( CExprTCPCloseAll ) : public CBase, public MBPExpression
    {
    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aObserver Pointer to observer.
        * @return New CBPExpression instance.
        */
        static CExprTCPCloseAll* NewL( MExpressionObserverTCP* aObserver );

        /**
        * Factory function.
        * @param aObserver Pointer to observer.
        * @return New CBPExpression instance.
        */
        static CExprTCPCloseAll* NewLC( MExpressionObserverTCP* aObserver );

        /**
        * Destructor.
        */
        ~CExprTCPCloseAll();

    public: // from MBPExpression

        TBool HandleRecievedMsgL( TDes8& aData, TInt& aStartPos, TInt& aLength );

    public: // new functions

        TInt TryParsing( TDes8& aData, TInt& aLength );

    private:

        /**
        * Default constructor.
        * @param aObserver Pointer to observer.
        */
        CExprTCPCloseAll( MExpressionObserverTCP* aObserver );

    private:    // Data

        MExpressionObserverTCP* iObserver;
    };

#endif      // CBPEXPRTCPCLOSEALL_H

// End of File
