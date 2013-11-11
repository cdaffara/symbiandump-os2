/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   trace library
*
*/

/*
* %version: 5 %
*/

#include "WlLddWlanLddConfig.h"
#include "wlanlddcommon.h"


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void os_traceprint( TUint32 aLevel, const TUint8* aString )
    {
    TraceDump( aLevel, (reinterpret_cast<const char*>(aString)) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void os_traceprint( TUint32 aLevel, const TUint8* aString, TUint32 aArg1 )
    {
    TraceDump( aLevel, (reinterpret_cast<const char*>(aString), aArg1) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void os_traceprint(     
    TUint32 aLevel, 
    const TUint8* aString, 
    const TUint8* aBeg,
    // one past last element to be traced
    const TUint8* aEnd )
    {
    TraceDump( aLevel, (reinterpret_cast<const char*>(aString)) );

    while ( aBeg != aEnd )
        {
        TraceDump( aLevel, (("0x%02x"), *aBeg) );
        ++aBeg;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void os_traceprint( 
    TUint32 aLevel, 
    const TUint8* aString, 
    const TMacAddress& aMac )
    {
    TraceDump( aLevel, (reinterpret_cast<const char*>(aString)) );
    TracePrint( aLevel, aMac );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void os_traceprint( 
    TUint32 aLevel, 
    const TUint8* aString, 
    const Sdot11MacHeader& aDot11Hdr )
    {
    os_traceprint( aLevel, aString );

    TraceDump( aLevel, (("frame control type: 0x%02x"), 
        static_cast<TUint32>(aDot11Hdr.iFrameControl.iType) ));
    TraceDump(aLevel, (("frame control control: 0x%02x"), 
        static_cast<TUint32>(aDot11Hdr.iFrameControl.iControl) ));
    os_traceprint(aLevel, (TUint8*)("address1:"), aDot11Hdr.iAddress1);
    os_traceprint(aLevel, (TUint8*)("address2:"), aDot11Hdr.iAddress2);
    os_traceprint(aLevel, (TUint8*)("address3:"), aDot11Hdr.iAddress3);
    TraceDump(aLevel, (("sequence control: 0x%02x"), 
        static_cast<TUint32>(aDot11Hdr.iSeqCtl) ));
    }
