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
* Description:  IPProxy UDP protocol expression for a regular UDP message
*                connection.
*
*/



#ifndef CBPEXPRUDPMSG_H
#define CBPEXPRUDPMSG_H

//  INCLUDES
#include <e32base.h>

#include "MBPExpression.h"

// CONSTANTS


// FORWARD DECLARATIONS

class MExpressionObserver;

// CLASS DECLARATION

/**
*  UDP protocol expression for a regular UDP message
*  to phone side port
*/
NONSHARABLE_CLASS( CExprUDPMsg ) : public CBase, public MBPExpression
    {
    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aObserver Pointer to observer.
        * @return New CBPExpression instance.
        */
        static CExprUDPMsg* NewL( MExpressionObserver* aObserver );

        /**
        * Factory function.
        * @param aObserver Pointer to observer.
        * @return New CBPExpression instance.
        */
        static CExprUDPMsg* NewLC( MExpressionObserver* aObserver );

        /**
        * Destructor.
        */
        ~CExprUDPMsg();

    public: // from MBPExpression

        TBool HandleRecievedMsgL( TDes8& aData, TInt& aStartPos, TInt& aLength );

    public: // new functions

        TInt TryParsingL( TDes8& aData, TInt& aLength );

    private:

        /**
        * Default constructor.
        * @param aObserver Pointer to observer.
        */
        CExprUDPMsg( MExpressionObserver* aObserver );

    private:    // Data

        MExpressionObserver* iObserver;
    };

#endif      // CBPEXPRUDPMSG_H

// End of File
