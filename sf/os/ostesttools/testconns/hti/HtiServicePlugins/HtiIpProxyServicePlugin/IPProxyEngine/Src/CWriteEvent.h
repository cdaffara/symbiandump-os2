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
* Description:  Write event for queuing data to be sent
*
*/



#ifndef CWRITEEVENT_H
#define CWRITEEVENT_H

//  INCLUDES
#include <e32base.h>
#include <es_sock.h>

// CLASS DECLARATION

/**
*  Write event for queuing data to be sent. The class only stores
*  data and ports.
*/
NONSHARABLE_CLASS( CWriteEvent ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Factory function.
        * @param aProtocolDesc Protocol description to store.
        * @param aData Data to store.
        * @param aPeerPort Peer port to store.
        * @param aOriginalPort Original port to store.
        */
        static CWriteEvent* NewL( TProtocolDesc aProtocolDesc,
            const TDesC8& aData, TInt aPeerPort, TInt aOriginalPort );

        /**
        * Factory function.
        * @param aProtocolDesc Protocol description to store.
        * @param aData Data to store.
        * @param aPeerPort Peer port to store.
        * @param aOriginalPort Original port to store.
        */
        static CWriteEvent* NewLC( TProtocolDesc aProtocolDesc,
            const TDesC8& aData, TInt aPeerPort, TInt aOriginalPort );

        /**
        * Destructor.
        */
        ~CWriteEvent();

    public: // New functions

        /**
        * @return Protocol description
        */
        TProtocolDesc ProtocolDesc() const;

        /**
        * @return Data
        */
        const TDesC8& Data() const;

        /**
        * @return Peer port
        */
        TInt PeerPort() const;

        /**
        * @return Original port
        */
        TInt OriginalPort() const;

    private:

        /**
        * Default constructor.
        * @param aProtocolDesc Protocol description to store.
        * @param aPeerPort Peer port to store.
        * @param aOriginalPort Original port to store.
        */
        CWriteEvent( TProtocolDesc aProtocolDesc, TInt aPeerPort, TInt aOriginalPort );

        /**
        * 2nd phase constructor.
        * @param aData Data to store
        */
        void ConstructL( const TDesC8& aData );


    private:    // Data
        HBufC8* iData;
        TInt iPeerPort;
        TInt iOriginalPort;
        TProtocolDesc iProtocolDesc;
    };

#endif      // CWRITEEVENT_H

// End of File
