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
* TDesLoggerOverflowHandler and TDes8LoggerOverflowHandler.
*
*/

#ifndef LOGGEROVERFLOW_H
#define LOGGEROVERFLOW_H

//  INCLUDES
#include "StifLogger.h"
#include "Output.h"
#include "RDebugOutput.h"

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
// TDesLoggerOverflowHandler is a STIF Test Framework StifLogger class.
// Class contains an over flow operations.

class TDesLoggerOverflowHandler 
        :public TDes16Overflow
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

    public:     // New functions

        /**
        * C++ default constructor.
        */
        TDesLoggerOverflowHandler( CStifLogger* aLogger, TInt aOverFlowSource );

        /**
        * C++ default constructor.
        */
        // For future needs
        // TDesLoggerOverflowHandler( CRDebugOutput* aRDebugOutput,
        //                         TInt aOverFlowSource );

        /**
        * Simple overflow handling(16 bit)
        */
        void Overflow( TDes16& aDes );

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

    public:     // Data

        /**
        * Pointer to CStifLogger class object
        */
        CStifLogger* iLogger;

        /**
        * Pointer to CRDebugOutput class object
        */
        CRDebugOutput* iRDebugOutput;

        /**
        * Over flow source
        */
        TInt iOverFlowSource;

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

// CLASS DECLARATION

// DESCRIPTION
// TDes8LoggerOverflowHandler is a STIF Test Framework StifLogger class.
// Class contains an over flow operations.

class TDes8LoggerOverflowHandler 
        :public TDes8Overflow
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

    public:     // New functions

        /**
        * C++ default constructor.
        */
        TDes8LoggerOverflowHandler( CStifLogger* aLogger, TInt aOverFlowSource );

        /**
        * C++ default constructor.
        */
        // For future needs
        // TDes8LoggerOverflowHandler( CRDebugOutput* aRDebugOutput,
        //                        TInt aOverFlowSource );

        /**
        * Simple overflow handling(8 bit)
        */
        void Overflow( TDes8& aDes );

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

    public:     // Data

        /**
        * Pointer to CStifLogger class object
        */
        CStifLogger* iLogger;

        /**
        * Pointer to CRDebugOutput class object
        */
        // For future needs
        // CRDebugOutput* iRDebugOutput;

        /**
        * Over flow source
        */
        TInt iOverFlowSource;

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // LOGGEROVERFLOW_H

// End of File
