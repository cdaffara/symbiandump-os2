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
* Description:   Converter between WHA types and dot11 types.
*
*/

/*
* %version: 10 %
*/

#include "config.h"
#include "umacwhatodot11typeconverter.h"
#include "802dot11.h"
#include "UmacContextImpl.h"

struct TWhaRate2Dot11Rate
    {
    WHA::TRate              iWhaRate;
    T802Dot11SupportedRate  iDot11Rate;
    };

const TWhaRate2Dot11Rate KWhaRate2Dot11RateTable[] = 
    {
        { WHA::KRate1Mbits, E802Dot11Rate1MBit },
        { WHA::KRate2Mbits, E802Dot11Rate2MBit },
        { WHA::KRate5_5Mbits, E802Dot11Rate5p5MBit },
        { WHA::KRate6Mbits, E802Dot11Rate6MBit },
        { WHA::KRate9Mbits, E802Dot11Rate9MBit },
        { WHA::KRate11Mbits, E802Dot11Rate11MBit },
        { WHA::KRate12Mbits, E802Dot11Rate12MBit },
        { WHA::KRate18Mbits, E802Dot11Rate18MBit },
        { WHA::KRate22Mbits, E802Dot11Rate22MBit },
        { WHA::KRate24Mbits, E802Dot11Rate24MBit },
        { WHA::KRate33Mbits, E802Dot11Rate33MBit },
        { WHA::KRate36Mbits, E802Dot11Rate36MBit },
        { WHA::KRate48Mbits, E802Dot11Rate48MBit },
        { WHA::KRate54Mbits, E802Dot11Rate54MBit },
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanWhaToDot11TypeConverter::Convert( 
    WHA::TRate aRateMask, 
    SSupportedRatesIE& aRateIe,
    SExtendedSupportedRatesIE& aExtRateIe )
    {
    aRateIe.Clear();    // make zero length
    aExtRateIe.Clear();

    const TWhaRate2Dot11Rate* pos = KWhaRate2Dot11RateTable;
    const TWhaRate2Dot11Rate* end = 
        pos + sizeof(KWhaRate2Dot11RateTable) / sizeof( TWhaRate2Dot11Rate);

    TUint32 cntr( 0 );
    while ( pos != end )
        {
        if ( aRateMask & pos->iWhaRate )
            {
            // we have a hit
            if ( cntr < KMaxNumberOfRates )
                {
                // space left in this IE
                aRateIe.Append( pos->iDot11Rate );
                }
            else
                {
                // have to use the extended rates IE
                aExtRateIe.Append( pos->iDot11Rate );
                }
            ++cntr;
            }
        else
            {
            // no hit
            }

        ++pos;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanWhaToDot11TypeConverter::ConvertToWhaTypes( 
    WlanContextImpl& aCtxImpl,
    const TSSID* aSsid,                 
    WHA::SSSID& aWhaSsid,
    const SChannels* aChannels,
    TUint32 aMinChannelTime,            
    TUint32 aMaxChannelTime,        
    WHA::SChannels*& aWhaChannels,
    TUint8& aWhaChannelCount )
    {
    const TUint32 endMask2dot4 = 0x00004000;
    const TUint32 endMask4dot9 = 0x00100000;
    
    TBool status ( ETrue );
    
    // =========================================================================
    // handle SSID
    // =========================================================================
        
    aWhaSsid.iSSIDLength = aSsid->ssidLength;
    os_memcpy( aWhaSsid.iSSID, aSsid->ssid, aWhaSsid.iSSIDLength );

    
    // =========================================================================
    // handle channels & bands
    // Note that only one band is allowed to be specified at a time
    // =========================================================================

    OsTracePrint( KScan, (TUint8*)
        ("UMAC: WlanWhaToDot11TypeConverter::ConvertToWhaTypes: frequency band mask: 0x%02x"), 
        aChannels->iBand );

    TUint32 channelMask ( 0 );
    aWhaChannelCount = 0;
       
    if ( aChannels->iBand & WHA::KBand2dot4GHzMask )
        {            
        // channels in the 2.4GHz band
        //
    
        channelMask = aChannels->iChannels2dot4GHz[1]; // HI part
        channelMask <<= 8;    
        channelMask |= aChannels->iChannels2dot4GHz[0]; // LO part

        // so how many channels have been specified to be scanned
        for ( TUint32 bit = 1; !(bit & endMask2dot4); bit <<= 1 )
            {
            if ( channelMask & bit )
                {
                ++aWhaChannelCount;
                }
            }
        }
    else if ( aChannels->iBand & WHA::KBand4dot9GHzMask )
        {            
        // channels in the 4.9GHz band
        //
        
        channelMask = aChannels->iChannels4dot9GHz[2];
        channelMask <<= 8;    
        channelMask |= aChannels->iChannels4dot9GHz[1];
        channelMask <<= 8;    
        channelMask |= aChannels->iChannels4dot9GHz[0];

        // so how many channels have been specified to be scanned
        for ( TUint32 bit = 1; !(bit & endMask4dot9); bit <<= 1 )
            {
            if ( channelMask & bit )
                {
                ++aWhaChannelCount;
                }
            }
        }
    else
        {
        OsTracePrint(
            KErrorLevel, 
            (TUint8*)
            ("UMAC: WlanWhaToDot11TypeConverter::ConvertToWhaTypes: ERROR: neither 2.4 nor 4.9 GHz band specified"));

        // implementation error
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    OsTracePrint(
        KScan, (TUint8*)
        ("UMAC: WlanWhaToDot11TypeConverter::ConvertToWhaTypes: channel bitmask: 0x%04x"), 
        channelMask );        
    OsTracePrint(
        KScan, (TUint8*)
        ("UMAC: WlanWhaToDot11TypeConverter::ConvertToWhaTypes: %d channels specified to be scanned"), 
        aWhaChannelCount );
        
    if ( !aWhaChannelCount  )
        {
        // no channels specified to be scanned. That's an implementation error

        OsTracePrint( KErrorLevel, (TUint8*)
            ("UMAC: WlanWhaToDot11TypeConverter::ConvertToWhaTypes: ERROR: no channels specified to be scanned") );
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );        
        }

    // allocate memory for the channels
    aWhaChannels = 
        static_cast<WHA::SChannels*>(os_alloc( 
            sizeof( WHA::SChannels ) * aWhaChannelCount )); 
    WHA::SChannels* ptr = aWhaChannels;
    
    if ( !aWhaChannels )
        {
        // alloc failed; we can't continue
        status = EFalse;
        }
    else // alloc success, continue
        {
        // convert to WHA channels
        //
                
        WHA::TChannelNumber channelNumber( 1 );

        if ( aChannels->iBand & WHA::KBand2dot4GHzMask )
            {            
            for ( TUint32 bit = 1; !(bit & endMask2dot4); bit <<= 1 )
                {
                if ( channelMask & bit )
                    {
                    ptr->iChannel = channelNumber;
                    ptr->iMinChannelTime = aMinChannelTime;
                    ptr->iMaxChannelTime = aMaxChannelTime;
                    ptr->iTxPowerLevel = aCtxImpl.iWlanMib.dot11CurrentTxPowerLevel;
                    ++ptr;
                    }
                ++channelNumber;
                }
            }            
        else if ( aChannels->iBand & WHA::KBand4dot9GHzMask )
            {
            for ( TUint32 bit = 1; !(bit & endMask4dot9); bit <<= 1 )
                {
                if ( channelMask & bit )
                    {
                    ptr->iChannel = channelNumber;
                    ptr->iMinChannelTime = aMinChannelTime;
                    ptr->iMaxChannelTime = aMaxChannelTime;
                    ptr->iTxPowerLevel = aCtxImpl.iWlanMib.dot11CurrentTxPowerLevel;
                    ++ptr;
                    }
                ++channelNumber;
                }
            }

#ifndef NDEBUG                                    
        // trace WHA channels        
        ptr = aWhaChannels;
        OsTracePrint(
            KScan, 
            (TUint8*)
            ("UMAC: WlanWhaToDot11TypeConverter::ConvertToWhaTypes: dump channels"));
            
        for ( TUint8 index = 0; index < aWhaChannelCount; ++index )
            {
            OsTracePrint(
                KScan, (TUint8*)
                ("UMAC: WlanWhaToDot11TypeConverter::ConvertToWhaTypes: channelNumber: %d "), 
                ptr->iChannel );
            ++ptr;
            }            
#endif
        } // else (alloc success)

    return status;
    }
