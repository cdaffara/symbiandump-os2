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
* Description:  UDP packet sender
*
*/



#ifndef CUDPSENDER_H
#define CUDPSENDER_H

//  INCLUDES
#include <e32base.h>
#include <in_sock.h>

// CONSTANTS
const TInt KWriteBufferSize = 128;

// FORWARD DECLARATIONS
class MUDPSenderObserver;
class CDesC8Array;

// CLASS DECLARATION

/**
*  Socket writer. Asynchronously writes data to opened socket.
*/
NONSHARABLE_CLASS( CUDPSender ) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aObserver Pointer to observer.
        * @return New CUDPSender instance.
        */
        static CUDPSender* NewL( MUDPSenderObserver* aObserver );

        /**
        * Factory function.
        * @param aObserver Pointer to observer.
        * @return New CUDPSender instance.
        */
        static CUDPSender* NewLC( MUDPSenderObserver* aObserver );

        /**
        * Destructor.
        */
        ~CUDPSender();

    public: // New functions

        /**
        * Issues writing data to UDP socket. If the request is already
        * pending the data will be written to queue and it will be
        * handled afterwards.
        * @param aPort Port to write to.
        * @param aData Data to be written.
        */
        void IssueWriteL( TUint aPort, const TDesC8& aData );

        /**
        * Resets transfer buffer.
        */
        void ResetTransferBuffer();

        /**
        * Continues writing if there is something in the buffer left.
        */
        void ContinueAfterError();

    private:
        /**
        * Writes one descriptor from queue.
        */
        void IssueWrite();

    protected: // Functions from base classes

        /**
        * From CActive. Pending request has been completed.
        */
        void RunL();

        /**
        * From CActive. Pending request has been cancelled.
        */
        void DoCancel();

        /**
        * From CActive. RunL has leaved.
        */
        TInt RunError( TInt aError );


    private:

        /**
        * Default constructor.
        * @param aObserver Pointer to observer.
        */
        CUDPSender( MUDPSenderObserver* aObserver );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:    // Data
        RSocketServ iSocketServ;
        CDesC8Array* iTransferBufferArray;
        MUDPSenderObserver* iObserver;
        RSocket iSocket;
        TBuf8<KWriteBufferSize> iWriteBuffer;
        RTimer iTimer;
        TBool iWaiting;
        TInetAddr iRemoteAddr;
    };

#endif      // CUDPSender_H

// End of File
