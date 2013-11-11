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
* Description:   Implementation of the WlanWsaJoin class
*
*/

/*
* %version: 14 %
*/

#include "config.h"
#include "UmacWsaJoin.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG 
const TInt8 WlanWsaJoin::iName[] = "wsa-join";
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaJoin::WlanWsaJoin() : 
    iMode( static_cast<WHA::TOperationMode>(0) ), 
    iBSSID ( WHA::KZeroMacAddr), iBand( 0 ),
    iChannel( 0 ), iBeaconInterval( 0 ),
    iBasicRateSet( 0 ), iAtimWindow( 0 ),
    iPreambleType( static_cast<WHA::TPreamble>(0) ), 
    iProbeForJoin( EFalse ) 
    {
    iSSID.iSSIDLength = 0;
    os_memset( &iSSID.iSSID, 0, sizeof( iSSID.iSSID ) );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaJoin::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

#ifndef NDEBUG 
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaJoin::Entry") );
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaJoin::Entry: iMode: %d"), 
        iMode );

    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWsaJoin::Entry(): BSSID:"), 
        reinterpret_cast<const ::TMacAddress&>(iBSSID) );

    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWsaJoin::Entry: SSID:") );
    TUint8 ssidTraceBuf[KMaxSSIDLength + 1]; // + 1 for NULL termination
    os_memset( ssidTraceBuf, 0, sizeof( ssidTraceBuf ) );
    os_memcpy( ssidTraceBuf, iSSID.iSSID, iSSID.iSSIDLength );
    OsTracePrint( KWsaCmdStateDetails, ssidTraceBuf );

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaJoin::Entry: iChannel: %d"), 
        iChannel );
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaJoin::Entry: iBeaconInterval: %d"),  
        iBeaconInterval );
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaJoin::Entry: iBasicRateSet: 0x%08x"), 
        iBasicRateSet );
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaJoin::Entry: iAtimWindow: %d"), 
        iAtimWindow );
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaJoin::Entry: iPreambleType: %d"), 
        iPreambleType );
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)("UMAC: WlanWsaJoin::Entry(): iProbeForJoin: %d"), 
        iProbeForJoin );

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-Join") );
#endif // !NDEBUG 
            
    // execute the command
    aCtxImpl.Wha().Join( 
        iMode, iBSSID, iSSID, iBand, iChannel, 
        iBeaconInterval, iBasicRateSet, iAtimWindow, 
        iPreambleType, iProbeForJoin );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaJoin::Set( 
    WlanContextImpl& /*aCtxImpl*/,
    WHA::TOperationMode aMode,
    WHA::TMacAddress& aBSSID,
    WHA::TBand aBand,
    WHA::SSSID& aSSID,
    WHA::TChannelNumber aChannel,
    TUint32 aBeaconInterval,
    WHA::TRate aBasicRateSet,
    TUint16 aAtimWindow,
    WHA::TPreamble aPreambleType,
    TBool aProbeForJoin )                                   
    {
    iMode = aMode;
    iBSSID = aBSSID;
    iSSID = aSSID;
    iBand = aBand;
    iChannel = aChannel;
    iBeaconInterval = aBeaconInterval;
    iBasicRateSet = aBasicRateSet;
    iAtimWindow = aAtimWindow;
    iPreambleType = aPreambleType;
    iProbeForJoin = aProbeForJoin;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaJoin::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WlanWsaJoin::CommandResponse") );    

    if ( aCommandId == WHA::EJoinResponse )
        {        
        TraverseToHistoryState( aCtxImpl, 
            aCommandId, aStatus, aCommandResponseParams );    
        }
    else
        {
        OsTracePrint( KErrorLevel, 
            (TUint8*)("UMAC: aCommandId: %d"), aCommandId );
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanWsaJoin::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif
