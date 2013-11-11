/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanDot11State class.
*
*/

/*
* %version: 90 %
*/

#include "config.h"
#include "UmacDot11State.h"
#include "UmacWsaAddKey.h"
#include "UmacWsaKeyIndexMapper.h"
#include "umacaddbroadcastwepkey.h"
#include "UmacWsaWriteMib.h"
#include "UmacWsaReadMib.h"
#include "umacwhaconfigurequeue.h"
#include "umacwhaconfigureac.h"
#include "umacconfiguretxautoratepolicy.h"
#include "UmacContextImpl.h"
#include "wha_mibDefaultvalues.h"
#include "FrameXferBlock.h"
#include "umacelementlocator.h"

struct TRate2NwsaRate
    {
    TRate       iTrate;
    WHA::TRate  iNwsaRate;
    };

const TRate2NwsaRate KTRate2NwsaRateTable[] =
    {
        { E1Mbps, WHA::KRate1Mbits },
        { E2Mbps, WHA::KRate2Mbits },
        { E5_5Mbps, WHA::KRate5_5Mbits },
        { E11Mbps, WHA::KRate11Mbits },
        { E22Mbps, WHA::KRate22Mbits },
    };

class TRate2NwsaRatePredicate
    {
public:

    explicit TRate2NwsaRatePredicate( const TRate aRate )
        : iKey( aRate ) {};

    TBool operator() ( const TRate2NwsaRate& aEntry ) const
        {
        return aEntry.iTrate == iKey;
        }

private:

    // Prohibit copy constructor.
    TRate2NwsaRatePredicate ( const TRate2NwsaRatePredicate & );
    // Prohibit assigment operator.
    TRate2NwsaRatePredicate& operator= ( const TRate2NwsaRatePredicate & );

    const TRate iKey;
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::Scan(
    WlanContextImpl& aCtxImpl,
    TScanMode aMode,                    
    const TSSID& aSSID,                 
    TRate aScanRate, 
    SChannels& aChannels,
    TUint32 aMinChannelTime,            
    TUint32 aMaxChannelTime,
    TBool aSplitScan )
    {

    WHA::TRate rate( 0 );
    ResolveScanRate( aCtxImpl, aScanRate, rate );

    // call the "real scan"
    return RealScan( aCtxImpl, aMode, aSSID, rate, aChannels, 
        aMinChannelTime, aMaxChannelTime, aSplitScan );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::StopScan( WlanContextImpl& aCtxImpl )
    {
    // as we are here it means that we have received a stop scan request
    // even though there is no scan ongoing. This should only happen if the
    // mgmt client has been in the process of making a stop scan request and 
    // hasn't noticed that the scan has already completed. Anyhow in this 
    // case we just complete the request
    OnOidComplete( aCtxImpl, KErrNone );
    // signal caller that no state transition occurred
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::SetPowerMode(
    WlanContextImpl& aCtxImpl,
    TPowerMode aPowerMode,
    TBool aDisableDynamicPowerModeManagement,
    TWlanWakeUpInterval aWakeupModeInLightPs, 
    TUint8 aListenIntervalInLightPs,
    TWlanWakeUpInterval aWakeupModeInDeepPs,
    TUint8 aListenIntervalInDeepPs )
    {
    OsTracePrint( KPwrStateTransition, (TUint8*)
        ("UMAC: WlanDot11State::SetPowerMode: aPowerMode: %d"), aPowerMode );

    TBool ret( EFalse );

    // store desired new dot11 power management mode by WLAN Mgmt Client
    aCtxImpl.ClientDot11PwrMgmtMode( aPowerMode );
    
    aCtxImpl.DynamicPwrModeMgtDisabled( aDisableDynamicPowerModeManagement );
    if ( aDisableDynamicPowerModeManagement )
        {
        aCtxImpl.StopPowerModeManagement();
        }
    
    // it is now also our desired dot11 power management mode
    aCtxImpl.DesiredDot11PwrMgmtMode( aCtxImpl.ClientDot11PwrMgmtMode() );

    aCtxImpl.SetClientLightPsModeConfig( 
        aWakeupModeInLightPs, 
        aListenIntervalInLightPs );

    aCtxImpl.SetClientDeepPsModeConfig( 
            aWakeupModeInDeepPs, 
            aListenIntervalInDeepPs );
    
    // in case WLAN Mgmt Client wishes to use PS mode, Light PS is the initial
    // desired PS mode configuration
    aCtxImpl.SetDesiredPsModeConfig( 
        aCtxImpl.ClientLightPsModeConfig() );
    
    if ( aCtxImpl.CurrentDot11PwrMgmtMode() !=
         aCtxImpl.ClientDot11PwrMgmtMode() )
        {
        // there is a difference in current dot11 power management mode and 
        // WLAN Mgmt Client's desired dot11 power management mode
        
        // callee will complete the mgmt command
        ret = OnDot11PwrMgmtTransitRequired( aCtxImpl );
        }
    else
        {
        // no dot11 power management mode transition required.
        
        // See if there is difference in desired vs. current wake-up setting,
        // which we only need to worry about if the requested pwr mgmt
        // mode is PS
        if ( aPowerMode == EPowerModePs && 
             DifferenceInPsModeWakeupSettings( aCtxImpl ) )
            {
            // callee shall complete the request
            ret = OnWlanWakeUpIntervalChange( aCtxImpl );
            }
        else
            {
            // no action required regarding the wake-up setting, either
            
            // one possibility is that the following has happened: WLAN Mgmt
            // client has requested us to use PS mode when possible, but we 
            // have dynamically changed to CAM mode because of data traffic. 
            // If that is the case and now the WLAN Mgmt client wants us
            // to stay in CAM mode, make sure that the dynamic power mode 
            // management is deactivated
            if ( aPowerMode == EPowerModeCam )
                {
                OsTracePrint( KPwrStateTransition, (TUint8*)
                    ("UMAC: WlanDot11State::SetPowerMode: CAM requested when we already are in CAM. Make sure that dynamic pwr mode mgmt is not active") );
                
                aCtxImpl.StopPowerModeManagement();                    
                }
            else
                {
                if ( !aDisableDynamicPowerModeManagement )
                    {
                    // in case the only change is that now dynamic pwr
                    // mode mgmt is again allowed, make sure it is active
                    aCtxImpl.StartPowerModeManagement();
                    }
                }
            }
        
        // complete the mgmt command
        OnOidComplete( aCtxImpl );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::EnableUserData(
    WlanContextImpl& aCtxImpl )
    {
    TBool stateChange( EFalse );
    aCtxImpl.iEnableUserData = ETrue;
    OnOidComplete( aCtxImpl, KErrNone );
    aCtxImpl.iUmac.UserDataReEnabled();

    // signal global state transition event
    return stateChange;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::DisableUserData(
    WlanContextImpl& aCtxImpl )
    {
    aCtxImpl.iEnableUserData = EFalse;
    OnOidComplete( aCtxImpl, KErrNone );
    // signal caller that no state transition occurred
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::OnConfigureUmacMib(
    WlanContextImpl& aCtxImpl,
    TUint16 aRTSThreshold,             
    TUint32 aMaxTxMSDULifetime )        
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::OnConfigureUmacMib") );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::OnConfigureUmacMib: RTSThreshold: %d"), 
        aRTSThreshold );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::OnConfigureUmacMib: maxTxMSDULifetime: %d"), 
        aMaxTxMSDULifetime );

    aCtxImpl.iWlanMib.dot11RTSThreshold = aRTSThreshold;
    
    for ( TUint i = 0; i < WHA::EQueueIdMax; ++i )
        {
        aCtxImpl.iWlanMib.dot11MaxTransmitMSDULifetime[i] = aMaxTxMSDULifetime;
        }

    aCtxImpl.iWlanMib.dot11MaxTransmitMSDULifetimeDefault = aMaxTxMSDULifetime;
            
    for ( TUint i = 0; i < WHA::EQueueIdMax; ++i )
        {
        aCtxImpl.iWlanMib.iMediumTime[i] = KDot11MediumTimeDefault;
        }        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::AddDefaultBroadcastWepKeyComplete( 
    WlanContextImpl& aCtxImpl ) const
    {
    OnOidComplete( aCtxImpl );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::ResolveScanRate( 
    WlanContextImpl& /*aCtxImpl*/,
    const TRate aRate, 
    WHA::TRate& aScanRate )
    {
    const TRate2NwsaRatePredicate unary_predicate( aRate );

    const TRate2NwsaRate* const end 
        = KTRate2NwsaRateTable 
        + (sizeof(KTRate2NwsaRateTable) / sizeof(TRate2NwsaRate));
    const TRate2NwsaRate* pos 
        = find_if( KTRate2NwsaRateTable, end, unary_predicate );

    if ( pos == end )
        {
        // not found; an implementation error
        OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
        }

    aScanRate = pos->iNwsaRate;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::NetworkCapabilityInformationMet(
    WlanContextImpl& aCtxImpl)
    {
    if ( // do this if channel agility bit is not set    
        ( !aCtxImpl.GetCapabilityInformation().IsChannelAgilityBitSet() ))        
        {
        // mimic the preamble bit from AP
        // as some APs might not let us in if not matched
        if ( aCtxImpl.GetCapabilityInformation().IsShortPreambleBitSet() )
            {
            (aCtxImpl.GetAssociationRequestFrame())
                .SetCapabilityShortPreamble();
            (aCtxImpl.GetHtAssociationRequestFrame())
                .SetCapabilityShortPreamble();
            (aCtxImpl.GetReassociationRequestFrame())
                .SetCapabilityShortPreamble();
            (aCtxImpl.GetHtReassociationRequestFrame())
                .SetCapabilityShortPreamble();                
                
            aCtxImpl.UseShortPreamble( ETrue );
            }
        else
            {
            (aCtxImpl.GetAssociationRequestFrame())
                .ClearCapabilityShortPreamble();
            (aCtxImpl.GetHtAssociationRequestFrame())
                .ClearCapabilityShortPreamble();
            (aCtxImpl.GetReassociationRequestFrame())
                .ClearCapabilityShortPreamble();
            (aCtxImpl.GetHtReassociationRequestFrame())
                .ClearCapabilityShortPreamble();                

            aCtxImpl.UseShortPreamble( EFalse );
            }

        // clear both CF fields as we don't support PCF
        aCtxImpl.GetAssociationRequestFrame().ClearCFfields();
        aCtxImpl.GetHtAssociationRequestFrame().ClearCFfields();
        aCtxImpl.GetReassociationRequestFrame().ClearCFfields();
        aCtxImpl.GetHtReassociationRequestFrame().ClearCFfields();

        // clear also all the fields we don't understand
        aCtxImpl.GetAssociationRequestFrame().ClearReservedFields();
        aCtxImpl.GetHtAssociationRequestFrame().ClearReservedFields();
        aCtxImpl.GetReassociationRequestFrame().ClearReservedFields();
        aCtxImpl.GetHtReassociationRequestFrame().ClearReservedFields();

        // we don't do PBCC
        aCtxImpl.GetAssociationRequestFrame().ClearCapabilityPbcc();
        aCtxImpl.GetHtAssociationRequestFrame().ClearCapabilityPbcc();
        aCtxImpl.GetReassociationRequestFrame().ClearCapabilityPbcc();
        aCtxImpl.GetHtReassociationRequestFrame().ClearCapabilityPbcc();
        
        if ( aCtxImpl.EncryptionStatus() != EEncryptionDisabled )
            {
            // privacy desired
            aCtxImpl.GetAssociationRequestFrame().SetWepBit();
            aCtxImpl.GetHtAssociationRequestFrame().SetWepBit();
            aCtxImpl.GetReassociationRequestFrame().SetWepBit();
            aCtxImpl.GetHtReassociationRequestFrame().SetWepBit();                
            }
        else
            {
            aCtxImpl.GetAssociationRequestFrame().ClearWepBit();
            aCtxImpl.GetHtAssociationRequestFrame().ClearWepBit();
            aCtxImpl.GetReassociationRequestFrame().ClearWepBit();
            aCtxImpl.GetHtReassociationRequestFrame().ClearWepBit();                
            }
        if ( aCtxImpl.RadioMeasurement() != EFalse )
            {
            // Radio measurements desired
            OsTracePrint (KInfoLevel, (TUint8 *)("UMAC: Radio measurements enabled"));
            aCtxImpl.GetAssociationRequestFrame().SetRMBit();
            aCtxImpl.GetHtAssociationRequestFrame().SetRMBit();
            aCtxImpl.GetReassociationRequestFrame().SetRMBit();
            aCtxImpl.GetHtReassociationRequestFrame().SetRMBit();                
            }
        else
            {
            OsTracePrint (KInfoLevel, (TUint8 *)("UMAC: Radio measurements disabled"));
            aCtxImpl.GetAssociationRequestFrame().ClearRMBit();
            aCtxImpl.GetHtAssociationRequestFrame().ClearRMBit();
            aCtxImpl.GetReassociationRequestFrame().ClearRMBit();
            aCtxImpl.GetHtReassociationRequestFrame().ClearRMBit();                
            }        
        return ETrue;
        }
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::AreSupportedRatesMet(
    WlanContextImpl& aCtxImpl, 
    TBool aCheckAlsoExtendedRates )
    {   
    OsTracePrint( KWarningLevel, 
        (TUint8*)("UMAC: WlanDot11State::AreSupportedRatesMet") );

    // make sure these critters are zeroed at the beginning

    aCtxImpl.GetMinBasicRate() = 0;
    aCtxImpl.GetMaxBasicRate() = 0;
    aCtxImpl.ClearBasicRateSet();

    // clear our existing supported rates IE
    aCtxImpl.GetOurSupportedRatesIE().Clear();
    // ... and our existing extended supported rates IE
    aCtxImpl.GetOurExtendedSupportedRatesIE().Clear();

    TUint32 tx_rates( 0 );
    TUint8 nwRate( 0 );

    // go through all supported rate elements in the supported rates IE 
    // ( max 8 ).
    // Remenber that basic rates are also always part of supported rates 
    // when building supported rates bitmask
    for ( TUint32 outer_idx = 0 
        // loop until element count in the IE is reached
        ; ( outer_idx != aCtxImpl.GetApSupportedRatesIE().GetElementLength() )
        // OR maximum length allowed for the IE is reached
        // NOTE! The 802.11 standard specifies that the max length of the 
        // information part of this IE is eight rates (eight bytes).
        // However at least some APs seem to put all their supported rates
        // into this element. In order to be able to associate with those
        // APs we have allocated enough space for all 802.11b/g rates in this
        // IE and hence the following constant in the loop end condition
        && outer_idx < KMaxNumberOfDot11bAndgRates
        ; ++outer_idx )
        {
        nwRate = (aCtxImpl.GetApSupportedRatesIE())[outer_idx];
        
        if ( !ProcessSingleSupportedRateElement( aCtxImpl, nwRate, tx_rates ) )
            {
            // unknown supported rates element encountered
            if ( nwRate & KBasicRateMask )
                {
                // it is part of BSS Basic Rate Set 
                if ( aCtxImpl.BssMembershipFeatureSupported( nwRate ) )
                    {
                    // it is a WLAN feature which we support, 
                    // so we can continue. No action required here
                    }
                 else
                    {
                    // we can't cope with it and shall abort

                    OsTracePrint( KWarningLevel, (TUint8*)
                        ("UMAC: WlanDot11State::AreSupportedRatesMet: network has unsupported mandatory rate/feature -> abort") );
                    OsTracePrint( KWarningLevel, (TUint8*)
                        ("UMAC: WlanDot11State::AreSupportedRatesMet: rate: 0x%02x"), 
                        nwRate);                    

                    return EFalse;
                    }
                }
            else
                {
                // unknown element is not part of BSS Basic Rate Set
                // we can cope with that 
                OsTracePrint( KWarningLevel, (TUint8*)
                    ("UMAC: WlanDot11State::AreSupportedRatesMet: init network connect") );
                OsTracePrint( KWarningLevel, (TUint8*)
                    ("UMAC: WlanDot11State::AreSupportedRatesMet: network has unsupported supported rate -> continue") );
                OsTracePrint( KWarningLevel, (TUint8*)
                    ("UMAC: WlanDot11State::AreSupportedRatesMet: rate: 0x%02x"), 
                    nwRate);
                }
            }
        } // end outer for

    if ( aCheckAlsoExtendedRates )
        {
        // go through all supported rate elements in the extended supported 
        // rates IE ( max 255 ).
        // Remember that basic rates are also always part of supported rates 
        // when building supported rates bitmask
        for ( TUint32 outer_idx = 0; 
              // loop until max element count in the IE is reached
              ( outer_idx != aCtxImpl.GetApExtendedSupportedRatesIE().GetElementLength() )
              // OR maximum length allowed for the IE is reached
              && outer_idx < KMaxNumberOfExtendedRates; 
              ++outer_idx )
            {
            nwRate = (aCtxImpl.GetApExtendedSupportedRatesIE())[outer_idx];        

            if ( !ProcessSingleSupportedRateElement( 
                    aCtxImpl, 
                    nwRate, 
                    tx_rates ) )
                {
                // unknown supported rates element encountered
                
                if ( nwRate & KBasicRateMask )
                    {
                    // it is part of BSS Basic Rate Set 
                    
                    if ( aCtxImpl.BssMembershipFeatureSupported( nwRate ) )
                        {
                        // it is a WLAN feature which we support, 
                        // so we can continue. No action required here
                        }
                     else
                        {
                        // we can't cope with it and shall abort

                        OsTracePrint( KWarningLevel, (TUint8*)
                            ("UMAC: WlanDot11State::AreSupportedRatesMet: network has unsupported mandatory rate/feature -> abort") );
                        OsTracePrint( KWarningLevel, (TUint8*)
                            ("UMAC: WlanDot11State::AreSupportedRatesMet: rate: 0x%02x"), 
                            nwRate);                    

                        return EFalse;
                        }
                    }
                else
                    {
                    // unknown element is not part of BSS Basic Rate Set
                    // we can cope with that 
                    OsTracePrint( KWarningLevel, (TUint8*)
                        ("UMAC: WlanDot11State::AreSupportedRatesMet: init network connect") );
                    OsTracePrint( KWarningLevel, (TUint8*)
                        ("UMAC: WlanDot11State::AreSupportedRatesMet: network has unsupported supported rate -> continue") );
                    OsTracePrint( KWarningLevel, (TUint8*)
                        ("UMAC: WlanDot11State::AreSupportedRatesMet: rate: 0x%02x"), 
                        nwRate);
                    }
                }
            } // for
        } // if

    if ( tx_rates )
        {
        // store common rates (between us & the nw) in our connection context
        aCtxImpl.RateBitMask( tx_rates );
    
        return ETrue;
        }
    else
        {
        // we ended up with an empty rate mask, i.e. the intersection
        // of network's supported rates and our supported rates is empty.
        // We are not able to join the network under these circumstances 
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::ProcessSingleSupportedRateElement(
    WlanContextImpl& aCtxImpl,
    const TUint8 aApRate,
    TUint32& aRateBitmask )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11State::ProcessSingleSupportedRateElement: processing AP rate: 0x%02x"), aApRate);

    // match 4 elements in our rates lookup table
    const TUint num_of_elems = 
        sizeof(aCtxImpl.iSupportedRatesLookUpTable) 
        / sizeof(WlanContextImpl::SupportedRateLookUp);

    for ( TUint32 inner_idx = 0; 
          inner_idx != ( num_of_elems + 1 ); 
          ++inner_idx )
        {
        if ( inner_idx == num_of_elems)
            {
            // if this block is reached it means that we have encountered
            // a supported rates element that is not in our lookup table
            OsTracePrint( KWarningLevel | KUmacDetails , (TUint8*)
                ("UMAC: unknown AP rate element found: 0x%02x"), aApRate);

            return EFalse;
            }
        if ( ( aApRate & (~KBasicRateMask) ) == 
             ( aCtxImpl.iSupportedRatesLookUpTable[inner_idx].iSupportedRate & 
               ( ~KBasicRateMask ) ) )
            {
            // make a rate for the tx rate adaptation object
            aRateBitmask |=  
                (aCtxImpl.iSupportedRatesLookUpTable[inner_idx].iWsaRate);

            // check if this critter is part of a mandatory rate set
            if ( aApRate & KBasicRateMask )
                {
                OsTracePrint( KUmacDetails, 
                    (TUint8*)("UMAC: rate is part of Basic Rate Set") );

                // yes it is 
                // construct basic rate set mask for Join NWSA command
                aCtxImpl.BasicRateSetBitSet( 
                    aCtxImpl.iSupportedRatesLookUpTable[inner_idx].iWsaRate );

                // store min basic rate
                if ( aCtxImpl.GetMinBasicRate() == 0 )
                    {
                    aCtxImpl.GetMinBasicRate() = 
                        (aCtxImpl.iSupportedRatesLookUpTable[inner_idx]).iWsaRate;
                    }
                // and store max basic rate
                if ( aCtxImpl.GetMaxBasicRate() < 
                    (aCtxImpl.iSupportedRatesLookUpTable[inner_idx]).iWsaRate )
                    {
                    aCtxImpl.GetMaxBasicRate() 
                        = (aCtxImpl.iSupportedRatesLookUpTable[inner_idx])
                        .iWsaRate;
                    }
                }

            // set this rate to our supported rates IE 
            // which we will send in assoc-request frame. If there's no space any 
            // more in supported rates IE use the extended supported rates IE            
            //
            if (aCtxImpl.GetOurSupportedRatesIE().GetElementLength() < KMaxNumberOfRates )
                {
                aCtxImpl.GetOurSupportedRatesIE().Append( aApRate );                
                }
            else
                {                
                aCtxImpl.GetOurExtendedSupportedRatesIE().Append( aApRate );
                }

            // break out of for loop, 
            // we got a match no need to traverse any longer
            break;
            }
        } // end for

    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11State::SetArpIpAddressTableMib(
    WlanContextImpl& aCtxImpl,
    TBool aEnableFiltering,
    TIpv4Address aIpv4Address )
    {
    const TUint32 KMibLength( sizeof( WHA::SarpIpAddressTable ) );

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: WlanDot11State::SetArpIpAddressTableMib: mibLength: %d"), 
        KMibLength );        

    // allocate memory for the mib to write
    WHA::SarpIpAddressTable* mib 
        = static_cast<WHA::SarpIpAddressTable*>
        (os_alloc( KMibLength )); 

    if ( !mib )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::SetArpIpAddressTableMib: memory allocating failed") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }
    
    if ( aEnableFiltering )
        {
        mib->iEnable = ETrue;
        mib->iIpV4Addr = aIpv4Address;
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: WlanDot11State::SetArpIpAddressTableMib: enable filtering using address: 0x%08x"),
            aIpv4Address );
        }
    else
        {
        mib->iEnable = EFalse;
        mib->iIpV4Addr = aIpv4Address;  // value not relevant in this case
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: WlanDot11State::SetArpIpAddressTableMib: disable filtering") );
        }

    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
        
    wha_cmd.Set( 
        aCtxImpl, 
        WHA::KMibArpIpAddressTable, 
        KMibLength, 
        mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd,            // next state
        // the ACT
        KCompleteManagementRequest
        );   

    os_free( mib ); // release the allocated memory

    // signal caller that a state transition occurred
    return ETrue;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::OnDot11PwrMgmtTransitRequired( 
    WlanContextImpl& aCtxImpl )
    {
    // the specified power state will be taken into use later
    // So nothing more to do at this point than completing the oid
    OnOidComplete( aCtxImpl );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WHA::TQueueId WlanDot11State::QueueId( 
    const WlanContextImpl& aCtxImpl,
    const TUint8* aDot11MacHeader ) const
    {
    // this initial value is always returned if we have a non-QoS connection
    WHA::TQueueId queue_id( WHA::ELegacy );

    if ( aCtxImpl.QosEnabled() )
        {
        // a QOS connection
        SQosDataMpduHeader* dot11_hdr 
            = reinterpret_cast<SQosDataMpduHeader*>(
            const_cast<TUint8*>(aDot11MacHeader));

        // determine frame type
        const TUint8 KFrameType( dot11_hdr->iHdr.iHdr.GetFrameControl().iType );
        
        if ( KFrameType == E802Dot11FrameTypeQosData )
            {
            // this is a QoS data frame so assign it to 
            // the correct queue according to the priority

            OsTracePrint( KQos, (TUint8*)
                ("UMAC: WlanDot11State::QueueId: 802.1d priority: %d"),
                dot11_hdr->iHdr.UserPriority() );

            queue_id = Queue( dot11_hdr->iHdr.UserPriority() );            
            }
        else 
            {
            // a non data type frame is put to voice queue
            queue_id = WHA::EVoice;
            }
        }
        
    OsTracePrint( KQos, (TUint8*)
        ("UMAC: WlanDot11State::QueueId: use queue: %d"),
        queue_id );

    return queue_id;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::TxDeauthenticate( 
    WlanContextImpl& aCtxImpl, 
    T802Dot11ManagementReasonCode aReason,
    TBool aWaitIfIntTxBufNotFree ) const
    {
    OsTracePrint( KUmacProtocolState | KUmacAuth, 
        (TUint8*)("UMAC: WlanDot11State::TxDeauthenticate") );
            
    TBool status ( EFalse );

    // client doesn't have to take care of the tx buffer header space
    // as the method below does that by itself
    TUint8* start_of_frame = aCtxImpl.TxBuffer( aWaitIfIntTxBufNotFree );

    if ( start_of_frame )
        {
        TUint32 frameLength ( 0 );

        // construct deauthenticate frame
        // note that we don't have to set SA because we have already set it
        // in the initialize phase of the state machine
    
        if ( aCtxImpl.HtSupportedByNw() )
            {
            // set the BSSID   
            (aCtxImpl.GetHtDeauthenticateFrame()).iHeader.iBSSID = aCtxImpl.GetBssId();
            // set the DA
            (aCtxImpl.GetHtDeauthenticateFrame()).iHeader.iDA = aCtxImpl.GetBssId();
            // set the reason code
            (aCtxImpl.GetHtDeauthenticateFrame()).iReasonCode.SetReasonCode(
                aReason );
            
            frameLength = sizeof( SHtDeauthenticateFrame );
            
            // copy deauthentication frame to tx-buffer to correct offset
            os_memcpy( 
                start_of_frame,
                &(aCtxImpl.GetHtDeauthenticateFrame()), 
                frameLength );            
            }
        else
            {
            // set the BSSID   
            (aCtxImpl.GetDeauthenticateFrame()).iHeader.iBSSID = aCtxImpl.GetBssId();
            // set the DA
            (aCtxImpl.GetDeauthenticateFrame()).iHeader.iDA = aCtxImpl.GetBssId();
            // set the reason code
            (aCtxImpl.GetDeauthenticateFrame()).iReasonCode.SetReasonCode(
                aReason );
    
            frameLength = sizeof( SDeauthenticateFrame );
            
            // copy deauthentication frame to tx-buffer to correct offset
            os_memcpy( 
                start_of_frame,
                &(aCtxImpl.GetDeauthenticateFrame()), 
                frameLength );
            }
    
        const WHA::TQueueId queue_id( QueueId( aCtxImpl, start_of_frame ) );
    
        // send deauthentication frame by pushing it to the packet scheduler
        status = aCtxImpl.PushPacketToPacketScheduler( 
                    start_of_frame, 
                    frameLength, 
                    queue_id,
                    E802Dot11FrameTypeDeauthentication,
                    NULL,
                    EFalse,
                    EFalse,
                    ETrue );
        }
    else
        {
        // we didn't get a Tx buffer => frame not sent. EFalse will be returned
        // to indicate that

        OsTracePrint( KUmacProtocolState | KUmacAuth, (TUint8*)
            ("UMAC: no free internal tx buf => frame not sent") );
        }

    return status;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::TxDisassociate(
    WlanContextImpl& aCtxImpl, 
    T802Dot11ManagementReasonCode aReason,
    TBool aWaitIfIntTxBufNotFree ) const
    {
    OsTracePrint( KUmacProtocolState | KUmacAssoc, 
        (TUint8*)("UMAC: WlanDot11State::TxDisassociate") );

    TBool status ( EFalse );

    // client doesn't have to take care of the tx buffer header space
    // as the method below does that by itself
    TUint8* start_of_frame = aCtxImpl.TxBuffer( aWaitIfIntTxBufNotFree );

    if ( start_of_frame )
        {
        TUint32 frameLength ( 0 );

        // NOTE: We don't set the address fields to frame here like 
        // in the case of dot11-deauthenticate frame, because in the case of
        // roaming we would use that BSS's information where we are roaming 
        // to - instead of the existing one 
    
        if ( aCtxImpl.HtSupportedByNw() )
            {
            (aCtxImpl.GetHtDisassociationFrame()).iReasonCode.SetReasonCode(
                aReason );    
            
            frameLength = sizeof( SHtDisassociateFrame );
            
            // copy disassociation frame to tx-buffer to correct offset
            os_memcpy( 
                start_of_frame,
                &(aCtxImpl.GetHtDisassociationFrame()), 
                frameLength );            
            }
        else
            {
            (aCtxImpl.GetDisassociationFrame()).iReasonCode.SetReasonCode(
                aReason );    
            
            frameLength = sizeof( SDisassociateFrame );
            
            // copy disassociation frame to tx-buffer to correct offset
            os_memcpy( 
                start_of_frame,
                &(aCtxImpl.GetDisassociationFrame()), 
                frameLength );
            }
    
        const WHA::TQueueId queue_id 
            = QueueId( aCtxImpl, start_of_frame );
    
        // send disassociation frame to the current AP by pushing it to the packet
        // scheduler
        status = aCtxImpl.PushPacketToPacketScheduler( 
                    start_of_frame, 
                    frameLength, 
                    queue_id,
                    E802Dot11FrameTypeDisassociation,
                    NULL,
                    EFalse,
                    EFalse,
                    ETrue );
        }
    else
        {
        // we didn't get a Tx buffer => frame not sent. EFalse will be returned
        // to indicate that

        OsTracePrint( KUmacProtocolState | KUmacAuth, (TUint8*)
            ("UMAC: no free internal tx buf => frame not sent") );
        }

    return status;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WHA::SAesPairwiseKey* WlanDot11State::CreateAesPtkCtx( 
    WlanContextImpl& aCtxImpl,
    WlanWsaAddKey& aWhaAddKey,
    const TUint8* aData, 
    const TMacAddress& aMacAddr )
    {
    // store info of AES PTK insertion
    aCtxImpl.PairWiseKeyType( WHA::EAesPairWiseKey );

    // allocate memory for the key structure
    WHA::SAesPairwiseKey* key = static_cast<WHA::SAesPairwiseKey*>
        ( os_alloc( sizeof( WHA::SAesPairwiseKey ) ) ); 

    if ( key )
        {
        os_memcpy( 
            key->iMacAddr.iMacAddress, 
            aMacAddr.iMacAddress,
            WHA::TMacAddress::KMacAddressLength );
        os_memcpy( key->iAesKey, aData, WHA::KAesKeyLength );
        
        aWhaAddKey.Set( 
            aCtxImpl, 
            WHA::EAesPairWiseKey,
            key,
            WlanWsaKeyIndexMapper::Extract( WHA::EAesPairWiseKey ) );
        }
    else
        {
        // left intentionally empty
        }

    return key;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WHA::STkipPairwiseKey* WlanDot11State::CreateTkipPtkCtx( 
    WlanContextImpl& aCtxImpl,
    WlanWsaAddKey& aWhaAddKey,
    const TUint8* aData, 
    T802Dot11WepKeyId aKeyIndex, 
    const TMacAddress& aMacAddr )
    {
    // store info of TKIP PTK insertion
    aCtxImpl.PairWiseKeyType( WHA::ETkipPairWiseKey );

    // allocate memory for the key structure
    // the caller of this method will deallocate the memory
    WHA::STkipPairwiseKey* key = static_cast<WHA::STkipPairwiseKey*>
        (os_alloc( sizeof( WHA::STkipPairwiseKey ) )); 

    if ( key )
        {
        os_memcpy( 
            key->iMacAddr.iMacAddress, 
            aMacAddr.iMacAddress,
            WHA::TMacAddress::KMacAddressLength );
        os_memcpy( key->iTkipKey, aData, WHA::KTKIPKeyLength );
        os_memcpy( 
            key->iRxMicKey, 
            aData + WHA::KTKIPKeyLength, 
            WHA::KMicLength );
        os_memcpy( 
            key->iTxMicKey, 
            aData + WHA::KTKIPKeyLength + WHA::KMicLength, 
            WHA::KMicLength);
        key->iKeyId =  static_cast<WHA::TPrivacyKeyId>(aKeyIndex);
        
        aWhaAddKey.Set( aCtxImpl, 
            WHA::ETkipPairWiseKey,
            key,
            WlanWsaKeyIndexMapper::Extract( WHA::ETkipPairWiseKey ) );
        }
    else
        {
        // left intentionally empty
        }
    
    return key;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WHA::SWepPairwiseKey* WlanDot11State::CreateUnicastWepKeyCtx(
    WlanContextImpl& aCtxImpl,
    WlanWsaAddKey& aWhaAddKey,
    const TMacAddress& aMacAddr,
    TUint32 aKeyLength,                      
    const TUint8* aKey )
    {
    // store info of WEP PTK insertion
    aCtxImpl.PairWiseKeyType( WHA::EWepPairWiseKey );

    // allocate memory for the key structure
    WHA::SWepPairwiseKey* key = static_cast<WHA::SWepPairwiseKey*>
        (os_alloc( WHA::SWepPairwiseKey::KHeaderSize + aKeyLength )); 

    if ( key )
        {
        os_memcpy( 
            &(key->iMacAddr), 
            aMacAddr.iMacAddress,
            sizeof(key->iMacAddr) );
        key->iKeyLengthInBytes = static_cast<TUint8>(aKeyLength);
        os_memcpy( key->iKey, aKey, key->iKeyLengthInBytes );
    
        aWhaAddKey.Set( aCtxImpl, 
            WHA::EWepPairWiseKey,
            key,
            WlanWsaKeyIndexMapper::Extract( WHA::EWepPairWiseKey ) );
        }
    else
        {
        // intentionally left empty
        }
    
    return key;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WHA::SWapiPairwiseKey* WlanDot11State::CreateWapiPtkCtx( 
    WlanContextImpl& aCtxImpl,
    WlanWsaAddKey& aWhaAddKey,
    const TUint8* aData, 
    T802Dot11WepKeyId aKeyIndex, 
    const TMacAddress& aMacAddr )
    {
    // store info of WAPI pairwise key insertion
    aCtxImpl.PairWiseKeyType( WHA::EWapiPairWiseKey );

    // allocate memory for the key structure
    // the caller of this method will deallocate the memory
    WHA::SWapiPairwiseKey* key = static_cast<WHA::SWapiPairwiseKey*>
        (os_alloc( sizeof( WHA::SWapiPairwiseKey ) )); 

    if ( key )
        {
        os_memcpy( 
            key->iMacAddr.iMacAddress, 
            aMacAddr.iMacAddress,
            WHA::TMacAddress::KMacAddressLength );

        key->iKeyId =  static_cast<WHA::TPrivacyKeyId>(aKeyIndex);

        os_memcpy( key->iWapiKey, aData, WHA::KWapiKeyLength );

        os_memcpy( 
            key->iMicKey, 
            aData + WHA::KWapiKeyLength, 
            WHA::KWapiMicKeyLength );
        
        aWhaAddKey.Set( aCtxImpl, 
            WHA::EWapiPairWiseKey,
            key,
            WlanWsaKeyIndexMapper::Extract( WHA::EWapiPairWiseKey ) );
        }
    else
        {
        // left intentionally empty
        }
    
    return key;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WHA::TRate WlanDot11State::InitialSpecialFrameTxRate( 
    const WlanContextImpl& aCtxImpl ) const
    {
    WHA::TRate rateToUse ( WHA::KRate1Mbits );
    
    const TUint32 basicRateSet = aCtxImpl.BasicRateSet();
    
    if ( basicRateSet )
        {
        // there is at least one supported basic rate (which is the way things
        // are supposed to be if the network is correctly configured)
        
        if ( basicRateSet & WHA::KRate1Mbits )
            {
            rateToUse = WHA::KRate1Mbits;
            }
        else if ( basicRateSet & WHA::KRate6Mbits )
            {
            rateToUse = WHA::KRate6Mbits;
            }
        else if ( basicRateSet & WHA::KRate2Mbits )
            {
            rateToUse = WHA::KRate2Mbits;
            }
        else if ( basicRateSet & WHA::KRate9Mbits )
            {
            rateToUse = WHA::KRate9Mbits;
            }
        else
            {
            // none of the rates above is a supported basic rate, which is 
            // rather odd.
            // Anyhow, figure out the lowest supported basic rate and use that

            WHA::TRate rateCandidate = WHA::KRate1Mbits;

            do            
                {
                if ( basicRateSet & rateCandidate )
                    {
                    rateToUse = rateCandidate;
                    // break the loop as we found what we are looking for
                    break;
                    }
                else
                    {
                    rateCandidate <<= 1;
                    }                
                
                } while ( rateCandidate <= WHA::KRate54Mbits );
            }                
        }
    else
        {
        // there are no supported basic rates; which is actually a network 
        // configuration error. 
        OsTracePrint( KWarningLevel, 
            (TUint8*)("UMAC: WlanDot11State::InitialSpecialFrameTxRate: Warning: no basic rates configured in nw"));
        
        // But as we don't absolutely have to find a 
        // basic rate here, just stay with 1Mbps (set above)
        }    
        
    OsTracePrint( KWsaTxDetails, 
        (TUint8*)("UMAC: WlanDot11State::InitialSpecialFrameTxRate: use rate: 0x%08x"), 
        rateToUse );

    return rateToUse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::DoErrorIndication( 
    WlanContextImpl& aCtxImpl, 
    WHA::TStatus /*aStatus*/ )
    {
    OsTracePrint( KWarningLevel, 
        (TUint8*)("UMAC * handle an error indication!") );

    // this is the one and only global error handler
    ChangeState( aCtxImpl, *this, aCtxImpl.iStates.iMacError );

    // signal with return value that a state transition occurred
    return ETrue;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::DoConsecutiveBeaconsLostIndication( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::DoRegainedBSSIndication( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::DoRadarIndication( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::DoRcpiIndication( 
    WlanContextImpl& /*aCtxImpl*/,
    WHA::TRcpi /*aRcpi*/ )
    {
    // not supported in default handler
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11State::DoPsModeErrorIndication( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::TxData( 
    WlanContextImpl& aCtxImpl,
    TDataBuffer& aDataBuffer,
    TBool /*aMore*/ )
    {
    OsTracePrint( KErrorLevel, (TUint8*)
        ("UMAC: Tx attempted when it's not allowed; frame ignored") );
    
    aCtxImpl.iUmac.OnTxProtocolStackDataComplete( 
        KErrNone,
        &aDataBuffer );        

    // no state change occurred
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::TxMgmtData( 
    WlanContextImpl& aCtxImpl,
    TDataBuffer& /*aDataBuffer*/ )
    {
    // as we are not connected to network, this frame cannot be sent.
    // Just complete the frame send request without error status; as
    // the WLAN Mgmt Client doesn't care about the status
    OnMgmtPathWriteComplete( aCtxImpl );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TAny* WlanDot11State::RequestForBuffer( 
    WlanContextImpl& /*aCtxImpl*/,             
    TUint16 /*aLength*/ )
    {
    // no functionality in default handler
    return NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::ReceivePacket( 
    WlanContextImpl& aCtxImpl, 
    WHA::TStatus /*aStatus*/,
    const void* /*aFrame*/,
    TUint16 /*aLength*/,
    WHA::TRate /*aRate*/,
    WHA::TRcpi /*aRcpi*/,
    WHA::TChannelNumber /*aChannel*/,
    TUint8* aBuffer,
    TUint32 /*aFlags*/ )
    {
    // release the Rx buffer
    aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::OnWhaCommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus /*aStatus*/,
    const WHA::UCommandResponseParams& aCommandResponseParams,
    TUint32 aAct )
    {
    // this command response method is supposed to be called
    // when a concrete dot11 state object does not care about 
    // the command response parameters

    if ( aAct == KCompleteManagementRequest )
        {
        // this must be a response to a command that was generated 
        // by this this dot11 state object default layer
        if ( aCommandId != WHA::EReadMIBResponse )
            {
            OnOidComplete( aCtxImpl );        
            }
        else
            {
            // as this is a MIB read request, we must supply the
            // response to the Mgmt Client
            if ( aCommandResponseParams.iReadMibResponse.iMib 
                == WHA::KMibStatisticsTable )
                {
                // convert to a 32bit value before forwarding
                TInt32 rcpi = reinterpret_cast
                    <const WHA::SstatisticsTable*>
                    (aCommandResponseParams.iReadMibResponse.iData)->iRcpi;

                OsTracePrint( 
                    KUmacDetails, 
                    (TUint8*)
                    ("UMAC: WlanDot11State::OnWhaCommandResponse(): last rcpi: %d"), 
                    rcpi );

                OnOidComplete( aCtxImpl, 
                               KErrNone, 
                               reinterpret_cast<const TAny*>(&rcpi),
                               sizeof(rcpi) );        
                }
            else if ( aCommandResponseParams.iReadMibResponse.iMib 
                == WHA::KMibCountersTable )
                {
                const WHA::ScountersTable* countersTable = reinterpret_cast
                    <const WHA::ScountersTable*>
                    (aCommandResponseParams.iReadMibResponse.iData);

                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanDot11State::OnWhaCommandResponse: countersTableMib: nbr of FCS errors in received MPDUs: %d"), 
                    countersTable->iFcsError );

                aCtxImpl.StoreFcsErrorCount( countersTable->iFcsError );
                
                // as we are about to report the frame statistics results,
                // it's the time to perform also the necessary calculations
                
                aCtxImpl.CalculateAverageTxMediaDelays();                
                aCtxImpl.CalculateAverageTotalTxDelays();
             
                const TStatisticsResponse& frameStatistics ( aCtxImpl.FrameStatistics() );
                   
#ifndef NDEBUG                
                // trace frame statistics

                OsTracePrint( 
                    KUmacDetails, (TUint8*)
                    ("UMAC: WlanDot11State::OnWhaCommandResponse: *** reported frame statistics ***:") );

                for ( TUint i = 0; i < EQueueIdMax; ++i )
                    {
                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: * Access Category: %d *"),
                        i );

                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: successfully received unicast data frames: %d"), 
                         frameStatistics.acSpecific[i].rxUnicastDataFrameCount );

                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: successfully transmitted unicast data frames: %d"), 
                         frameStatistics.acSpecific[i].txUnicastDataFrameCount );

                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: successfully received multicast data frames: %d"), 
                         frameStatistics.acSpecific[i].rxMulticastDataFrameCount );
                    
                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: successfully transmitted multicast data frames: %d"), 
                         frameStatistics.acSpecific[i].txMulticastDataFrameCount );

                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: nbr of data frame transmit retries: %d"), 
                         frameStatistics.acSpecific[i].txRetryCount );

                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: nbr of data frame WLAN delivery failures: %d"), 
                         frameStatistics.acSpecific[i].txErrorCount );                    

                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: average data frame Tx media delay in microsecs: %d"), 
                         frameStatistics.acSpecific[i].txMediaDelay );                    

                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: average data frame total Tx delay in microsecs: %d"), 
                         frameStatistics.acSpecific[i].totalTxDelay );    
                         
                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: data frame total Tx delay bin 0 count: %d"), 
                         frameStatistics.acSpecific[i].totalTxDelayBin0 );

                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: data frame total Tx delay bin 1 count: %d"), 
                         frameStatistics.acSpecific[i].totalTxDelayBin1 );

                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: data frame total Tx delay bin 2 count: %d"), 
                         frameStatistics.acSpecific[i].totalTxDelayBin2 );

                    OsTracePrint( 
                        KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::OnWhaCommandResponse: data frame total Tx delay bin 3 count: %d"), 
                         frameStatistics.acSpecific[i].totalTxDelayBin3 );
                    }

                OsTracePrint( 
                    KUmacDetails, (TUint8*)
                    ("UMAC: WlanDot11State::OnWhaCommandResponse: all ACs: nbr of FCS errors in received MPDUs: %d"), 
                     frameStatistics.fcsErrorCount );

#endif

                OnOidComplete( aCtxImpl, 
                               KErrNone, 
                               reinterpret_cast<const TAny*>(&frameStatistics),
                               sizeof( frameStatistics ) );

                // the statistics are cleared after reporting them. It is safe
                // to do it here as the information has already been copied
                aCtxImpl.ResetFrameStatistics();
                }
            // -- == WHA::KMibStatisticsTable --
            else
                {
                // this thing should never happen as we are not reading any 
                // other MIBs; an implementation error
                OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );                
                // well exclusion to this is the dot11stationId MIB that 
                // is read by dot11initphase object, but it overrides this
                // method so this code is not executed in that case.
                }
            }
        }   
    // -- aAct == KCompleteManagementRequest --
    else
        {
        // this is not a response to a command that was generated by this
        // dot11 state object.
        // which means that the originator of this command does not care
        // about the response parameters.
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::OnWlanWakeUpIntervalChange( 
    WlanContextImpl& aCtxImpl )
    {
    OnOidComplete( aCtxImpl );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WHA::TQueueId WlanDot11State::Queue( TUint8 aPriority )
    {
    // Mapping of 802.1d priorities to WMM Access Categories, and hence to
    // WHA queue (id)s, as specified in WiFi WMM Specification v1.1
    const WHA::TQueueId K802Dot1dPriority2WhaQueueTable[] = 
        { WHA::ELegacy,     // for priority 0
          WHA::EBackGround, // for priority 1
          WHA::EBackGround, // for priority 2
          WHA::ELegacy,     // for priority 3
          WHA::EVideo,      // for priority 4
          WHA::EVideo,      // for priority 5
          WHA::EVoice,      // for priority 6
          WHA::EVoice };    // for priority 7
          
    return ( aPriority > 7 ) ? 
        WHA::ELegacy :  
        K802Dot1dPriority2WhaQueueTable[aPriority];    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::UapsdEnabledInNetwork( const SRxWmmIeData& aRxWmmIE )
    {
    return ( aRxWmmIE.IsUapsdBitSet() );
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::UapsdEnabledInNetwork( const SWmmParamElemData& aWmmParamElem )
    {
    return ( aWmmParamElem.IsUapsdBitSet() );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::EnableQos( WlanContextImpl& aCtxImpl, 
    TBool aUapsdEnabledInNw )
    {
    // enable QoS
    aCtxImpl.QosEnabled( ETrue );

    // first clear our WMM IE so that U-APSD flags will be set only when 
    // they are supposed to be set
    aCtxImpl.OurWmmIe().Clear();                    

    if ( aUapsdEnabledInNw )
        {
        OsTracePrint( 
            KQos, (TUint8*)
            ("UMAC: WlanDot11State::EnableQos(): u-apsd is supported by nw"));
        
        // make a note that U-APSD is supported/enabled in the nw
        aCtxImpl.UapsdEnabled( ETrue );
        
        // make a WMM AC both trigger & delivery enabled 
        // if U-APSD is supported by the nw (this we already know to be true)
        // and
        // if U-APSD usage has been requested by WLAN mgmt client for the AC 
        // in question
        aCtxImpl.OurWmmIe().SetUapsdFlags( static_cast<TQosInfoUapsdFlag>(
            ( aCtxImpl.UapsdRequestedForVoice() ? EAcVoUapsdFlag : 0 ) |
            ( aCtxImpl.UapsdRequestedForVideo() ? EAcViUapsdFlag : 0 ) |
            ( aCtxImpl.UapsdRequestedForBackground() ? EAcBkUapsdFlag : 0 ) |
            ( aCtxImpl.UapsdRequestedForBestEffort() ? EAcBeUapsdFlag : 0 )) );
                                           
        // set max service period length for the ACs as requested by WLAN 
        // mgmt client
        aCtxImpl.OurWmmIe().SetMaxSpLen( aCtxImpl.UapsdMaxSpLen() );        
        }
    else
        {
        OsTracePrint( 
            KQos, (TUint8*)
            ("UMAC: WlanDot11State::EnableQos(): u-apsd not supported in nw"));
        
        aCtxImpl.UapsdEnabled( EFalse );        
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::DetermineAcUapsdUsage( WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.UapsdEnabled() )
        {
        if ( aCtxImpl.UapsdRequestedForVoice() )
            {
            OsTracePrint( KQos, (TUint8*)
                ("UMAC: WlanDot11State::DetermineAcUapsdUsage: u-apsd used for Voice"));
            aCtxImpl.UapsdUsedForVoice( ETrue );
            }
        else
            {
            OsTracePrint( KQos, (TUint8*)
                ("UMAC: WlanDot11State::DetermineAcUapsdUsage: u-apsd NOT used for Voice"));
            aCtxImpl.UapsdUsedForVoice( EFalse );            
            }            

        if ( aCtxImpl.UapsdRequestedForVideo() )
            {
            OsTracePrint( KQos, (TUint8*)
                ("UMAC: WlanDot11State::DetermineAcUapsdUsage: u-apsd used for Video"));
            aCtxImpl.UapsdUsedForVideo( ETrue );    
            }
        else
            {
            OsTracePrint( KQos, (TUint8*)
                ("UMAC: WlanDot11State::DetermineAcUapsdUsage: u-apsd NOT used for Video"));
            aCtxImpl.UapsdUsedForVideo( EFalse );            
            }            

        if ( aCtxImpl.UapsdRequestedForBestEffort() )
            {
            OsTracePrint( KQos, (TUint8*)
                ("UMAC: WlanDot11State::DetermineAcUapsdUsage: u-apsd used for Best Effort"));
            aCtxImpl.UapsdUsedForBestEffort( ETrue );
            }
        else
            {
            OsTracePrint( KQos, (TUint8*)
                ("UMAC: WlanDot11State::DetermineAcUapsdUsage: u-apsd NOT used for Best Effort"));
            aCtxImpl.UapsdUsedForBestEffort( EFalse );            
            }            

        if ( aCtxImpl.UapsdRequestedForBackground() )
            {
            OsTracePrint( KQos, (TUint8*)
                ("UMAC: WlanDot11State::DetermineAcUapsdUsage: u-apsd used for Background"));
            aCtxImpl.UapsdUsedForBackground( ETrue );
            }
        else
            {
            OsTracePrint( KQos, (TUint8*)
                ("UMAC: WlanDot11State::DetermineAcUapsdUsage: u-apsd NOT used for Background"));
            aCtxImpl.UapsdUsedForBackground( EFalse );            
            }            
        }
    else
        {
        OsTracePrint( KQos, (TUint8*)
            ("UMAC: WlanDot11State::DetermineAcUapsdUsage: u-apsd NOT used for any AC"));
        aCtxImpl.UapsdUsedForVoice( EFalse );
        aCtxImpl.UapsdUsedForVideo( EFalse );
        aCtxImpl.UapsdUsedForBestEffort( EFalse );
        aCtxImpl.UapsdUsedForBackground( EFalse );        
        }

    // now when U-APSD usage per AC has been determined, freeze the dynamic 
    // power mode mgmt traffic override/ignoration settings
    aCtxImpl.FreezePwrModeMgmtTrafficOverride();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::ResetAcParameters( 
    WlanContextImpl& aCtxImpl,
    WHA::TQueueId aAccessCategory,
    TBool aUseAandGvalues )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ResetAcParameters: Reset parameters of AC: %d"),
        aAccessCategory );        
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ResetAcParameters: aUseAandGvalues: %d"),
        aUseAandGvalues );        

    switch ( aAccessCategory )
        {
        case WHA::ELegacy:
            aCtxImpl.CwMinVector()[WHA::ELegacy] = 
                aUseAandGvalues ? KDot11BeCwMinAandG : KDot11BeCwMinB;            
            aCtxImpl.CwMaxVector()[WHA::ELegacy] = KDot11BeCwMax;
            aCtxImpl.AifsVector()[WHA::ELegacy] = KDot11BeAifsn;
            aCtxImpl.TxOplimitVector()[WHA::ELegacy] = KDot11BeTxopLimit;        
            break;
        case WHA::EBackGround:
            aCtxImpl.CwMinVector()[WHA::EBackGround] = 
                aUseAandGvalues ? KDot11BgCwMinAandG : KDot11BgCwMinB;
            aCtxImpl.CwMaxVector()[WHA::EBackGround] = KDot11BgCwMax;
            aCtxImpl.AifsVector()[WHA::EBackGround] = KDot11BgAifsn;
            aCtxImpl.TxOplimitVector()[WHA::EBackGround] = KDot11BgTxopLimit;        
            break;
        case WHA::EVideo:
            aCtxImpl.CwMinVector()[WHA::EVideo] = 
                aUseAandGvalues ? KDot11ViCwMinAandG : KDot11ViCwMinB;            
            aCtxImpl.CwMaxVector()[WHA::EVideo] = 
                aUseAandGvalues ? KDot11ViCwMaxAandG : KDot11ViCwMaxB;        
            aCtxImpl.AifsVector()[WHA::EVideo] = KDot11ViAifsn;
            aCtxImpl.TxOplimitVector()[WHA::EVideo] = 
                aUseAandGvalues ? KDot11ViTxopLimitAandG : KDot11ViTxopLimitB;        
            break;
        case WHA::EVoice:
            aCtxImpl.CwMinVector()[WHA::EVoice] = 
                aUseAandGvalues ? KDot11VoCwMinAandG : KDot11VoCwMinB;            
            aCtxImpl.CwMaxVector()[WHA::EVoice] = 
                aUseAandGvalues ? KDot11VoCwMaxAandG : KDot11VoCwMaxB;        
            aCtxImpl.AifsVector()[WHA::EVoice] = KDot11VoAifsn;
            aCtxImpl.TxOplimitVector()[WHA::EVoice] = 
                aUseAandGvalues ? KDot11VoTxopLimitAandG : KDot11VoTxopLimitB;        
            break;        
        default:
            {
            // catch implementation error

            OsTracePrint( KErrorLevel, 
                (TUint8*)("UMAC: WlanDot11State::ResetAcParameters: unsupported access category: %d"), 
                aAccessCategory );        
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            }        
        }
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::ResetAcParameters( 
    WlanContextImpl& aCtxImpl, 
    TBool aUseAandGvalues )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ResetAcParameters: Reset parameters of all ACs. aUseAandGvalues: %d"),
        aUseAandGvalues );        
    
    // Set the default values for a QoS connection for every AC

    ResetAcParameters( aCtxImpl, WHA::ELegacy, aUseAandGvalues );
    ResetAcParameters( aCtxImpl, WHA::EBackGround, aUseAandGvalues );
    ResetAcParameters( aCtxImpl, WHA::EVideo, aUseAandGvalues );
    ResetAcParameters( aCtxImpl, WHA::EVoice, aUseAandGvalues );
    
    // reset also the WMM Parameter Set Count to initial (not defined) value
    aCtxImpl.WmmParameterSetCount( KWmmParamSetNotDefined );            
    }    

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::AcParametersValid( 
    WlanContextImpl& aCtxImpl,
    WHA::TQueueId aAccessCategory )
    {
    TBool status ( ETrue );
    
    if ( aAccessCategory < WHA::EHcca )
        {
        if ( // AIFSN validity per the WMM specification
             (aCtxImpl.AifsVector())[aAccessCategory] < KDot11AifsnMin ||
             // CwMin & CwMax sanity check 
             (aCtxImpl.CwMinVector())[aAccessCategory] >=
             (aCtxImpl.CwMaxVector())[aAccessCategory] )
            {
            status = EFalse;
            }
        else
            {
            // parameters are reasonable. No action needed
            }
        }
    else
        {
        // catch implementation error

        OsTracePrint( KErrorLevel, 
            (TUint8*)("UMAC: WlanDot11State::AcParametersValid: unsupported aAccessCategory: %d"), 
            aAccessCategory );        
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    return status;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::ParseAcParameters( 
    WlanContextImpl& aCtxImpl, 
    const SWmmParamElemData& aWmmParamElem )
    {
    for( TUint8 i = 0; i < KNumOfWmmACs; i++ )
        {
        switch ( aWmmParamElem.iAcParams[i].AccessCategory() )
            {
            case EAcBestEffort:
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanDot11State::ParseAcParameters: parsing best effort AC parameters"));
                (aCtxImpl.CwMinVector())[WHA::ELegacy] = 
                    aWmmParamElem.iAcParams[i].CwMin();
                (aCtxImpl.CwMaxVector())[WHA::ELegacy] = 
                    aWmmParamElem.iAcParams[i].CwMax();
                (aCtxImpl.AifsVector())[WHA::ELegacy] = 
                    aWmmParamElem.iAcParams[i].Aifsn();
                (aCtxImpl.TxOplimitVector())[WHA::ELegacy] = 
                    aWmmParamElem.iAcParams[i].TxOpLimit();
                (aCtxImpl.AcmVector())[WHA::ELegacy] = 
                    ( aWmmParamElem.iAcParams[i].AdmissionControlMandatory() )? 
                        ETrue : EFalse;

                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: aCntrl mandatory: %d"), 
                    aWmmParamElem.iAcParams[i].AdmissionControlMandatory());
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: cwmin: %d"), (aCtxImpl.CwMinVector())[WHA::ELegacy]);
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: cwmax: %d"), (aCtxImpl.CwMaxVector())[WHA::ELegacy]);
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: aifsn: %d"), (aCtxImpl.AifsVector())[WHA::ELegacy]);
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: txOpLimit: %d"), (aCtxImpl.TxOplimitVector())[WHA::ELegacy]);
                    
                if ( !AcParametersValid( aCtxImpl, WHA::ELegacy ) )
                    {
                    OsTracePrint( KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::ParseAcParameters: BE parameters not reasonble. Resetting to defaults") );
                    ResetAcParameters( 
                        aCtxImpl, 
                        WHA::ELegacy, 
                        aCtxImpl.ErpIePresent() );
                    }                    
                break;
            case EAcBackground:
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanDot11State::ParseAcParameters: parsing background AC parameters"));
                (aCtxImpl.CwMinVector())[WHA::EBackGround] = 
                    aWmmParamElem.iAcParams[i].CwMin();
                (aCtxImpl.CwMaxVector())[WHA::EBackGround] = 
                    aWmmParamElem.iAcParams[i].CwMax();
                (aCtxImpl.AifsVector())[WHA::EBackGround] = 
                    aWmmParamElem.iAcParams[i].Aifsn();
                (aCtxImpl.TxOplimitVector())[WHA::EBackGround] = 
                    aWmmParamElem.iAcParams[i].TxOpLimit();
                (aCtxImpl.AcmVector())[WHA::EBackGround] = 
                    ( aWmmParamElem.iAcParams[i].AdmissionControlMandatory() )? 
                        ETrue : EFalse;

                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: aCntrl mandatory: %d"), 
                    aWmmParamElem.iAcParams[i].AdmissionControlMandatory());
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: cwmin: %d"), (aCtxImpl.CwMinVector())[WHA::EBackGround]);
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: cwmax: %d"), (aCtxImpl.CwMaxVector())[WHA::EBackGround]);
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: aifsn: %d"), (aCtxImpl.AifsVector())[WHA::EBackGround]);
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: txOpLimit: %d"), (aCtxImpl.TxOplimitVector())[WHA::EBackGround]);    
                    
                if ( !AcParametersValid( aCtxImpl, WHA::EBackGround ) )
                    {
                    OsTracePrint( KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::ParseAcParameters: BG parameters not reasonble. Resetting to defaults") );
                    ResetAcParameters( 
                        aCtxImpl, 
                        WHA::EBackGround, 
                        aCtxImpl.ErpIePresent() );
                    }
                break;
            case EAcVideo:
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanDot11State::ParseAcParameters: parsing video AC parameters"));
                (aCtxImpl.CwMinVector())[WHA::EVideo] = 
                    aWmmParamElem.iAcParams[i].CwMin();
                (aCtxImpl.CwMaxVector())[WHA::EVideo] = 
                    aWmmParamElem.iAcParams[i].CwMax();
                (aCtxImpl.AifsVector())[WHA::EVideo] = 
                    aWmmParamElem.iAcParams[i].Aifsn();
                (aCtxImpl.TxOplimitVector())[WHA::EVideo] = 
                    aWmmParamElem.iAcParams[i].TxOpLimit();
                (aCtxImpl.AcmVector())[WHA::EVideo] = 
                    ( aWmmParamElem.iAcParams[i].AdmissionControlMandatory() )? 
                        ETrue : EFalse;
                
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: aCntrl mandatory: %d"), 
                    aWmmParamElem.iAcParams[i].AdmissionControlMandatory());
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: cwmin: %d"), (aCtxImpl.CwMinVector())[WHA::EVideo]);
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: cwmax: %d"), (aCtxImpl.CwMaxVector())[WHA::EVideo]);
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: aifsn: %d"), (aCtxImpl.AifsVector())[WHA::EVideo]);
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: txOpLimit: %d"), (aCtxImpl.TxOplimitVector())[WHA::EVideo]);    
                    
                if ( !AcParametersValid( aCtxImpl, WHA::EVideo ) )
                    {
                    OsTracePrint( KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::ParseAcParameters: VI parameters not reasonble. Resetting to defaults") );
                    ResetAcParameters( 
                        aCtxImpl, 
                        WHA::EVideo, 
                        aCtxImpl.ErpIePresent() );
                    }
                break;
            case EAcVoice:
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanDot11State::ParseAcParameters: parsing voice AC parameters"));
                (aCtxImpl.CwMinVector())[WHA::EVoice] = 
                    aWmmParamElem.iAcParams[i].CwMin();
                (aCtxImpl.CwMaxVector())[WHA::EVoice] = 
                    aWmmParamElem.iAcParams[i].CwMax();
                (aCtxImpl.AifsVector())[WHA::EVoice] = 
                    aWmmParamElem.iAcParams[i].Aifsn();
                (aCtxImpl.TxOplimitVector())[WHA::EVoice] = 
                    aWmmParamElem.iAcParams[i].TxOpLimit();
                (aCtxImpl.AcmVector())[WHA::EVoice] = 
                    ( aWmmParamElem.iAcParams[i].AdmissionControlMandatory() )? 
                        ETrue : EFalse;
                    
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: aCntrl mandatory: %d"), 
                    aWmmParamElem.iAcParams[i].AdmissionControlMandatory());
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: cwmin: %d"), (aCtxImpl.CwMinVector())[WHA::EVoice]);
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: cwmax: %d"), (aCtxImpl.CwMaxVector())[WHA::EVoice]);
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: aifsn: %d"), (aCtxImpl.AifsVector())[WHA::EVoice]);
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: txOpLimit: %d"), (aCtxImpl.TxOplimitVector())[WHA::EVoice]);    
                    
                if ( !AcParametersValid( aCtxImpl, WHA::EVoice ) )
                    {
                    OsTracePrint( KUmacDetails, (TUint8*)
                        ("UMAC: WlanDot11State::ParseAcParameters: VO parameters not reasonble. Resetting to defaults") );
                    ResetAcParameters( 
                        aCtxImpl, 
                        WHA::EVoice, 
                        aCtxImpl.ErpIePresent() );
                    }
                break;
            default:
                OsTracePrint( KWarningLevel, 
                    (TUint8*)("UMAC: WARNING: Unknown AC: %d => parameters ignored"), 
                    aWmmParamElem.iAcParams[i].AccessCategory() );        
            }        
        }
        
    // store the current Parameter Set Count. 
    aCtxImpl.WmmParameterSetCount( aWmmParamElem.ParameterSetCount() );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ParseAcParameters: param set cnt: %d"), 
        aCtxImpl.WmmParameterSetCount() );    
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::AddTkIPKey( 
    WlanContextImpl& aCtxImpl,
    const TUint8* aData, 
    TUint32 /*aLength*/,
    T802Dot11WepKeyId aKeyIndex, 
    const TMacAddress& aMacAddr )
    {    
    TBool ret( EFalse );
    WlanWsaAddKey& wsa_cmd( aCtxImpl.WsaAddKey() );    
    WHA::STkipPairwiseKey* key( CreateTkipPtkCtx( 
        aCtxImpl, 
        wsa_cmd,
        aData, 
        aKeyIndex, 
        aMacAddr ) 
        );

    if ( key )
        {
        ret = ETrue;       
        // change global state: entry procedure triggers action
        ChangeState( aCtxImpl, 
            *this,              // prev state
            wsa_cmd,            // next state
            // the ACT
            KCompleteManagementRequest
            );   

        os_free( key ); // allways remember to release the memory
        }
    else
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::AddTkIPKey(): memory allocation failed") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::StoreTxRatePolicyInfo( 
    WlanContextImpl& aCtxImpl,
    const TTxRatePolicy& aRatePolicy,
    const TQueue2RateClass& aQueue2RateClass,
    const TInitialMaxTxRate4RateClass& aInitialMaxTxRate4RateClass,
    const TTxAutoRatePolicy& aAutoRatePolicy,
    const THtMcsPolicy& aHtMcsPolicy )
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanDot11State::StoreTxRatePolicyInfo: store provided Tx rate policy configuration data for later use"));

    TTxRatePolicy& ratePolicy = aCtxImpl.RatePolicy();
    os_memcpy( &ratePolicy, &aRatePolicy, sizeof( TTxRatePolicy ) );

    TQueue2RateClass& queue2RateClass = aCtxImpl.Queue2RateClass();
    os_memcpy( &queue2RateClass, &aQueue2RateClass, sizeof( TQueue2RateClass ) );

    TInitialMaxTxRate4RateClass& initialMaxTxRate4RateClass = 
        aCtxImpl.InitialMaxTxRate4RateClass();
    os_memcpy( 
        &initialMaxTxRate4RateClass, 
        &aInitialMaxTxRate4RateClass, 
        sizeof( TInitialMaxTxRate4RateClass ) );    

    TTxAutoRatePolicy& autoRatePolicy = aCtxImpl.AutoRatePolicy();
    os_memcpy( &autoRatePolicy, &aAutoRatePolicy, sizeof( TTxAutoRatePolicy ) );
    
    THtMcsPolicy& htMcsPolicy = aCtxImpl.HtMcsPolicy();
    os_memcpy( &htMcsPolicy, &aHtMcsPolicy, sizeof( THtMcsPolicy ) );    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigureTxRatePolicies( 
    WlanContextImpl& aCtxImpl, 
    TBool aCompleteMgmtRequest )
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanDot11State::ConfigureTxRatePolicies: rate bitmask (intersection of AP and our supported rates): 0x%08x"),
        aCtxImpl.RateBitMask() );
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanDot11State::ConfigureTxRatePolicies: aCompleteMgmtRequest: %d"),
        aCompleteMgmtRequest );

    TBool status ( ETrue );
    // retrieve reference to the stored rate policy
    TTxRatePolicy& ratePolicy ( aCtxImpl.RatePolicy() );
    // retrieve reference to the stored Tx queue 2 Rate Class mapping
    TQueue2RateClass& queue2RateClass ( aCtxImpl.Queue2RateClass() );
    // retrieve reference to the stored initial max Tx rate per rate class -
    // information
    TInitialMaxTxRate4RateClass& initialMaxTxRate4RateClass( 
        aCtxImpl.InitialMaxTxRate4RateClass() );
    // retrieve reference to the stored auto rate policy
    TTxAutoRatePolicy& autoRatePolicy ( aCtxImpl.AutoRatePolicy() );
    // retrieve reference to the stored HT MCS policy
    THtMcsPolicy& htMcsPolicy ( aCtxImpl.HtMcsPolicy() );

    if ( aCtxImpl.WHASettings().iNumOfTxRateClasses < 
         ratePolicy.numOfPolicyObjects )
        {
        // WHA layer doesn't support as many rate classes as has been
        // provided to us. 
        
        ResortToSingleTxRatePolicy(
            aCtxImpl,
            ratePolicy,
            queue2RateClass );
        }

    TWhaRateMasks rateMasks;
    os_memset( rateMasks, 0, sizeof( rateMasks ) );
 
    FinalizeTxRatePolicy(
        aCtxImpl,
        ratePolicy,
        rateMasks,
        initialMaxTxRate4RateClass );

    if ( aCtxImpl.WHASettings().iCapability & 
         WHA::SSettings::KAutonomousRateAdapt )
        {
        //=====================================================================
        // lower layer supports autonomous rate adaptation so we will let it
        // handle the rate adaptation. 
        //=====================================================================
        
        FinalizeTxAutoratePolicy(
            aCtxImpl,
            ratePolicy,
            autoRatePolicy );
 
        SpecialTxAutoratePolicy(
            aCtxImpl,
            ratePolicy,
            autoRatePolicy,
            htMcsPolicy );

        ConfigureForTxAutoratePolicy(
            aCtxImpl,
            ratePolicy,
            queue2RateClass,
            htMcsPolicy,
            aCompleteMgmtRequest );
        }
    else
        {
        //=====================================================================
        // WHA layer doesn't support autonomous rate adaptation so we need to
        // take care of rate adaption. Perform the relevant configuration
        //=====================================================================
        
        status = ConfigureForTxRatePolicy(
            aCtxImpl,
            ratePolicy,
            rateMasks,
            queue2RateClass,
            initialMaxTxRate4RateClass,
            aCompleteMgmtRequest );
        } // else

    return status;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::DifferenceInPsModeWakeupSettings(
    const WlanContextImpl& aCtxImpl ) const
    {
    TBool difference( EFalse );
    
    // 1st determine the current PS mode wake-up setting

    const WHA::TWlanWakeUpInterval currentWakeupMode ( 
        aCtxImpl.iWlanMib.iWlanWakeupInterval );
    const TUint8 currentListenInterval ( 
        aCtxImpl.iWlanMib.iWlanListenInterval );
    
    // and the desired wake-up setting
    const TDot11PsModeWakeupSetting KDesiredPsModeConfig (
        aCtxImpl.DesiredPsModeConfig() );
    
    if ( currentWakeupMode != 
         static_cast<WHA::TWlanWakeUpInterval>(
             KDesiredPsModeConfig.iWakeupMode) )
        {
        // difference in wake-up mode
        difference = ETrue;

        OsTracePrint( KPwrStateTransition, (TUint8*)
            ("UMAC: WlanDot11State::DifferenceInPsModeWakeupSettings: difference in wake-up mode") );
        }
    else
        {
        // wake-up mode is unchanged
        
        if ( KDesiredPsModeConfig.iWakeupMode 
             == EWakeUpIntervalEveryNthBeacon || 
             KDesiredPsModeConfig.iWakeupMode 
             == EWakeUpIntervalEveryNthDtim )
            {
            // for these wake-up modes there can be a
            // difference in the listen interval. Check that
            if ( currentListenInterval != KDesiredPsModeConfig.iListenInterval )
                {
                difference = ETrue;

                OsTracePrint( KPwrStateTransition, (TUint8*)
                    ("UMAC: WlanDot11State::DifferenceInPsModeWakeupSettings: difference in listen interval") );
                }
            else
                {
                // no difference in listen interval either
                // (return value is already correct)
                }
            }
        else
            {
            // for these wake-up modes a possible difference in listen
            // interval is not meaningful => no difference (return 
            // value is already correct)
            }
        }

    OsTracePrint( KPwrStateTransition, (TUint8*)
        ("UMAC: WlanDot11State::DifferenceInPsModeWakeupSettings: difference: %d"),
        difference );
        
    return difference;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigureHtCapabilities( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ConfigureHtCapabilities") );

    // allocate memory for the mib to write
    WHA::ShtCapabilities* mib 
        = static_cast<WHA::ShtCapabilities*>
        (os_alloc( sizeof( WHA::ShtCapabilities ) )); 

    if ( !mib )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::ConfigureHtCapabilities: abort") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }

    // reset MIB before starting to set the values
    os_memset( mib, 0, sizeof( WHA::ShtCapabilities ) );

    //=====================
    // Set the MIB contents
    //=====================
    
    mib->iHtSupport = aCtxImpl.HtSupportedByNw();

    // currently HT is supported only in infrastructure networks
    mib->iPeerMac = WHA::KBroadcastMacAddr;

    mib->iRxStbc = aCtxImpl.GetNwHtCapabilitiesIe().iData.StbcRx();

    mib->iMaxAmpduLength = 
        aCtxImpl.GetNwHtCapabilitiesIe().iData.MaxAmpduLenExponent();

    // if a feature is supported by the nw, set it in the capabilities bit
    // mask. Otherwise it is left unset
    
    if ( aCtxImpl.GetNwHtCapabilitiesIe().iData.LdpcRx() )
        {
        mib->iPeerFeatures |= WHA::KLdpcRx;
        }    
    if ( aCtxImpl.GetNwHtCapabilitiesIe().iData.FortyMhzOperation() )
        {
        mib->iPeerFeatures |= WHA::K40MhzChannel;
        }
    if ( aCtxImpl.GetNwHtCapabilitiesIe().iData.GreenfieldFormat() )
        {
        mib->iPeerFeatures |= WHA::KGreenfieldFormat;
        }
    if ( aCtxImpl.GetNwHtCapabilitiesIe().iData.ShortGiFor20Mhz() )
        {
        mib->iPeerFeatures |= WHA::KShortGiFor20Mhz;
        }
    if ( aCtxImpl.GetNwHtCapabilitiesIe().iData.ShortGiFor40Mhz() )
        {
        mib->iPeerFeatures |= WHA::KShortGiFor40Mhz;
        }
    if ( aCtxImpl.GetNwHtCapabilitiesIe().iData.StbcTx() )
        {
        mib->iPeerFeatures |= WHA::KStbcTx;
        }
    if ( aCtxImpl.GetNwHtCapabilitiesIe().iData.DelayedBlockAck() )
        {
        mib->iPeerFeatures |= WHA::KDelayedBlockAck;
        }
    if ( aCtxImpl.GetNwHtCapabilitiesIe().iData.DsssCckIn40Mhz() )
        {
        mib->iPeerFeatures |= WHA::KDsssCckIn40Mhz;
        }
    if ( aCtxImpl.GetNwHtCapabilitiesIe().iData.LsigTxopProtection() )
        {
        mib->iPeerFeatures |= WHA::KLsigTxopProtection;
        }
    if ( aCtxImpl.GetNwHtCapabilitiesIe().iData.Pco() )
        {
        mib->iPeerFeatures |= WHA::KPco;
        }
    if ( aCtxImpl.GetNwHtCapabilitiesIe().iData.Htc() )
        {
        mib->iPeerFeatures |= WHA::KHtcField;
        }
    if ( aCtxImpl.GetNwHtCapabilitiesIe().iData.RdResponder() )
        {
        mib->iPeerFeatures |= WHA::KReverseDirectionResp;
        }

    os_memcpy( 
        mib->iMcsSet, 
        aCtxImpl.GetNwHtCapabilitiesIe().iData.iRxMcsBitmask,
        sizeof( mib->iMcsSet ) );

    mib->iAmpduSpacing = 
        aCtxImpl.GetNwHtCapabilitiesIe().iData.MinMpduStartSpacing();

    mib->iMcsFeedback = aCtxImpl.GetNwHtCapabilitiesIe().iData.McsFeedback();

    mib->iTxBeamFormingCapab = 
        aCtxImpl.GetNwHtCapabilitiesIe().iData.TransmitBeamformingCapabilities();

    mib->iAntennaSelCapab = 
        aCtxImpl.GetNwHtCapabilitiesIe().iData.AselCapabilities();

    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
        
    wha_cmd.Set( 
        aCtxImpl, 
        WHA::KMibHtCapabilities, 
        sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );   

    os_free( mib ); // release the memory

    // signal caller that a state transition occurred
    return ETrue;    
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11State::ResetHtCapabilitiesMib( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ResetHtCapabilitiesMib") );

    // allocate memory for the mib to write
    WHA::ShtCapabilities* mib 
        = static_cast<WHA::ShtCapabilities*>
        (os_alloc( sizeof( WHA::ShtCapabilities ) )); 

    if ( !mib )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::ResetHtCapabilitiesMib: abort") );
        DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }

    // reset the MIB to its default value
    *mib = WHA::KHtCapabilitiesMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibHtCapabilities, sizeof( *mib ), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );
    
    os_free( mib ); // release the memory
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11State::ResetHtBlockAckConfigureMib( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ResetHtBlockAckConfigureMib") );

    // allocate memory for the mib to write
    WHA::ShtBlockAckConfigure* mib 
        = static_cast<WHA::ShtBlockAckConfigure*>
        (os_alloc( sizeof( WHA::ShtBlockAckConfigure ) )); 

    if ( !mib )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::ResetHtBlockAckConfigureMib: abort") );
        DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }
    
    // reset the MIB to its default value
    *mib = WHA::KHtBlockAckConfigureMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibHtBlockAckConfigure, sizeof( *mib ), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );

    os_free( mib ); // release the memory
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigureHtBssOperation( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ConfigureHtBssOperation") );

    // allocate memory for the mib to write
    WHA::ShtBssOperation* mib 
        = static_cast<WHA::ShtBssOperation*>
        (os_alloc( sizeof( WHA::ShtBssOperation ) )); 

    if ( !mib )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::ConfigureHtBssOperation: abort") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }

    // reset MIB before starting to set the values
    os_memset( mib, 0, sizeof( WHA::ShtBssOperation ) );

    //=====================
    // Set the MIB contents
    //=====================
    
    if ( aCtxImpl.GetNwHtOperationIe().iData.NonGreenfieldPresent() )
        {
        mib->iInfo |= WHA::ShtBssOperation::KNonGreenfieldPresent;
        }
    if ( aCtxImpl.GetNwHtOperationIe().iData.PcoActive() )
        {
        mib->iInfo |= WHA::ShtBssOperation::KPcoActive;
        }
    if ( aCtxImpl.GetNwHtOperationIe().iData.RifsMode() )
        {
        mib->iInfo |= WHA::ShtBssOperation::KRifsPermitted;
        }
    if ( aCtxImpl.GetNwHtOperationIe().iData.DualCtsProtection() )
        {
        mib->iInfo |= WHA::ShtBssOperation::KDualCtsProtReq;
        }
    if ( aCtxImpl.GetNwHtOperationIe().iData.DualBeacon() )
        {
        mib->iInfo |= WHA::ShtBssOperation::KSecondaryBeaconTx;
        }
    if ( aCtxImpl.GetNwHtOperationIe().iData.LsigTxopProtection() )
        {
        mib->iInfo |= WHA::ShtBssOperation::KLsigTxopProtection;
        }

    os_memcpy( 
        mib->iMcsSet, 
        aCtxImpl.GetNwHtOperationIe().iData.iBasicMcsSet,
        sizeof( mib->iMcsSet ) );

    mib->iOpMode = 
        aCtxImpl.GetNwHtOperationIe().iData.HtProtection();

    mib->iSecChOffset = 
        aCtxImpl.GetNwHtOperationIe().iData.SecondaryChOffset();

    mib->iApChWidth = 
        aCtxImpl.GetNwHtOperationIe().iData.ChWidth();    

    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
        
    wha_cmd.Set( 
        aCtxImpl, 
        WHA::KMibHtBssOperation, 
        sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );   

    os_free( mib ); // release the memory

    // signal caller that a state transition occurred
    return ETrue;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::HtcFieldPresent(
    WlanContextImpl& aCtxImpl,
    const TAny* aFrame,
    TUint32 aFlags )
    {
    TBool status ( EFalse );
    
    if ( aCtxImpl.WHASettings().iCapability & WHA::SSettings::KHtOperation )
        {
        // we can interpret the frame to have this header even if it would
        // be a 802.11 mgmt frame as the header content that is relevant in
        // this method is the same
        const SDataFrameHeader* frameHdr 
            = reinterpret_cast<const SDataFrameHeader*>(aFrame);
     
        if ( aFlags & WHA::KHtPacket && frameHdr->IsOrderBitSet() )
            {
            status = ETrue;
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: WlanDot11State::HtcFieldPresent: yes") );
            }
        }
    
    return status;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::OutgoingMulticastDataFrame( 
    const SDataFrameHeader* aDataFrameHdr )
    {
    if ( aDataFrameHdr->IsToDsBitSet() )
        {
        // frame to infrastructure nw. Address 3 == DA
        
        return IsGroupBitSet( aDataFrameHdr->iAddress3 );
        }
    else
        {
        // frame to IBSS. Address 1 == DA
        
        return IsGroupBitSet( aDataFrameHdr->iAddress1 );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::UpdateTxDataFrameStatistics( 
    WlanContextImpl& aCtxImpl,
    WHA::TQueueId aAccessCategory,
    WHA::TStatus aStatus,
    TBool aMulticastData, 
    TUint aAckFailures,
    TUint32 aMediaDelay,
    TUint aTotalTxDelay )
    {
    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: WlanDot11State::UpdateTxDataFrameStatistics: aAccessCategory: %d"),
        aAccessCategory );

    if ( aStatus == WHA::KSuccess )
        {
        if ( aMulticastData )
            {
            OsTracePrint( KWsaTxDetails, (TUint8*)
                ("UMAC: WlanDot11State::UpdateTxDataFrameStatistics: inc tx mcast cnt") );

            aCtxImpl.IncrementTxMulticastDataFrameCount( aAccessCategory );
            }
        else
            {
            OsTracePrint( KWsaTxDetails, (TUint8*)
                ("UMAC: WlanDot11State::UpdateTxDataFrameStatistics: inc tx unicast cnt") );

            aCtxImpl.IncrementTxUnicastDataFrameCount( aAccessCategory );
            }
            
        OsTracePrint( KWsaTxDetails, (TUint8*)
            ("UMAC: WlanDot11State::UpdateTxDataFrameStatistics: ack failures: %d"),
            aAckFailures );

        aCtxImpl.IncrementTxRetryCount( aAccessCategory, aAckFailures );
        
        OsTracePrint( KWsaTxDetails, (TUint8*)
            ("UMAC: WlanDot11State::UpdateTxDataFrameStatistics: aMediaDelay: %d"),
            aMediaDelay );

        aCtxImpl.IncrementTxMediaDelay( aAccessCategory, aMediaDelay );
        
        OsTracePrint( KWsaTxDetails, (TUint8*)
            ("UMAC: WlanDot11State::UpdateTxDataFrameStatistics: aTotalTxDelay: %d"),
            aTotalTxDelay );

        aCtxImpl.IncrementTotalTxDelay( aAccessCategory, aTotalTxDelay );
        
        aCtxImpl.UpdateTotalTxDelayHistogram( aAccessCategory, aTotalTxDelay );
        }
    else
        {
        OsTracePrint( KWsaTxDetails, (TUint8*)
            ("UMAC: WlanDot11State::UpdateTxDataFrameStatistics: inc tx error cnt") );
        aCtxImpl.IncrementTxErrorCount( aAccessCategory );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::XferDot11FrameToMgmtClient( 
    WlanContextImpl& aCtxImpl,
    const void* aFrame,
    TUint32 aLength,
    const WHA::TRcpi aRcpi,
    TUint8* aBuffer ) const
    {
    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanDot11State::XferDot11FrameToMgmtClient: aRcpi: %d"), 
        aRcpi);

    TBool status ( ETrue );
    
    TDataBuffer* metaHdr ( aCtxImpl.GetRxFrameMetaHeader() );
    
    if ( metaHdr )
        {
        // set frame length 
        metaHdr->KeSetLength( aLength );

        // set frame type
        metaHdr->FrameType( TDataBuffer::KDot11Frame );

        // set RCPI for every frame transferred to the mgmt client
        metaHdr->KeSetRcpi( aRcpi );

        // set the offset to the beginning of the Rx buffer from the beginning
        // of the meta header. Note that this may be also negative
        metaHdr->KeSetBufferOffset(
            aBuffer
            - reinterpret_cast<TUint8*>(metaHdr) );
        
        // set the offset to the beginning of the actual frame within the
        // Rx buffer
        metaHdr->KeSetOffsetToFrameBeginning( 
            reinterpret_cast<const TUint8*>(aFrame)   // frame beginning
            - aBuffer );                              // buffer beginning
                    
        // complete
        aCtxImpl.iUmac.MgmtDataReceiveComplete( metaHdr, 1 );
        }
    else
        {
        // no memory available for the meta header. In this case we have no
        // other choice than to discard the received frame. 
        aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
        // inform the caller about the situation
        status = EFalse;
        OsTracePrint( KWarningLevel | KRxFrame, (TUint8*)
            ("UMAC: WlanDot11State::XferDot11FrameToMgmtClient: WARNING: no memory for meta hdr => abort rx") );
        }
        
    return status;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::AddMulticastTKIPKey( 
    WlanContextImpl& aCtxImpl,
    T802Dot11WepKeyId aKeyIndex,
    TUint32 /*aLength*/,
    const TUint8* aData )
    {        
    // store info of TKIP GTK insertion
    aCtxImpl.GroupKeyType( WHA::ETkipGroupKey );

    // allocate memory for the key structure
    WHA::STkipGroupKey* key = static_cast<WHA::STkipGroupKey*>
        (os_alloc( sizeof(WHA::STkipGroupKey) )); 

    if ( !key )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, 
            (TUint8*)("UMAC: WlanDot11State::AddMulticastTKIPKey: memory allocation failed") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }

    os_memcpy( key->iTkipKey, aData, WHA::KTKIPKeyLength );
    os_memcpy( key->iRxMicKey, aData + WHA::KTKIPKeyLength, KMicLength );
    key->iKeyId =  static_cast<WHA::TPrivacyKeyId>(aKeyIndex);
    // for now we fill this field with zeroes
    os_memset( key->iRxSequenceCounter, 0, WHA::KRxSequenceCounterLength );
    
    WlanWsaAddKey& wsa_cmd = aCtxImpl.WsaAddKey();   
    
    wsa_cmd.Set( aCtxImpl, 
        WHA::ETkipGroupKey,
        key,
        WlanWsaKeyIndexMapper::Extract( WHA::ETkipGroupKey ) );
    
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd,            // next state
        // the ACT
        KCompleteManagementRequest
        );                           

    os_free( key ); // allways remember to release the memory

    // signal caller that a state transition occurred
    return ETrue;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::AddAesKey( 
    WlanContextImpl& aCtxImpl,
    const TUint8* aData, 
    TUint32 /*aLength*/,
    const TMacAddress& aMacAddr )
    {     
    TBool ret( EFalse );
    WlanWsaAddKey& wsa_cmd( aCtxImpl.WsaAddKey() );    
    WHA::SAesPairwiseKey* key( CreateAesPtkCtx( 
        aCtxImpl, 
        wsa_cmd,
        aData, 
        aMacAddr ) 
        );

    if ( key )
        {
        ret = ETrue;       
        // change global state: entry procedure triggers action
        ChangeState( aCtxImpl, 
            *this,              // prev state
            wsa_cmd,            // next state
            // the ACT
            KCompleteManagementRequest
            );   

        os_free( key ); // allways remember to release the memory
        }
    else
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::AddAesKey(): memory allocation failed") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::AddMulticastAesKey( 
    WlanContextImpl& aCtxImpl,
    T802Dot11WepKeyId aKeyIndex,
    TUint32 /*aLength*/,
    const TUint8* aData )
    {    
    // store info of AES GTK insertion
    aCtxImpl.GroupKeyType( WHA::EAesGroupKey );

    // allocate memory for the key structure
    WHA::SAesGroupKey* key = static_cast<WHA::SAesGroupKey*>
        ( os_alloc( sizeof( WHA::SAesGroupKey ) ) ); 

    if ( !key )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::AddMulticastAesKey(): memory allocation failed") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }
    
    os_memcpy( key->iAesKey, aData, WHA::KAesKeyLength );
    key->iKeyId =  static_cast<WHA::TPrivacyKeyId>(aKeyIndex);
    // for now we fill this field with zeroes
    os_memset( key->iRxSequenceCounter, 0, WHA::KRxSequenceCounterLength );
    
    WlanWsaAddKey& wsa_cmd = aCtxImpl.WsaAddKey();    
    
    wsa_cmd.Set( aCtxImpl, 
        WHA::EAesGroupKey,
        key,
        WlanWsaKeyIndexMapper::Extract( WHA::EAesGroupKey ) );
    
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd,            // next state
        // the ACT
        KCompleteManagementRequest
        );   

    os_free( key ); // allways remember to release the memory
    
    // signal caller that a state transition occurred
    return ETrue;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::AddUnicastWepKey(
    WlanContextImpl& aCtxImpl,
    const TMacAddress& aMacAddr,
    TUint32 aKeyLength,                      
    const TUint8 aKey[KMaxWEPKeyLength])
    {
    // store info of WEP PTK insertion
    aCtxImpl.PairWiseKeyType( WHA::EWepPairWiseKey );

    // allocate memory for the key structure
    WHA::SWepPairwiseKey* key = static_cast<WHA::SWepPairwiseKey*>
        (os_alloc( WHA::SWepPairwiseKey::KHeaderSize + aKeyLength )); 

    if ( !key )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::AddUnicastWepKey(): memory allocation failed") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }

    os_memcpy( 
        &(key->iMacAddr), 
        aMacAddr.iMacAddress,
        sizeof(key->iMacAddr) );
    key->iKeyLengthInBytes = static_cast<TUint8>(aKeyLength);
    os_memcpy( key->iKey, aKey, key->iKeyLengthInBytes );
    
    WlanWsaAddKey& wsa_cmd = aCtxImpl.WsaAddKey();    
    wsa_cmd.Set( aCtxImpl, 
        WHA::EWepPairWiseKey,
        key,
        WlanWsaKeyIndexMapper::Extract( WHA::EWepPairWiseKey ) );
    
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd,            // next state
        // the ACT
        KCompleteManagementRequest
        );                           
    
    os_free( key ); // allways remember to release the memory

    // signal caller that a state transition occurred
    return ETrue;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::OnAddBroadcastWepKey(
    WlanContextImpl& aCtxImpl,
    TUint32 aKeyIndex,             
    TBool aUseAsDefaulKey,                
    TBool aUseAsPairwiseKey,
    TUint32 aKeyLength,                      
    const TUint8 aKey[KMaxWEPKeyLength],
    const TMacAddress& aMac )
    { 
    WlanAddBroadcastWepKey& complex_wha_cmd( 
        aCtxImpl.AddBroadcastWepKey() );

    complex_wha_cmd.Set( aMac, aKeyIndex, aUseAsDefaulKey, 
        aUseAsPairwiseKey, aKeyLength, aKey );
            
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        complex_wha_cmd     // next state
        ); 

    // signal caller that a state transition occurred
    return ETrue;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::AddMulticastWapiKey( 
    WlanContextImpl& aCtxImpl,
    T802Dot11WepKeyId aKeyIndex,
    TUint32 /*aLength*/,
    const TUint8* aData )
    {
    if ( !(aCtxImpl.WHASettings().iCapability & WHA::SSettings::KWapi) )
        {
        // WAPI not supported by wlanpdd => abort key setting
        
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::AddMulticastWapiKey: WAPI not supported by wlanpdd -> abort") );

        OnOidComplete( aCtxImpl, KErrNotSupported );            
        return EFalse;        
        }

    // store info of WAPI group key insertion
    aCtxImpl.GroupKeyType( WHA::EWapiGroupKey );

    // allocate memory for the key structure
    WHA::SWapiGroupKey* key = static_cast<WHA::SWapiGroupKey*>
        (os_alloc( sizeof(WHA::SWapiGroupKey) )); 

    if ( !key )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, 
            (TUint8*)("UMAC: WlanDot11State::AddMulticastWapiKey: memory allocation failed") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }

    os_memcpy( key->iWapiKey, aData, WHA::KWapiKeyLength );

    os_memcpy( 
        key->iMicKey, 
        aData + WHA::KWapiKeyLength, 
        WHA::KWapiMicKeyLength );

    key->iKeyId =  static_cast<WHA::TPrivacyKeyId>(aKeyIndex);
    
    WlanWsaAddKey& wsa_cmd = aCtxImpl.WsaAddKey();   
    
    wsa_cmd.Set( aCtxImpl, 
        WHA::EWapiGroupKey,
        key,
        WlanWsaKeyIndexMapper::Extract( WHA::EWapiGroupKey ) );
    
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd,            // next state
        // the ACT
        KCompleteManagementRequest
        );                           

    os_free( key ); // release the memory

    // signal caller that a state transition occurred
    return ETrue;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::AddUnicastWapiKey( 
    WlanContextImpl& aCtxImpl,
    const TUint8* aData,
    TUint32 /*aLength*/,
    T802Dot11WepKeyId aKeyIndex,
    const TMacAddress& aMacAddr )
    {
    TBool ret( EFalse );

    if ( !(aCtxImpl.WHASettings().iCapability & WHA::SSettings::KWapi) )
        {
        // WAPI not supported by wlanpdd => abort key setting
        
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::AddUnicastWapiKey: WAPI not supported by wlanpdd -> abort") );

        OnOidComplete( aCtxImpl, KErrNotSupported );            
        return ret;        
        }

    WlanWsaAddKey& wsa_cmd( aCtxImpl.WsaAddKey() );    

    WHA::SWapiPairwiseKey* key( CreateWapiPtkCtx( 
        aCtxImpl, 
        wsa_cmd,
        aData, 
        aKeyIndex, 
        aMacAddr ) 
        );

    if ( key )
        {
        ret = ETrue;       
        // change global state: entry procedure triggers action
        ChangeState( aCtxImpl, 
            *this,              // prev state
            wsa_cmd,            // next state
            // the ACT
            KCompleteManagementRequest
            );   

        os_free( key ); // release the memory
        }
    else
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::AddUnicastWapiKey: memory allocation failed") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11State::AddMulticastAddr(
    WlanContextImpl& aCtxImpl,
    const TMacAddress& aMacAddr )
    {
    TBool stateTransitionOccurred( EFalse );
    
    OsTracePrint( 
        KWlmCmdDetails, 
        (TUint8*)
        ("UMAC: WlanDot11State::AddMulticastAddr(): addr to be added:"),
        aMacAddr);

    if ( aCtxImpl.MulticastFilteringDisAllowed() )
        {
        OsTracePrint( 
            KWlmCmdDetails, 
            (TUint8*)
            ("UMAC: WlanDot11State::AddMulticastAddr(): Multicast filtering "
             "disallowed"));
            
        OnOidComplete( aCtxImpl, KErrGeneral );        
        }
    else
        {        
        if ( aCtxImpl.WHASettings().iNumOfGroupTableEntrys > 
             aCtxImpl.MulticastAddressCount() )
            {
            // wha layer is able to take in an address
            
            // 1st try to add the address to our own internal bookkeeping
            WlanContextImpl::TGroupAddStatus addStatus = 
                aCtxImpl.AddMulticastAddress( aMacAddr );

            switch ( addStatus )
                {
                case WlanContextImpl::EOk:
                    OsTracePrint( 
                        KWlmCmdDetails, 
                        (TUint8*)
                        ("UMAC: WlanDot11State::AddMulticastAddr(): Address "
                         "will be added to the MIB"));
                    // the address needed to be added and adding went ok.
                    // Now update the group addresses MIB
                    stateTransitionOccurred = SetGroupAddressesTableMib( aCtxImpl ); 
                    break;
                case WlanContextImpl::EAlreadyExists: 
                    OsTracePrint( 
                        KWlmCmdDetails, 
                        (TUint8*)
                        ("UMAC: WlanDot11State::AddMulticastAddr(): Address "
                         "already exists"));
                    // the specified address already exists so there's no need
                    // to update the group addresses MIB
                    // just complete the request with OK status
                    OnOidComplete( aCtxImpl );
                    stateTransitionOccurred = EFalse;           
                    break;
                case WlanContextImpl::EFull:
                    OsTracePrint( 
                        KWlmCmdDetails, 
                        (TUint8*)
                        ("UMAC: WlanDot11State::AddMulticastAddr(): Internal "
                         "address table full; disallow multicast filtering"));
                    // we are not able to take in any more addresses.
                    // We will totally disable the multicast filtering
                    // and we won't allow it to be enabled any more during 
                    // the current nw connection
                    //
                    aCtxImpl.ResetMulticastAddresses();               
                    aCtxImpl.MulticastFilteringDisAllowed( ETrue );
                    stateTransitionOccurred = 
                        SetGroupAddressesTableMib( aCtxImpl );
                    break;
                default:
                    // programming error
                    OsTracePrint( KErrorLevel, (TUint8*)
                        ("UMAC: addStatus: %d"), addStatus );
                    OsAssert( (TUint8*)("UMAC: panic"), 
                        (TUint8*)(WLAN_FILE), __LINE__ );
                }
            }
        else
            {
            OsTracePrint( 
                KWlmCmdDetails, 
                (TUint8*)
                ("UMAC: WlanDot11State::AddMulticastAddr(): WHA not able to "
                 "accept address; disallow multicast filtering"));
            // wha layer is not able to take in an address. Either this is one 
            // address too many, or it doesn't support even a single address.
            // In either case we will totally disable the multicast filtering
            // and we won't allow it to be enabled any more during the current 
            // nw connection
            aCtxImpl.ResetMulticastAddresses();               
            aCtxImpl.MulticastFilteringDisAllowed( ETrue );
            stateTransitionOccurred = SetGroupAddressesTableMib( aCtxImpl );
            }
        }

    // signal caller whether a state transition occurred or not
    return stateTransitionOccurred;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11State::RemoveMulticastAddr(
    WlanContextImpl& aCtxImpl,
    TBool aRemoveAll,
    const TMacAddress& aMacAddr )
    {
    TBool stateTransitionOccurred( EFalse );
    
    OsTracePrint( 
        KWlmCmdDetails, 
        (TUint8*)
        ("UMAC: WlanDot11State::RemoveMulticastAddr(): addr to be removed:"),
        aMacAddr);

    if ( aCtxImpl.MulticastFilteringDisAllowed() )
        {
        OsTracePrint( 
            KWlmCmdDetails, 
            (TUint8*)
            ("UMAC: WlanDot11State::RemoveMulticastAddr(): Multicast filtering "
             "disallowed"));
        // filtering is not allowed currently so there can't be any addresses
        // to remove. Just complete the request with OK status            
        OnOidComplete( aCtxImpl );        
        }
    else
        {
        if ( aRemoveAll )        
            {
            OsTracePrint( 
                KWlmCmdDetails, 
                (TUint8*)
                ("UMAC: WlanDot11State::RemoveMulticastAddr(): remove all"));
            // remove all addresses; naturally will also disable filtering
            aCtxImpl.ResetMulticastAddresses();
            stateTransitionOccurred = SetGroupAddressesTableMib( aCtxImpl );            
            }
        else
            {            
            // 1st remove the specified address from our own internal 
            // bookkeeping, if it exists
            if ( aCtxImpl.RemoveMulticastAddress( aMacAddr ) )
                {
                OsTracePrint( 
                    KWlmCmdDetails, 
                    (TUint8*)
                    ("UMAC: WlanDot11State::RemoveMulticastAddr(): removing "
                     "the specified address"));
                // it existed, so update the group addresses MIB, too
                stateTransitionOccurred = SetGroupAddressesTableMib( aCtxImpl );                 
                }
            else
                {
                OsTracePrint( 
                    KWlmCmdDetails, 
                    (TUint8*)
                    ("UMAC: WlanDot11State::RemoveMulticastAddr(): specified "
                     "address doesn't exist, nothing to do"));
                // it did't exist, so there's nothing to remove
                // Just complete the request with OK status            
                OnOidComplete( aCtxImpl );                    
                }
            }
        }

    // signal caller whether a state transition occurred or not
    return stateTransitionOccurred;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt WlanDot11State::InitNetworkConnect( 
    WlanContextImpl& aCtxImpl,
    TUint16 aScanResponseFrameBodyLength,
    const TUint8* aScanResponseFrameBody ) const
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11State::InitNetworkConnect") );
    
    // 1st clear our BSS Membership feature list
    aCtxImpl.ClearBssMembershipFeatureList();
    
    if ( aCtxImpl.WHASettings().iCapability & WHA::SSettings::KHtOperation )
        {
        // wlanpdd indicates HT support. Record that in our BSS Membership
        // features
        aCtxImpl.AddBssMembershipFeature( E802Dot11HtPhy );
        }
    
    const SScanResponseFixedFields* scanResponseFixedFields = 
        reinterpret_cast<const SScanResponseFixedFields*>( 
            aScanResponseFrameBody );

    // store capability info from scan response frame body to our context
    aCtxImpl.GetCapabilityInformation() 
        = scanResponseFixedFields->iCapability.CapabilityInformationField();

    // and set it also as the initial value to our association request frame 
    // templates
    aCtxImpl.GetAssociationRequestFrame().iFixedFields.iCapabilityInfo 
        = aCtxImpl.GetCapabilityInformation();    
    aCtxImpl.GetHtAssociationRequestFrame().iFixedFields.iCapabilityInfo 
        = aCtxImpl.GetCapabilityInformation();    

    // ... and to to our reassociation request frame templates
    aCtxImpl.GetReassociationRequestFrame().iFixedFields.iCapabilityInfo 
        = aCtxImpl.GetCapabilityInformation();    
    aCtxImpl.GetHtReassociationRequestFrame().iFixedFields.iCapabilityInfo 
        = aCtxImpl.GetCapabilityInformation();        
    
    // use short slot time if supported by the network
    aCtxImpl.UseShortSlotTime( 
        aCtxImpl.GetCapabilityInformation().IsShortSlotTimeBitSet() );

    //=============================================
    // check for WAPI
    //=============================================
    if ( aCtxImpl.EncryptionStatus() == EEncryptionWAPI && 
         !(aCtxImpl.WHASettings().iCapability & WHA::SSettings::KWapi) )
        {
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::InitNetworkConnect: WAPI requested but not supported by wlanpdd -> abort") );
        
        return KErrNotSupported;
        }
        
    //=============================================
    // do we meet network capability requirements
    //=============================================
    
    if ( !NetworkCapabilityInformationMet( aCtxImpl ) )
        {
        // requirements not met

        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::InitNetworkConnect: network capabilities not met -> abort") );

        return KWlanErrUnsupportedNwConf;
        }

    // initialize element locator for locating IEs from the scan response 
    // frame body
    WlanElementLocator elementLocator( 
        reinterpret_cast<const TUint8*>( scanResponseFixedFields + 1 ), 
        aScanResponseFrameBodyLength - 
        sizeof( SScanResponseFixedFields ) );

    TUint8 elementDatalength( 0 );
    const TUint8* elementData( NULL );
        
    //=============================================
    // determine the channel of the network
    //=============================================

    // locate DS parameter set information element
    if ( elementLocator.InformationElement( 
        E802Dot11DsParameterSetIE,
        elementDatalength, 
        &elementData ) == WlanElementLocator::EWlanLocateOk )
        {
        // ...and store it to our context
        aCtxImpl.NetworkChannelNumeber( *elementData );
        }
    else
        {
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::InitNetworkConnect: 802Dot11DsParameterSetIE not found -> abort") );

        return KWlanErrUnsupportedNwConf;
        }

    //=============================================
    // determine the beacon interval of the network
    //=============================================

    const TUint32 beacon_interval = scanResponseFixedFields->BeaconInterval();

    if ( beacon_interval )
        {        
        // ...and store it to our context
        aCtxImpl.NetworkBeaconInterval( beacon_interval );
        }
    else
        {
        OsTracePrint( KWarningLevel, 
            (TUint8*)("UMAC: WlanDot11State::InitNetworkConnect: zero beacon interval -> abort") );

        return KWlanErrUnsupportedNwConf;
        }

    //=============================================
    // determine the need to use protection
    //=============================================

    // locate ERP information element
    if ( elementLocator.InformationElement( 
        E802Dot11ErpInformationIE,
        elementDatalength, 
        &elementData ) == WlanElementLocator::EWlanLocateOk )
        {
        // ERP IE present -> set the protection level according to the Use
        // Protection bit
        aCtxImpl.ProtectionBitSet( *elementData & KUseProtectionMask );
        // make also a note of ERP IE presence, which means that the nw in 
        // question is a 802.11a/g nw (instead of a 802.11b nw)
        aCtxImpl.ErpIePresent( ETrue );
        } 
    else
        {
        // ERP IE not present 
        aCtxImpl.ProtectionBitSet( EFalse );
        }
    
    if ( aCtxImpl.NetworkOperationMode() == WHA::EBSS )
        {
        //=============================================
        //
        // only for infrastructure mode connections
        //
        //=============================================

        //=============================================
        // determine WMM / QoS information
        //=============================================
        
        // locate WMM information element
        if ( elementLocator.InformationElement( 
            E802Dot11VendorSpecificIE,
            KWmmElemOui,
            KWmmElemOuiType,
            KWmmInfoElemOuiSubType,
            elementDatalength,
            &elementData ) == WlanElementLocator::EWlanLocateOk )
            {        
            // WMM IE present
            OsTracePrint( KUmacDetails, (TUint8*)
                ("UMAC: WlanDot11State::InitNetworkConnect: WMM IE present"));
                
            EnableQos( aCtxImpl, UapsdEnabledInNetwork( 
                reinterpret_cast<const SRxWmmIeData&>( *elementData ) ) );
                
            // as there are no WMM parameter values available in the WMM IE
            // use the default AC parameter values until we get the values
            // in (re)association response
            ResetAcParameters( aCtxImpl, aCtxImpl.ErpIePresent() );                
            } 
        else
            {
            // WMM IE not present. Check if WMM Parameter Element exists instead
            if ( elementLocator.InformationElement( 
                E802Dot11VendorSpecificIE,
                KWmmElemOui,
                KWmmElemOuiType,
                KWmmParamElemOuiSubtype,
                elementDatalength,
                &elementData ) == WlanElementLocator::EWlanLocateOk )
                {        
                // WMM Parameter Element present
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanDot11State::InitNetworkConnect: WMM param elem present"));
                    
                EnableQos( aCtxImpl, UapsdEnabledInNetwork( 
                    reinterpret_cast<const SWmmParamElemData&>( *elementData ) ) );
                
                // as the parameter element is present, use the opportunity to
                // parse the AC (QoS) parameters. 
                // However, reset them 1st to their default values in case 
                // the nw would provide values for some AC multiple times and 
                // leave the parameters for some AC unspecified
                //
                ResetAcParameters( aCtxImpl, aCtxImpl.ErpIePresent() );
                ParseAcParameters( aCtxImpl,
                    reinterpret_cast<const SWmmParamElemData&>( *elementData ) );
                } 
            else
                {
                // WMM Parameter Element not present either => no QoS, no U-APSD
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanDot11State::InitNetworkConnect: neither WMM IE nor WMM param elem present"));
                    
                aCtxImpl.QosEnabled( EFalse );
                aCtxImpl.UapsdEnabled( EFalse );
                }
            }
        
        //=================================================================
        // perform 802.11n related actions & checks if lower layer supports
        // HT operation
        //=================================================================
    
        if ( aCtxImpl.WHASettings().iCapability & WHA::SSettings::KHtOperation )
            {
            if ( !HandleDot11n( aCtxImpl,elementLocator ) )
                {
                OsTracePrint( KWarningLevel, (TUint8*)
                    ("UMAC: WlanDot11State::InitNetworkConnect: Nw's 802.11n requirements not met -> abort") );
        
                return KWlanErrUnsupportedNwConf;
                }
            }
        else
            {
            // lower layer doesn't support HT, so we must handle the network
            // as a non-HT network
            aCtxImpl.HtSupportedByNw( EFalse );
            }
        }
    else        
        {
        //=============================================
        //
        // only for IBSS mode connections
        //
        // ============================================
        
        //=============================================
        // determine ATIM window
        //=============================================

        // locate IBSS Parameter Set element
        if ( elementLocator.InformationElement( 
            E802Dot11IbssParameterSetIE,
            elementDatalength, 
            &elementData ) == WlanElementLocator::EWlanLocateOk )
            {
            // store it to our context

            // note that elementData points to the IE data and not the
            // preceding IE header. That's why we need to back up the pointer
            // by SInformationElementHeader length before doing the cast
            aCtxImpl.AtimWindow( 
                ( reinterpret_cast<const SIbssParameterSetIE*>(
                    elementData - sizeof( SInformationElementHeader ) ) 
                )->AtimWindow() );
            }                                                
        else
            {
            OsTracePrint( KUmacDetails, (TUint8*)
                ("UMAC: WlanDot11State::InitNetworkConnect: atim not present, PS not used"));

            // as IBSS Parameter Set element is not present, power saving
            // is not used in the IBSS network we are going to join. So we 
            // will set the ATIM window to zero (to denote that PS is not used)
            aCtxImpl.AtimWindow( 0 );
            }
        }

    //=============================================
    // do we meet mandatory network rates
    //=============================================

    // locate supported rates IE
    if ( elementLocator.InformationElement( 
        E802Dot11SupportedRatesIE,
        elementDatalength, 
        &elementData ) == WlanElementLocator::EWlanLocateOk )
        {
        // ...and store it to our context
        aCtxImpl.GetApSupportedRatesIE().SetIeData( 
            elementData, 
            elementDatalength );        
        }
    else
        {
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::InitNetworkConnect: supported rates IE not found -> abort") );

        return KWlanErrUnsupportedNwConf;
        }

    // locate extended supported rates information element
    if ( elementLocator.InformationElement( 
        E802Dot11ExtendedRatesIE,
        elementDatalength, 
        &elementData ) == WlanElementLocator::EWlanLocateOk )
        {
        OsTracePrint( KInfoLevel, (TUint8*)
            ("UMAC: WlanDot11State::InitNetworkConnect: E802Dot11ExtendedRatesIE present") );

        // ...and store it to our context
        aCtxImpl.GetApExtendedSupportedRatesIE().SetIeData( elementData, elementDatalength );

        // check if we meet mandatory rates; in this case check also extended
        // supported rates
        if ( !AreSupportedRatesMet( aCtxImpl, ETrue ) )
            {
            OsTracePrint( KWarningLevel, (TUint8*)
                ("UMAC: WlanDot11State::InitNetworkConnect: rates not met -> abort") );

            return KWlanErrUnsupportedNwConf;
            }
        }
    else
        {
        OsTracePrint( KInfoLevel, (TUint8*)
            ("UMAC: WlanDot11State::InitNetworkConnect: E802Dot11ExtendedRatesIE not present") );

        // check if we meet mandatory rates; in this case extended supported
        // rates don't need to be checked
        if ( !AreSupportedRatesMet( aCtxImpl, EFalse ) )
            {
            OsTracePrint( KWarningLevel, 
                (TUint8*)("UMAC: WlanDot11State::InitNetworkConnect: rates not met -> abort") );

            return KWlanErrUnsupportedNwConf;
            }            
        }

    //=============================================
    // determine U-APSD usage for the ACs/Tx queues
    //=============================================
    DetermineAcUapsdUsage( aCtxImpl );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::SetTxPowerLevel(
    WlanContextImpl& aCtxImpl,
    TUint32 aLevel)
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)
        ("UMAC: WlanDot11State::SetTxPowerLevel(): aLevel: %d"), 
        aLevel );

    // allocate memory for the mib to write
    WHA::Sdot11CurrentTxPowerLevel* mib 
        = static_cast<WHA::Sdot11CurrentTxPowerLevel*>
        (os_alloc( sizeof(WHA::Sdot11CurrentTxPowerLevel) )); 

    if ( !mib )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, 
            (TUint8*)("UMAC * SetRcpiTriggerLevel * abort") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }
    
    mib->iDot11CurrentTxPowerLevel = aLevel;
        
    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
        
    wha_cmd.Set( 
        aCtxImpl, 
        WHA::KMibDot11CurrentTxPowerLevel, 
        sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd,            // next state
        // the ACT
        KCompleteManagementRequest
        );   

    os_free( mib ); // always remember to release the memory

    // store the new power level also to our soft mib
    aCtxImpl.iWlanMib.dot11CurrentTxPowerLevel = aLevel;
    
    // signal caller that a state transition occurred
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::GetLastRcpi(
    WlanContextImpl& aCtxImpl )
    {
    TBool statechange ( EFalse );
    WHA::TRcpi whaRcpi( 0 );
    
    if ( aCtxImpl.GetLatestMedianRcpiFromPredictor( os_systemTime(), whaRcpi ) )
        {
        // we have a median filtered RCPI value available so we can return that
        // directly to WLAN Mgmt Client

        // convert to a 32bit value before returning
        const TInt32 rcpi ( whaRcpi );

        OnOidComplete( aCtxImpl, 
                       KErrNone, 
                       reinterpret_cast<const TAny*>(&rcpi),
                       sizeof( rcpi ) );        
        }
    else
        {
        // we need to get the RCPI from lower layers
        
        WlanWsaReadMib& wha_cmd = aCtxImpl.WsaReadMib();
        wha_cmd.Set( aCtxImpl, WHA::KMibStatisticsTable );
        
        // change global state: entry procedure triggers action
        ChangeState( aCtxImpl, 
            *this,                  // previous state
            wha_cmd,                // next state
            // the ACT
            KCompleteManagementRequest
            );                       

        // signal caller that a state transition occurred
        statechange = ETrue;
        }

    return statechange;
    }
        
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::OnPacketTransferComplete( 
    WlanContextImpl& aCtxImpl, 
    TUint32 aPacketId,
    TDataBuffer* aMetaHeader )
    {
    // complete the transfer
    if ( aPacketId == E802Dot11FrameTypeData )
        {
        OnTxProtocolStackDataComplete( aCtxImpl, aMetaHeader );
        }
    else if ( aPacketId == E802Dot11FrameTypeDataEapol || 
              aPacketId == E802Dot11FrameTypeManagementAction || 
              aPacketId == E802Dot11FrameTypeTestFrame )
        {
        OnMgmtPathWriteComplete( aCtxImpl );
        
        aCtxImpl.iUmac.OnOtherTxDataComplete();
        }
    else
        {
        // this frame Tx request didn't come from above us (i.e. neither 
        // through the user data nor the management data API) but is
        // related to a frame Tx we have done internally. So, we need to 
        // mark the internal Tx buffer free again
        aCtxImpl.MarkInternalTxBufFree();
        
        aCtxImpl.iUmac.OnOtherTxDataComplete();
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::OnPacketSendComplete(
    WlanContextImpl& aCtxImpl, 
    WHA::TStatus aStatus,
    TUint32 /*aPacketId*/,
    WHA::TRate aRate,
    TUint32 /*aPacketQueueDelay*/,
    TUint32 /*aMediaDelay*/,
    TUint /*aTotalTxDelay*/,
    TUint8 /*aAckFailures*/,
    WHA::TQueueId aQueueId,
    WHA::TRate aRequestedRate,
    TBool /*aMulticastData*/ )
    {
    aCtxImpl.OnTxCompleted( aRate, 
        static_cast<TBool>(aStatus == WHA::KSuccess), 
        aQueueId,
        aRequestedRate );    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::CallPacketSchedule( 
    WlanContextImpl& aCtxImpl,
    TBool aMore )
    {
    aCtxImpl.SchedulePackets( aMore );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::OnPacketFlushEvent(
    WlanContextImpl& aCtxImpl, 
    TUint32 aPacketId,
    TDataBuffer* aMetaHeader )
    {
    if ( aPacketId == E802Dot11FrameTypeData )
        {
        OnTxProtocolStackDataComplete( aCtxImpl, aMetaHeader );
        }
    else if ( aPacketId == E802Dot11FrameTypeDataEapol ||
              aPacketId == E802Dot11FrameTypeManagementAction || 
              aPacketId == E802Dot11FrameTypeTestFrame )
        {
        // complete with an error code if WLAN Mgmt Client frame
        // transmit fails
        OnMgmtPathWriteComplete( aCtxImpl, KErrGeneral );
        }
    else
        {
        // this frame Tx request didn't come from above us (i.e. neither 
        // through the user data nor the management data API) but is
        // related to a frame Tx we have done internally. So there's
        // nothing to complete upwards. But we need to mark the internal
        // Tx buffer free again
        aCtxImpl.MarkInternalTxBufFree();
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::OnPacketPushPossible( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // intentionally left empty
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::Indication( 
    WlanContextImpl& aCtxImpl, 
    WHA::TIndicationId aIndicationId,
    const WHA::UIndicationParams& aIndicationParams )
    {
    switch ( aIndicationId )
        {
        case WHA::EError:
            OsTracePrint( KWarningLevel, 
                (TUint8*)("UMAC: WHA error indication received!") );
            DoErrorIndication( aCtxImpl, aIndicationParams.iError.iStatus );
            break;
        case WHA::EBssLost:
            DoConsecutiveBeaconsLostIndication( aCtxImpl );
            break;
        case WHA::EBSSRegained:
            DoRegainedBSSIndication( aCtxImpl );
            break;
        case WHA::ERadar:
            DoRadarIndication( aCtxImpl );
            break;
        case WHA::ERcpi:
            DoRcpiIndication( aCtxImpl, aIndicationParams.iRcpi.iRcpi );
            break;
        case WHA::EPsModeError:
            DoPsModeErrorIndication( aCtxImpl );
            break;
        default:
            // implementation error
            OsTracePrint( KErrorLevel, 
                (TUint8*)("UMAC: aIndicationId: %d"), aIndicationId );
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigureTxQueue( 
    WlanContextImpl& aCtxImpl,
    WHA::TQueueId aQueueId,
    TBool aCompleteManagementRequest )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11State::ConfigureTxQueue: aQueueId: %d"),
        aQueueId );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ConfigureTxQueue: aCompleteManagementRequest: %d"), 
        aCompleteManagementRequest );

    WHA::TPsScheme psScheme( WHA::ERegularPs );
    
    // enable U-APSD for the AC/Queue in question when necessary. 
    // Otherwise stick to regular PS
    switch ( aQueueId )
        {
        case WHA::ELegacy:
            if ( aCtxImpl.UapsdUsedForBestEffort() )
                {
                psScheme = WHA::EUapsd;
                }                
            break;
        case WHA::EBackGround:
            if ( aCtxImpl.UapsdUsedForBackground() )
                {
                psScheme = WHA::EUapsd;
                }
            break;
        case WHA::EVideo:
            if ( aCtxImpl.UapsdUsedForVideo() )
                {
                psScheme = WHA::EUapsd;
                }
            break;
        case WHA::EVoice:
            if ( aCtxImpl.UapsdUsedForVoice() )
                {
                psScheme = WHA::EUapsd;
                }
            break;
        default:
            // catch programming error
            OsTracePrint( KErrorLevel, (TUint8*)
                ("UMAC: ERROR: unsupported queue, aQueueId: %d"), aQueueId );
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    WlanWhaConfigureQueue& wha_command = aCtxImpl.WhaConfigureQueue();

    wha_command.Set( 
        aQueueId,
        aCtxImpl.iWlanMib.dot11MaxTransmitMSDULifetime[aQueueId], 
        psScheme, 
        WHA::ENormal,
        aCtxImpl.iWlanMib.iMediumTime[aQueueId] );

    const TUint32 KNoNeedToCompleteManagementRequest = 0;
        
    // change global state: entry procedure triggers action
    ChangeState( 
        aCtxImpl, 
        *this,              // prev state
        wha_command,        // next state
        aCompleteManagementRequest ? KCompleteManagementRequest : 
                                     KNoNeedToCompleteManagementRequest );

    // signal caller that a state transition occurred
    return ETrue;                                     
    }
                     
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigureAcParams( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11State::ConfigureAcParams") );

    WlanWhaConfigureAc& wha_command = aCtxImpl.WhaConfigureAc();

    wha_command.Set( 
        aCtxImpl.CwMinVector(),
        aCtxImpl.CwMaxVector(),
        aCtxImpl.AifsVector(),
        aCtxImpl.TxOplimitVector() );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_command         // next state
        );                       
        
    // signal caller that a state transition occurred
    return ETrue;
    }    
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::SetCtsToSelfMib( 
    WlanContextImpl& aCtxImpl )
    {
    WHA::SctsToSelf* mib 
        = static_cast<WHA::SctsToSelf*>
        (os_alloc( sizeof( WHA::SctsToSelf ) )); 

    if ( !mib )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::SetCtsToSelfMib(): memory allocation failed") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }

    if ( aCtxImpl.ProtectionBitSet() )
        {
        OsTracePrint( 
            KUmacDetails, 
            (TUint8*)("UMAC: WlanDot11State::SetCtsToSelfMib(): enable CTS to self") );
            
        mib->iCtsToSelf = ETrue;
        }
    else
        {
        OsTracePrint( 
            KUmacDetails, 
            (TUint8*)("UMAC: WlanDot11State::SetCtsToSelfMib(): disable CTS to self") );
            
        mib->iCtsToSelf = EFalse;
        }

    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
    wha_cmd.Set( 
        aCtxImpl, WHA::KMibCtsToSelf, sizeof(*mib), mib );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );  
    
    // as the parameters have been supplied we can now deallocate
    os_free( mib );
    
    // signal caller that a state transition occurred
    return ETrue;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigureBssLost( 
    WlanContextImpl& aCtxImpl,
    TUint32 aBeaconLostCount,
    TUint8 aFailedTxPacketCount )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ConfigureBssLost") );

    // store & take the new failed Tx packet count threshold into use
    aCtxImpl.iWlanMib.iFailedTxPacketCountThreshold = aFailedTxPacketCount;
    // set the beacon lost count mib    
    return SetBeaconLostCountMib( aCtxImpl, aBeaconLostCount );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11State::SetTxRateAdaptParams( 
    WlanContextImpl& aCtxImpl,
    TUint8 aMinStepUpCheckpoint,
    TUint8 aMaxStepUpCheckpoint,
    TUint8 aStepUpCheckpointFactor,
    TUint8 aStepDownCheckpoint,
    TUint8 aMinStepUpThreshold,
    TUint8 aMaxStepUpThreshold,
    TUint8 aStepUpThresholdIncrement,
    TUint8 aStepDownThreshold,
    TBool aDisableProbeHandling )
    {
    aCtxImpl.SetTxRateAdaptationAlgorithmParams(
        aMinStepUpCheckpoint,
        aMaxStepUpCheckpoint,
        aStepUpCheckpointFactor,
        aStepDownCheckpoint,
        aMinStepUpThreshold,
        aMaxStepUpThreshold,
        aStepUpThresholdIncrement,
        aStepDownThreshold,
        aDisableProbeHandling );

    OnOidComplete( aCtxImpl, KErrNone );

    // signal caller that no state transition occurred
    return EFalse;    
    }

// ---------------------------------------------------------------------------
// At this point we only store the provided configuration data. It will be
// taken into use when we know the nw we are going to join - i.e. just prior
// actually joining that nw
// ---------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigureTxRatePolicies( 
    WlanContextImpl& aCtxImpl,
    const TTxRatePolicy& aRatePolicy,
    const TQueue2RateClass& aQueue2RateClass,
    const TInitialMaxTxRate4RateClass& aInitialMaxTxRate4RateClass,
    const TTxAutoRatePolicy& aAutoRatePolicy,
    const THtMcsPolicy& aHtMcsPolicy )
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanDot11State::ConfigureTxRatePolicies"));

    StoreTxRatePolicyInfo( 
        aCtxImpl,
        aRatePolicy,
        aQueue2RateClass,
        aInitialMaxTxRate4RateClass,
        aAutoRatePolicy,
        aHtMcsPolicy );
        
    OnOidComplete( aCtxImpl, KErrNone );

    // signal caller that no state transition occurred
    return EFalse;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11State::SetPowerModeManagementParameters(        
    WlanContextImpl& aCtxImpl,
    TUint32 aToLightPsTimeout,
    TUint16 aToLightPsFrameThreshold,
    TUint32 aToActiveTimeout,
    TUint16 aToActiveFrameThreshold,
    TUint32 aToDeepPsTimeout,
    TUint16 aToDeepPsFrameThreshold,
    TUint16 aUapsdRxFrameLengthThreshold )
    {
    aCtxImpl.SetPowerModeManagementParameters(
        aToLightPsTimeout,
        aToLightPsFrameThreshold,
        aToActiveTimeout,
        aToActiveFrameThreshold,
        aToDeepPsTimeout,
        aToDeepPsFrameThreshold,
        aUapsdRxFrameLengthThreshold );
    
    OnOidComplete( aCtxImpl, KErrNone );

    // signal caller that no state transition occurred
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigurePwrModeMgmtTrafficOverride( 
    WlanContextImpl& aCtxImpl,
    TBool aStayInPsDespiteUapsdVoiceTraffic,
    TBool aStayInPsDespiteUapsdVideoTraffic,
    TBool aStayInPsDespiteUapsdBestEffortTraffic, 
    TBool aStayInPsDespiteUapsdBackgroundTraffic,
    TBool aStayInPsDespiteLegacyVoiceTraffic,
    TBool aStayInPsDespiteLegacyVideoTraffic,
    TBool aStayInPsDespiteLegacyBestEffortTraffic,
    TBool aStayInPsDespiteLegacyBackgroundTraffic )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ConfigurePwrModeMgmtTrafficOverride"));

    aCtxImpl.ConfigurePwrModeMgmtTrafficOverride( 
        aStayInPsDespiteUapsdVoiceTraffic,
        aStayInPsDespiteUapsdVideoTraffic,
        aStayInPsDespiteUapsdBestEffortTraffic, 
        aStayInPsDespiteUapsdBackgroundTraffic,
        aStayInPsDespiteLegacyVoiceTraffic,
        aStayInPsDespiteLegacyVideoTraffic,
        aStayInPsDespiteLegacyBestEffortTraffic,
        aStayInPsDespiteLegacyBackgroundTraffic );

    // Note, that in this case the dynamic power mode mgmt traffic 
    // override/ignoration settings will be frozen later (during connect 
    // operation, once we know the network capabilites) so that they become 
    // effective

    OnOidComplete( aCtxImpl, KErrNone );

    // signal caller that no state transition occurred
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::GetFrameStatistics( WlanContextImpl& aCtxImpl )
    {
    WlanWsaReadMib& wha_cmd = aCtxImpl.WsaReadMib();
    wha_cmd.Set( aCtxImpl, WHA::KMibCountersTable );
    
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,                  // previous state
        wha_cmd,                // next state
        // the ACT
        KCompleteManagementRequest
        );                       

    // signal caller that state transition occurred
    return ETrue;
    }

// -----------------------------------------------------------------------------
// At this point we only store the values provided by WLAN mgmt client. They
// will be used later when we (re-)associate to an AP.
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigureUapsd( 
    WlanContextImpl& aCtxImpl,
    TMaxServicePeriodLength aMaxServicePeriodLength,
    TBool aUapsdForVoice,
    TBool aUapsdForVideo,
    TBool aUapsdForBestEffort,
    TBool aUapsdForBackground )
    {
    // this cast is safe as the types are effectively the same
    aCtxImpl.UapsdMaxSpLen() = 
        static_cast<TQosInfoUapsdMaxSpLen>(aMaxServicePeriodLength);
    
    aCtxImpl.UapsdRequestedForVoice( aUapsdForVoice );
    aCtxImpl.UapsdRequestedForVideo( aUapsdForVideo );
    aCtxImpl.UapsdRequestedForBestEffort( aUapsdForBestEffort );
    aCtxImpl.UapsdRequestedForBackground( aUapsdForBackground );    
    
    OnOidComplete( aCtxImpl, KErrNone );

    // signal caller that no state transition occurred
    return EFalse;        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::GetMacAddress(
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::GetMacAddress: mac address:"), 
        aCtxImpl.iWlanMib.dot11StationId );

    OnOidComplete( 
        aCtxImpl, 
        KErrNone, 
        &(aCtxImpl.iWlanMib.dot11StationId), 
        sizeof(aCtxImpl.iWlanMib.dot11StationId) );

    // signal caller that no state transition occurred
    return EFalse;    
    }    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigureArpIpAddressFiltering(
    WlanContextImpl& aCtxImpl,
    TBool aEnableFiltering,
    TIpv4Address aIpv4Address )
    {
    return SetArpIpAddressTableMib(
            aCtxImpl,
            aEnableFiltering,
            aIpv4Address );
    }

// -----------------------------------------------------------------------------
// At this point we only store the provided configuration.
// It will be passed to the lower layers when connecting to a HT network
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigureHtBlockAck(
    WlanContextImpl& aCtxImpl, 
    TUint8 aTxBlockAckUsage,
    TUint8 aRxBlockAckUsage )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ConfigureHtBlockAck()") );

    WHA::ShtBlockAckConfigure& blockAckConf ( 
        aCtxImpl.GetHtBlockAckConfigure() );
    blockAckConf.iTxBlockAckUsage = aTxBlockAckUsage;
    blockAckConf.iRxBlockAckUsage = aRxBlockAckUsage;

    OnOidComplete( aCtxImpl, KErrNone );
    
    // signal caller that no state transition occurred
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigureProprietarySnapHdr(
    WlanContextImpl& aCtxImpl, 
    const TSnapHeader& aSnapHeader )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::ConfigureProprietarySnapHdr") );

    // store the provided SNAP header for later use
    os_memcpy( 
        reinterpret_cast<TUint8*>(&(aCtxImpl.GetProprietarySnapHeader())),
        reinterpret_cast<const TUint8*>(&aSnapHeader),
        sizeof( SSnapHeader ) );

    OnOidComplete( aCtxImpl, KErrNone );
    
    // signal caller that no state transition occurred
    return EFalse;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11State::SetGroupAddressesTableMib(
    WlanContextImpl& aCtxImpl )
    {
    const TMacAddress* multicastAddresses( NULL );
    const TUint32 nbrOfAddrs( 
        aCtxImpl.GetMulticastAddresses( multicastAddresses ) );

    TUint32 mibLength(  
        // mib header length
        WHA::Sdot11GroupAddressesTable::KHeaderSize
        // + mib data length
        + ( sizeof( TMacAddress ) * nbrOfAddrs ) );

    // align length of MIB to 4-byte boundary
    mibLength = Align4( mibLength );
    
    OsTracePrint( 
        KWlmCmdDetails, 
        (TUint8*)
        ("UMAC: WlanDot11State::SetGroupAddressesTableMib(): mibLength: %d"), 
        mibLength );        

    // allocate memory for the mib to write
    WHA::Sdot11GroupAddressesTable* mib 
        = static_cast<WHA::Sdot11GroupAddressesTable*>
        (os_alloc( mibLength )); 

    if ( !mib )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11State::SetGroupAddressesTableMib(): memory "
             "allocation failed") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }
    
    if ( nbrOfAddrs )
        {
        // at least one address exists, so enable multicast address filtering
        mib->iEnable = ETrue;
        }
    else
        {
        // no addresses, so disable filtering
        mib->iEnable = EFalse;
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: WlanDot11State::SetGroupAddressesTableMib(): no addresses; "
             "disable filtering") );
        }

    mib->iNumOfAddrs = nbrOfAddrs;
    
    // copy the multicast addresses after the mib header
    os_memcpy( mib->iAddrData,
               reinterpret_cast<TUint8*>(const_cast<TMacAddress*>(
                    multicastAddresses)),
               ( sizeof( TMacAddress ) * nbrOfAddrs ) );
        
    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
        
    wha_cmd.Set( 
        aCtxImpl, 
        WHA::KMibDot11GroupAddressesTable, 
        mibLength, 
        mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd,            // next state
        // the ACT
        KCompleteManagementRequest
        );   

    os_free( mib ); // release the allocated memory

    // signal caller that a state transition occurred
    return ETrue;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::SetBeaconLostCountMib(
    WlanContextImpl& aCtxImpl,
    TUint32 aBeaconLostCount )
    {
    OsTracePrint( 
        KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11State::SetBeaconLostCountMib(): aBeaconLostCount: %d"), 
        aBeaconLostCount );

    // allocate memory for the mib to write
    WHA::SbeaconLostCount* mib 
        = static_cast<WHA::SbeaconLostCount*>
        (os_alloc( sizeof( WHA::SbeaconLostCount ) )); 

    if ( !mib )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, 
            (TUint8*)("UMAC: WlanDot11State::SetBeaconLostCountMib: abort") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }
    
    mib->iLostCount = aBeaconLostCount;
        
    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
        
    wha_cmd.Set( 
        aCtxImpl, 
        WHA::KMibBeaconLostCount, 
        sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd,            // next state
        // the ACT
        KCompleteManagementRequest
        );   

    os_free( mib ); // always remember to release the memory

    // store the new beacon lost count also to our soft mib
    aCtxImpl.iWlanMib.iBeaconLostCount = aBeaconLostCount;
    
    // signal caller that a state transition occurred
    return ETrue;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::ResortToSingleTxRatePolicy(
    WlanContextImpl& aCtxImpl,
    TTxRatePolicy& aRatePolicy,
    TQueue2RateClass& aQueue2RateClass ) const
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanDot11State::ResortToSingleTxRatePolicy: WARNING: PDD "
         "supports only %d policy objects ... "),
        aCtxImpl.WHASettings().iNumOfTxRateClasses );
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: ... and %d objects have been provided to us"),
        aRatePolicy.numOfPolicyObjects );
    
    // In this case - from the rate classes / autorate policies provided to
    // us by WLAN Mgmt client - we will use only the rate class / autorate 
    // policy specified for legacy Tx Queue / AC, i.e. the 1st one at index 0
    
    aRatePolicy.numOfPolicyObjects = 1;
    for ( TUint queueId = ELegacy; queueId < EQueueIdMax; ++queueId )
        {
        aQueue2RateClass[queueId] = 0;
        }
    
#ifndef NDEBUG
    if ( (aCtxImpl.Queue2RateClass())[ELegacy] != 0 )
        {
        OsTracePrint( KErrorLevel | KTxRateAdapt, (TUint8*)
            ("UMAC: WlanDot11State::ResortToSingleTxRatePolicy: ERROR: policy "
             "for legacy not specified as the 1st in the policy array") );
        OsAssert( (TUint8*)("UMAC: panic"), 
            (TUint8*)(WLAN_FILE), __LINE__ );            
        }        
#endif
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::FinalizeTxRatePolicy(
    WlanContextImpl& aCtxImpl,
    TTxRatePolicy& aRatePolicy,
    TWhaRateMasks& aRateMasks,
    TInitialMaxTxRate4RateClass& aInitialMaxTxRate4RateClass ) const
    {
    for ( TUint rateClassInd = 0; 
          rateClassInd < aRatePolicy.numOfPolicyObjects; 
          ++rateClassInd )
        {
        // build a rate mask as an "intersection" of
        //   rates in the provided rate class AND
        //   rates supported by the nw and AND
        //   rates supported by WHA layer.
        // Also keep the nbr of tx attempts in the rate class for a particular 
        // rate if that rate is supported by both the nw and WHA layer. 
        // Otherwise set the nbr of tx attemps to zero for that rate
        
        HandleRates( 
            aCtxImpl, 
            aRatePolicy.txRateClass[rateClassInd], 
            aRateMasks[rateClassInd] );

        if ( !( aRateMasks[rateClassInd] ) )
            {
            // the provided rate class was such that we ended up with an empty
            // rate mask. To recover from this situation we will update the
            // rate class definition on the fly to contain the rates which both
            // the WHA layer and the nw support
            RecoverRatePolicy(
                aCtxImpl,
                aRatePolicy,
                rateClassInd,
                aRateMasks[rateClassInd] );
            // adjust also the Max Tx Rate for this rate class so that the 
            // highest possible rate will be used initially
            aInitialMaxTxRate4RateClass[rateClassInd] = WHA::KRate54Mbits;
            }        
        } // for
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::HandleRate(
    WlanContextImpl& aCtxImpl,
    WHA::TRate aRate,
    TUint8& aTxAttempts,
    WHA::TRate& aRateMask ) const
    {        
    if ( aCtxImpl.RateBitMask() & aRate )
        {
        // rate is supported both by us and by the nw. 
        
        if ( aTxAttempts )
            {
            // non-zero Tx attempts defined => include the rate in dynamic Tx 
            // rate adaptation rates
            aRateMask |= aRate;            
            }
        }
    else
        {
        // rate is not supported either by us or by the nw. Set zero Tx attempts
        aTxAttempts = 0;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::HandleRates(
    WlanContextImpl& aCtxImpl,
    TTxRateClass& aRateClass,
    WHA::TRate& aRateMask ) const
    {
    HandleRate( aCtxImpl, WHA::KRate54Mbits, aRateClass.txPolicy54, 
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate48Mbits, aRateClass.txPolicy48,
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate36Mbits, aRateClass.txPolicy36,
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate33Mbits, aRateClass.txPolicy33,
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate24Mbits, aRateClass.txPolicy24,
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate22Mbits, aRateClass.txPolicy22,
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate18Mbits, aRateClass.txPolicy18,
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate12Mbits, aRateClass.txPolicy12,
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate11Mbits, aRateClass.txPolicy11,
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate9Mbits, aRateClass.txPolicy9,
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate6Mbits, aRateClass.txPolicy6,
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate5_5Mbits, aRateClass.txPolicy5_5,
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate2Mbits, aRateClass.txPolicy2,
        aRateMask );

    HandleRate( aCtxImpl, WHA::KRate1Mbits, aRateClass.txPolicy1,
        aRateMask );    

    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanDot11State::HandleRates: resulting rate mask: 0x%08x"),
        aRateMask );
    }
    

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::RecoverRatePolicy(
    WlanContextImpl& aCtxImpl,
    TTxRatePolicy& aRatePolicy,
    TUint aRateClassInd,
    WHA::TRate& aRateMask ) const
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanDot11State::RecoverRatePolicy: aRateClassInd: %d"),
        aRateClassInd );

    const TUint8 KTxAttempts = 1;

    // start with Tx attempts == 1 for all Tx rates
    os_memset( 
        &aRatePolicy.txRateClass[aRateClassInd], 
        KTxAttempts,
        sizeof( TUint8 ) * KMaxNumberOfDot11bAndgRates );

    HandleRates( aCtxImpl, aRatePolicy.txRateClass[aRateClassInd], aRateMask );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::FinalizeTxAutoratePolicy(
    WlanContextImpl& aCtxImpl,
    const TTxRatePolicy& aRatePolicy,
    TTxAutoRatePolicy& aAutoRatePolicy ) const
    {
    for ( TUint rateClassInd = 0; 
          rateClassInd < aRatePolicy.numOfPolicyObjects; 
          ++rateClassInd )
        {
        // build a rate mask as an "intersection" of
        //   rates in the provided auto rate class AND
        //   rates supported by the nw and AND
        //   rates supported by WHA layer.
        aAutoRatePolicy[rateClassInd] = 
            aAutoRatePolicy[rateClassInd] & aCtxImpl.RateBitMask();
        
        if ( !( aAutoRatePolicy[rateClassInd] ) )
            {
            // the provided rate class was such that we ended up with an
            // empty rate mask. To recover from this situation we will 
            // update the rate class definition on the fly to contain
            // the rates which both the WHA layer and the nw support
            aAutoRatePolicy[rateClassInd] = aCtxImpl.RateBitMask();
            }        
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::SpecialTxAutoratePolicy(
    WlanContextImpl& aCtxImpl,
    TTxRatePolicy& aRatePolicy,
    TTxAutoRatePolicy& aAutoRatePolicy,
    THtMcsPolicy& aHtMcsPolicy ) const
    {
    if ( aRatePolicy.numOfPolicyObjects >= 
         aCtxImpl.WHASettings().iNumOfTxRateClasses )
        {
        // there's no room in the lower layers for a special policy
        // disable special policy use
        aCtxImpl.SpecialTxAutoRatePolicy( 0 );
        
        OsTracePrint( KTxRateAdapt, (TUint8*)
            ("UMAC: WlanDot11State::SpecialTxAutoratePolicy: no room") );
        
        return;
        }
    
    const TUint KMaxNbrOfItemsToPick(1);
    
    // start with an empty rate mask
    aAutoRatePolicy[aRatePolicy.numOfPolicyObjects] = 0;
    
    const WHA::TRate commonRates( aCtxImpl.RateBitMask() );

    // pick the 802.11b/g rate(s) for the special policy
    
    WHA::TRate rate( WHA::KRate1Mbits );
    TUint cntPicked(0);
    do
        {
        if ( rate & commonRates )
            {
            aAutoRatePolicy[aRatePolicy.numOfPolicyObjects] |= rate;
            ++cntPicked;
            }
        
        rate <<= 1;        
        } while ( ( cntPicked < KMaxNbrOfItemsToPick ) && 
                  ( rate <= KRate54Mbits ) );
    
    // start with an empty MCS set
    for ( TUint mcsBucket = 0; 
          mcsBucket < WHA::KHtMcsSetLength; 
          ++mcsBucket )    
        {
        aHtMcsPolicy[aRatePolicy.numOfPolicyObjects][mcsBucket] = 0; 
        }
    
    // pick the 802.11n MCS(s) for the special policy
    
    const SHtCapabilitiesIE& htCapabilitiesIe( 
        aCtxImpl.GetNwHtCapabilitiesIe() );
    const TUint KLastMcsBucket = WHA::KHtMcsSetLength - 1;
    const TUint8 KMcsCountInLastBucket( 5 );
    TUint8 mcsCount( 8 );

    cntPicked = 0;
    TUint mcsBucket = 0;
    do
        {
        if ( mcsBucket == KLastMcsBucket )
            {
            // there are only 5 MCSs in the last "bucket" per 802.11n std.
            mcsCount = KMcsCountInLastBucket;
            }
        
        TUint8 mcs(1);
        TUint mcsCounter( 0 );
        do
            {
            if ( mcs &
                 ( aCtxImpl.WHASettings().iHtCapabilities.iTxMcs[mcsBucket] ) &
                 ( htCapabilitiesIe.iData.iRxMcsBitmask[mcsBucket] ) )
                {
                aHtMcsPolicy[aRatePolicy.numOfPolicyObjects][mcsBucket] |= mcs;
                ++cntPicked;
                }
            
            mcs <<= 1;
            ++mcsCounter;
            } while ( ( cntPicked < KMaxNbrOfItemsToPick ) && 
                      ( mcsCounter < mcsCount ) );
                
        ++mcsBucket;
        } while ( ( cntPicked < KMaxNbrOfItemsToPick ) && 
                  ( mcsBucket < WHA::KHtMcsSetLength ) );
    
    // set the retry counts
    //
    const TUint8 KSpecialShortRetryLimit = 10;
    const TUint8 KSpecialLongRetryLimit = 4;
    aRatePolicy.txRateClass[aRatePolicy.numOfPolicyObjects].shortRetryLimit = 
        KSpecialShortRetryLimit;
    aRatePolicy.txRateClass[aRatePolicy.numOfPolicyObjects].longRetryLimit = 
        KSpecialLongRetryLimit;
    
    // now we have an additional policy
    ++(aRatePolicy.numOfPolicyObjects);
    // enable special policy use
    aCtxImpl.SpecialTxAutoRatePolicy( aRatePolicy.numOfPolicyObjects );

    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanDot11State::SpecialTxAutoratePolicy: policy id: %d"),
        aRatePolicy.numOfPolicyObjects );
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::ConfigureForTxAutoratePolicy(
    WlanContextImpl& aCtxImpl,
    const TTxRatePolicy& aRatePolicy,
    const TQueue2RateClass& aQueue2RateClass,
    THtMcsPolicy& aHtMcsPolicy,
    TBool aCompleteMgmtRequest )
    {
    // store the Tx queue to rate class mapping
    for ( TUint queueId = ELegacy; queueId < EQueueIdMax; ++queueId )
        {
        aCtxImpl.SetTxRatePolicy( 
            static_cast<WHA::TQueueId>(queueId), 
            // rate class ids start from 1, hence the + 1
            aQueue2RateClass[queueId] + 1 );
        }

    // make sure that our MCS policy contains only MCSs that both the NW
    // and the lower layers support
    HandleHtMcsPolicy( 
        aCtxImpl, 
        aHtMcsPolicy,
        aRatePolicy.numOfPolicyObjects );
    
    // change to the state which performs the rest of the configuration
    
    WlanConfigureTxAutoRatePolicy& complexWhaCmd( 
        aCtxImpl.ConfigureTxAutoRatePolicy() );

    complexWhaCmd.Set( aCompleteMgmtRequest );
            
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        complexWhaCmd       // next state
        );    
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11State::HandleHtMcsPolicy(
    WlanContextImpl& aCtxImpl,
    THtMcsPolicy& aHtMcsPolicy,
    TUint aNbrOfMcsSets ) const
    {
    OsTracePrint( KInfoLevel, (TUint8*)
        ("UMAC: WlanDot11State::HandleHtMcsPolicy") );
    
    const SHtCapabilitiesIE& htCapabilitiesIe( 
        aCtxImpl.GetNwHtCapabilitiesIe() );
    
    for ( TUint mcsSet = 0; mcsSet < aNbrOfMcsSets; ++mcsSet )
        {
        for ( TUint mcsBucket = 0; 
              mcsBucket < WHA::KHtMcsSetLength; 
              ++mcsBucket )    
            {
            aHtMcsPolicy[mcsSet][mcsBucket] = 
                aCtxImpl.WHASettings().iHtCapabilities.iTxMcs[mcsBucket] &
                htCapabilitiesIe.iData.iRxMcsBitmask[mcsBucket] &
                aHtMcsPolicy[mcsSet][mcsBucket];
            }        
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11State::ConfigureForTxRatePolicy(
    WlanContextImpl& aCtxImpl,
    const TTxRatePolicy& aRatePolicy,
    const TWhaRateMasks& aRateMasks,
    const TQueue2RateClass& aQueue2RateClass,
    const TInitialMaxTxRate4RateClass& aInitialMaxTxRate4RateClass,
    TBool aCompleteMgmtRequest )
    {
    for ( TUint rateClassInd = 0; 
          rateClassInd < aRatePolicy.numOfPolicyObjects; 
          ++rateClassInd )
        {
        // provide the ratemask for this rate class to rate adaptation.
        // Rate class ids start from 1, hence the + 1
        if ( !aCtxImpl.SetTxRateAdaptationRates( 
                rateClassInd + 1, 
                aRateMasks[rateClassInd] ) )
            {
            // alloc failure; we cannot continue
            OsTracePrint( KWarningLevel | KTxRateAdapt, (TUint8*)
                ("UMAC: WlanDot11State::ConfigureForTxRatePolicy: WARNING: "
                 "alloc failure in rate adaptation"));
            
            return EFalse;  // indicate fatal error
            }        

        // set the initial max Tx rate for this rate class

        aCtxImpl.SetCurrentMaxTxRate( 
            // rate class ids start from 1, hence the + 1
            rateClassInd + 1, 
            aInitialMaxTxRate4RateClass[rateClassInd] );
        }
        
    // inform rate adaptation about the Tx queue to rate class mapping
    for ( TUint queueId = ELegacy; queueId < EQueueIdMax; ++queueId )
        {
        aCtxImpl.SetTxRatePolicy( 
            static_cast<WHA::TQueueId>(queueId), 
            // rate class ids start from 1, hence the + 1
            aQueue2RateClass[queueId] + 1 );
        }

    // update the Rate Policy MIB

    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    const TUint16 mibLength = sizeof( WHA::StxRatePolicy ) 
        // there is space for one policy object (rate class) in the 
        // StxRatePolicy struct, so space for any additional objects needs to
        // be allocated in addition to that
        + sizeof( WHA::StxRateClass ) * 
          ( aRatePolicy.numOfPolicyObjects - 1 );
     
    wsa_cmd.Set( 
        aCtxImpl, 
        WHA::KMibTxRatePolicy, 
        mibLength,
        // note that the types WHA::StxRatePolicy and TTxRatePolicy are
        // effectively equivalent, so this is ok
        &aRatePolicy );
        
    const TUint32 KNotNecessary2Complete ( 0 );
            
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd,            // next state
        aCompleteMgmtRequest ? KCompleteManagementRequest : 
                               KNotNecessary2Complete
        );
    
    return ETrue;  // indicate success & state change
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11State::HandleHtCapabilities( 
    WlanContextImpl& aCtxImpl,
    WlanElementLocator& aElementLocator ) const
    {
    TBool status ( ETrue );
    TUint8 elementDatalength( 0 );
    const TUint8* elementData( NULL );
    
    // try to locate HT capabilities element
    if ( aElementLocator.InformationElement( 
            E802Dot11HtCapabilitiesIE,
            elementDatalength, 
            &elementData ) == WlanElementLocator::EWlanLocateOk )
        {
        // found, so store it to our context
        aCtxImpl.GetNwHtCapabilitiesIe().SetIeData( 
            elementData, 
            elementDatalength );

        // this also means that the target nw supports HT
        aCtxImpl.HtSupportedByNw( ETrue ); 

        OsTracePrint( KInfoLevel, (TUint8*)
            ("UMAC: WlanDot11State::HandleHtCapabilities: HT capabilities "
             "element present") );
        }
    else
        {
        // not found => target nw doesn't support HT
        aCtxImpl.HtSupportedByNw( EFalse ); 
        
        OsTracePrint( KInfoLevel, (TUint8*)
            ("UMAC: WlanDot11State::HandleHtCapabilities: HT capabilities "
             "element not found => HT not supported") );
        }
    
    return status;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11State::HandleHtOperation( 
    WlanContextImpl& aCtxImpl,
    WlanElementLocator& aElementLocator ) const
    {
    TBool status ( ETrue );
    TUint8 elementDatalength( 0 );
    const TUint8* elementData( NULL );
    
    // try to locate HT Operation element
    if ( aElementLocator.InformationElement( 
            E802Dot11HtOperationIE,
            elementDatalength, 
            &elementData ) == WlanElementLocator::EWlanLocateOk )
        {
        // found, so store it to our context
        aCtxImpl.GetNwHtOperationIe().SetIeData( 
            elementData, 
            elementDatalength );

        OsTracePrint( KInfoLevel, (TUint8*)
            ("UMAC: WlanDot11State::HandleHtOperation: element present. "
             "HT supported") );
        }
    else
        {
        // not found even though HT capabilities element is present. That's a 
        // protocol error. The only way we can try to cope with that is to 
        // handle the target nw as a non-HT nw
        aCtxImpl.HtSupportedByNw( EFalse );
        // in this case we need to remove HT also from our BSS membership 
        // feature list
        aCtxImpl.RemoveBssMembershipFeature( E802Dot11HtPhy );
        
        OsTracePrint( KInfoLevel, (TUint8*)
            ("UMAC: WlanDot11State::HandleHtOperation: element not found; "
             "protocol error => HT disabled ") );
        }
    
    return status;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11State::HandleDot11n( 
    WlanContextImpl& aCtxImpl,
    WlanElementLocator& aElementLocator ) const
    {
    TBool status ( ETrue ); 
    
    if ( ( aCtxImpl.PairwiseCipher() == EWlanCipherSuiteTkip ) || 
         !( aCtxImpl.QosEnabled() ) ||
         !( aCtxImpl.FeaturesAllowed() & KWlanHtOperation ) )
        {
        // as the control is here it means that 
        // - the WLAN vendor implementation
        // supports HT AND EITHER
        // - TKIP will be used as the pairwise cipher OR
        // - the target nw doesn't support WMM OR
        // - HT use has been denied by WLAN Mgmt client
        // In these cases we must not use HT functionality, even if the target 
        // nw supported it. We achieve that by handling the target nw as
        // a non-HT nw
        aCtxImpl.HtSupportedByNw( EFalse );
        // in this case we need to remove HT also from our BSS membership 
        // feature list
        aCtxImpl.RemoveBssMembershipFeature( E802Dot11HtPhy );
        
        OsTracePrint( KInfoLevel, (TUint8*)
            ("UMAC: WlanDot11State::HandleDot11n: TKIP as pairwise cipher "
             "OR WMM not supported OR HT use denied => HT disabled") );
        }
    else
        {
        status = HandleHtCapabilities( aCtxImpl, aElementLocator ) ;
        
        // if HT capabilities element is present and ok
        if ( aCtxImpl.HtSupportedByNw() && status  )
            {
            // check also HT Operation element
            status = HandleHtOperation( aCtxImpl, aElementLocator );
            }
        }
    
    return status;
    }
