/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   osa debug library implementation
*
*/

/*
* %version: 4 %
*/

#include "osa_includeme.h"
#include <wlanosa.h>
#include "osacarray.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void MWlanOsa::FormattedPrint( TUint32 aLevel, 
                               const TInt8* aFmt, 
                               ... )
    {
#ifndef NDEBUG
    
    if ( DEBUG_LEVEL & aLevel ) 
        {
        TPtrC8 format((const TText8*)aFmt);
        
        const TInt KTraceBufLen( 512 );
        TBuf8<KTraceBufLen> msg;

        VA_LIST list;
        VA_START(list,aFmt);
        msg.Zero();
        Kern::AppendFormat( msg, (const char*)aFmt, list );
        msg.Append('\0');
        Kern::Printf( (const char*)&msg[0] );
        VA_END(list);
        }
#endif // NDEBUG
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void MWlanOsa::Assert( const TInt8* aFile, TInt aLine, TBool aExpression )
    {
#ifndef NDEBUG

    if ( !aExpression )
        {
        TraceDump( MWlanOsa::KFatalLevel, 
            (reinterpret_cast<const char*>(aFile)) );
        TraceDump( MWlanOsa::KFatalLevel, (("Line: %d"), aLine));

        Kern::Fault(("[WLAN] Subsystem programmed fault"), 0);
        }
    else
        {
        // left intentionally empty
        }

#endif // NDEBUG
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void MWlanOsa::Dump( TUint32 aLevelMask, 
                     const TInt8* aData, 
                     TInt aDataSize )
    {
#ifndef NDEBUG
    
    // Defines how many values in a row
    const TInt ValuesInRow = 8;

    const TInt LineWidth = 3*ValuesInRow;
    const char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', 
                           '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    if ( DEBUG_LEVEL & aLevelMask ) 
        {
        TBuf8< LineWidth > line;
        line.Zero();
        for ( TInt i=0; i<aDataSize; i++ )
            {
            line.Append( hex[*aData/16] );
            line.Append( hex[*aData%16] );
            if ( !((i+1)%ValuesInRow) || aDataSize == (i + 1) )
                {
                line.Append( '\0' );
                Kern::Printf( (const char*)&line[0] );
                line.Zero();
                }
            else
                {
                line.Append( ' ' );
                }
            aData++;
            }
        }

#endif // NDEBUG
    }







