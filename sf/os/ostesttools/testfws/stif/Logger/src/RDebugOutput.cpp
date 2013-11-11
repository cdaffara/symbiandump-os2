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
* Description: This module contains implementation of CRDebugOutput 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "Output.h"
#include "RDebugOutput.h"
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

    Class: CRDebugOutput

    Method: CRDebugOutput

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: TLoggerType aLoggerType: in: File type
                TBool aWithTimeStamp: in: For timestamp
                TBool aWithLineBreak: in: For line break
                TBool aWithEventRanking: in: For events ranking to file

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CRDebugOutput::CRDebugOutput( CStifLogger::TLoggerType aLoggerType,
                                TBool aWithTimeStamp,
                                TBool aWithLineBreak,
                                TBool aWithEventRanking ) :
    COutput()
    {
    // Indicates file type
    iLoggerType = aLoggerType;

    // Time stamp indicator
    iWithTimeStamp = aWithTimeStamp;

    // Line break indicator
    iWithLineBreak = aWithLineBreak;

    // Log event ranking indicator
    iWithEventRanking = aWithEventRanking;

    // Data buffer
    iDataHBuf = NULL;
    iDataHBuf1 = NULL;
    iDataHBuf2 = NULL;

    }

/*
-------------------------------------------------------------------------------

    Class: CRDebugOutput

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CRDebugOutput::ConstructL( TInt aStaticBufferSize )
    {
    if( aStaticBufferSize != 0)
        {
        iDataHBuf1 = HBufC::NewL( aStaticBufferSize );      
        iDataHBuf2 = HBufC::NewL( aStaticBufferSize );      
        iStaticBufferSize = aStaticBufferSize;
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CRDebugOutput

    Method: NewL

    Description: Two-phased constructor.

    Parameters: const TDesC& aTestPath: in: Log path
                const TDesC& aTestFile: in: Log filename
                TLoggerType aLoggerType: in: File type
                TBool aOverWrite: in: For file overwrite
                TBool aWithTimeStamp: in: For timestamp
                TBool aWithLineBreak: in: For line break
                TBool aWithEventRanking: in: For events ranking to file
                TBool aThreadIdToLogFile: in: Indicator to thread id adding to
                                              end of the log file
                TBool aCreateLogDir: in: Indicator to directory creation

    Return Values: CRDebugOutput*: pointer to CFileOutput object

    Errors/Exceptions: Leaves if ConstructL leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
CRDebugOutput* CRDebugOutput::NewL( const TDesC& /* aTestPath */,
                                    const TDesC& /* aTestFile */,
                                    CStifLogger::TLoggerType aLoggerType,
                                    TBool /* aOverWrite */,
                                    TBool aWithTimeStamp,
                                    TBool aWithLineBreak,
                                    TBool aWithEventRanking,
                                    TBool /* aThreadIdToLogFile */,
                                    TBool /* aCreateLogDir */,
                                    TInt aStaticBufferSize)
    {
    // Create CFileOutput object fileWriter
    CRDebugOutput* rdebugWriter = new (ELeave) CRDebugOutput( 
                                                        aLoggerType,
                                                        aWithTimeStamp,
                                                        aWithLineBreak,
                                                        aWithEventRanking );

    CleanupStack::PushL( rdebugWriter );
    rdebugWriter->ConstructL( aStaticBufferSize );
    CleanupStack::Pop( rdebugWriter );

    return rdebugWriter;

    }

