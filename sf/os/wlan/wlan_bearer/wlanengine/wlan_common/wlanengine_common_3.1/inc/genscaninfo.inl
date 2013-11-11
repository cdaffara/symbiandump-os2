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
* Description:  Implementation of ScanInfo inline methods.
*
*/


#include "genscanoffsets.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
inline u8_t ScanInfo::SignalNoiseRatio() const
    {
    return *( current_m + CNTRL_RX_SNR_OFFSET );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
inline u8_t ScanInfo::RXLevel() const
    {
    if( !current_m )
        {
        return 0;
        }
    
    return *( current_m + CNTRL_RX_LEVEL_OFFSET );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
inline void ScanInfo::BSSID(
    u8_t bssid[BSSID_LENGTH] ) const
    {
    u8_t* dst = bssid;
    if ( !dst || !current_m )
        {
        return;
        }

    const u8_t* src = current_m + MGMT_BSSID_OFFSET;
    const u8_t* end = src + BSSID_LENGTH;

    for ( ; src < end; ++dst, ++src )
        {
        *dst = *src;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
inline u16_t ScanInfo::BeaconInterval() const
    {
    if( !current_m )
        {
        return 0;
        }
    
    return *( reinterpret_cast<const u16_t*>( current_m + MGMT_BEACON_INTERVAL_OFFSET ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
inline u16_t ScanInfo::Capability() const
    {
    if( !current_m )
        {
        return 0;
        }
    
    return *( reinterpret_cast<const u16_t*>( current_m + MGMT_CAPABILITY_OFFSET ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
inline bool_t ScanInfo::Privacy() const
    {
    if ( Capability() & SCAN_CAPABILITY_BIT_MASK_PRIVACY )
        {
        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
inline WlanOperatingMode ScanInfo::OperatingMode() const
    {
    if ( Capability() & SCAN_CAPABILITY_BIT_MASK_ESS )
        {
        return WlanOperatingModeInfra;
        }

    return WlanOperatingModeAdhoc;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
inline void ScanInfo::Timestamp( u8_t timestamp[TIMESTAMP_LENGTH] ) const
    {
    u8_t* dst = timestamp;
    if ( !dst )
        {
        return;
        }

    const u8_t* src = current_m + MGMT_TIMESTAMP_OFFSET;
    const u8_t* end = src + TIMESTAMP_LENGTH;

    for ( ; src < end; ++dst, ++src )
        {
        *dst = *src;
        }
    }
