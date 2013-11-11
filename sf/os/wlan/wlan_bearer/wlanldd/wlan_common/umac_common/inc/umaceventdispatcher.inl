/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Event dispatcher that is used to serialize MAC prototocl
*                statemachine access
*
*/

/*
* %version: 10 %
*/

#include "umacinternaldefinitions.h"


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline WlanEventDispatcher::~WlanEventDispatcher() 
    {
    iOid = NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline WlanEventDispatcher::TCommandResp::TCommandResp() :
    iId( static_cast<WHA::TCompleteCommandId>( 0 ) ),
    iStatus( static_cast<WHA::TStatus>( 0 ) )
    {
    os_memset( &iParams, 0, sizeof(iParams) );
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanEventDispatcher::Enable( 
    TChannel aChannelMask )
    {
    OsTracePrint( KEventDispatcher, (TUint8*)
        ("UMAC * eventdispatcher * enable events") );

    iDisableChannelMask &= ~aChannelMask;

    OsTracePrint( KEventDispatcher, 
        (TUint8*)("current mask: 0x%02x"),  iDisableChannelMask );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanEventDispatcher::Disable( 
    TChannel aChannelMask )
    {
    OsTracePrint( KEventDispatcher, (TUint8*)
        ("UMAC * eventdispatcher * disable events") );

    iDisableChannelMask |= aChannelMask;

    OsTracePrint( KEventDispatcher, 
        (TUint8*)("current mask: 0x%02x"),  iDisableChannelMask );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanEventDispatcher::Register( const TOIDHeader& aOid )
    {
    OsTracePrint( KEventDispatcher, (TUint8*)
        ("UMAC * eventdispatcher * register OID") );
    OsTracePrint( KEventDispatcher, (TUint8*)("OID id: 0x%08x"), aOid.oid_id );

    if ( iOid )
        {
        // programming error
        OsAssert( (TUint8*)("UMAC * panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    iOid = &aOid;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanEventDispatcher::Register( TInternalEvent aInternalEvent )
    {
    OsTracePrint( KEventDispatcher, (TUint8*)
        ("UMAC: WlanEventDispatcher::Register(): register internal event: %d"),
        aInternalEvent );
    iInternalEvents |= aInternalEvent;
    
    OsTracePrint( KEventDispatcher, (TUint8*)
        ("UMAC: WlanEventDispatcher::Register(): iInternalEvents: %d"),
        iInternalEvents );    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanEventDispatcher::ChannelEnabled( 
    TChannel aChannel ) const 
    {
    return !(iDisableChannelMask & aChannel);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TBool WlanEventDispatcher::CommandCompletionRegistered( 
    WHA::TCompleteCommandId aCommandId ) const
    {
    return ( iCommandCompletionRegistered && 
             aCommandId == iWhaCommandCompletionParams.iId );
    }
