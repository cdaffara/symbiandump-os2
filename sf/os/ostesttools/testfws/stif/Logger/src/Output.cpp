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
* Description: This module contains implementation of COutput 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "Output.h"
#include "FileOutput.h"
#include "NullOutput.h"
#include "LoggerOverFlow.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: COutput

    Method: COutput

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
COutput::COutput()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: COutput

    Method: NewL

    Description: Two-phased constructor.

    NOTE: At the moment there is only one output module implemented

    Parameters: const TDesC& aTestPath: in: Path to logged information
                const TDesC& aTestFile: in: Log name for information
                TLoggerType aLoggerType: in: Log file type
                TOutput aOutput: in: Output source
                TBool aOverWrite: in: Indicator to file overwrite
                TBool aWithTimeStamp: in: Indicator to time stamp
                TBool aWithLineBreak: in: Indicator to line break
                TBool aWithEventRanking: in: Indicator to event ranking
                TBool aThreadIdToLogFile: in: Indicator to thread id adding to
                                              end of the log file
                TBool aCreateLogDir: in: Indicator to directory creation
                TInt  aStaticBufferSize
                TBool aUnicode: in: Indicator if file has to be in unicode format

    Return Values: COutput*: pointer to COutput object

    Errors/Exceptions:  Leaves if called CFileOutput::NewL method fails

    Status: Proposal

-------------------------------------------------------------------------------
*/
COutput* COutput::NewL( const TDesC& aTestPath,
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
                        TBool aUnicode )
    {

    if ( aOutput == CStifLogger::EFile )
        {
        CFileOutput* fileOutput = NULL;
        // Create CFileOutput object fileOutput
        TRAPD( err, fileOutput = CFileOutput::NewL( aTestPath,
                                                    aTestFile,
                                                    aLoggerType,
                                                    aOverWrite,
                                                    aWithTimeStamp,
                                                    aWithLineBreak,
                                                    aWithEventRanking,
                                                    aThreadIdToLogFile,
                                                    aCreateLogDir,
                                                    aStaticBufferSize,
                                                    aUnicode ) );

        // Probably path or file name is over allowed size
        if ( err == KErrArgument )
            {
            User::Leave( KErrArgument );
            return NULL; // Never return because leave above
            }
        // Not enough memory, @js
        else if ( err == KErrNoMemory )
            {
            User::Leave( KErrNoMemory );
            return NULL;
            }                      
        // If e.g. path not found so we create CNullOutput to avoiding leave
        // operation. CNullOutput looks logging although it won't.
        else if ( err != KErrNone )
            {
            CNullOutput* nullOutput = CNullOutput::NewL();
            nullOutput->SetOutputCreationResult( err, aOutput );
            return nullOutput;
            }
        // CFileOutput creation made succesfully
        else
            {
            fileOutput->SetOutputCreationResult( err, aOutput );
            return fileOutput;
            }
        }
    else if ( aOutput == CStifLogger::ERDebug )
        {
        CRDebugOutput* rdebugOutput = NULL;
        // Create CRDebugOutput object rdebugOutput
        TRAPD( err, rdebugOutput = CRDebugOutput::NewL( aTestPath, 
                                                        aTestFile,
                                                        aLoggerType,
                                                        aOverWrite,
                                                        aWithTimeStamp,
                                                        aWithLineBreak,
                                                        aWithEventRanking,
                                                        aThreadIdToLogFile,
                                                        aCreateLogDir,
                                                        aStaticBufferSize ) );

        // Probably path or file name is over allowed size
        if ( err == KErrArgument )
            {
            User::Leave( KErrArgument );
            return NULL; // Never return because leave above
            }
        // If e.g. no memory so we create CNullOutput to avoiding leave
        // operation. CNullOutput looks logging although it won't.
        else if ( err != KErrNone )
            {
            CNullOutput* nullOutput = CNullOutput::NewL();
            nullOutput->SetOutputCreationResult( err, aOutput );
            return nullOutput;
            }
        // CRDebugOutput creation made succesfully
        else
            {
            rdebugOutput->SetOutputCreationResult( err, aOutput );
            return rdebugOutput;
            }
        }
    else
        {
        User::Leave( KErrArgument );
        return NULL; // Never return because leave above
        }

    }

/*
-------------------------------------------------------------------------------

    Class: COutput

    Method: ~COutput

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
COutput::~COutput()
    {

    }

// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
