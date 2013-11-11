/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#ifndef __HELLOTRACETYPES2DUP_H__
#define __HELLOTRACETYPES2DUP_H__

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "HelloTraceTypes2DupTraces.h"
#endif

inline void HelloTraceTypes::SignedIntegersDup()
    {
    TInt8  eightBit     = - (TInt8)  1 << 6;
    OstTraceExt1( TRACE_INTERNALS, HELLOTRACETYPES_SIGNEDINTEGERS2DUP_8, 
                  "8 bit signed: %hhd", eightBit );
    
    TInt16 sixteenBit   = - (TInt16) 1 << 14; 
    OstTraceExt1( TRACE_INTERNALS, HELLOTRACETYPES_SIGNEDINTEGERS2DUP_16, 
                  "16 bit signed: %hd", sixteenBit );

    TInt32 thirtyTwoBit = - (TInt32) 1 << 30; 
    OstTrace1( TRACE_INTERNALS, HELLOTRACETYPES_SIGNEDINTEGERS2DUP_32, 
               "32 bit signed: %d", thirtyTwoBit );

    TInt64 sixtyFourBit = - (TInt64) 1 << 62; 
    OstTraceExt1( TRACE_INTERNALS, HELLOTRACETYPES_SIGNEDINTEGERS2DUP_64, 
                  "64 bit signed: %Ld", sixtyFourBit );
    }

inline void HelloTraceTypes::UnsignedIntegersDup()
    {
    TUint8  eightBit     = (TUint8)  1 << 6;
    OstTraceExt1( TRACE_INTERNALS, HELLOTRACETYPES_UNSIGNEDINTEGERS2DUP_8, 
                  "8 bit unsigned: %hhu", eightBit );
    
    TUint16 sixteenBit   = (TUint16) 1 << 14; 
    OstTraceExt1( TRACE_INTERNALS, HELLOTRACETYPES_UNSIGNEDINTEGERS2DUP_16, 
                  "16 bit unsigned: %hu", sixteenBit );

    TUint32 thirtyTwoBit = (TUint32) 1 << 30; 
    OstTrace1( TRACE_INTERNALS, HELLOTRACETYPES_UNSIGNEDINTEGERS2DUP_32, 
               "32 bit unsigned: %u", thirtyTwoBit );

    TUint64 sixtyFourBit = (TUint64) 1 << 62; 
    OstTraceExt1( TRACE_INTERNALS, HELLOTRACETYPES_UNSIGNEDINTEGERS2DUP_64, 
                  "64 bit unsigned: %Lu", sixtyFourBit );
    }

inline void HelloTraceTypes::DescriptorsDup()
    {
    _LIT8(KAscii, "Some ASCII text");
    OstTraceExt1( TRACE_INTERNALS, HELLOTRACETYPES_ASCII2DUP, "8 bit descriptor: %s", KAscii() );

    _LIT16(KUnicode, "Some unicode text");    
    OstTraceExt1( TRACE_INTERNALS, HELLOTRACETYPES_UNICODE2DUP, "16 bit descriptor: %S", KUnicode() );
    }

#endif  // __HELLOTRACETYPES2DUP_H__

