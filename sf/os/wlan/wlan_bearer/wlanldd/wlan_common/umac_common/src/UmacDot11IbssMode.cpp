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
* Description:   Implementation of the WlanDot11IbssMode class.
*
*/

/*
* %version: 21 %
*/

#include "config.h"
#include "UmacDot11IbssMode.h"
#include "UmacContextImpl.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11IbssMode::DoSetTxMpduDaAddress( 
    SDataFrameHeader& aDataFrameHeader, 
    const TMacAddress& aMac ) const
    {
    aDataFrameHeader.iAddress1 = aMac;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11IbssMode::DoIsRxFrameSAourAddress( 
    WlanContextImpl& aCtxImpl,
    const SDataFrameHeader& aFrameHeader,
    const SAmsduSubframeHeader* /*aSubFrameHeader*/) const
    {
    return aFrameHeader.iAddress2 == aCtxImpl.iWlanMib.dot11StationId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11IbssMode::DoIsValidAddressBitCombination(
    const SDataFrameHeader& aFrameHeader ) const
    {
    return !( aFrameHeader.IsFromDsBitSet() 
        || aFrameHeader.IsToDsBitSet() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11IbssMode::DoBuildEthernetFrame(
    TDataBuffer& aBuffer,
    const SDataMpduHeader& aDot11DataMpdu,
    const TUint8* aStartOfEtherPayload,
    TUint aEtherPayloadLength,
    TBool /*aAmsdu*/,
    TUint8* /*aCopyBuffer*/ )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11InfrastructureMode::DoBuildEthernetFrame"));

    // copy SA to the correct location.
    os_memcpy( 
        const_cast<TUint8*>(aStartOfEtherPayload) - sizeof( TMacAddress ), 
        reinterpret_cast<const TUint8*>(
            aDot11DataMpdu.iHdr.iAddress2.iMacAddress ),
        sizeof( TMacAddress ) );

    // copy DA to the correct location.
    os_memcpy( 
        const_cast<TUint8*>(aStartOfEtherPayload) 
            - ( 2 * sizeof( TMacAddress ) ), 
        reinterpret_cast<const TUint8*>(
            aDot11DataMpdu.iHdr.iAddress1.iMacAddress ),
        sizeof( TMacAddress ) );
    
    // set the length
    aBuffer.KeSetLength( 
        aEtherPayloadLength 
        + ( sizeof( TMacAddress ) * 2 ) );

    // set the frame type
    aBuffer.FrameType( TDataBuffer::KEthernetFrame );
    
    // set the offset to the beginning of the ready ethernet frame 
    // from the beginning of the Rx buf
    aBuffer.KeSetOffsetToFrameBeginning( 
        aStartOfEtherPayload - ( 2 * sizeof( TMacAddress ) ) // frame beginning
        - aBuffer.KeGetBufferStart() );                      // buffer beginning

    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanDot11IbssMode::DoBuildEthernetFrame: offset to frame beginning: %d"),
        aBuffer.KeOffsetToFrameBeginning());
    }
