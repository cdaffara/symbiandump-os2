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
* Description:  UDP protocol implementation for IP proxy
*
*/



#ifndef CPROTOCOLUDP_H
#define CPROTOCOLUDP_H

//  INCLUDES
#include "MBPProtocol.h"
#include "MBPExpression.h"
#include "MExpressionObserverUDP.h"

#include <e32base.h>


// FORWARD DECLARATIONS
class MProtocolObserverUDP;
class MSocketWriter;

// CLASS DECLARATION

/**
* UDP protocol implementation. Writes and interpretes data in
* protocol frame.
*/
NONSHARABLE_CLASS( CProtocolUDP ) : public CBase, public MBPProtocol,
                                    public MExpressionObserver
    {

    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aObserver Pointer to observer.
        * @return New CProtocol instance.
        */
        static CProtocolUDP* NewL( MProtocolObserverUDP* aObserver );

        /**
        * Factory function.
        * @param aObserver Pointer to observer.
        * @return New CProtocol instance.
        */
        static CProtocolUDP* NewLC( MProtocolObserverUDP* aObserver );

        /**
        * Destructor.
        */
        ~CProtocolUDP();

    public: // From MExpressionObserverUDP

        void FrameStarted();
        void FrameParsedL( TUint aPort, const TDesC8& aData );
        void ProtocolErrorL( TInt aErrorCode, const TDesC8& aReceivedData );

    public: // From MBPProtocol

        void WriteFrameL( MSocketWriter& aSocketWriter,
            TUint aPeerPort, TUint aOriginalPort, const TDesC8& aData ) const;

        TBool HandleReceivedDataL( TDes8& aData, TInt& aStartPos, TInt& aLength );

    protected:

        /**
        * Default constructor.
        * @param aObserver Pointer to observer.
        */
        CProtocolUDP( MProtocolObserverUDP* aObserver );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();


    private:    // Owned data

        CArrayPtr<MBPExpression>* iExpressionArray;

    private:    // Not owned data
        MProtocolObserverUDP* iObserver;

    };

#endif      // CPROTOCOLUDP_H

// End of File
