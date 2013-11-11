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
* Description:  TCP protocol implementation for IP proxy
*
*/



#ifndef CPROTOCOLTCP_H
#define CPROTOCOLTCP_H

//  INCLUDES
#include "MBPProtocol.h"
#include "MBPExpression.h"
#include "MExpressionObserverTCP.h"

#include <e32base.h>


// FORWARD DECLARATIONS
class MProtocolObserverTCP;
class MSocketWriter;

// CLASS DECLARATION

/**
* HTTPProxy protocol implementation. Writes and interpretes data in
* protocol frame.
*/
NONSHARABLE_CLASS( CProtocolTCP ) : public CBase, public MBPProtocol,
                                    public MExpressionObserverTCP
    {

    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aObserver Pointer to observer.
        * @return New CProtocol instance.
        */
        static CProtocolTCP* NewL( MProtocolObserverTCP* aObserver );

        /**
        * Factory function.
        * @param aObserver Pointer to observer.
        * @return New CProtocol instance.
        */
        static CProtocolTCP* NewLC( MProtocolObserverTCP* aObserver );

        /**
        * Destructor.
        */
        ~CProtocolTCP();

    public: // From MExpressionObserverTCP

        void FrameStarted();
        void FrameParsedL( TUint aPort, const TDesC8& aData );
        void ProtocolErrorL( TInt aErrorCode, const TDesC8& aReceivedData );
        void OpenLocalTCPConnectionL( TUint aPort );
        void OpenListeningTCPConnectionL( TUint aPort );
        void CloseTCPConnectionL( TUint aPort );
        void CloseAllTCPConnections();

    public: // From MBPProtocol

        void WriteFrameL( MSocketWriter& aSocketWriter,
            TUint aPeerPort, TUint aOriginalPort, const TDesC8& aData ) const;

        TBool HandleReceivedDataL( TDes8& aData, TInt& aStartPos, TInt& aLength );

    public: // New functions

        /**
        * Sends command via HTI to close the TCP connection
        * @param aPort Remote port to close
        */
        void SendCloseTCPConnection( MSocketWriter& aSocketWriter,
                                          TUint aPort );

    protected:

        /**
        * Default constructor.
        * @param aObserver Pointer to observer.
        */
        CProtocolTCP( MProtocolObserverTCP* aObserver );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();


    private:    // Owned data

        CArrayPtr<MBPExpression>* iExpressionArray;

    private:    // Not owned data
        MProtocolObserverTCP* iObserver;

    };

#endif      // CPROTOCOLTCP_H

// End of File
