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
* Description: This module contains implementation of CDataLogger 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "DataLogger.h"
#include "Output.h"

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

    Class: CDataLogger

    Method: CDataLogger

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: COutput* output: in: Output source

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CDataLogger::CDataLogger( COutput* output )
    {
    	iOutput = output;
    }

/*
-------------------------------------------------------------------------------

    Class: CDataLogger

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CDataLogger::ConstructL()
    {
    
    }

/*
-------------------------------------------------------------------------------

    Class: CDataLogger

    Method: NewL

    Description: Two-phased constructor.

    Parameters: const TDesC& aTestPath: in: Log path
                const TDesC& aTestFile: in: Log filename
                TLoggerType aLoggerType: in: File type
                TOutput aOutput: in: Output source
                TBool aOverWrite: in: For file overwrite
                TBool aWithTimeStamp: in: For timestamp
                TBool aWithLineBreak: in: For line break
                TBool aWithEventRanking: in: For events ranking to file
                TBool aThreadIdToLogFile: in: Indicator to thread id adding to
                                              end of the log file
                TBool aCreateLogDir: in: Indicator to directory creation
                TInt  aStaticBufferSize
                TBool aUnicode: not used: Indicator if file has to be in unicode format.

    Return Values: CDataLogger*: pointer to CDataLogger object

    Errors/Exceptions: Leaves if called COutput::NewL method fails

    Status: Proposal

-------------------------------------------------------------------------------
*/
CDataLogger* CDataLogger::NewL( const TDesC& aTestPath,
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
                                TBool /*aUnicode*/ )
    {
    __TRACE( KInfo, ( _L( "STIFLOGGER: Creating data logger" ) ) );

    aWithTimeStamp = EFalse;    // Time stamp not allowed in data logging
    aWithLineBreak = EFalse;    // Line break not allowed in data logging
    aWithEventRanking = EFalse; // Event ranking not allowed in data logging

    TBool aUnicode(EFalse); //For data type unicode is not allowed
    // Create COutput object output
    COutput* output = COutput::NewL( aTestPath,
                                        aTestFile,
                                        aLoggerType,
                                        aOutput,
                                        aOverWrite,
                                        aWithTimeStamp,
                                        aWithLineBreak,
                                        aWithEventRanking,
                                        aThreadIdToLogFile,
                                        aCreateLogDir,
                                        aStaticBufferSize,
                                        aUnicode );
                                        
    CleanupStack::PushL( output ); 

	__ASSERT_ALWAYS( output != NULL, User::Leave( KErrNotFound ) );
    // Create CDataLogger object dataLogger and bind to COutput
    CDataLogger* dataLogger = new (ELeave) CDataLogger( output );
    // DATA logger owns output object and it will be destroyed in STIFLogger
    // destructor.
    CleanupStack::Pop( output ); 

    CleanupStack::PushL( dataLogger );
    dataLogger->ConstructL();
    CleanupStack::Pop( dataLogger );
    
    
    return dataLogger;

    }

/*
-------------------------------------------------------------------------------

    Class: CDataLogger

    Method: ~CDataLogger

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CDataLogger::~CDataLogger()
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CDataLogger

    Method: Send

    Description: Send style information and 16 bit data to the output module.

    Parameters: TInt TStyle: in: Text forming
                const TDesC& aData: in: Data to be logged

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDataLogger::Send( TInt /*TStyle*/, const TDesC& aData )
    {
    // No text styling implemented
    return iOutput->Write( EFalse, ETrue, EFalse, aData );

    }

/*
-------------------------------------------------------------------------------

    Class: CDataLogger

    Method: Send

    Description: Send style information and 8 bit data to the output module.

    Parameters: TInt TStyle: in: Text forming
                const TDesC8& aData: in: Data to be logged

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDataLogger::Send( TInt /*TStyle*/, const TDesC8& aData )
    {
    // No text styling implemented
    return iOutput->Write( EFalse, ETrue, ETrue, aData );

    }

// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
