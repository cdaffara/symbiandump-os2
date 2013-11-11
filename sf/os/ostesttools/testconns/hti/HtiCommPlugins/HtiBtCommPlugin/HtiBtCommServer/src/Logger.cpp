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
* Description:  Logger implementation.
*
*/


// INCLUDE FILES
#include "Logger.h"
#include <f32file.h>

#ifdef __ENABLE_LOGGING__

// CONSTANTS
_LIT(KDebugStr, "DEBUG: ");
_LIT(KInfoStr,  "INFO:  ");
_LIT(KWarnStr,  "WARN:  ");
_LIT(KErrStr,   "ERROR: ");


/*---------------------------------------------------------------------------*/
void LogBytes( const TDesC8& aData, TInt aMaxBytesToLog )
    {
    TInt i = 0;
    while ( i < aData.Length() && i < aMaxBytesToLog )
        {
        TBuf<64> hexbuf;
        TBuf<32> stringbuf;
        TInt k=0;
        while ( i < aMaxBytesToLog && i < aData.Length() && k < 16 )
            {
            const TUint8 uint8 = aData[i];
            hexbuf.AppendFormat( _L( "%02X "), uint8 );

            if ( uint8 == '%' )
                {
                stringbuf.Append( _L( "%%" ) ); // escape character in format string
                }
            else if ( uint8 < 32 || uint8 > 126 )
                {
                stringbuf.Append( _L(".") );
                }
            else
                {
                stringbuf.Append( ( TChar ) uint8 );
                }
            i++;
            k++;
            }
        TBuf<128> finalbuf;
        finalbuf.Copy( hexbuf );
        finalbuf.Append( _L(" | " ) );
        finalbuf.Append( stringbuf );
        DebugLog( finalbuf );
        }
    }

/*---------------------------------------------------------------------------*/
class TOverflowHandler : public TDes16Overflow
    {
    void Overflow( TDes16& aDes )
        {
        TBuf<KLogEntryMaxLength> logString( _L("LOG ERROR: overflow: ") );
        logString.Append( aDes.Left(
            KLogEntryMaxLength - logString.Length() ) );
        aDes.Copy( logString );
        }
    };

/*---------------------------------------------------------------------------*/
// Usage example: DOLOG(_L("Logstring"));
void DebugLog( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    Log( KLogLevelDebug, aFmt, list );
    }

/*---------------------------------------------------------------------------*/
void InfoLog( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    Log( KLogLevelInfo, aFmt, list );
    }

/*---------------------------------------------------------------------------*/
void WarnLog( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    Log( KLogLevelWarning, aFmt, list );
    }

/*---------------------------------------------------------------------------*/
void ErrLog( TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START( list, aFmt );
    Log( KLogLevelError, aFmt, list );
    }

/*---------------------------------------------------------------------------*/
void Log( TInt aLevel, TRefByValue<const TDesC> aText, VA_LIST list )
    {

    if ( aLevel < KMinimumLogLevel )
        return;

    _LIT( KLineFeed, "\n" );

    /**
     *  Log time format (see TTime) is
     *  Day-Month-Year Hours:Minutes:Seconds:Milliseconds
     *
     *  Example: 30-12-2004 23:00:55:990
     */
    _LIT( KLogTimeFormat, "%F%D-%M-%Y %H:%T:%S:%*C3" );

    TBuf8<KLogEntryMaxLength> writeBuffer;
    TBuf16<KLogEntryMaxLength> logEntry;
    RFs FileServer;
    RFile File;

    if ( FileServer.Connect() != KErrNone )
        {
        FileServer.Close(); // just in case
        User::Panic( KLogPanicCategory(), KPanicFsConnectFailed );
        return;
        }

    // Open file for writing, if exists. Othervise create new file.
    if ( File.Open( FileServer, KLogFileName(), EFileWrite ) != KErrNone )
        {
        if ( File.Create( FileServer, KLogFileName(), EFileWrite )
            != KErrNone )
            {
            FileServer.Close();
            User::Panic( KLogPanicCategory(), KPanicFileCreateFailed );
            }
        }

    TTime currentTime;
    currentTime.UniversalTime();
    TBuf<32> timeString;

    // currentTime is now in universal time. Convert it to home time.
    TLocale locale;
    TTimeIntervalSeconds universalTimeOffset( locale.UniversalTimeOffset() );
    TTimeIntervalHours daylightSaving( 0 );
    if ( locale.QueryHomeHasDaylightSavingOn() )
        {
        daylightSaving = 1;
        }
    currentTime = currentTime + universalTimeOffset + daylightSaving;
    currentTime.FormatL( timeString, KLogTimeFormat );

    // Add LogString to the end of file and close the file
    TInt currentSize = 0, returnCode;
    writeBuffer.Append( timeString );
    writeBuffer.Append( _L(": ") );

    if ( aLevel < KLogLevelInfo )
        writeBuffer.Append(KDebugStr);
    else if ( aLevel < KLogLevelWarning )
        writeBuffer.Append(KInfoStr);
    else if ( aLevel < KLogLevelError )
        writeBuffer.Append(KWarnStr);
    else
        writeBuffer.Append(KErrStr);

    logEntry.AppendFormatList( aText, list ); //, &overFlowHandler );
    writeBuffer.Append( logEntry.Left(
        KLogEntryMaxLength - writeBuffer.Length() ) );
    writeBuffer.Append( KLineFeed );
    File.Size( currentSize );
    returnCode = File.Write( currentSize,
                             writeBuffer,
                             writeBuffer.Length() );
    File.Close();
    // Close file server session
    FileServer.Close();

    if ( returnCode != KErrNone )
        {
        User::Panic( KLogPanicCategory(), KPanicFileWriteFailed );
        }
    }

#endif // __ENABLE_LOGGING__
