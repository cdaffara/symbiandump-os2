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
* Description:   Implementation of the WlanDot11IbssNormalMode class.
*
*/

/*
* %version: 15 %
*/

#include "config.h"
#include "umacdot11ibssnormalmode.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG
const TInt8 WlanDot11IbssNormalMode::iName[] = "dot11ibss::dot11-normalmode";
#endif

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11IbssNormalMode::Entry( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // nothing here
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11IbssNormalMode::Exit( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // nothing here
    }

#ifndef NDEBUG 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TInt8* WlanDot11IbssNormalMode::GetStateName( 
    TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11IbssNormalMode::Disconnect( 
    WlanContextImpl& aCtxImpl )
    {
    // set completion code to idle state
    // as it does the request completion
    aCtxImpl.iStates.iIdleState.Set( KErrNone );
    ChangeState( aCtxImpl, 
        *this,                                  // prev state
        aCtxImpl.iStates.iSoftResetState        // next state
        );                      

    // notify of state transition
    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11IbssNormalMode::Connect(
    WlanContextImpl& aCtxImpl,
    const TSSID& aSSID,                 
    const TMacAddress& aBSSID,          
    TUint16 /*aAuthAlgorithmNbr*/,      
    TEncryptionStatus aEncryptionStatus,
    TBool /*aIsInfra*/,
    TUint16 aScanResponseFrameBodyLength,
    const TUint8* aScanResponseFrameBody,
    const TUint8* aIeData,
    TUint16 aIeDataLength )
    {
    TBool ret( ETrue );

    (aCtxImpl.GetBssId())= aBSSID;   
    (aCtxImpl.GetSsId()) = aSSID;   
    (aCtxImpl.EncryptionStatus()) = aEncryptionStatus;           
    aCtxImpl.NetworkOperationMode( WHA::EIBSS );

    // store Tx IE data for later access
    // pointers supplied are valid to the point the
    // corresponding completion method is called
    aCtxImpl.IeData( aIeData );
    aCtxImpl.IeDataLength( aIeDataLength );

    TInt status(KErrNone);

    // check do we meet the requirements for the network
    // and construct necessary objects for establishing the connection
    if ( ( status = InitNetworkConnect( 
            aCtxImpl,
            aScanResponseFrameBodyLength, 
            aScanResponseFrameBody ) ) == KErrNone )
        {
        // continue

        // make WHA types
        WHA::SSSID ssid;
        ssid.iSSIDLength = aSSID.ssidLength;
        os_memcpy( ssid.iSSID, aSSID.ssid, ssid.iSSIDLength );

        ChangeState( aCtxImpl, 
            *this,                                  // prev state
            aCtxImpl.iStates.iPrepareForIbssMode    // next state
            );            
        }
    else    // --- InitNetworkConnect failure ---
        {
        // abort
        ret = EFalse;
        OnOidComplete( aCtxImpl, status );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11IbssNormalMode::RealScan(
    WlanContextImpl& aCtxImpl,
    TScanMode aMode,                    
    const TSSID& aSSID,                 
    TUint32 aScanRate,                    
    SChannels& aChannels,
    TUint32 aMinChannelTime,            
    TUint32 aMaxChannelTime,
    TBool aSplitScan )    
    {
    // scanning mode requested
    // set parameters
    // NOTE: OID command parameters are guaranteed to be valid
    // to the point a correcponding completion method is called

    aCtxImpl.iStates.iIbssScanningMode.Set( 
        aCtxImpl, aMode, aSSID, aScanRate, aChannels, 
        aMinChannelTime, aMaxChannelTime, aSplitScan );

    ChangeState( aCtxImpl, 
        *this,                                  // prev state
        aCtxImpl.iStates.iIbssScanningMode      // next state
        );                      
    return ETrue; // global statemachine transition will occur 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11IbssNormalMode::OnBeaconFrameRx( 
    WlanContextImpl& aCtxImpl,
    const TAny* aFrame,
    const TUint32 aLength,
    WHA::TRcpi aRcpi,
    TUint8* aBuffer )
    {
    OsTracePrint( KWlmIndication | KRxFrame, (TUint8*)
        ("UMAC: WlanDot11IbssNormalMode::OnBeaconFrameRx"));

    // pass the frame to the mgmt client
    XferDot11FrameToMgmtClient( aCtxImpl, aFrame, aLength, aRcpi, aBuffer );      
    }