/*
-------------------------------------------------------------------------------

    Class: CRDebugOutput

    Method: ~CRDebugOutput

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CRDebugOutput::~CRDebugOutput()
    {
    delete iDataHBuf1;
    delete iDataHBuf2;
    }

/*
-------------------------------------------------------------------------------

    Class: CRDebugOutput

    Method: Write

    Description: Write log information or data to the file. 16 bit.

    Parameters: TBool aWithTimeStamp: in: Is time stamp flag used
                TBool aWithLineBreak: in: Is line break flag used
                TBool aWithEventRanking: in: Is event ranking flag used
                const TDesC& aData: in: Logged or saved data

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CRDebugOutput::Write( TBool aWithTimeStamp,
                            TBool aWithLineBreak,
                            TBool aWithEventRanking,
                            const TDesC& aData )
    {
    TInt currentEndPos( 0 );        // Current position in aData parsing
                                    // to partitions

    // a logInfo size must be at least the minimumSpace size
    TInt minimumSpace = ( KMaxTimeStamp + KMaxHtmlLineBreak + KMaxLineBreak +
                            KMaxEventRanking + KMaxSpace );
        
    TPtr logInfo(0, 0);
    HBufC* dataHBuf = NULL;    
    
    if(iDataHBuf1 != NULL)                            
        {
        TInt messageLength = aData.Length() + minimumSpace;
        if( messageLength > iStaticBufferSize )
            {
             RDebug::Print(_L("Warning: buffer size too small, not able to log!"));
             return KErrOverflow;
            }
        // Using the HBufC created in ConstructL
        logInfo.Set( iDataHBuf1->Des() ); // Memory allocation for data
        
        }
    else
        {
        // Buffer size is minimum space plus aData(aData length may be 0 but
        // time stamp is used)
        if( ( aData.Length() + minimumSpace ) < ( KMaxLogData + minimumSpace ) )
            {
            dataHBuf = HBufC::New( aData.Length() + minimumSpace );
            }
        // Buffer size is over KMaxLogData
        else
            {
            dataHBuf = HBufC::New( aData.Length() );
            }
        if( dataHBuf == NULL )
            {
            return KErrNoMemory;
            }
        logInfo.Set( dataHBuf->Des() ); // Memory allocation for data
        }

    logInfo.Copy( aData );          // Copy data
    
    TPtr data( 0, 0 );

    // Add time stamp and/or event ranking if required
    EventAndTimeCombiner( logInfo, data, aWithTimeStamp,
                            aWithLineBreak, aWithEventRanking );

    while( data.Length() > currentEndPos )
        {
        // In this loop round data length is/under 254
        if( ( data.Length() - currentEndPos ) <= KMaxRDebug )
            {
            // Current positions to rest of data position
            logInfo = data.Mid( currentEndPos, ( data.Length() - currentEndPos ) );
            // Prevent rdebug from parsing text string as formating string
            RDebug::Print( _L( "%S" ), &logInfo );
            }
        // In this loop round data length is over 254
        else
            {
            // Current positions to KMaxRDebug that includes space for ' ->'
            logInfo = data.Mid( currentEndPos, KMaxRDebug );
            // Prevent rdebug from parsing text string as formating string
            RDebug::Print( _L( "%S ->" ), &logInfo );
            }
        currentEndPos = currentEndPos + KMaxRDebug;
        }

    delete dataHBuf;

    if(iDataHBuf2 == NULL)
        {
        delete iDataHBuf;
        iDataHBuf = NULL;
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CRDebugOutput

    Method: Write

    Description: Write log information or data to the file. 8 bit.

    Parameters: TBool aWithTimeStamp: in: Is time stamp flag used
                TBool aWithLineBreak: in: Is line break flag used
                TBool aWithEventRanking: in: Is event ranking flag used
                const TDesC8& aData: in: Logged or saved data

    Return Values: TInt

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CRDebugOutput::Write( TBool aWithTimeStamp,
                            TBool aWithLineBreak,
                            TBool aWithEventRanking,
                            const TDesC8& aData )
    {
    TInt currentEndPos( 0 );        // Current position in aData parsing
                                    // to partitions
    TPtr logInfo(0, 0);

    // a logInfo size must be at least the minimumSpace size
    TInt minimumSpace = ( KMaxTimeStamp + KMaxHtmlLineBreak + KMaxLineBreak +
                            KMaxEventRanking + KMaxSpace );
    HBufC* dataHBuf = NULL;
    if(iDataHBuf1 != NULL)                            
        {
        if( ( aData.Length() + minimumSpace ) > iStaticBufferSize )
            {
             RDebug::Print(_L("Warning: buffer size too small, not able to log!"));
             return KErrOverflow;
            }       
        logInfo.Set( iDataHBuf1->Des() ); // Memory allocation for data
        }
    else
        {
        // 8 bit data to 16 bit because RDebug::Print don't support 8 bit printing
        
        // Buffer size is minimum space plus aData(aData length may be 0 but
        // time stamp is used)
        if( ( aData.Length() + minimumSpace ) < ( KMaxLogData + minimumSpace ) )
            {
            dataHBuf = HBufC::New( aData.Length() + minimumSpace );
            }
        // Buffer size is over KMaxLogData
        else
            {
            dataHBuf = HBufC::New( aData.Length() );
            }
        if( dataHBuf == NULL )
            {
            return KErrNoMemory;
            }
        logInfo.Set( dataHBuf->Des() ); // Memory allocation for data
        }
        
    logInfo.Copy( aData );          // Copy data

    TPtr data( 0, 0 );

    // Add time stamp and/or event ranking if required
    EventAndTimeCombiner( logInfo, data, aWithTimeStamp,
                            aWithLineBreak, aWithEventRanking );
    while( data.Length() > currentEndPos )
        {
        // In this loop round data length is/under 254
        if( ( data.Length() - currentEndPos ) <= KMaxRDebug )
            {
            // Current positions to rest of data position
            logInfo = data.Mid( currentEndPos, ( data.Length() - currentEndPos ) );
            // Prevent rdebug from parsing text string as formating string
            RDebug::Print( _L( "%S" ), &logInfo );
            }
        // In this loop round data length is over 254
        else
            {
            // Current positions to KMaxRDebug that includes space for ' ->'
            logInfo = data.Mid( currentEndPos, KMaxRDebug );
            // Prevent rdebug from parsing text string as formating string
            RDebug::Print( _L( "%S ->" ), &logInfo );
            }
        currentEndPos = currentEndPos + KMaxRDebug;
        }

    delete dataHBuf;

    if(iDataHBuf2 == NULL)
       {
       delete iDataHBuf;
       iDataHBuf = NULL;
       }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CRDebugOutput

    Method: EventAndTimeCombiner    

    Description: Event ranking and time stamp combimer.

    Parameters: TLogInfo aLogInfo: inout: Overflowed data
                TPtr8& aData: inout: Combined data
                TBool aWithTimeStamp: in: Is time stamp flag used
                TBool aWithLineBreak: in: Is line break flag used
                TBool aWithEventRanking: in: Is event ranking flag used

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CRDebugOutput::EventAndTimeCombiner( TPtr& aLogInfo,
                                            TPtr& aData,
                                            TBool aWithTimeStamp,
                                            TBool aWithLineBreak,
                                            TBool aWithEventRanking )
    {
    TInt extraLength( 0 );

    // Extra space calculation
    if( iWithEventRanking && aWithEventRanking )
        {
        extraLength = extraLength + KMaxEventRanking + KMaxSpace;
        }
    if( ( iLoggerType == CStifLogger::EHtml ) && iWithLineBreak && aWithLineBreak )
        {
        extraLength = extraLength + KMaxHtmlLineBreak;
        }
    if( iWithLineBreak && aWithLineBreak )
        {
        extraLength = extraLength + KMaxLineBreak;
        }
    if( aWithTimeStamp && iWithTimeStamp )
        {
        extraLength = extraLength + KMaxTimeStamp;
        }

    // Calculated space
    TInt space = aLogInfo.Length();

    if(iDataHBuf2 != NULL)                            
        {
        aData.Set( iDataHBuf2->Des() );  // Memory allocation for data
        aData.Copy( _L("") ); // Initialize aData buffer
        }
    else
    {        
        iDataHBuf = HBufC::New( space + extraLength );
        if( iDataHBuf == NULL )
            {
            return;
            }
        aData.Set( iDataHBuf->Des() );  // Memory allocation for data
    }

    // Event ranking
    if( iWithEventRanking && aWithEventRanking && !iWithTimeStamp )
        {
        EventRanking( aData );          // Event ranking to data
        aData.Append( aLogInfo );       // Unicode aData to normal text
        }
    // Time stamp
    else if( iWithTimeStamp && aWithTimeStamp )
        {
        // With event ranking
        if( iWithEventRanking && aWithEventRanking )
            {
            EventRanking( aData );      // Event ranking to data
            }
        AddTimeStampToData( aData );    // Add time stamp
        aData.Append( aLogInfo );       // Unicode aData to normal text
        }
    else
        {
        aData.Copy( aLogInfo );         // Unicode aData to normal text
        }

    // NOTE: If need some special line break do it with logging phase

    // If html logging and line break is used add the line break.
    if ( ( iLoggerType == CStifLogger::EHtml ) && iWithLineBreak && aWithLineBreak )
        {
        aData.Append( _L( "<BR>" ) );
        aData.Append( _L( "\n" ) );  // To make html file more readable
                                    // with text editor
        }

    // Other cases line break is normal '\n' if logging is used
    else if( iWithLineBreak && aWithLineBreak )
        {
        aData.Append( 0x0D ); // 13 or '\' in Symbian OS
        aData.Append( 0x0A ); // 10 or 'n' in Symbian OS
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: EventRanking

    Description: For event ranking logging.

    Parameters: TPtr8& aData: inout: Data with event ranking

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CRDebugOutput::EventRanking( TPtr& aData )
    {
    // Returns the machine's current tick count.
    TUint tick = User::TickCount();
    tick &= 0xffff;
    aData.AppendNum( tick );
    aData.Append( _L( " " ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: DateAndTime

    Description: For date and time logging.

    Parameters: TPtr8& aData: inout: Data with time stamps

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CRDebugOutput::AddTimeStampToData( TPtr& aData )
    {
    TStifLoggerTimeStamp8 timeStamp;
    GetDateAndTime( timeStamp );
    aData.Append( timeStamp );

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: GetDateAndTime

    Description: For date and time logging.

    Parameters: TStifLoggerTimeStamp& aDataAndTime: inout: Generated date and time

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CRDebugOutput::GetDateAndTime( TStifLoggerTimeStamp8& aDataAndTime )
    {
    TInt num( 0 );
    TInt microseconds( 0 );

    // Append date
    TTime time;
    time.HomeTime();
    TDateTime dateTime( time.DateTime() );

    num = dateTime.Day() + 1;
    if( num < 10 )
        {
        aDataAndTime.Append( '0' );
        }
    // Append month
    aDataAndTime.AppendNum( num );  
    num = dateTime.Month() + 1;     // returns enum
    if( num < 10 )
        {
        aDataAndTime.Append( '0' );
        }
    aDataAndTime.AppendNum( num );
    aDataAndTime.AppendNum( dateTime.Year() );

    // Append time
    aDataAndTime.Append( ' ' );
    num = dateTime.Hour();
    if( num < 10 )
        {
        aDataAndTime.Append( '0' );
        }
    aDataAndTime.AppendNum( num );
    num = dateTime.Minute();
    if( num < 10 )
        {
        aDataAndTime.Append( '0' );
        }
    aDataAndTime.AppendNum( num );

    num = dateTime.Second();
    microseconds = dateTime.MicroSecond();  // Microseconds

    // Seconds
    if( num < 10 )
        {
        aDataAndTime.Append( '0' );
        }
    aDataAndTime.AppendNum( num );

   // Milliseconds
    aDataAndTime.Append( '.' );
    aDataAndTime.AppendNumFixedWidth( microseconds, EDecimal, 3 );

    // NOTE: If you add more spaces etc. remember increment KMaxTimeStamp size!

    if ( iLoggerType == CStifLogger::EHtml )
        {
        aDataAndTime.Append( _L("....") );
        }
    else
        {
        aDataAndTime.Append( _L("    ") );
        }

    }

// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
