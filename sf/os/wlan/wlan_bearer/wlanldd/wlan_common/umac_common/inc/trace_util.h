/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definition of some trace utility methods.
*
*/

/*
* %version: 6 %
*/

#ifndef TRACEUTIL_H
#define TRACEUTIL_H

#include "802dot11.h"

inline void TracePrint( 
    const TUint32 aTraceLevel, 
    const TUint16* aDataBegin, 
    const TUint16* const aDataEnd )
    {
    while ( aDataBegin != aDataEnd )
        {
        TraceDump(aTraceLevel, (("0x%04x"), *aDataBegin));
        ++aDataBegin;
        }
    }

/**
* Generic data trace function
* @param aTraceLevel tracing level bitmask
* @param aData begin of data to be traced
* @param aLength length of the data
*/
inline void TracePrint( 
    const TUint32 aTraceLevel, 
    const TUint8* aData, 
    const TUint32 aLength )
    {

    const TUint8* const end = aData + aLength;
    while ( end != aData )
        {
        TraceDump(aTraceLevel, (("%c"), *aData));
        ++aData;
        }
    }

inline void TracePrint( const TUint32 aTraceLevel, const TMacAddress& aMac )
    {
    TraceDump(aTraceLevel, (("%02x:%02x:%02x:%02x:%02x:%02x"), 
        aMac.iMacAddress[0], aMac.iMacAddress[1], aMac.iMacAddress[2], 
        aMac.iMacAddress[3], aMac.iMacAddress[4], aMac.iMacAddress[5]));
    }

#endif      // TRACEUTIL_H
