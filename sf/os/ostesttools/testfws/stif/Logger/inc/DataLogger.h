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
* Description: This file contains the header file of the CDataLogger.
*
*/

#ifndef DATALOGGER_H
#define DATALOGGER_H

//  INCLUDES
#include "StifLogger.h"
#include "FileOutput.h"
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
class COutput;

// CLASS DECLARATION

// DESCRIPTION
// CDataLogger is a STIF Test Framework StifLogger class.
// Class contains a file logging operations.

class CDataLogger
            :public CStifLogger
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDataLogger* NewL( const TDesC& aTestPath,
                                    const TDesC& aTestFile,
                                    TLoggerType aLoggerType,
                                    TOutput aOutput,
                                    TBool aOverWrite,
                                    TBool aWithTimeStamp,
                                    TBool aWithLineBreak,
                                    TBool aWithEventRanking,
                                    TBool aThreadIdToLogFile,
                                    TBool aCreateLogDir,
                                    TInt aStaticBufferSize,
                                    TBool aUnicode );

        /**
        * Destructor.
        */
        ~CDataLogger();

    public:     // New functions

        /**
        * Sent data to the output module. 16 bit. 
        * aStyle parameter is not supporter in data logging.
        */
        TInt Send( TInt /*aStyle*/, const TDesC& aData );

        /**
        * Sent data to the output module. 8 bit.
        * aStyle parameter is not supporter in data logging.
        */
        TInt Send( TInt /*aStyle*/, const TDesC8& aData );

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CDataLogger( COutput* output );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // DATALOGGER_H

// End of File
