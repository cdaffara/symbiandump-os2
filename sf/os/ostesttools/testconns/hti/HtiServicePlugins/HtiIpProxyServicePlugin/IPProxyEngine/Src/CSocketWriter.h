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
* Description:  Socket writer
*
*/



#ifndef CSOCKETWRITER_H
#define CSOCKETWRITER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KWriteBufferSize = 128;

// FORWARD DECLARATIONS
class MSocketWriterObserver;
class RSocket;
class CDesC8Array;

// CLASS DECLARATION

/**
*  Socket writer. Asynchronously writes data to opened socket.
*/
NONSHARABLE_CLASS( CSocketWriter ) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aSocket RSocket instance which is used to write data.
        * @param aUDPRemotePort Remote UDB port.
        * @return New CSocketWriter instance.
        */
        static CSocketWriter* NewL( RSocket& aSocket,
            TInt aUDPRemotePort = -1 );

        /**
        * Factory function.
        * @param aSocket RSocket instance which is used to write data.
        * @param aUDPRemotePort Remote UDB port.
        * @return New CSocketWriter instance.
        */
        static CSocketWriter* NewLC( RSocket& aSocket,
            TInt aUDPRemotePort = -1 );

        /**
        * Destructor.
        */
        ~CSocketWriter();

    public: // New functions

        /**
        * Sets observer.
        * @param aObserver Pointer to observer.
        */
        void SetObserver( MSocketWriterObserver* aObserver );

        /**
        * Issues writing data to socket. If the request is already
        * pending the data will be written to queue and it will be
        * handled afterwards.
        * @param aData Data to be written.
        */
        void IssueWriteL( const TDesC8& aData );

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
        */
        CSocketWriter( RSocket& aSocket, TInt aUDPRemotePort = -1 );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:    // Data
        CDesC8Array* iTransferBufferArray;
        MSocketWriterObserver* iObserver;
        RSocket& iSocket;
        TBuf8<KWriteBufferSize> iWriteBuffer;
        RTimer iTimer;
        TBool iWaiting;
        const TInt iUDPRemotePort;
    };

#endif      // CSOCKETWRITER_H

// End of File
