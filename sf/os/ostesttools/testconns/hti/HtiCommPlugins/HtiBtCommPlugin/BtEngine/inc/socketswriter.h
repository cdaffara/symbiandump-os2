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
* Description:  Writes to socket.
*
*/


#ifndef __SOCKETSWRITER_H__
#define __SOCKETSWRITER_H__

// INCLUDES
#include <in_sock.h>
#include "timeoutnotifier.h"

// FORWARD DECLARATIONS
class CTimeOutTimer;
class MSocketObserver;

const TInt KSocketWriteBufferSize = 1024;
const TInt KTransferBufferSize = 8 * 1024;

// CLASS DECLARATION
class CSocketsWriter : public CActive, public MTimeOutNotifier
    {
    public:

        static CSocketsWriter* NewL( MSocketObserver& aEngineNotifier,
                                     RSocket& aSocket );
        virtual ~CSocketsWriter();

        /**
         * Add data to iTransferBuffer and start sending if not already sending
         * Leaves with KErrOverflow, if there is not enough free space.
         * This can be called as long as there is space in buffer.
         */
        void SendL(const TDesC8& aData);

        void CancelSending();

        /**
        * Return number of bytes that can be added to iTransferBuffer.
        */
        TInt FreeSpaceInSendBuffer();

        TInt SendBufferMaxSize();

    protected: // from CActive

        void DoCancel();
        void RunL();

    protected: // from MTimeOutNotifier

        void TimerExpired();

    private: // Constructors and destructors

        CSocketsWriter( MSocketObserver& aEngineNotifier, RSocket& aSocket );
        void ConstructL();

    private: // New functions

        void SendNextPacket();

    private: // Enumerations

        enum TWriteState
            {
            ESending,
            EIdle // nothing to be sent
            };

    private: // Data

        RSocket& iSocket;
        MSocketObserver& iObserver;
        TBuf8<KTransferBufferSize> iTransferBuffer;
        TBuf8<KSocketWriteBufferSize> iWriteBuffer;
        CTimeOutTimer* iTimer;
        TInt iTimeOut; // microseconds
        TWriteState iWriteStatus;
    };

#endif // __SOCKETSWRITER_H__

// End of File
