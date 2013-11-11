/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Debug utilities
*
*/


#include <wlancontrolinterface.h>


const TUint KPrintLineLength = 300;
const TUint KPrintHexPerLine = 8;
const TUint KPrintHexLineLength = KPrintHexPerLine * 3;

// -----------------------------------------------------------------------------
// Debug::PrintBuffer
// -----------------------------------------------------------------------------
//
inline void Debug::PrintBuffer( TUint aLength, const TUint8* aData )
    {
    ASSERT( aData != NULL);
    TBuf<KPrintHexLineLength> line;
    TUint idx( 0 );
    for ( ; idx < aLength; ++idx )
        {
        if ( idx && !( idx % KPrintHexPerLine ) )
            {
            RDebug::Print( line );
            line.Zero();
            }
        line.AppendFormat( _L( "%02X " ), aData[idx] );
        }
    RDebug::Print( line );
    }

// -----------------------------------------------------------------------------
// Debug::PrintString
// -----------------------------------------------------------------------------
//
inline void Debug::PrintString( const TDesC& aString, TUint aLength, const TUint8* aData )
    {
    TBuf<KPrintLineLength> line;

    line.Append( aString );
    
    TPtrC8 buf8( aData, aLength );
    
    TBuf<KPrintLineLength> buf16;
    buf16.Copy( buf8 );

    line.Append( buf16 );
    
    RDebug::Print( _L( "%S" ), &line );
    }

// -----------------------------------------------------------------------------
// Debug::PrintTimestamp
// -----------------------------------------------------------------------------
//
inline void Debug::PrintTimestamp( const TDesC& aString )
    {
    TBuf<KPrintLineLength> line;
    TTime time;
    time.HomeTime();
    
    time.FormatL( line, _L( "%H:%T:%S:%C" ) );
    line.Append( _L( " " ) );
    line.Append( aString );
    
    RDebug::Print( line );
    }

// -----------------------------------------------------------------------------
// Debug::PrintRates
// -----------------------------------------------------------------------------
//
inline void Debug::PrintRates( const TDesC& aString, TUint aRates )
    {
    TBuf<KPrintLineLength> line;
    line.Append( aString );

    if ( aRates & TWlanRate54mbit )
        {
        line.Append( _L( "54Mbit/s " ) );
        }
    if ( aRates & TWlanRate48mbit )
        {
        line.Append( _L( "48Mbit/s " ) );
        }
    if ( aRates & TWlanRate36mbit )
        {
        line.Append( _L( "36Mbit/s " ) );
        }
    if ( aRates & TWlanRate33mbit )
        {
        line.Append( _L( "33Mbit/s " ) );
        }
    if ( aRates & TWlanRate24mbit )
        {
        line.Append( _L( "24Mbit/s " ) );
        }
    if ( aRates & TWlanRate22mbit )
        {
        line.Append( _L( "22Mbit/s " ) );
        }
    if ( aRates & TWlanRate18mbit )
        {
        line.Append( _L( "18Mbit/s " ) );
        }
    if ( aRates & TWlanRate12mbit )
        {
        line.Append( _L( "12Mbit/s " ) );
        }
    if ( aRates & TWlanRate11mbit )
        {
        line.Append( _L( "11Mbit/s " ) );
        }
    if ( aRates & TWlanRate9mbit )
        {
        line.Append( _L( "9Mbit/s " ) );
        }
    if ( aRates & TWlanRate6mbit )
        {
        line.Append( _L( "6Mbit/s " ) );
        }
    if ( aRates & TWlanRate5p5mbit )
        {
        line.Append( _L( "5.5Mbit/s " ) );
        }
    if ( aRates & TWlanRate2mbit )
        {
        line.Append( _L( "2Mbit/s " ) );
        }
    if ( aRates & TWlanRate1mbit )
        {
        line.Append( _L( "1Mbit/s " ) );
        }

    RDebug::Print( line );       
    }
