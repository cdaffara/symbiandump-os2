/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanWsaConfigure class
*
*/

/*
* %version: 24 %
*/

#include "config.h"
#include "UmacWsaConfigure.h"
#include "UmacContextImpl.h"

#ifndef RD_WLAN_DDK
#include <wha_types.h>
#else
#include <wlanwha_types.h>
#endif // RD_WLAN_DDK

#ifndef NDEBUG 
const TInt8 WlanWsaConfigure::iName[] = "wsa-configure";
#endif // !NDEBUG

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanWsaConfigure::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: pda size: %d"), iLength);
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: pda start address: 0x%08x"), 
        reinterpret_cast<TUint32>(iData));        

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-Configure") );
            
    // execute the command
    aCtxImpl.Wha().Configure( 
        *(static_cast<const WHA::SConfigureData*>(iData)), 
        aCtxImpl.WHASettings() );
    
#ifndef NDEBUG 
    // WHA settings object should be set
    // trace it
    
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: WSASettings object:") );
        
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: iNumOfSSIDs: %d"), 
        aCtxImpl.WHASettings().iNumOfSSIDs  );
        
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: iRxBufAlignment (DEPRECATED): %d"), 
        aCtxImpl.WHASettings().iRxBufAlignment  );                        

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: iNumOfBytesForSsid: %d"), 
        aCtxImpl.WHASettings().iNumOfBytesForSsid  );

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: iRates: 0x%08x"), 
        aCtxImpl.WHASettings().iRates );

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: iBand: %d"), 
        aCtxImpl.WHASettings().iBand );

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: iRxoffset: %d"), 
        aCtxImpl.WHASettings().iRxoffset );

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: iNumOfGroupTableEntrys: %d"), 
        aCtxImpl.WHASettings().iNumOfGroupTableEntrys );

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: iNumOfTxRateClasses: %d"), 
        aCtxImpl.WHASettings().iNumOfTxRateClasses );

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: iTxFrameTrailerSpace: %d"), 
        aCtxImpl.WHASettings().iTxFrameTrailerSpace  );

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: iTxFrameHeaderSpace: %d"), 
        aCtxImpl.WHASettings().iTxFrameHeaderSpace  );

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: iFlagsMask: 0x%04x"), 
        aCtxImpl.WHASettings().iFlagsMask  );

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: power levels per band (from lowest to highest):") );

    for ( TUint i = 0; i < WHA::SSettings::KNumOfBandsSupported; ++i )
        {
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iMinPowerLevel: %d"), 
            aCtxImpl.WHASettings().iTxPowerRange[i].iMinPowerLevel );
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iMaxPowerLevel: %d"), 
            aCtxImpl.WHASettings().iTxPowerRange[i].iMaxPowerLevel );
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iStepping: %d"), 
            aCtxImpl.WHASettings().iTxPowerRange[i].iStepping );
        }

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaConfigure::Entry: iCapability: 0x%08x"), 
        aCtxImpl.WHASettings().iCapability  );
    
    if ( aCtxImpl.WHASettings().iCapability & WHA::SSettings::KHtOperation )
        {
        // trace HT related information

        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iHTCapabilitiesBitMask: 0x%08x"), 
            aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask );
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iRxMaxDataRate: %d"), 
            aCtxImpl.WHASettings().iHtCapabilities.iRxMaxDataRate );
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iChannelWidth: %d"), 
            aCtxImpl.WHASettings().iHtCapabilities.iChannelWidth );
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iRxStbc: %d"), 
            aCtxImpl.WHASettings().iHtCapabilities.iRxStbc );
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iMaxAmsdu: %d"), 
            aCtxImpl.WHASettings().iHtCapabilities.iMaxAmsdu );
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iMaxAmpdu: %d"), 
            aCtxImpl.WHASettings().iHtCapabilities.iMaxAmpdu );
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iAmpduSpacing: %d"), 
            aCtxImpl.WHASettings().iHtCapabilities.iAmpduSpacing );
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iRxMcs:"),
            aCtxImpl.WHASettings().iHtCapabilities.iRxMcs,
            aCtxImpl.WHASettings().iHtCapabilities.iRxMcs + 
            WHA::KHtMcsSetLength );
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iTxMcs:"),
            aCtxImpl.WHASettings().iHtCapabilities.iTxMcs,
            aCtxImpl.WHASettings().iHtCapabilities.iTxMcs + 
            WHA::KHtMcsSetLength );
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iPcoTransTime: %d"), 
            aCtxImpl.WHASettings().iHtCapabilities.iPcoTransTime );
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWsaConfigure::Entry: iMcsFeedback: %d"), 
            aCtxImpl.WHASettings().iHtCapabilities.iMcsFeedback );
        }
#endif    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanWsaConfigure::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    if ( aCommandId == WHA::EConfigureResponse )
        {
        TraverseToHistoryState( aCtxImpl, 
            aCommandId, 
            aStatus,
            aCommandResponseParams );    
        }
    else
        {
        OsTracePrint( KErrorLevel, 
            (TUint8*)("UMAC: aCommandId: %d"), aCommandId );
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanWsaConfigure::Set( 
    WlanContextImpl& /*aCtxImpl*/, 
    const TAny* aData, 
    TUint32 aLength )
    {
    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WlanWsaConfigure::Set: user space PDA address: 0x%08x"),
        reinterpret_cast<TUint32>(aData) );    
    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WlanWsaConfigure::Set: length of PDA: %d"), aLength );
    
    iData = aData;
    iLength = aLength;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanWsaConfigure::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif
