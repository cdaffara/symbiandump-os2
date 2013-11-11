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
* Description: This file contains the header file of the 
* CUIEngineRemote.
*
*/


#ifndef STIF_TFW_IF_REMOTE_H
#define STIF_TFW_IF_REMOTE_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <stifinternal/UIEngineContainer.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None


// FORWARD DECLARATIONS
// None


// CLASS DECLARATION


// DESCRIPTION

// CUIEngineRemote is a class that is used for remote protocol 
// forwarding.
class CUIEngineRemote
        :public CActive
    {
    public: // Enumerations
        // None

    private: // Enumerations
        enum TRemoteStatus
            {
            EIdle,
            EPending,
            EUIMsgPending,
            };

    public: // Constructors and destructor

        /**
        * NewL is two-phased constructor.
        */
        static CUIEngineRemote* NewL( CUIEngineContainer* aUIEngineContainer );

        /**
        * Destructor of CUIEngineRemote.
        */
        ~CUIEngineRemote();

    public: // New functions

        /**
        * StartL starts testing.
        */
        void StartL( RTestCase& aTestCase );

    public: // Functions from base classes

        /**
        * From CActive RunL handles request completion.
        */
        void RunL();

        /**
        * From CActive DoCancel handles request cancellation.
        */
        void DoCancel();
        /**
        * RunError derived from CActive handles errors from active object 
        * handler.
        */        
        TInt RunError(TInt aError);

    protected: // New functions
        // None

    protected: // Functions from base classes
        // None

    private: // New functions

        /** 
        * C++ default constructor.
        */
        CUIEngineRemote( CUIEngineContainer* aUIEngineContainer);

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();


        /**
        * Start request.
        */
        void Start();

    public: //Data
        // None
    
    protected: // Data
        // None

    private: // Data

        // Pointer to UIEngine
        CUIEngineContainer*             iUIEngineContainer;

        // Handle to Test Case
        RTestCase                       iTestCase;
        
        // Protocol message type
        TStifCommand                    iRemoteType;
        TStifCommandPckg                iRemoteTypePckg;

        // Protocol message length
        TInt                            iMsgLen;
        TPckg<TInt>                     iMsgLenPckg;

        TRemoteStatus                   iState;
    public: // Friend classes
        // None

    protected: // Friend classes
        // None

    private: // Friend classes
        // None

    };



#endif // STIF_TFW_IF_REMOTE_H

// End of File
