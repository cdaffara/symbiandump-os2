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
* Description:   Implementation of the Umac class
*
*/

/*
* %version: 57 %
*/

#include "config.h"
#include "UmacContextImpl.h"

#include "Umac.h"
#include "umacoidmsg.h"


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
Umac::Umac() : iPimpl( NULL ), iManagementRequestPending( EFalse )
    {
    os_memset( &iOidOutputData, 0, sizeof( iOidOutputData ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
Umac::~Umac()
    {
    if ( iPimpl )
        {
        OsTracePrint( KInitLevel, 
            (TUint8*)("UMAC: Umac::~Umac(): deallocate internal impl"));        

        iPimpl->~WlanContextImpl();
        os_free( iPimpl );
        iPimpl = NULL;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void Umac::OnTimeout( TWlanTimer aTimer )
    {
    if ( iPimpl->WsaCmdActive() ) // WHA layer in use 
        {
        // ...defer access
        
        switch ( aTimer )
            {
            case EWlanDefaultTimer:
                iPimpl->RegisterEvent( KTimeout );

                OsTracePrint( KEventDispatcher, 
                    (TUint8*)("UMAC: Umac::OnTimeout(): timeout event registered"));
                break;
            case EWlanVoiceCallEntryTimer:
                iPimpl->RegisterEvent( KVoiceCallEntryTimerTimeout );

                OsTracePrint( KEventDispatcher, 
                    (TUint8*)("UMAC: Umac::OnTimeout(): Voice Call Entry timer timeout event registered"));
                break;
            case EWlanNullTimer:
                iPimpl->RegisterEvent( KNullTimerTimeout );

                OsTracePrint( KEventDispatcher, 
                    (TUint8*)("UMAC: Umac::OnTimeout(): null timer timeout event registered"));
                break;
            case EWlanNoVoiceTimer:
                iPimpl->RegisterEvent( KNoVoiceTimerTimeout );

                OsTracePrint( KEventDispatcher, 
                    (TUint8*)("UMAC: Umac::OnTimeout(): No Voice timer timeout event registered"));
                break;
            case EWlanKeepAliveTimer:
                iPimpl->RegisterEvent( KKeepAliveTimerTimeout );

                OsTracePrint( KEventDispatcher, 
                    (TUint8*)("UMAC: Umac::OnTimeout(): Keep Alive timer timeout event registered"));
                break;
            case EWlanActiveToLightPsTimer:
                iPimpl->RegisterEvent( KActiveToLightPsTimerTimeout );

                OsTracePrint( KEventDispatcher, 
                    (TUint8*)("UMAC: Umac::OnTimeout(): Active to Light PS timer timeout event registered"));
                break;
            case EWlanLightPsToActiveTimer:
                iPimpl->RegisterEvent( KLightPsToActiveTimerTimeout );

                OsTracePrint( KEventDispatcher, 
                    (TUint8*)("UMAC: Umac::OnTimeout(): Light PS to Active timer timeout event registered"));
                break;
            case EWlanLightPsToDeepPsTimer:
                iPimpl->RegisterEvent( KLightPsToDeepPsTimerTimeout );

                OsTracePrint( KEventDispatcher, 
                    (TUint8*)("UMAC: Umac::OnTimeout(): Light PS to Deep PS timer timeout event registered"));
                break;
            default:
                OsTracePrint( KErrorLevel, 
                    (TUint8*)("UMAC: unknown timer: %d"), aTimer );
                OsAssert(
                    (TUint8*)("UMAC: panic"), 
                    (TUint8*)(WLAN_FILE), __LINE__ );
            }        
        }
    else
        {
        // WHA layer not in use, proceed with the event handling immediately

        switch ( aTimer )
            {
            case EWlanDefaultTimer:
                iPimpl->CurrentState().OnTimeout( *iPimpl );        
                break;
            case EWlanVoiceCallEntryTimer:
                iPimpl->CurrentState().OnVoiceCallEntryTimerTimeout( *iPimpl );        
                break;
            case EWlanNullTimer:
                iPimpl->CurrentState().OnNullTimerTimeout( *iPimpl );        
                break;
            case EWlanNoVoiceTimer:
                iPimpl->CurrentState().OnNoVoiceTimerTimeout( *iPimpl );        
                break;
            case EWlanKeepAliveTimer:
                iPimpl->CurrentState().OnKeepAliveTimerTimeout( *iPimpl );        
                break;
            case EWlanActiveToLightPsTimer:
                iPimpl->CurrentState().OnActiveToLightPsTimerTimeout( *iPimpl );        
                break;
            case EWlanLightPsToActiveTimer:
                iPimpl->CurrentState().OnLightPsToActiveTimerTimeout( *iPimpl );        
                break;
            case EWlanLightPsToDeepPsTimer:
                iPimpl->CurrentState().OnLightPsToDeepPsTimerTimeout( *iPimpl );
                break;
            default:
                OsTracePrint( KErrorLevel, 
                    (TUint8*)("UMAC: unknown timer: %d"), aTimer );
                OsAssert(
                    (TUint8*)("UMAC: panic"), 
                    (TUint8*)(WLAN_FILE), __LINE__ );
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void Umac::OnDfc( TAny* aCtx )
    {
    if ( !(iPimpl->WsaCmdActive()) ) 
        {
        // WHA layer NOT in use 
        // this is an implementation error as we only use dfc callback
        // to break WHA command dispatching context, 
        // when we issue a synchronous command
        OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
        }

    iPimpl->CurrentState().OnDfc( aCtx );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void Umac::CompleteManagementCommand( 
    TInt aReason, 
    const TAny* aData,
    TUint32 aLengthInBytes )
    {
    if ( iManagementRequestPending )
        {
        // complete pending managemement request
        iManagementRequestPending = EFalse;
        iOidOutputData.iOidData = aData;
        iOidOutputData.iLengthOfDataInBytes = aLengthInBytes;
        OnOidCompleted( aReason, iOidOutputData );
        }
    else
        {
        // no request pending so there's nothing to do
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void Umac::AttachWsa( WHA::Wha* aWha )
    {
    OsTracePrint( KInitLevel, (TUint8*)("UMAC: Umac::AttachWsa(): 0x%08x"), 
        reinterpret_cast<TUint32>(aWha) );    

    // just forward to next layer 
    iPimpl->AttachWsa( aWha );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::Init()
    {
    OsTracePrint( KInitLevel, (TUint8*)("UMAC: Umac::Init()"));    

    TBool ret( EFalse );

    // deallocation of objecs allocated here is done in destructor

    iPimpl = static_cast<WlanContextImpl*>
        (os_alloc( sizeof(WlanContextImpl) ));

    if ( iPimpl )
        {
        new (iPimpl) WlanContextImpl( *this );
        ret = iPimpl->Initialize();
        }
    else
        {
        // failed
        OsTracePrint( KErrorLevel, 
            (TUint8*)("UMAC: Umac::Init(): alloc internal impl failure -> abort"));                
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void Umac::BootUp(
    const TUint8* aPda, 
    TUint32 aPdaLength,
    const TUint8* aFw, 
    TUint32 aFwLength )
    {
    OsTracePrint( KInitLevel, (TUint8*)("UMAC: * BootUp()"));  
    iManagementRequestPending = ETrue;
    iPimpl->BootUp( aPda, aPdaLength, aFw, aFwLength );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void Umac::FinitSystem()
    {
    OsTracePrint( KInitLevel, (TUint8*)("UMAC: * FinitSystem()"));  
    iManagementRequestPending = ETrue;

    if ( iPimpl )
        {
        iPimpl->CurrentState().FinitSystem( *iPimpl );
        }
    else
        {
        // nothing to do. Just complete the WLAN Mgmt client request 
        CompleteManagementCommand( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TMacAddress& Umac::StationId() const
    {
    return iPimpl->iWlanMib.dot11StationId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void Umac::SetTxOffset()
    {
    iPimpl->CurrentState().SetProtocolStackTxOffset( *iPimpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void Umac::TxProtocolStackData( 
    TDataBuffer& aDataBuffer,
    TBool aMore )
    {
    iPimpl->CurrentState().TxData( *iPimpl, aDataBuffer, aMore );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void Umac::GetFrameExtraSpaceForVendor( 
    TUint8& aRxOffset,
    TUint8& aTxHeaderSpace,
    TUint8& aTxTrailerSpace ) const
    {
    aRxOffset = iPimpl->WHASettings().iRxoffset;
    aTxHeaderSpace = iPimpl->WHASettings().iTxFrameHeaderSpace;
    aTxTrailerSpace = iPimpl->WHASettings().iTxFrameTrailerSpace;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::TxPermitted( TWhaTxQueueState& aTxQueueState ) const
    {
    return ( iPimpl->ProtocolStackTxDataAllowed() && 
             iPimpl->GetWhaTxStatus( aTxQueueState ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::UserDataTxEnabled() const
    {
    return ( iPimpl->ProtocolStackTxDataAllowed() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void Umac::WriteMgmtFrame(TDataBuffer& aDataBuffer)
    {
    iPimpl->CurrentState().TxMgmtData( *iPimpl, aDataBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void Umac::HandleOid(
    const TOIDHeader* aOid,             
    TAny* aOutputBuffer,
    TUint aOutputBufferSize )
    {
    iManagementRequestPending = ETrue;

    iOidOutputData.iBufferSupplied = aOutputBuffer;
    iOidOutputData.iLengthOfBufferSupplied = aOutputBufferSize;
    iOidOutputData.iOidId = aOid->oid_id;

    if (// WHA layer in use 
        iPimpl->WsaCmdActive()
        || // OR
        // oid channel is disabled
        !(iPimpl->ChannelEnabled( WlanEventDispatcher::KOidChannel )) )
        {
        // ...defer access
        iPimpl->RegisterEvent( *aOid );
        return;
        }

    switch(aOid->oid_id)
        {
        case E802_11_CONFIGURE:
            HandleConfigure( aOid );
            break;
        case E802_11_CONNECT:
            HandleConnect( aOid );
            break;
        case E802_11_START_IBSS:
            HandleStartIBSS(aOid);
            break;
        case E802_11_SCAN:
            HandleScan( aOid );
            break;
        case E802_11_STOP_SCAN:
            HandleStopScan();
            break;               
        case E802_11_DISCONNECT:
            HandleDisconnect();
            break;
        case E802_11_SET_POWER_MODE:
            HandleSetPowerMode(aOid);
            break;
        case E802_11_SET_RCPI_TRIGGER_LEVEL:
            HandleSetRcpiTriggerLevel(aOid);
            break;
        case E802_11_SET_TX_POWER_LEVEL:
            HandleSetTxPowerLevel(aOid);
            break;
        case E802_11_GET_LAST_RCPI:
            HandleGetLastRcpi();
            break;
        case E802_11_DISABLE_USER_DATA:
            HandleDisableUserData();
            break;
        case E802_11_ENABLE_USER_DATA:
            HandleEnableUserData();
            break;
        case E802_11_ADD_CIPHER_KEY:
            HandleAddCipherKey(aOid);
            break;            
        case E802_11_ADD_MULTICAST_ADDR:
            HandleAddMulticastAddr(aOid);
            break;            
        case E802_11_REMOVE_MULTICAST_ADDR:
            HandleRemoveMulticastAddr(aOid);
            break;            
        case E802_11_CONFIGURE_BSS_LOST:
            HandleConfigureBssLost(aOid);
            break;            
        case E802_11_SET_TX_RATE_ADAPT_PARAMS:
            HandleSetTxRateAdaptParams(aOid);
            break;            
        case E802_11_CONFIGURE_TX_RATE_POLICIES:
            HandleConfigureTxRatePolicies(aOid);
            break;            
        case E802_11_SET_POWER_MODE_MGMT_PARAMS:
            HandleSetPowerModeMgmtParams(aOid);
            break;            
        case E802_11_CONFIGURE_PWR_MODE_MGMT_TRAFFIC_OVERRIDE:
            HandleConfigurePwrModeMgmtTrafficOverride(aOid);
            break;            
        case E802_11_GET_FRAME_STATISTICS:
            HandleGetFrameStatistics();
            break;            
        case E802_11_CONFIGURE_UAPSD:
            HandleConfigureUapsd(aOid);
            break;            
        case E802_11_CONFIGURE_TX_QUEUE:
            HandleConfigureTxQueue(aOid);
            break;            
        case E802_11_GET_MAC_ADDRESS:
            HandleGetMacAddress();
            break;
        case E802_11_CONFIGURE_ARP_IP_ADDRESS_FILTERING:
            HandleConfigureArpIpAddressFiltering(aOid);
            break;
        case E802_11_CONFIGURE_HT_BLOCK_ACK:
            HandleConfigureHtBlockAck(aOid);
            break;
        case E802_11_CONFIGURE_PROPRIETARY_SNAP_HDR:
            HandleConfigureProprietarySnapHdr(aOid);
            break;
        default:
            OsTracePrint( KErrorLevel, 
                (TUint8*)("UMAC: OID: 0x%08x"), aOid->oid_id );    
            OsAssert((TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleConnect(const TAny *aInputBuffer)
    {
    OsTracePrint( KWlmCmd, (TUint8*)("Umac::HandleConnect: WLM-E802_11_CONNECT") ); 
    
    const TConnectMsg* msg 
        = static_cast<const TConnectMsg*>(aInputBuffer);

#ifndef NDEBUG 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: SSID:") ); 
    TUint8 ssidTraceBuf[KMaxSSIDLength + 1]; // + 1 for NULL termination
    os_memset( ssidTraceBuf, 0, sizeof( ssidTraceBuf ) );
    os_memcpy( ssidTraceBuf, msg->SSID.ssid, msg->SSID.ssidLength );
    OsTracePrint( KWlmCmdDetails, ssidTraceBuf ); 

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: BSSID:"), 
        msg->BSSID ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: auth algorithm nbr: %d"), 
        msg->authAlgorithmNbr ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: encryptionStatus: %d"), 
        msg->encryptionStatus ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: pairwiseCipher: %d"), 
        msg->pairwiseCipher ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: isInfra: %d"), 
        msg->isInfra ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: scanResponseFrameBody (pointer): 0x%08x"), 
        reinterpret_cast<TUint32>(msg->scanResponseFrameBody) ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: scanResponseFrameBodyLength: %d"), 
        msg->scanResponseFrameBodyLength ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: ieData (pointer): 0x%08x"), 
        reinterpret_cast<TUint32>(msg->ieData) ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: ieDataLength: %d"), 
        msg->ieDataLength ); 
    if ( msg->ieData )
        {
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("Umac::HandleConnect: ieData data:"), 
            msg->ieData, msg->ieData + msg->ieDataLength  ); 
        }        
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: invalidatePairwiseKey (bool): %d"), 
        msg->invalidatePairwiseKey ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: invalidateGroupKey (bool): %d"), 
        msg->invalidateGroupKey ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: radiomeasurement (bool): %d"), 
        msg->radioMeasurement );		
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConnect: pairwise key length: %d"), 
        msg->pairwiseKey.length );
#endif // !NDEBUG 

    // make sure that these are clear when starting the connect operation
    //
    iPimpl->SetScanResponseFrameBodyLength( 0 );  
    iPimpl->SetScanResponseFrameBody( NULL );

    // invalidate cipher keys per mgmt client instructions
    
    if ( msg->invalidatePairwiseKey )
        {
        iPimpl->PairWiseKeyType( WHA::EKeyNone );
        }
    if ( msg->invalidateGroupKey )
        {
        iPimpl->GroupKeyType( WHA::EKeyNone );
        }

    iPimpl->PairwiseCipher( msg->pairwiseCipher );

    //Set Radio Measurement setting for later use
    iPimpl->RadioMeasurement( msg->radioMeasurement );
    
    if ( msg->pairwiseKey.length != 0 )
        {
        iPimpl->RoamingPairwiseKey( &(msg->pairwiseKey) );
        }
    else
        {
        iPimpl->RoamingPairwiseKey( NULL );
        }
    
    return iPimpl->CurrentState().Connect( 
        *iPimpl, 
        msg->SSID, 
        msg->BSSID, 
        msg->authAlgorithmNbr, 
        msg->encryptionStatus, 
        msg->isInfra,
        msg->scanResponseFrameBodyLength,
        msg->scanResponseFrameBody,
        msg->ieData,
        msg->ieDataLength );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleStartIBSS(const TAny *aInputBuffer)
    {
    OsTracePrint( KWlmCmd, 
        (TUint8*)("UMAC: Umac::HandleStartIBSS(): WLM-E802_11_START_IBSS") ); 

    const TStartIBSSMsg* msg 
        = static_cast<const TStartIBSSMsg*>(aInputBuffer);

    return iPimpl->CurrentState().StartIBSS( 
        *iPimpl, 
        msg->SSID, 
        msg->beaconInterval, 
        msg->ATIM, 
        msg->channel, 
        msg->encryptionStatus );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleDisconnect()
    {
    OsTracePrint( KWlmCmd, 
        (TUint8*)("UMAC: Umac::HandleDisconnect(): WLM-E802_11_DISCONNECT") ); 

    return iPimpl->CurrentState().Disconnect( *iPimpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleSetPowerMode(const TAny *aInputBuffer)
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleSetPowerMode: WLM-E802_11_SET_POWER_MODE") ); 

    const TSetPowerModeMsg* msg 
        = static_cast<const TSetPowerModeMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerMode: powerMode (0=CAM, 1=PS): %d"), 
        msg->powerMode );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerMode: disableDynamicPowerModeManagement: %d"), 
        msg->disableDynamicPowerModeManagement );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerMode: wakeupModeInLightPs (0=all Beacons, 1=all DTims, 2=Nth beacon, 3=Nth Dtim): %d"), 
        msg->wakeupModeInLightPs ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerMode: listenIntervalInLightPs: %d"), 
        msg->listenIntervalInLightPs ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerMode: wakeupModeInDeepPs (0=all Beacons, 1=all DTims, 2=Nth beacon, 3=Nth Dtim): %d"), 
        msg->wakeupModeInDeepPs ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerMode: listenIntervalInDeepPs: %d"), 
        msg->listenIntervalInDeepPs ); 
    
    return iPimpl->CurrentState().SetPowerMode( 
        *iPimpl, 
        msg->powerMode,
        msg->disableDynamicPowerModeManagement,
        msg->wakeupModeInLightPs,
        msg->listenIntervalInLightPs,
        msg->wakeupModeInDeepPs,
        msg->listenIntervalInDeepPs );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleSetRcpiTriggerLevel(const TAny *aInputBuffer)
    {
    OsTracePrint( KWlmCmd, 
        (TUint8*)("UMAC: Umac::HandleSetRcpiTriggerLevel: WLM-E802_11_SET_RCPI_TRIGGER_LEVEL") ); 

    const TSetRcpiTriggerLevelMsg* msg 
        = static_cast<const TSetRcpiTriggerLevelMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetRcpiTriggerLevel: RcpiTrigger: %d"), 
        msg->RcpiTrigger );

    return iPimpl->CurrentState().SetRcpiTriggerLevel( 
        *iPimpl, 
        msg->RcpiTrigger );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleSetTxPowerLevel(const TAny *aInputBuffer)
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleSetTxPowerLevel: WLM-E802_11_SET_TX_POWER_LEVEL") ); 

    const TSetTxPowerLevelMsg* msg 
        = static_cast<const TSetTxPowerLevelMsg*>(aInputBuffer);

    return iPimpl->CurrentState().SetTxPowerLevel(
        *iPimpl, 
        msg->level );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleConfigure(const TAny *aInputBuffer)
    {
    OsTracePrint( KWlmCmd, 
        (TUint8*)("UMAC: Umac::HandleConfigure(): WLM-E802_11_CONFIGURE") ); 
    
    const TConfigureMsg* msg = static_cast<const TConfigureMsg*>(aInputBuffer);

#ifndef NDEBUG
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigure: allowedWlanFeatures: 0x%08x"), 
        msg->allowedWlanFeatures );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigure: RTSThreshold: %d"), 
        msg->RTSThreshold );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigure: maxTxMSDULifetime: %d"), 
        msg->maxTxMSDULifetime );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigure: voiceCallEntryTimeout: %d"), 
        msg->voiceCallEntryTimeout );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigure: voiceCallEntryTxThreshold: %d"), 
        msg->voiceCallEntryTxThreshold );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigure: voiceNullTimeout: %d"), 
        msg->voiceNullTimeout );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigure: noVoiceTimeout: %d"), 
        msg->noVoiceTimeout );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigure: keepAliveTimeout: %d"), 
        msg->keepAliveTimeout );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigure: spRcpiIndicationLevel: %d"), 
        msg->spRcpiIndicationLevel );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigure: spTimeToCountPrediction: %d"), 
        msg->spTimeToCountPrediction );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigure: spMinIndicationInterval: %d"), 
        msg->spMinIndicationInterval );
#endif
    
    iPimpl->FeaturesAllowed( msg->allowedWlanFeatures );
    
    return iPimpl->CurrentState().Configure( 
        *iPimpl, 
        msg->RTSThreshold, 
        msg->maxTxMSDULifetime,
        msg->voiceCallEntryTimeout,
        msg->voiceCallEntryTxThreshold,
        msg->voiceNullTimeout,
        msg->noVoiceTimeout,
        msg->keepAliveTimeout,
        msg->spRcpiIndicationLevel,
        msg->spTimeToCountPrediction,
        msg->spMinIndicationInterval );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleScan( const TAny *aInputBuffer )
    {
    OsTracePrint( KWlmCmd, 
        (TUint8*)("UMAC: Umac::HandleScan: WLM-E802_11_SCAN") ); 

    TScanMsg* msg 
        = static_cast<TScanMsg*>(const_cast<TAny *>(aInputBuffer));

#ifndef NDEBUG 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleScan: mode: %d (0: active, 1: passive)"), 
        msg->mode );
    OsTracePrint( KWlmCmdDetails, 
        (TUint8*)("UMAC: Umac::HandleScan: SSID length: %d"),  
        msg->SSID.ssidLength );

    OsTracePrint( KWlmCmdDetails, 
        (TUint8*)("UMAC: Umac::HandleScan: SSID:") );
    TUint8 ssidTraceBuf[KMaxSSIDLength + 1]; // + 1 for NULL termination
    os_memset( ssidTraceBuf, 0, sizeof( ssidTraceBuf ) );
    os_memcpy( ssidTraceBuf, msg->SSID.ssid, msg->SSID.ssidLength );
    OsTracePrint( KWlmCmdDetails, ssidTraceBuf );

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleScan: scanRate: %d"), 
        msg->scanRate );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleScan: minChannelTime: %d"), 
        msg->minChannelTime );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleScan: maxChannelTime: %d"), 
        msg->maxChannelTime );
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleScan: splitScan: %d"), 
        msg->splitScan );
#endif // !NDEBUG 

    return iPimpl->CurrentState().Scan( 
        *iPimpl, 
        msg->mode,
        msg->SSID,
        msg->scanRate,
        msg->channels,
        msg->minChannelTime, 
        msg->maxChannelTime,
        msg->splitScan );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleStopScan()
    {
    OsTracePrint( KWlmCmd, 
        (TUint8*)("UMAC: Umac::HandleStopScan: WLM-E802_11_STOP_SCAN") ); 

    return iPimpl->CurrentState().StopScan( *iPimpl );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleGetLastRcpi()
    {
    OsTracePrint( KWlmCmd, 
        (TUint8*)("UMAC: Umac::HandleGetLastRcpi: WLM-E802_11_GET_LAST_RCPI") ); 

    return iPimpl->CurrentState().GetLastRcpi(
        *iPimpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleDisableUserData()
    {
    OsTracePrint( KWlmCmd, 
        (TUint8*)("UMAC: Umac::HandleDisableUserData: WLM-E802_11_DISABLE_USER_DATA") ); 

    return iPimpl->CurrentState().DisableUserData( *iPimpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleEnableUserData()
    {
    OsTracePrint( KWlmCmd, 
        (TUint8*)("UMAC: Umac::HandleEnableUserData: WLM-E802_11_ENABLE_USER_DATA") ); 

    return iPimpl->CurrentState().EnableUserData( *iPimpl );
    }
  
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleAddCipherKey(const TAny *aInputBuffer)
    {
    TBool ret( EFalse );

    OsTracePrint( KWlmCmd, 
        (TUint8*)("UMAC: Umac::HandleAddCipherKey: WLM-E802_11_ADD_CIPHER_KEY") ); 

    const TAddCipherKeyMsg* msg 
        = static_cast<const TAddCipherKeyMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, 
        (TUint8*)("UMAC: Umac::HandleAddCipherKey(): cipherSuite: %d"), msg->cipherSuite ); 
    OsTracePrint( KWlmCmdDetails, 
        (TUint8*)("UMAC: Umac::HandleAddCipherKey(): keyIndex: %d"), msg->keyIndex ); 
    OsTracePrint( KWlmCmdDetails, 
        (TUint8*)("UMAC: Umac::HandleAddCipherKey(): length: %d"), msg->length ); 

    for(TUint8 i = 0; i < msg->length; i++ )
        {
        OsTracePrint( KWlmCmdDetails, 
            (TUint8*)("UMAC: Umac::HandleAddCipherKey(): key data: 0x%02x"), 
            msg->data[i] );         
        }

    OsTracePrint( KWlmCmdDetails, 
        (TUint8*)("UMAC: Umac::HandleAddCipherKey(): macAddress:"), msg->macAddress ); 
    OsTracePrint( KWlmCmdDetails, 
        (TUint8*)("UMAC: Umac::HandleAddCipherKey(): useAsDefaultKey: %d"), 
        (msg->useAsDefaultKey)? 1 : 0 );
        
    switch( msg->cipherSuite )
        {
        case EWlanCipherSuiteTkip:
            if ( msg->macAddress == KBroadcastMacAddr )
                {
                OsTracePrint( KWlmCmdDetails, 
                    (TUint8*)("UMAC: Umac::HandleAddCipherKey(): add multicast TKIP key") ); 

                ret = iPimpl->CurrentState().AddMulticastTKIPKey( 
                    *iPimpl, 
                    static_cast<T802Dot11WepKeyId>(msg->keyIndex),
                    msg->length, 
                    msg->data );
                }
            else
                {                
                OsTracePrint( KWlmCmdDetails, 
                    (TUint8*)("UMAC: Umac::HandleAddCipherKey(): add pairwise TKIP key") ); 
                ret = iPimpl->CurrentState().AddTkIPKey( 
                    *iPimpl, 
                    msg->data,
                    msg->length, 
                    static_cast<T802Dot11WepKeyId>(msg->keyIndex),
                    msg->macAddress );
                }                                    
            break;            
        case EWlanCipherSuiteCcmp:
            if ( msg->macAddress == KBroadcastMacAddr )
                {
                OsTracePrint( KWlmCmdDetails, 
                    (TUint8*)("UMAC: Umac::HandleAddCipherKey(): add multicast AES key") ); 

                ret = iPimpl->CurrentState().AddMulticastAesKey( 
                    *iPimpl, 
                    static_cast<T802Dot11WepKeyId>(msg->keyIndex),
                    msg->length, 
                    msg->data );
                }
            else
                {                
                OsTracePrint( KWlmCmdDetails, 
                    (TUint8*)("UMAC: Umac::HandleAddCipherKey(): add pairwise AES key") ); 
                ret = iPimpl->CurrentState().AddAesKey( 
                    *iPimpl, 
                    msg->data,
                    msg->length, 
                    msg->macAddress );
                }                                    
            break;            
        case EWlanCipherSuiteWep:
            if ( msg->macAddress == KBroadcastMacAddr )
                {
                OsTracePrint( KWlmCmdDetails, 
                    (TUint8*)("UMAC: Umac::HandleAddCipherKey(): add broadcast wep key") ); 
                ret = iPimpl->CurrentState().AddBroadcastWepKey( 
                    *iPimpl, 
                    msg->keyIndex,
                    msg->useAsDefaultKey,
                    msg->length,
                    msg->data,
                    msg->macAddress );                
                }
            else
                {
                OsTracePrint( KWlmCmdDetails, 
                    (TUint8*)("UMAC: Umac::HandleAddCipherKey(): add unicast wep key") ); 
                ret = iPimpl->CurrentState().AddUnicastWepKey( 
                    *iPimpl, 
                    msg->macAddress,
                    msg->length,
                    msg->data );                
                }
            break;            
        case EWlanCipherSuiteWapi:
            if ( msg->macAddress == KBroadcastMacAddr )
                {
                OsTracePrint( KWlmCmdDetails, 
                    (TUint8*)("UMAC: Umac::HandleAddCipherKey(): add multicast wapi key") ); 
                ret = iPimpl->CurrentState().AddMulticastWapiKey( 
                    *iPimpl, 
                    static_cast<T802Dot11WepKeyId>(msg->keyIndex),
                    msg->length, 
                    msg->data );
                }
            else
                {
                OsTracePrint( KWlmCmdDetails, 
                    (TUint8*)("UMAC: Umac::HandleAddCipherKey(): add pairwise wapi key") ); 
                ret = iPimpl->CurrentState().AddUnicastWapiKey( 
                    *iPimpl, 
                    msg->data,
                    msg->length,
                    static_cast<T802Dot11WepKeyId>(msg->keyIndex),
                    msg->macAddress );
                }
            break;            
        default:
            OsTracePrint( 
                KErrorLevel, 
                (TUint8*)("UMAC: Umac::HandleAddCipherKey(): cipherSuite: %d"), 
                msg->cipherSuite  );    
            OsAssert(
                (TUint8*)("UMAC: Umac::HandleAddCipherKey(): panic"),
                (TUint8*)(WLAN_FILE), __LINE__ );        
        }    

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleAddMulticastAddr(const TAny *aInputBuffer)
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleAddMulticastAddr(): WLM-E802_11_ADD_MULTICAST_ADDR") ); 

    const TAddMulticastAddrMsg* msg 
        = static_cast<const TAddMulticastAddrMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleAddMulticastAddr(): macAddress:"), 
        msg->macAddress ); 
        
    return iPimpl->CurrentState().AddMulticastAddr( 
        *iPimpl,
        msg->macAddress );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleRemoveMulticastAddr(const TAny *aInputBuffer)
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleRemoveMulticastAddr(): WLM-E802_11_REMOVE_MULTICAST_ADDR") );

    const TRemoveMulticastAddrMsg* msg 
        = static_cast<const TRemoveMulticastAddrMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleRemoveMulticastAddr(): removeAll: %d"), 
        msg->removeAll ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleRemoveMulticastAddr(): macAddress:"), 
        msg->macAddress ); 
        
    return iPimpl->CurrentState().RemoveMulticastAddr( 
        *iPimpl,
        msg->removeAll,
        msg->macAddress );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleConfigureBssLost(const TAny *aInputBuffer)
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleConfigureBssLost(): WLM-E802_11_CONFIGURE_BSS_LOST") );

    const TConfigureBssLostMsg* msg 
        = static_cast<const TConfigureBssLostMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureBssLost: beaconLostCount: %d"), 
        msg->beaconLostCount ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureBssLost: failedTxPacketCount: %d"), 
        msg->failedTxPacketCount ); 
        
    return iPimpl->CurrentState().ConfigureBssLost( 
        *iPimpl,
        msg->beaconLostCount,
        msg->failedTxPacketCount );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleSetTxRateAdaptParams(const TAny *aInputBuffer)
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleSetTxRateAdaptParams: WLM-E802_11_SET_TX_RATE_ADAPT_PARAMS") );

    const TSetTxRateAdaptationParamsMsg* msg 
        = static_cast<const TSetTxRateAdaptationParamsMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetTxRateAdaptParams: minStepUpCheckpoint: %d"), 
        msg->minStepUpCheckpoint ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetTxRateAdaptParams: maxStepUpCheckpoint: %d"), 
        msg->maxStepUpCheckpoint ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetTxRateAdaptParams: stepUpCheckpointFactor: %d"), 
        msg->stepUpCheckpointFactor ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetTxRateAdaptParams: stepDownCheckpoint: %d"), 
        msg->stepDownCheckpoint ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetTxRateAdaptParams: minStepUpThreshold: %d"), 
        msg->minStepUpThreshold ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetTxRateAdaptParams: maxStepUpThreshold: %d"), 
        msg->maxStepUpThreshold ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetTxRateAdaptParams: stepUpThresholdIncrement: %d"), 
        msg->stepUpThresholdIncrement ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetTxRateAdaptParams: stepDownThreshold: %d"), 
        msg->stepDownThreshold ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetTxRateAdaptParams: disableProbeHandling: %d"), 
        msg->disableProbeHandling ); 
        
    return iPimpl->CurrentState().SetTxRateAdaptParams( 
        *iPimpl,
        msg->minStepUpCheckpoint,
        msg->maxStepUpCheckpoint,
        msg->stepUpCheckpointFactor,
        msg->stepDownCheckpoint,
        msg->minStepUpThreshold,
        msg->maxStepUpThreshold,
        msg->stepUpThresholdIncrement,
        msg->stepDownThreshold,
        msg->disableProbeHandling );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleConfigureTxRatePolicies( const TAny *aInputBuffer )
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleConfigureTxRatePolicies: WLM-E802_11_CONFIGURE_TX_RATE_POLICIES") );

    const TConfigureTxRatePoliciesMsg* msg 
        = static_cast<const TConfigureTxRatePoliciesMsg*>(aInputBuffer);

#ifndef NDEBUG

    // trace the rate policy objects

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureTxRatePolicies: numOfPolicyObjects: %d"), 
        msg->ratePolicy.numOfPolicyObjects ); 

    for ( TUint i = 0; i < msg->ratePolicy.numOfPolicyObjects; ++i )    
        {
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: Tx policy object ind: %d; values:"), 
            i ); 

        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 54 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy54 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 48 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy48 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 36 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy36 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 33 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy33 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 24 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy24 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 22 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy22 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 18 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy18 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 12 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy12 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 11 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy11 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 9 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy9 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 6 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy6 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 5.5 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy5_5 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 2 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy2 ); 
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: 1 Mbps: %d"), 
             msg->ratePolicy.txRateClass[i].txPolicy1 );                 

        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: shortRetryLimit: %d"), 
             msg->ratePolicy.txRateClass[i].shortRetryLimit );                 

        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: longRetryLimit: %d"), 
             msg->ratePolicy.txRateClass[i].longRetryLimit );                 

        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: flags: %d"), 
             msg->ratePolicy.txRateClass[i].flags );
        
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: autoRatePolicy: 0x%08x"), 
             msg->autoRatePolicy[i] );
        }

    // trace rate policy object to use for every Tx queue
    
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureTxRatePolicies: Rate policy object to use for a Tx queue:") ); 

    for ( TUint j = 0; j < EQueueIdMax; ++j )    
        {
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: Queue id: %d"), 
            j ); 

        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: rate policy object ind: %d"), 
            msg->queue2RateClass[j] ); 
        }

    // trace initial max Tx rate to use for every rate policy object
    
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureTxRatePolicies: initial max Tx rate to use for a policy object:") ); 

    for ( TUint k = 0; k < msg->ratePolicy.numOfPolicyObjects; ++k )    
        {
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: policy object ind: %d"), 
            k ); 

        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: initial max Tx rate: 0x%08x"), 
            msg->initialMaxTxRate4RateClass[k] ); 
        }

    for ( TUint m = 0; m < msg->ratePolicy.numOfPolicyObjects; ++m )    
        {
        OsTracePrint( KWlmCmdDetails, (TUint8*)
            ("UMAC: Umac::HandleConfigureTxRatePolicies: mcs set ind: %d"), 
            m ); 

        for ( TUint mcsBucket = 0; mcsBucket < KHtMcsSetLength; ++mcsBucket )    
            {
            OsTracePrint( KWlmCmdDetails, (TUint8*)
                ("UMAC: 0x%02x"), 
                msg->htMcsPolicy[m][mcsBucket] );
            }
        }
    
#endif

    return iPimpl->CurrentState().ConfigureTxRatePolicies( 
        *iPimpl,
        msg->ratePolicy,
        msg->queue2RateClass,
        msg->initialMaxTxRate4RateClass,
        msg->autoRatePolicy,
        msg->htMcsPolicy );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleSetPowerModeMgmtParams(const TAny *aInputBuffer)
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleSetPowerModeMgmtParams(): WLM-E802_11_SET_POWER_MODE_MGMT_PARAMS") );

    const TSetPowerModeMgmtParamsMsg* msg 
        = static_cast<const TSetPowerModeMgmtParamsMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerModeMgmtParams: toLightPsTimeout: %d"), 
        msg->toLightPsTimeout ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerModeMgmtParams: toLightPsFrameThreshold: %d"), 
        msg->toLightPsFrameThreshold ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerModeMgmtParams: toActiveTimeout: %d"), 
        msg->toActiveTimeout ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerModeMgmtParams: toActiveFrameThreshold: %d"), 
        msg->toActiveFrameThreshold ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerModeMgmtParams: toDeepPsTimeout: %d"), 
        msg->toDeepPsTimeout ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerModeMgmtParams: toDeepPsFrameThreshold: %d"), 
        msg->toDeepPsFrameThreshold ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleSetPowerModeMgmtParams: uapsdRxFrameLengthThreshold: %d"), 
        msg->uapsdRxFrameLengthThreshold ); 

    return iPimpl->CurrentState().SetPowerModeManagementParameters( 
        *iPimpl,
        msg->toLightPsTimeout,
        msg->toLightPsFrameThreshold,
        msg->toActiveTimeout,
        msg->toActiveFrameThreshold,
        msg->toDeepPsTimeout,
        msg->toDeepPsFrameThreshold,
        msg->uapsdRxFrameLengthThreshold );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleConfigurePwrModeMgmtTrafficOverride( const TAny *aInputBuffer )
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleConfigurePwrModeMgmtTrafficOverride: WLM-E802_11_CONFIGURE_PWR_MODE_MGMT_TRAFFIC_OVERRIDE") );

    const TConfigurePwrModeMgmtTrafficOverrideMsg* msg 
        = static_cast<const TConfigurePwrModeMgmtTrafficOverrideMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigurePwrModeMgmtTrafficOverride: stayInPsDespiteUapsdVoiceTraffic: %d"), 
        msg->stayInPsDespiteUapsdVoiceTraffic ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigurePwrModeMgmtTrafficOverride: stayInPsDespiteUapsdVideoTraffic: %d"), 
        msg->stayInPsDespiteUapsdVideoTraffic ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigurePwrModeMgmtTrafficOverride: stayInPsDespiteUapsdBestEffortTraffic: %d"), 
        msg->stayInPsDespiteUapsdBestEffortTraffic ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigurePwrModeMgmtTrafficOverride: stayInPsDespiteUapsdBackgroundTraffic: %d"), 
        msg->stayInPsDespiteUapsdBackgroundTraffic ); 

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigurePwrModeMgmtTrafficOverride: stayInPsDespiteLegacyVoiceTraffic: %d"), 
        msg->stayInPsDespiteLegacyVoiceTraffic ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigurePwrModeMgmtTrafficOverride: stayInPsDespiteLegacyVideoTraffic: %d"), 
        msg->stayInPsDespiteLegacyVideoTraffic ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigurePwrModeMgmtTrafficOverride: stayInPsDespiteLegacyBestEffortTraffic: %d"), 
        msg->stayInPsDespiteLegacyBestEffortTraffic ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigurePwrModeMgmtTrafficOverride: stayInPsDespiteLegacyBackgroundTraffic: %d"), 
        msg->stayInPsDespiteLegacyBackgroundTraffic ); 

    return iPimpl->CurrentState().ConfigurePwrModeMgmtTrafficOverride( 
        *iPimpl,
        msg->stayInPsDespiteUapsdVoiceTraffic,
        msg->stayInPsDespiteUapsdVideoTraffic,
        msg->stayInPsDespiteUapsdBestEffortTraffic,
        msg->stayInPsDespiteUapsdBackgroundTraffic,        
        msg->stayInPsDespiteLegacyVoiceTraffic,
        msg->stayInPsDespiteLegacyVideoTraffic,
        msg->stayInPsDespiteLegacyBestEffortTraffic,
        msg->stayInPsDespiteLegacyBackgroundTraffic );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleGetFrameStatistics()
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleSetPowerModeMgmtParams: WLM-E802_11_GET_FRAME_STATISTICS") );

    return iPimpl->CurrentState().GetFrameStatistics( *iPimpl );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleConfigureUapsd( const TAny *aInputBuffer )
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleSetPowerModeMgmtParams: WLM-E802_11_CONFIGURE_UAPSD") );

    const TConfigureUapsdMsg* msg 
        = static_cast<const TConfigureUapsdMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureUapsd: maxServicePeriodLength: 0x%02x (0x00: all, 0x20: 2, 0x40: 4, 0x60: 6"), 
        msg->maxServicePeriodLength ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureUapsd: uapsdForVoice: %d"), 
        msg->uapsdForVoice ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureUapsd: uapsdForVideo: %d"), 
        msg->uapsdForVideo ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureUapsd: uapsdForBestEffort: %d"), 
        msg->uapsdForBestEffort ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureUapsd: uapsdForBackground: %d"), 
        msg->uapsdForBackground ); 
    
    return iPimpl->CurrentState().ConfigureUapsd( 
        *iPimpl,
        msg->maxServicePeriodLength,
        msg->uapsdForVoice,
        msg->uapsdForVideo,
        msg->uapsdForBestEffort,
        msg->uapsdForBackground );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleConfigureTxQueue( const TAny *aInputBuffer )
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleConfigureTxQueue: WLM-E802_11_CONFIGURE_TX_QUEUE") );

    const TConfigureTxQueueMsg* msg 
        = static_cast<const TConfigureTxQueueMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureTxQueue: queueId: %d"), 
        msg->queueId ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureTxQueue: mediumTime: %d"), 
        msg->mediumTime ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureTxQueue: maxTxMSDULifetime: %d"), 
        msg->maxTxMSDULifetime ); 

    return iPimpl->CurrentState().ConfigureTxQueueIfNecessary( 
        *iPimpl,
        msg->queueId,
        msg->mediumTime,
        msg->maxTxMSDULifetime );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleGetMacAddress()
    {
    OsTracePrint( KWlmCmd, 
        (TUint8*)("UMAC: Umac::HandleGetMacAddress: WLM-E802_11_GET_MAC_ADDRESS") ); 

    return iPimpl->CurrentState().GetMacAddress( *iPimpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleConfigureArpIpAddressFiltering( const TAny *aInputBuffer )
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleConfigureArpIpAddressFiltering: WLM-E802_11_CONFIGURE_ARP_IP_ADDRESS_FILTERING") );

    const TConfigureArpIpAddressFilteringMsg* msg 
        = static_cast<const TConfigureArpIpAddressFilteringMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureArpIpAddressFiltering: enableFiltering: %d"), 
        msg->enableFiltering ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureArpIpAddressFiltering: ipV4Addr: 0x%08x"), 
        msg->ipV4Addr ); 

    return iPimpl->CurrentState().ConfigureArpIpAddressFiltering( 
        *iPimpl,
        msg->enableFiltering,
        msg->ipV4Addr );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleConfigureHtBlockAck( const TAny *aInputBuffer )
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleConfigureHtBlockAck: WLM-E802_11_CONFIGURE_HT_BLOCK_ACK") );

    const TConfigureHtBlockAckMsg* msg 
        = static_cast<const TConfigureHtBlockAckMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureHtBlockAck: iTxBlockAckUsage: 0x%02x"), 
        msg->iTxBlockAckUsage ); 
    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("UMAC: Umac::HandleConfigureHtBlockAck: iRxBlockAckUsage: 0x%02x"), 
        msg->iRxBlockAckUsage ); 

    return iPimpl->CurrentState().ConfigureHtBlockAck( 
        *iPimpl,
        msg->iTxBlockAckUsage,
        msg->iRxBlockAckUsage );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::HandleConfigureProprietarySnapHdr( const TAny *aInputBuffer )
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: Umac::HandleConfigureProprietarySnapHdr: WLM-E802_11_CONFIGURE_PROPRIETARY_SNAP_HDR") );

    const TConfigureProprietarySnapHdrMsg* msg 
        = static_cast<const TConfigureProprietarySnapHdrMsg*>(aInputBuffer);

    OsTracePrint( KWlmCmdDetails, (TUint8*)
        ("Umac::HandleConfigureProprietarySnapHdr: snapHdr:"), 
        reinterpret_cast<const TUint8*>(&(msg->snapHdr)), 
        reinterpret_cast<const TUint8*>(&(msg->snapHdr)) 
        + sizeof( TSnapHeader ) );     
    
    return iPimpl->CurrentState().ConfigureProprietarySnapHdr( 
        *iPimpl,
        msg->snapHdr );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::OnWhaCommandComplete( 
    WHA::TCompleteCommandId aCompleteCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandCompletionParams& aCommandCompletionParams )
    {
    return iPimpl->OnDeferredWhaCommandComplete(
        aCompleteCommandId, 
        aStatus,
        aCommandCompletionParams );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Umac::OnInternalEvent( TInternalEvent aInternalEvent )
    {
    return iPimpl->OnDeferredInternalEvent( aInternalEvent );
    }
