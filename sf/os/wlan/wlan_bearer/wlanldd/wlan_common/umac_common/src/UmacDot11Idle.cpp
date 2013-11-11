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
* Description:   Implementation of the UmacDot11Idle class
*
*/

/*
* %version: 43 %
*/

#include "config.h"
#include "UmacDot11Idle.h"
#include "UmacContextImpl.h"
#include "UmacWsaWriteMib.h"
#include "umacwharelease.h"
#include "wha_mibDefaultvalues.h"
#include "UmacWsaAddKey.h"
#include "UmacWsaKeyIndexMapper.h"

#ifndef NDEBUG
const TInt8 WlanDot11Idle::iName[] = "dot11-idle";

const TUint8 WlanDot11Idle::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"EWRITEMIB"},
        {"EFINIT"}
    };

const TUint8 WlanDot11Idle::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, {"ETXCOMPLETE"}, {"ESCAN"}, 
        {"ECONNECT"}, {"ECONNECTIBSS"}, {"EDISCONNECT"}, 
        {"ERELEASE"}, {"EABORT"}
    };
#endif

// ================= MEMBER FUNCTIONS =======================

#ifndef NDEBUG 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TInt8* WlanDot11Idle::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::Set( TInt aCompletionCode ) 
    { 
    iEventMask |= KCompleteUponEntry;
    iCompletionCode = aCompletionCode; 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::CompleteScanUponEntry()
    {
    iEventMask |= KIndicateScanCompletionUponEntry;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::Entry( WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( (TUint8*)("UMAC * panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    if ( iState != EINIT )
        {
        // this is NOT the start of the the FSM actions
        // note that we send the ETXCOMPLETE event as the states
        // that wait for it are the only ones that can be interrupted
        // as they are asynchronous operations by nature
        // and wait for corresponding WHA completion method
        Fsm( aCtxImpl, ETXCOMPLETE );
        }
    else
        {
        // this is the start of the the FSM actions
        Fsm( aCtxImpl, ESTATEENTRY );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::Exit( WlanContextImpl& /*aCtxImpl*/ )
    {
    // reset fsm for the next time we come back to this state
    iState = EINIT;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt WlanDot11Idle::StartIBSS(
    WlanContextImpl& aCtxImpl,
    const TSSID& aSSID,                 
    TUint32 aBeaconInterval,            
    TUint32 aAtim,                      
    TUint32 aChannel,                   
    TEncryptionStatus aEncryptionStatus)
    {
    aCtxImpl.NetworkOperationMode( WHA::EIBSS );
    GenerateRandomBssIDForIbss( aCtxImpl );
    (aCtxImpl.GetSsId()) = aSSID;   
    aCtxImpl.NetworkChannelNumeber( aChannel );
    aCtxImpl.NetworkBeaconInterval( aBeaconInterval );
    aCtxImpl.AtimWindow( aAtim );
    aCtxImpl.UseShortPreamble( ETrue );
    (aCtxImpl.EncryptionStatus()) = aEncryptionStatus;           

    // clear & set our supported rates
    //
    aCtxImpl.GetOurSupportedRatesIE().Clear();
    aCtxImpl.GetOurExtendedSupportedRatesIE().Clear();
    
    TUint rateCount ( 0 );
    TUint8 rate_to_add( 0 );
    aCtxImpl.ClearBasicRateSet();    
    aCtxImpl.GetMinBasicRate() = 0;
    aCtxImpl.GetMaxBasicRate() = 0;
    
    for (TUint i = 0; i < KMaxNumberOfDot11bAndgRates; ++i )
        {
        OsTracePrint( 
            KUmacDetails, 
            (TUint8*)("UMAC: WlanDot11Idle::StartIBSS(): iSupportedRate: %d"),
            aCtxImpl.iSupportedRatesLookUpTable[i].iSupportedRate );

        if ( aCtxImpl.iSupportedRatesLookUpTable[i].iSupportedRate)
            {
            if ( rateCount < KMaxNumberOfRates )
                {
                rate_to_add 
                    = aCtxImpl.iSupportedRatesLookUpTable[i].iSupportedRate;
                
                if ( rate_to_add == E802Dot11Rate1MBit ||
                     rate_to_add == E802Dot11Rate2MBit ||
                     rate_to_add == E802Dot11Rate5p5MBit ||
                     rate_to_add == E802Dot11Rate11MBit )
                    {
                    // make this rate a basic rate
                    rate_to_add |= KBasicRateMask;
                    // and add it to our WHA basic rate mask
                    aCtxImpl.BasicRateSetBitSet( 
                        aCtxImpl.iSupportedRatesLookUpTable[i].iWsaRate );
                    
                    // store min basic rate
                    if ( aCtxImpl.GetMinBasicRate() == 0 )
                        {
                        aCtxImpl.GetMinBasicRate() = 
                            aCtxImpl.iSupportedRatesLookUpTable[i].iWsaRate;
                        }
                    
                    // store max basic rate
                    if ( aCtxImpl.GetMaxBasicRate() < 
                         aCtxImpl.iSupportedRatesLookUpTable[i].iWsaRate )
                        {
                        aCtxImpl.GetMaxBasicRate() 
                            = aCtxImpl.iSupportedRatesLookUpTable[i].iWsaRate;
                        }
                    }

                aCtxImpl.GetOurSupportedRatesIE().Append( rate_to_add );

                ++rateCount;
                OsTracePrint( 
                    KUmacDetails, 
                    (TUint8*)("UMAC: rateCount: %d"), rateCount );
                }
            else
                {
                aCtxImpl.GetOurExtendedSupportedRatesIE().Append( 
                    aCtxImpl.iSupportedRatesLookUpTable[i].iSupportedRate );

                OsTracePrint( KUmacDetails, 
                    (TUint8*)("UMAC: %d:th rate added to ext rates"), i + 1 );                    
                }
            }
        }

#ifndef NDEBUG 
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11Idle::StartIBSS(): supported rates len: %d"),
        aCtxImpl.GetOurSupportedRatesIE().iHeader.iLength );                    


    TUint8* ptr 
        = reinterpret_cast<TUint8*>
        (&(aCtxImpl.GetOurSupportedRatesIE().iSupportedRatesIE));
    for (TUint8 j = 0; j < aCtxImpl.GetOurSupportedRatesIE().iHeader.iLength; j++ )
        {
        OsTracePrint( 
            KUmacDetails, 
            (TUint8*)("UMAC: WlanDot11Idle::StartIBSS(): supported rate: %d"),
            *ptr );                    
        ptr++;
        }

    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11Idle::StartIBSS(): ext supported rates len: %d"),
        aCtxImpl.GetOurExtendedSupportedRatesIE().iHeader.iLength );                    

    ptr = reinterpret_cast<TUint8*>
        (&(aCtxImpl.GetOurExtendedSupportedRatesIE().iSupportedRatesIE));
    for (TUint8 j = 0 
        ; j < aCtxImpl.GetOurExtendedSupportedRatesIE().iHeader.iLength 
        ; j++ )
        {
        OsTracePrint( KUmacDetails, 
            (TUint8*)("UMAC: extended supported rate: %d"), *ptr );
        ptr++;
        }

    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: min basic WHA rate: 0x%08x"), aCtxImpl.GetMinBasicRate() );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: max basic WHA rate: 0x%08x"), aCtxImpl.GetMaxBasicRate() );
    
#endif
    
    // as we are starting our own IBSS nw, the "common" rates between us
    // and the network are our supported rates
    aCtxImpl.RateBitMask( aCtxImpl.WHASettings().iRates );

    // determine U-APSD usage for the ACs/Tx queues; which in this (IBSS) case
    // means disabling U-APSD 
    DetermineAcUapsdUsage( aCtxImpl );

    // inform the next state that we are starting a new IBSS
    aCtxImpl.iStates.iPrepareForIbssMode.Set( ETrue );

    Fsm( aCtxImpl, ECONNECTIBSS );

    // global state transition will occur
    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Idle::Connect(
    WlanContextImpl& aCtxImpl,
    const TSSID& aSSID,                 
    const TMacAddress& aBSSID,          
    TUint16 aAuthAlgorithmNbr,      
    TEncryptionStatus aEncryptionStatus,
    TBool aIsInfra,
    TUint16 aScanResponseFrameBodyLength,
    const TUint8* aScanResponseFrameBody,
    const TUint8* aIeData,
    TUint16 aIeDataLength )
    {
    TBool ret( ETrue );

    (aCtxImpl.GetBssId())= aBSSID;   
    (aCtxImpl.GetSsId()) = aSSID;   
    (aCtxImpl.EncryptionStatus()) = aEncryptionStatus;           
    (aCtxImpl.AuthenticationAlgorithmNumber()) = aAuthAlgorithmNbr;
    // set the BSSID field
    (aCtxImpl.GetAuthenticationFrame()).iHeader.iBSSID 
        = (aCtxImpl.GetBssId());
    (aCtxImpl.GetHtAuthenticationFrame()).iHeader.iBSSID 
        = (aCtxImpl.GetBssId());
    // set the DA field
    (aCtxImpl.GetAuthenticationFrame()).iHeader.iDA 
        = (aCtxImpl.GetBssId()); 
    (aCtxImpl.GetHtAuthenticationFrame()).iHeader.iDA 
        = (aCtxImpl.GetBssId()); 
    // set the SA field
    (aCtxImpl.GetAuthenticationFrame()).iHeader.iSA 
        = aCtxImpl.iWlanMib.dot11StationId;
    (aCtxImpl.GetHtAuthenticationFrame()).iHeader.iSA 
        = aCtxImpl.iWlanMib.dot11StationId;
    aCtxImpl.NetworkOperationMode( 
        aIsInfra ? WHA::EBSS : WHA::EIBSS );

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

        // infrastructure or IBSS mode
        if ( aIsInfra )
            {
            OsTracePrint( KUmacDetails, (TUint8*)
                ("UMAC: WlanDot11Idle::Connect: infra"));
                
            Fsm( aCtxImpl, ECONNECT );            
            }
        else    // --- IBSS mode ---
            {
            OsTracePrint( KUmacDetails, (TUint8*)
                ("UMAC: WlanDot11Idle::Connect: IBSS"));
                
            Fsm( aCtxImpl, ECONNECTIBSS );
            }
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
TBool WlanDot11Idle::Disconnect( 
    WlanContextImpl& aCtxImpl )
    {
    Fsm( aCtxImpl, EDISCONNECT );
        
    // global state transition will occur
    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Idle::RealScan(
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

    aCtxImpl.iStates.iIdleScanningMode.Set( 
        aMode, aSSID, aScanRate, aChannels, 
        aMinChannelTime, aMaxChannelTime, aSplitScan );

    Fsm( aCtxImpl, ESCAN );

    return ETrue; // global statemachine transition will occur 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::FinitSystem( 
    WlanContextImpl& aCtxImpl )
    {
    Fsm( aCtxImpl, ERELEASE );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Idle::AddBroadcastWepKey(
    WlanContextImpl& aCtxImpl,
    TUint32 aKeyIndex,             
    TBool aUseAsDefaulKey,                
    TUint32 aKeyLength,                      
    const TUint8 aKey[KMaxWEPKeyLength],
    const TMacAddress& aMac )
    {
    return OnAddBroadcastWepKey( aCtxImpl, aKeyIndex, aUseAsDefaulKey, 
        EFalse, // do not set as PTK
        aKeyLength, aKey, aMac );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Idle::AddUnicastWepKey(
    WlanContextImpl& aCtxImpl,
    const TMacAddress& aMacAddr,
    TUint32 aKeyLength,                      
    const TUint8 aKey[KMaxWEPKeyLength])
    {
    // allocate memory for the key structure
    WHA::SWepPairwiseKey* key = static_cast<WHA::SWepPairwiseKey*>
        (os_alloc( WHA::SWepPairwiseKey::KHeaderSize + aKeyLength )); 

    if ( !key )
        {
        // allocation failure
        Fsm( aCtxImpl, EABORT );
        return EFalse;
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

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::ChangeInternalState( 
    WlanContextImpl& aCtxImpl, 
    TState aNewState )
    {
    iState = aNewState;
    Fsm( aCtxImpl, ESTATEENTRY );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC * dot11-idle * FSM EVENT") );
#ifndef NDEBUG
    OsTracePrint( KUmacDetails, (TUint8*)("event:"));
    OsTracePrint( KUmacDetails, iEventName[aEvent] );
    OsTracePrint( KUmacDetails, (TUint8*)("state:"));
    OsTracePrint( KUmacDetails, iStateName[iState] );
#endif

    switch ( aEvent )
        {
        case ESTATEENTRY:
            OnStateEntryEvent( aCtxImpl );
            break;
        case ETXCOMPLETE:
            OnTxCompleteEvent( aCtxImpl );
            break;
        case ESCAN:
            OnScanEvent( aCtxImpl );
            break;
        case ECONNECT:
            OnConnectEvent( aCtxImpl );
            break;
        case ECONNECTIBSS:
            OnConnectIbssEvent( aCtxImpl );
            break;
        case EDISCONNECT:
            OnDisconnectEvent( aCtxImpl );
            break;
        case ERELEASE:
            OnReleaseEvent( aCtxImpl );
            break;
        case EABORT:
            OnAbortEvent( aCtxImpl );
            break;
        default:
            // catch internal FSM programming error
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("event:"));
            OsTracePrint( KErrorLevel, iEventName[aEvent] );                
#endif
            OsAssert( (TUint8*)("* UMAC * panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EINIT:
            if ( aCtxImpl.Reassociate() )
                {
                // a roaming case
                ChangeInternalState( aCtxImpl, EFINIT );
                }
            else
                {
                // not a roaming case
                ChangeInternalState( aCtxImpl, EWRITEMIB );
                }
            break;
        case EWRITEMIB:
            WriteSleepModeMib( aCtxImpl );
            break;
        case EFINIT:
            // fsm execution complete 

            // execute OID completion if necessary
            CompleteOid( aCtxImpl );
            // indicate scan completion if necessary
            IndicateScanCompletion( aCtxImpl );
            break;
        default:
            // catch internal FSM programming error
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("state:"));
            OsTracePrint( KErrorLevel, iStateName[iState] );
#endif
            OsAssert( (TUint8*)("* UMAC * panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EWRITEMIB:
            ChangeInternalState( aCtxImpl, EFINIT );
            break;
        case EFINIT:
            // default handler has issued a WHA command and we 
            // have come back to this object
            // we have absolutely nothing to do here
            // expect mayby complete something
            CompleteOid( aCtxImpl );
            break;
        default:
            // catch internal FSM programming error
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("state:"));
            OsTracePrint( KErrorLevel, iStateName[iState] );
#endif
            OsAssert( (TUint8*)("* UMAC * panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// ---------------------------------------------------------
// simulate macnotresponding error
// ---------------------------------------------------------
//
void WlanDot11Idle::OnAbortEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KWarningLevel, (TUint8*)("UMAC * dot11-idle * abort") );

    DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::OnScanEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,                                  // prev state
        aCtxImpl.iStates.iIdleScanningMode      // next state
        );      
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::OnConnectEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,                               // prev state
        aCtxImpl.iStates.iPrepareForBssMode  // next state
        );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::OnConnectIbssEvent(
    WlanContextImpl& aCtxImpl )
    {
    ChangeState( aCtxImpl, 
        *this,                                  // prev state
        aCtxImpl.iStates.iPrepareForIbssMode    // next state
        );            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::OnDisconnectEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // set completion code for the oid 
    Set( KErrNone );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,                              // prev state
        aCtxImpl.iStates.iSoftResetState    // next state
        );      
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::OnReleaseEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // register oid completion by setting completion value
    Set( KErrNone );
    // and execute transition
    ChangeState( aCtxImpl, 
        *this,                      // prev state
        aCtxImpl.WhaRelease()       // next state
        );      
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::WriteSleepModeMib( 
    WlanContextImpl& aCtxImpl )
    {
    WHA::SsleepMode* mib 
        = static_cast<WHA::SsleepMode*>(os_alloc( sizeof( WHA::SsleepMode ) )); 

    if ( !mib )
        {
        // allocation failure
        Fsm( aCtxImpl, EABORT );
        return;
        }

    // allocation success continue
    mib->iMode = WHA::KLowPowerMode;
    
    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
    wha_cmd.Set( 
        aCtxImpl, WHA::KMibSleepMode, sizeof(*mib), mib );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );           

    // as the parameters have been supplied we can now deallocate
    os_free( mib );       
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::GenerateRandomBssIDForIbss( 
    WlanContextImpl& aCtxImpl ) const
    {
    // generate random BSSID for IBSS
    TMacAddress mac;
    TUint16* ptr = reinterpret_cast<TUint16*>(mac.iMacAddress);
    const TUint16* const ptr_end 
        = ptr + (sizeof(TMacAddress) / sizeof(TUint16));

    while ( ptr != ptr_end )
        {
        *ptr = aCtxImpl.Random();
        ++ptr;
        }

    // the Universal/Local bit must be set ( 2nd bit of octet 0 )
    // the Induvidual/Group bit must be cleared ( 1st bit of octet 0 )
    GroupBit( mac, EFalse );    // clear
    LocalBit( mac );             // set

    // store the BSSID
    aCtxImpl.GetBssId() = mac;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::CompleteOid( 
    WlanContextImpl& aCtxImpl )
    {
    if ( iEventMask & KCompleteUponEntry )
        {
        iEventMask &= ~KCompleteUponEntry;

        OnOidComplete( aCtxImpl, iCompletionCode );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Idle::IndicateScanCompletion( 
    WlanContextImpl& aCtxImpl )
    {
    if ( iEventMask & KIndicateScanCompletionUponEntry )
        {
        iEventMask &= ~KIndicateScanCompletionUponEntry;

        OsTracePrint( KScan, (TUint8*)
            ("UMAC: WlanDot11Idle::IndicateScanCompletion: Send scan complete indication"));
    
        OnInDicationEvent( aCtxImpl, EScanCompleted );
        }
    }
