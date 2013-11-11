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
* Description: This file contains the header file of the COutput.
*
*/

#ifndef OUTPUT_H
#define OUTPUT_H

//  INCLUDES

#include "StifLogger.h"
#include "LoggerTracing.h"

// CONSTANTS
// Maximum length of the event ranking
const TInt KMaxEventRanking = 5;

// Maximum length of the space
const TInt KMaxSpace = 1;

// Maximum length of the HTML line break('<BR>' + '\n')
const TInt KMaxHtmlLineBreak = 6;

// Maximum length of the line break(13 or '\' and 10 or 'n' in Symbian OS)
const TInt KMaxLineBreak = 2;

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
// COutput is a STIF Test Framework StifLogger class.
// Class contains a file logging operations.

class COutput
        :public CBase
    {
    public:     // Enumerations
            
    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static COutput* NewL( const TDesC& aTestPath,
                                const TDesC& aTestFile,
                                CStifLogger::TLoggerType aLoggerType,
                                CStifLogger::TOutput aOutput,
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
        virtual ~COutput();

    public:     // New functions

        /**
        * C++ default constructor.
        */
        COutput();

        /**
        * Pure virtual. Write log information or data to the file.
        * 16 bit.
        */
        virtual TInt Write( TBool /* aWithTimeStamp */,
                            TBool /* aWithTimeLineBreak */,
                            TBool /* aWithEventRanking */,
                            const TDesC& /* aData */ ) = 0;

        /**
        * Pure virtual. Write log information or data to the file.
        * 8 bit.
        */
        virtual TInt Write( TBool /* aWithTimeStamp */,
                            TBool /* aWithTimeLineBreak */,
                            TBool /* aWithEventRanking */,
                            const TDesC8& /* aData */ ) = 0;

        /**
        * Return output type.
        */
        inline TInt OutputCreationResult( CStifLogger::TOutput& aType )
            { 
            aType = iType;  
            return iCreationResult; 
            };
            
        /**
        * Set output type.
        */
        inline void SetOutputCreationResult( TInt aResult, 
                                                CStifLogger::TOutput aType )
            { 
            iType = aType;  
            iCreationResult = aResult; 
            };
        
    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

    public:     // Data

    protected:  // Data
        // Output type
        CStifLogger::TOutput    iType;
        TInt                    iCreationResult;

    private:    // Data        

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // OUTPUT_H

// End of File
