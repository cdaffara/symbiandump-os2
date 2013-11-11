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
* Description:   Implementation of the WlanWsaScan class
*
*/

/*
* %version: 15 %
*/

#include "config.h"
#include "UmacWsaScan.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG 
const TInt8 WlanWsaScan::iName[] = "wsa-scan";
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaScan::~WlanWsaScan() 
    {
    iChannels = NULL;
    iSsid = NULL;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaScan::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWsaScan::Entry: iMaxTransmitRate: 0x%08x"), 
        iMaxTransmitRate );
    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWsaScan::Entry: iScanType: %d"), iScanType );
    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWsaScan::Entry: iNumOfProbeRequests: %d"),  
        iNumOfProbeRequests );
    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWsaScan::Entry: iSplitScan: %d"), iSplitScan );
    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWsaScan::Entry: iNumOfSSID: %d"), iNumOfSSID );

#ifndef NDEBUG 
    if ( iNumOfSSID )
        {
        TUint8 ssidTraceBuf[KMaxSSIDLength + 1]; // + 1 for NULL termination

        OsTracePrint( KWsaCmdStateDetails, 
            (TUint8*)("UMAC: WlanWsaScan::Entry: SSIDs:") );

        const WHA::SSSID* ssid_end = iSsid + iNumOfSSID;     

        for ( const WHA::SSSID* ssid_ptr = iSsid 
            ; ssid_ptr != ssid_end ; ++ssid_ptr )
            {
            // trace the SSIDs 
        
            os_memset( ssidTraceBuf, 0, sizeof( ssidTraceBuf ) );
            os_memcpy( ssidTraceBuf, ssid_ptr->iSSID, ssid_ptr->iSSIDLength );
            
            OsTracePrint( KWsaCmdStateDetails, (TUint8*)
                ("UMAC: WlanWsaScan::Entry: SSID length: %d, SSID:"),
                ssid_ptr->iSSIDLength );
            OsTracePrint( KWsaCmdStateDetails, ssidTraceBuf );
            }
        }
#endif // !NDEBUG 

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-Scan") );
            
    // execute the command
    aCtxImpl.Wha().Scan( 
        iMaxTransmitRate, 
        iBand,
        iNumOfChannels,
        iChannels, 
        iScanType, 
        iNumOfProbeRequests, 
        iSplitScan, 
        iNumOfSSID, 
        iSsid );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaScan::Set(             
    WlanContextImpl& /*aCtxImpl*/,
    TUint32 aMaxTransmitRate, 
    WHA::TBand aBand,
    TUint8 aNumOfChannels,
    const WHA::SChannels* aChannels, 
    WHA::TScanType aScanType,
    TUint8 aNumOfProbeRequests,
    TBool aSplitScan,
    TUint8 aNumOfSSID,
    const WHA::SSSID* aSSID )
    {
    iMaxTransmitRate = aMaxTransmitRate;
    iBand = aBand;
    iNumOfChannels = aNumOfChannels;
    iChannels = aChannels;
    iScanType = aScanType;
    iNumOfProbeRequests = aNumOfProbeRequests;
    iSplitScan = aSplitScan;
    iNumOfSSID = aNumOfSSID;
    iSsid = aSSID;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaScan::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    OsTracePrint( KWsaCmdState, 
        (TUint8*)("UMAC* WlanWsaScan::CommandResponse()") );    

    if ( aCommandId == WHA::EScanCommandResponse )
        {
        TraverseToHistoryState( aCtxImpl, 
            aCommandId, aStatus, aCommandResponseParams );           
        }
    else
        {
        OsTracePrint( KErrorLevel, 
            (TUint8*)("UMAC: aCommandId: %d"), aCommandId );
        OsAssert( (TUint8*)("UMAC * panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanWsaScan::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif
