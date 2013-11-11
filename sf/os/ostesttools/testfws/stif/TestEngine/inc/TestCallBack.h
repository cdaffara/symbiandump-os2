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
* CTestCallBack class.
*
*/

#ifndef TESTCALLBACK_H
#define TESTCALLBACK_H

//  INCLUDES
#include <e32base.h>

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
// Simple class for callbacks.

class CCallBack 
    :public CAsyncCallBack
    {
    public: // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor
        /**
        * C++ constructor.
        */
        //CCallBack( TInt aPriority ):CAsyncCallBack( aPriority ) {}

        /**
        * C++ constructor.
        */
        CCallBack( TCallBack& aCallBack, TInt aPriority ):
            CAsyncCallBack( aCallBack, aPriority ) {}

        /**
        * C++ destructor.
        */
        ~CCallBack(){ Cancel(); }

    public: // New functions
        /**
        * Return reference to TRequestStatus member.
        */
        TRequestStatus& Status(){ return iStatus; }

        /**
        * Set CCallBack active.
        */
        void SetActive()
            { 
            iStatus = KRequestPending; 
            CActive::SetActive(); 
            };

    public: // Functions from base classes

        /**
        * Handle completion.
        */
        void RunL(){ iCallBack.CallBack(); }

    protected:  // New functions

    protected:  // Functions from base classes

    private:

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // TESTCALLBACK_H

// End of File
