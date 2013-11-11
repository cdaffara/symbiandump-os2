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
* Description:  CHtiBtCommServer declaration
*
*/


#ifndef __HTIBTCOMMSERVER_H__
#define __HTIBTCOMMSERVER_H__

//  INCLUDES

#include <e32def.h>
#include <e32base.h>

//  CONSTANTS

// Reasons for BtCommServer panic
enum TBtCommServerPanic
    {
    EBadRequest,
    EBadDescriptor,
    EMainSchedulerError,
    ESvrCreateServer,
    ESvrStartServer,
    ECreateTrapCleanup,
    ENotImplementedYet,
    EFailedToInitMessageBox,
    EFailedToOpenBTconnection,
    EFailedToOpenPhonetChannel,
    EFailedToRecPhonetMsg,
    EFailedToSendPhonetMsg,
    ETooManyAsyncRequests
    };

//  FUNCTION PROTOTYPES

// Function to panic the server
GLREF_C void PanicServer( TBtCommServerPanic aPanic );

// Thread function for server
GLREF_C TInt BtCommServerThread( TAny * );

//  FORWARD DECLARATIONS
class CHtiBtCommServerSession;
class CHtiBtReaderWriter;

// CLASS DECLARATION

/**
* Server class for BT communication.
*/
class CHtiBtCommServer : public CServer2
    {

    private: // Constructors and destructor
       /**
        * C++ default constructor.
        */
        CHtiBtCommServer( TInt aPriority );

       /**
        * Second Phase constructor
        */
        void ConstructL();

    public:
       /**
        * Symbian Two-phased constructor.
        */
        static CHtiBtCommServer* NewL();

       /**
        * Destructor.
        */
        ~CHtiBtCommServer();

    public: // callbacks from session

        /**
        * Session was deleted notification.
        * Session notifies server, when client is disconnecting
        * and session is being deleted. Server will start shutdown.
        */
        void SessionFreed();

        /**
        * Session was created notification.
        * @param aSession Session, which is notifying server of its creation.
        */
        void SessionCreated( CHtiBtCommServerSession* aSession );

    public:
       /**
        * Creates a new client session
        * @param aVersion Version of the server
        * @param aMessage Client message
        * @return Pointer to the client session
        */
        virtual CSession2* NewSessionL( const TVersion& aVersion,
            const RMessage2& aMessage ) const;

       /**
        * Creates server thread
        * @param aStarted semaphore
        * @return Error code
        */
        static TInt ThreadFunction( TAny* aStarted );

public: // Data
    // Priority of the server
    enum TBtServPriority
        {
        EBtCommServerPriority = 100
        };

private:

    CHtiBtCommServerSession* iSession; // Not owned. Only one session allowed!

    };

#endif // __HTIBTCOMMSERVER_H__

// End of file
