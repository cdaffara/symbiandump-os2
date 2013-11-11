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
* CUIStorePopup.
*
*/

#ifndef STIF_TFW_IF_REMOTE_H
#define STIF_TFW_IF_REMOTE_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <stifinternal/UIStore.h>

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

// CUIStorePopup is a class that is used for remote protocol 
// forwarding.
class CUIStorePopup
        :public CActive
    {
    public: // Enumerations
        // None

    private: // Enumerations
        // None

    public: // Constructors and destructor

        /**
        * NewL is two-phased constructor.
        */
        static CUIStorePopup* NewL( CUIStore* aUIStore,
                                    CUIEngineContainer* aContainer,
                                    CStifTFwIfProt* aRemoteMsg,
                                    CUIStore::TPopupPriority aPopupPriority,
                                    const TDesC& aMsg );

        /**
        * Destructor of CUIStorePopup.
        */
        ~CUIStorePopup();

    public: // New functions
    
        TInt Start( const TDesC& aLine1, 
                    const TDesC& aLine2 );
                    
        inline const CUIEngineContainer* Container()
            { return iContainer; };
        
        inline TBool IsEventPopup()
            { return ( iRemoteMsg->iCmdType == 
                       CStifTFwIfProt::ECmdRequest ); }
        
        // Returns message
        inline TDesC& GetMessage()
            { 
            if( iMsg == NULL )
                {
                User::Panic( _L("iMsg was NULL"), -1 );
                }
            return *iMsg;
            };

        // Returns popup priority
        inline const CUIStore::TPopupPriority GetPriority()
            { 
            return iPopupPriority;
            };
        
    public: // Functions from base classes

        /**
        * From CActive RunL handles request completion.
        */
        void RunL();

        /**
        * From CActive DoCancel handles request cancellation.
        */
        void DoCancel();

    protected: // New functions
        // None

    protected: // Functions from base classes
        // None

    private: // New functions

        /** 
        * C++ default constructor.
        */
        CUIStorePopup( CUIStore* aUIStore,
                       CUIEngineContainer* aContainer,
                       CStifTFwIfProt* aRemoteMsg,
                       CUIStore::TPopupPriority aPopupPriority);

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( const TDesC& aMsg );


    public: //Data
        // None
    
    protected: // Data
        // None

    private: // Data

        // Pointer to UIEngine
        CUIStore*                       iUIStore;

        // Protocol message
        CStifTFwIfProt*                 iRemoteMsg;
        
        // Keycode of the pressed key
        TKeyCode                        iKey;
        
        // test case waiting our response
        CUIEngineContainer*             iContainer;
        
        TInt                            iPopupWindow;
        
        // Popup priority
        CUIStore::TPopupPriority        iPopupPriority;
        
        // Popup message
        HBufC                           * iMsg;
        
    public: // Friend classes
        // None

    protected: // Friend classes
        // None

    private: // Friend classes
        // None

    };



#endif // STIF_TFW_IF_REMOTE_H

// End of File
