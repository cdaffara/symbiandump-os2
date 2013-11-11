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
* Description:  Reads from socket.
*
*/


#ifndef __SOCKETSREADER_H__
#define __SOCKETSREADER_H__

// INCLUDES
#include <in_sock.h>

// FORWARD DECLARATIONS
class MSocketObserver;

// CLASS DECLARATION

const TInt KReadBufferMaxSize = 2 * 1024;

class CSocketsReader : public CActive
    {
    public: // Constructors and destructors

        static CSocketsReader* NewL( MSocketObserver& aObserver,
                                     RSocket& aSocket );
        virtual ~CSocketsReader();

    public: // New functions

        void ReadAsync(); // Use Cancel() to cancel

    protected: // from CActive

        void DoCancel();
        void RunL();

    private: // Constructors and destructors

        CSocketsReader( MSocketObserver& aObserver, RSocket& aSocket );
        void ConstructL();

    private: // New functions

        void IssueRead();

    private: // Data

        RSocket& iSocket;
        MSocketObserver& iObserver;
        TBuf8<KReadBufferMaxSize> iBuffer;
        TSockXfrLength iLen;
    };

#endif // __SOCKETSREADER_H__

// End of File
