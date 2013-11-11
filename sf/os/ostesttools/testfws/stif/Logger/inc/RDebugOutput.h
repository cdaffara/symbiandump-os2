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
* Description: This file contains the header file of the CRDebugOutput.
*
*/

#ifndef RDEBUGOUTPUT_H
#define RDEBUGOUTPUT_H

//  INCLUDES
#include <f32file.h>

#include "StifLogger.h"
#include "Output.h"
#include "LoggerTracing.h"

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
// CRDebugOutput is a STIF Test Framework StifLogger class.
// Class contains a RDebug logging operations.
class CRDebugOutput 
            :public COutput
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CRDebugOutput* NewL( const TDesC& /* aTestPath */,
                                    const TDesC& /* aTestFile */,
                                    CStifLogger::TLoggerType aLoggerType,
                                    TBool /* aOverWrite */,
                                    TBool aWithTimeStamp,
                                    TBool aWithLineBreak,
                                    TBool aWithEventRanking,
                                    TBool /* aThreadIdToLogFile */,
                                    TBool /* aCreateLogDir */,
                                    TInt aStaticBufferSize );

        /**
        * Destructor.
        */
        ~CRDebugOutput();

    public:     // New functions

        /**
        * Write RDebug information.
        * 16 bit.
        */
        TInt Write( TBool aWithTimeStamp,
                    TBool aWithLineBreak,
                    TBool aWithEventRanking,
                    const TDesC& aData );

        /**
        * Write RDebug information.
        * 8 bit.
        */
        TInt Write( TBool aWithTimeStamp,
                    TBool aWithLineBreak,
                    TBool aWithEventRanking,
                    const TDesC8& aData );

        /**
        * Event ranking and time stamp combimer.
        */
        void EventAndTimeCombiner( TPtr& aLogInfo,
                                    TPtr& aData,
                                    TBool aWithTimeStamp,
                                    TBool aWithLineBreak,
                                    TBool aWithEventRanking );

        /**
        * Add event ranking, specify number to line.
        */
        void EventRanking( TPtr& aData );
                    
        /**
        * Add date, time and line break to data.
        */
        void AddTimeStampToData( TPtr& aData );

        /**
        * Get date and time.
        */
        void GetDateAndTime( TStifLoggerTimeStamp8& aDataAndTime );

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CRDebugOutput( CStifLogger::TLoggerType aLoggerType,
                        TBool aWithTimeStamp,
                        TBool aWithLineBreak,
                        TBool aWithEventRanking );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TInt aStaticBufferSize );

    public:     // Data

    protected:  // Data

    private:    // Data

        /**
        * Data buffer
        */
        HBufC* iDataHBuf;

        /**
        * Indicates file type.
        */
        CStifLogger::TLoggerType iLoggerType;

        /**
        * Time stamp indicator.
        */
        TBool iWithTimeStamp;

        /**
        * Line break indicator.
        */
        TBool iWithLineBreak;

        /**
        * Event ranking indicator.
        */
        TBool iWithEventRanking;

        /**
        * DataHBuf for logging purposes
        */      
        HBufC * iDataHBuf1;      

        /**
        * DataHBuf for logging purposes
        */      
        HBufC * iDataHBuf2;      
        
        TInt iStaticBufferSize;
       
    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // RDEBUGOUTPUT_H

// End of File
