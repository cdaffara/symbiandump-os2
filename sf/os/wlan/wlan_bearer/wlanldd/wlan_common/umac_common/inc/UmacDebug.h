/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Debugging/tracing related declarations & definitions
*
*/

/*
* %version: 17 %
*/

#ifndef UMACDEBUG_H
#define UMACDEBUG_H

struct TMacAddress;

const TUint32 KCritLevel                = 0x00000001;
const TUint32 KSeriousLevel             = 0x00000002;
const TUint32 KErrorLevel               = 0x00000004;
const TUint32 KWarningLevel             = 0x00000008;
const TUint32 KInfoLevel                = 0x00000010;

const TUint32 KInitLevel                = 0x00000020;
const TUint32 KUmacProtocolState        = 0x00000040;
const TUint32 KWsaCmdState              = 0x00000080;
const TUint32 KMutex                    = 0x00000100;
const TUint32 KWsaCallback              = 0x00000200;
const TUint32 KUmacMgmtCallback         = 0x00000400;
const TUint32 KUmacProtocolCallback     = 0x00000800;
const TUint32 KWsaCmdStateDetails       = 0x00001000;
const TUint32 KQos                      = 0x00002000;
const TUint32 KWlmCmd                   = 0x00004000;
const TUint32 KWlmIndication            = 0x00008000;
const TUint32 KWlmCmdDetails            = 0x00010000;
const TUint32 KUmacDetails              = 0x00020000;
const TUint32 KWsaTx                    = 0x00040000;
const TUint32 KWsaTxDetails             = 0x00080000;
const TUint32 KUmacAuth                 = 0x00100000;
const TUint32 KUmacAssoc                = 0x00200000;
const TUint32 KPacketScheduler          = 0x00400000;
const TUint32 KMemory                   = 0x00800000;
const TUint32 KTxRateAdapt              = 0x01000000;
const TUint32 KRxFrame                  = 0x02000000;
const TUint32 KScan                     = 0x04000000;
const TUint32 KPwrStateTransition       = 0x08000000;
const TUint32 KDot11StateTransit        = 0x10000000;
const TUint32 KEventDispatcher          = 0x20000000;

/* for temporary trace */
const TUint32 KScratch                  = 0x80000000;


extern void os_traceprint( 
    TUint32 aLevel, 
    const TUint8* aString );
extern void os_traceprint( 
    TUint32 aLevel, 
    const TUint8* aString, 
    TUint32 aArg1 );
extern void os_traceprint( 
    TUint32 aLevel, 
    const TUint8* aString, 
    const TUint8* aBeg,
    /* one past last element to be traced */
    const TUint8* aEnd );                          
extern void os_traceprint( 
    TUint32 aLevel, 
    const TUint8* aString, 
    const TMacAddress& aMac );
extern void os_traceprint( 
    TUint32 aLevel, 
    const TUint8* aString, 
    const Sdot11MacHeader& aDot11Hdr );

extern void os_assert( 
    const TUint8* aError, 
    const TUint8* aFile, 
    TUint32 aLine );


inline void OsAssert(     
    const TUint8* aError,
    const TUint8* aFile,
    TUint32 aLine )
    {
    os_assert( aError, aFile, aLine );
    }


#ifndef NDEBUG /* for debug builds */

inline void OsTracePrint( 
    TUint32 aLevel, 
    const TUint8* aString )
    {
    os_traceprint( aLevel, aString );
    }

inline void OsTracePrint( 
    TUint32 aLevel, 
    const TUint8* aString, 
    TUint32 aArg1 )
    {
    os_traceprint( aLevel, aString, aArg1 );
    }

inline void OsTracePrint( 
    TUint32 aLevel, 
    const TUint8* aString, 
    const TUint8* aBeg,
    // one past last element to be traced
    const TUint8* aEnd )
    {
    os_traceprint( aLevel, aString, aBeg, aEnd );
    }

inline void OsTracePrint( 
    TUint32 aLevel, 
    const TUint8* aString, 
    const TMacAddress& aMac )
    {
    os_traceprint( aLevel, aString, aMac );
    }

inline void OsTracePrint( 
    TUint32 aLevel, 
    const TUint8* aString, 
    const Sdot11MacHeader& aDot11Hdr )
    {
    os_traceprint( aLevel, aString, aDot11Hdr );
    }

#else /* for release builds */

#define OsTracePrint(...)   /* empty */

#endif // !NDEBUG


#endif // UMACDEBUG_H
