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
* Description:  Debug utilities
*
*/


#ifndef AM_DEBUG_H
#define AM_DEBUG_H

#include <e32svr.h>

#ifdef _DEBUG

/**
* Class for producing debug traces
* @lib
* @since Series 60 3.0
*/
class Debug
    {
    public:
        static inline void PrintBuffer( TUint aLength, const TUint8* aData );
        static inline void PrintString( const TDesC& aString, TUint aLength, const TUint8* aData );
        static inline void PrintTimestamp( const TDesC& aString );
        static inline void PrintRates( const TDesC& aString, TUint aRates );
    };

#define DEBUG(a)                    RDebug::Print(_L(a))
#define DEBUG1(a,b)                 RDebug::Print(_L(a),b)
#define DEBUG2(a,b,c)               RDebug::Print(_L(a),b,c)
#define DEBUG3(a,b,c,d)             RDebug::Print(_L(a),b,c,d)
#define DEBUG4(a,b,c,d,e)           RDebug::Print(_L(a),b,c,d,e)
#define DEBUG5(a,b,c,d,e,f)         RDebug::Print(_L(a),b,c,d,e,f)
#define DEBUG6(a,b,c,d,e,f,g)       RDebug::Print(_L(a),b,c,d,e,f,g)
#define DEBUG7(a,b,c,d,e,f,g,h)     RDebug::Print(_L(a),b,c,d,e,f,g,h)
#define DEBUG8(a,b,c,d,e,f,g,h,i)   RDebug::Print(_L(a),b,c,d,e,f,g,h,i)
#define DEBUG9(a,b,c,d,e,f,g,h,i,j) RDebug::Print(_L(a),b,c,d,e,f,g,h,i,j)
#define DEBUG_MAC(a)                RDebug::Print(_L("%02X%02X%02X%02X%02X%02X"),a[0],a[1],a[2],a[3],a[4],a[5])
#define DEBUG_RATES(a,b)            Debug::PrintRates(_L(a),b)
#define DEBUG_BUFFER(a,b)           Debug::PrintBuffer(a,b)
#define DEBUG1S(a,b,c)              Debug::PrintString(_L(a),b,c)
#define DEBUGT(a)                   Debug::PrintTimestamp(_L(a))

#include "am_debug.inl"

#else // _DEBUG

#define DEBUG(a)                    /* _DEBUG is not defined. */
#define DEBUG1(a,b)                 /* _DEBUG is not defined. */
#define DEBUG2(a,b,c)               /* _DEBUG is not defined. */
#define DEBUG3(a,b,c,d)             /* _DEBUG is not defined. */
#define DEBUG4(a,b,c,d,e)           /* _DEBUG is not defined. */
#define DEBUG5(a,b,c,d,e,f)         /* _DEBUG is not defined. */
#define DEBUG6(a,b,c,d,e,f,g)       /* _DEBUG is not defined. */
#define DEBUG7(a,b,c,d,e,f,g,h)     /* _DEBUG is not defined. */
#define DEBUG8(a,b,c,d,e,f,g,h,i)   /* _DEBUG is not defined. */
#define DEBUG9(a,b,c,d,e,f,g,h,i,j) /* _DEBUG is not defined. */
#define DEBUG_MAC(a)                /* _DEBUG is not defined. */
#define DEBUG_RATES(a,b)            /* _DEBUG is not defined. */
#define DEBUG_BUFFER(a,b)           /* _DEBUG is not defined. */
#define DEBUG1S(a,b,c)              /* _DEBUG is not defined. */
#define DEBUGT(a)                   /* _DEBUG is not defined. */

#endif // _DEBUG

#endif // AM_DEBUG_H
