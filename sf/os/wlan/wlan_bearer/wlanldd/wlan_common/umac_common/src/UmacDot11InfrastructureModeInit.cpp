/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of WlanDot11InfrastructureModeInit class
*
*/

/*
* %version: 40 %
*/

#include "config.h"
#include "UmacDot11InfrastructureModeInit.h"
#include "UmacContextImpl.h"
#include "UmacWsaWriteMib.h"
#include "UmacWsaSetBssParameters.h"
#include "umacelementlocator.h"


// amount of memory allocated in the mib write buffer
const TUint KMibBufferAllocLen( 100 );

#ifndef NDEBUG
const TInt8 WlanDot11InfrastructureModeInit::iName[] 
    = "dot11-infrastructuremodeinit";

const TUint8 WlanDot11InfrastructureModeInit::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"EENABLEBEACONRX"}, 
        {"ESETNULLDATAFRAMETEMPLATE"},
        {"ESETQOSNULLDATAFRAMETEMPLATE"},
        {"ESETPSPOLLTEMPLATE"},
        {"EWAIT4BEACON"}, 
        {"EDISABLEBEACONRX"},                         
        {"ESETBSSPARAMS"}, 
        {"ECONFBEACONFILTERIETABLE"}, 
        {"EWAIT4USERDATAENABLE"}, 
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11InfrastructureModeInit::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, 
        {"ETXCOMPLETE"}, 
        {"EBEACONPROCESSED"},
        {"EUSERDATAENABLE"}, 
        {"EABORT"} 
    };
#endif

// beacon has been processed
const TUint32 KBeaconHandled        = (1 << 0);
// scan OID has been issued
const TUint32 KScanning             = (1 << 1);
// BSS Parameters have been set
const TUint32 KBssParamsSet       	= (1 << 2);

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanDot11InfrastructureModeInit::~WlanDot11InfrastructureModeInit()
    {
    iMibMemoryBuffer = NULL;
    }

#ifndef NDEBUG 
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
const TInt8* WlanDot11InfrastructureModeInit::GetStateName( 
    TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::Entry( 
    WlanContextImpl& aCtxImpl)
    {
    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( (TUint8*)("UMAC* panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    if ( !(aCtxImpl.DispatchEvent()) )
        {
        // no state transition occurred
        // dispatcher might or might not have had an event to be dispatched
        // in any case we are still in the current state and can continue...
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
    else    // --- !(aCtxImpl.DispatchEvent())
        {
        // state transition occurred
        // we are no longer in the current state,
        // so we won't do anything as we might mess things up
        }   
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::Exit( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    if ( !(iFlags & KScanning) )
        {
        // we are about to make a transition to another dot11 state
        // and that state is not the scanning state
        // from scanning state we shall come back to this dot11 state
        // so basically this means that
        // only thing we shall do is to reset our FSM for the next round...
        iState = EINIT; 
        os_free( iMibMemoryBuffer );
        iMibMemoryBuffer = NULL;
        }
    else
        {
        iFlags &= ~KScanning;

        OsTracePrint( KScan, (TUint8*)
            ("UMAC * dot11-infrastructuremodeinit * switch to scan mode") );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanDot11InfrastructureModeInit::ChangeInternalState( 
    WlanContextImpl& aCtxImpl, 
    TState aNewState )
    {
    iState = aNewState;
    Fsm( aCtxImpl, ESTATEENTRY );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC * dot11-infrastructuremodeinit * FSM EVENT") );
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
        case EBEACONPROCESSED:
            OnBeaconProcessedEvent( aCtxImpl );
            break;
        case EUSERDATAENABLE:
            OnUserDataEnableEvent( aCtxImpl );
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

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EINIT:                
            iMibMemoryBuffer = os_alloc( KMibBufferAllocLen );
            if ( iMibMemoryBuffer )
                {
                // reset event flag and continue with the state traversal
                iFlags = 0;
                ChangeInternalState( aCtxImpl, EENABLEBEACONRX );
                }
            else
                {
                // allocation failure
                Fsm( aCtxImpl, EABORT );
                }
            break;
        case EENABLEBEACONRX:
            // enable beacon frame reception
            EnableBeaconRx( aCtxImpl );
            break;
        case ESETNULLDATAFRAMETEMPLATE:
            // configure NULL data frame template
            SetNullDataFrameTemplate( aCtxImpl );
            break;
        case ESETQOSNULLDATAFRAMETEMPLATE:
            // configure NULL data frame template
            SetQosNullDataFrameTemplate( aCtxImpl );
            break;
        case ESETPSPOLLTEMPLATE:
            // configure ps poll frame template
            SetPsPollFrameTemplate( aCtxImpl );
            break;
        case ESETBSSPARAMS:
            // set the BSS parameters
            SetBssParams( aCtxImpl );
            break;
        case EDISABLEBEACONRX:
            // disable beacon frame reception
            DisableBeaconRx( aCtxImpl );
            break;
        case ECONFBEACONFILTERIETABLE:
            // configure beaconfiltertable MIB
            ConfigureBeaconFilterIeTable( aCtxImpl );
            break;
        case ECONTINUEDOT11TRAVERSE:
            // continue dot11 state traversal to dot11awake mode
            // or to dot11eneterdoze state
            ContinueDot11StateTraversal( aCtxImpl );
            break;        
        // these critters don't have an entry action
        // but the entry event gets sent automatically by the framework 
        // from state traversal method so they have to be defined here
        case EWAIT4BEACON:
        case EWAIT4USERDATAENABLE:
            break;
        default:
            // catch internal FSM programming error
            OsTracePrint( 
                KErrorLevel, 
                (TUint8*)
                ("UMAC: WlanDot11InfrastructureModeInit::OnStateEntryEvent(): state: %d"), 
                iState);        
            OsAssert( 
                (TUint8*)
                ("UMAC: WlanDot11InfrastructureModeInit::OnStateEntryEvent(): panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EWAIT4BEACON:
            // event handler for this state is needed
            // as we may re-enter this dot11 state (e.g. from scanning state 
            // or because we autonomously requested a WHA cmd to be executed)
            // while waiting for this event to occur
            if ( iFlags & KBeaconHandled )
                {
                // give a correct event for the FSM critter
                Fsm( aCtxImpl, EBEACONPROCESSED );
                }
            break;
        case EWAIT4USERDATAENABLE:
            // event handler for this state is needed
            // as we may re-enter this dot11 state (e.g. from scanning state 
            // or because we autonomously requested a WHA cmd to be executed)
            // while waiting for this event to occur
            if ( aCtxImpl.ProtocolStackTxDataAllowed() )
                {
                // give a correct event for the FSM critter
                Fsm( aCtxImpl, EUSERDATAENABLE );
                }
            break;
        case EENABLEBEACONRX:
            ChangeInternalState( aCtxImpl, 
                ESETNULLDATAFRAMETEMPLATE );
            break;
        case ESETNULLDATAFRAMETEMPLATE:
            ChangeInternalState( aCtxImpl, 
                ESETQOSNULLDATAFRAMETEMPLATE );
            break;
        case ESETQOSNULLDATAFRAMETEMPLATE:
            ChangeInternalState( aCtxImpl, 
                ESETPSPOLLTEMPLATE );
            break;
        case ESETPSPOLLTEMPLATE:
            // as we have configured the ps poll frame we shall now
            // complete the request
            CompleteConnectRequest( aCtxImpl, KErrNone );
            ChangeInternalState( aCtxImpl, 
                EWAIT4BEACON );
            break;
        case EDISABLEBEACONRX:
            ChangeInternalState( aCtxImpl, 
                ESETBSSPARAMS );
            break;
        case ESETBSSPARAMS:
            ChangeInternalState( aCtxImpl, 
                ECONFBEACONFILTERIETABLE );
            break;
        case ECONFBEACONFILTERIETABLE:
            if ( aCtxImpl.ProtocolStackTxDataAllowed() )
                {
                // user data enabled continue
                // state traversal
                ChangeInternalState( aCtxImpl, 
                    ECONTINUEDOT11TRAVERSE );
                }
            else
                {
                // user data is not enabled wait 4 it
                ChangeInternalState( aCtxImpl, 
                    EWAIT4USERDATAENABLE );
                }

            break;
        default:
            // catch internal FSM programming error
            OsTracePrint( KErrorLevel, (TUint8*)("state: %d"), iState);        
            OsAssert( (TUint8*)("UMAC * panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::OnBeaconProcessedEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EWAIT4BEACON:
            // continue state traversal
            ChangeInternalState( aCtxImpl, EDISABLEBEACONRX );
            break;        
        default:
            // if the beacon gets processed in an earlier internal
            // state than EWAIT4BEACON, we won't change to 
            // EDISABLEBEACONRX as it would disrupt the sequence 
            // of actions. So we will do this internal state change
            // later in its due time            
            break;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::OnUserDataEnableEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // continue state traversal
    ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
    }

// ---------------------------------------------------------
// simulate macnotresponding error
// ---------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::OnAbortEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KWarningLevel, 
        (TUint8*)("UMAC * dot11-infrastructuremodeinit * abort") );

    DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::EnableBeaconRx( 
    WlanContextImpl& aCtxImpl )
    {    
    const TUint32 KBeaconRxEnableValue( 1 );

    WHA::SbeaconFilterEnable* mib 
        = static_cast<WHA::SbeaconFilterEnable*>(iMibMemoryBuffer);

    // enable reception of every beacon from the BSS 
    mib->iEnable = EFalse;
    // even though this attribute is nowadays deprecated, we still continue to
    // set it in this case; to be on the safe side
    mib->iCount = KBeaconRxEnableValue;
           
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibBeaconFilterEnable, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::DisableBeaconRx( 
    WlanContextImpl& aCtxImpl )
    {    
    const TUint32 KBeaconRxDisableValue( 0 );

    WHA::SbeaconFilterEnable* mib 
        = static_cast<WHA::SbeaconFilterEnable*>(iMibMemoryBuffer);

    // disable reception of beacons except those specified 
    // by beaconFilterIeTable
    mib->iEnable = ETrue;
    mib->iCount = KBeaconRxDisableValue;

    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();

    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibBeaconFilterEnable, sizeof(*mib), mib );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );           
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::SetNullDataFrameTemplate( 
    WlanContextImpl& aCtxImpl )
    {
    TUint32 mibLength( 
        WHA::StemplateFrame::KHeaderSize
        + sizeof( SNullDataFrame ) ); 

    // align length of MIB to 4-byte boundary
    mibLength = Align4( mibLength );

    if ( mibLength > KMibBufferAllocLen )
        {
        // programming error
        OsTracePrint( KErrorLevel, (TUint8*)("UMAC: mib length: %d"), mibLength);
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
                            
    WHA::StemplateFrame* mib 
        = static_cast<WHA::StemplateFrame*>(iMibMemoryBuffer);

    mib->iFrameType = WHA::KNullDataTemplate;

    mib->iInitialTransmitRate = InitialSpecialFrameTxRate( aCtxImpl );
    mib->iLength = sizeof( SNullDataFrame );

    // copy the null data frame after the mib header
    os_memcpy( mib->iTemplateData,
               reinterpret_cast<TUint8*>(&(aCtxImpl.NullDataFrame())),
               sizeof( SNullDataFrame ) );
              
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();

    wsa_cmd.Set( 
        aCtxImpl, 
        WHA::KMibTemplateFrame, 
        mibLength,
        mib );
   
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );     
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::SetQosNullDataFrameTemplate( 
    WlanContextImpl& aCtxImpl )
    {
    const TUint16 KQosNullDataFrameLength ( aCtxImpl.QosNullDataFrameLength() );
    TUint16 mibLength( 
        WHA::StemplateFrame::KHeaderSize
        + KQosNullDataFrameLength ); 

    // align length of MIB to 4-byte boundary
    mibLength = Align4( mibLength );

    if ( mibLength > KMibBufferAllocLen )
        {
        // programming error
        OsTracePrint( KErrorLevel, (TUint8*)("UMAC: mib length: %d"), mibLength);
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
                            
    WHA::StemplateFrame* mib 
        = static_cast<WHA::StemplateFrame*>(iMibMemoryBuffer);

    mib->iFrameType = WHA::KQosNullDataTemplate;
    mib->iInitialTransmitRate = InitialSpecialFrameTxRate( aCtxImpl );
    mib->iLength = KQosNullDataFrameLength;

    // note that the Order bit of the Frame Control field of the QoS Null data
    // frame template has already been given the correct value in 
    // WlanMacActionState::OnDot11InfrastructureModeStateSpaceEntry() method
    
    // copy the QoS null data frame after the mib header
    os_memcpy( mib->iTemplateData,
               reinterpret_cast<TUint8*>(&(aCtxImpl.QosNullDataFrame())),
               KQosNullDataFrameLength );
              
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();

    wsa_cmd.Set( 
        aCtxImpl, 
        WHA::KMibTemplateFrame, 
        mibLength,
        mib );
   
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );     
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::SetPsPollFrameTemplate( 
    WlanContextImpl& aCtxImpl )
    {
    TUint32 mibLength( 
        WHA::StemplateFrame::KHeaderSize
        + sizeof( SPsPoll ) ); 

    // align length of MIB to 4-byte boundary
    mibLength = Align4( mibLength );

    if ( mibLength > KMibBufferAllocLen )
        {
        // programming error
        OsTracePrint( KErrorLevel, (TUint8*)("UMAC: mib length: %d"), mibLength);
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    WHA::StemplateFrame* mib 
        = static_cast<WHA::StemplateFrame*>(iMibMemoryBuffer);

    mib->iFrameType = WHA::KPSPollTemplate;

    mib->iInitialTransmitRate = InitialSpecialFrameTxRate( aCtxImpl );
    mib->iLength = sizeof( SPsPoll );

    // construct the ps poll frame after the mib header
    new (mib->iTemplateData) SPsPoll( 
        static_cast<TUint16>(aCtxImpl.Aid()), 
        aCtxImpl.GetBssId(),
        aCtxImpl.iWlanMib.dot11StationId );
              
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();

    wsa_cmd.Set( 
        aCtxImpl, 
        WHA::KMibTemplateFrame, 
        mibLength,
        mib );
   
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );     
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::SetBssParams( 
    WlanContextImpl& aCtxImpl )
    {
    WlanWsaSetBssParameters& wsa_cmd = aCtxImpl.WsaSetBssParameters();

    OsTracePrint( KDot11StateTransit | KUmacDetails, (TUint8*)
        ("UMAC * dot11-infrastructuremodeinit * DTIM period: %d"), iDtim );
    OsTracePrint( KDot11StateTransit | KUmacDetails, 
        (TUint8*)("UMAC * dot11-infrastructuremodeinit * AID: 0x%04x"), 
        aCtxImpl.Aid() );

    wsa_cmd.Set( aCtxImpl, iDtim, aCtxImpl.Aid() );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   

    iFlags |= KBssParamsSet;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::OnBeaconFrameRx( 
    WlanContextImpl& aCtxImpl,
    const TAny* aFrame,
    const TUint32 aLength,
    WHA::TRcpi aRcpi,
    TUint8* aBuffer )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11InfrastructureModeInit::OnBeaconFrameRx()") );

    if ( !(iFlags & KBeaconHandled) )
        {
        OsTracePrint( KDot11StateTransit | KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11InfrastructureModeInit::OnBeaconFrameRx: process beacon frame") );

        // buffer begin
        const TUint8* ptr = reinterpret_cast<const TUint8*>(aFrame);

        // bypass mac header, timestamp fixed field, beacon interval fixed field
        // and capability fixed field
        const TUint8 offset = 
            KMacHeaderLength 
            + KTimeStampFixedFieldLength 
            + KBeaconIntervalFixedFieldLength
            + KCapabilityInformationFixedFieldLength;        
        ptr += offset; // we now point to the beginning of IEs
        
        // ---- begin: extract DTIM period ----

        WlanElementLocator elementLocator( ptr, aLength - offset );
        TUint8 elementDatalength( 0 );
        const TUint8* elementData( NULL );

        // if TIM IE is not found or the value of DTIM period in the IE is 
        // zero, we use this value for DTIM period
        const TUint8 KRecoveryDtimPeriod ( 1 );
        
        if ( WlanElementLocator::EWlanLocateOk == 
             elementLocator.InformationElement( 
                 E802Dot11TimIE, 
                 elementDatalength, 
                 &elementData ) )        
            {            
            // extract DTIM period
            
            const TUint KDtimOffset ( 1 );
            iDtim = elementData[KDtimOffset] ? 
                elementData[KDtimOffset] : KRecoveryDtimPeriod;

            OsTracePrint( KDot11StateTransit | KUmacDetails, (TUint8*)  
                ("UMAC:  WlanDot11InfrastructureModeInit::OnBeaconFrameRx: DTIM period: %d"), 
                iDtim );
            }
        else
            {
            // TIM IE not found
            OsTracePrint( KWarningLevel, (TUint8*)
                ("UMAC: WlanDot11InfrastructureModeInit::OnBeaconFrameRx: WARNING: TIM IE extraction failure") );

            iDtim = KRecoveryDtimPeriod;

            OsTracePrint( KWarningLevel, (TUint8*)
                ("UMAC: WlanDot11InfrastructureModeInit::OnBeaconFrameRx: fake DTIM period to %d"), 
                iDtim );
            }
        
        // ---- end: extract DTIM period ----

        // we now point just after TIM IE

        iFlags |= KBeaconHandled;   // mark us handled
        
        // forward the beacon frame to Wlan Mgmt client
        OsTracePrint( KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11InfrastructureModeInit::OnBeaconFrameRx: fwd the beacon to Wlan Mgmt client") );
        XferDot11FrameToMgmtClient( aCtxImpl, aFrame, aLength, aRcpi, aBuffer );

        if ( !(aCtxImpl.WsaCmdActive()) )
            {
            // no WHA command pending send event to the fsm
            Fsm( aCtxImpl, EBEACONPROCESSED );
            }

        }
    else    // --- !(iFlags & KBeaconHandled) ---
        {
        // we have already processed a beacon in this dot11 state
        // so we have nothing to do

        // except to release the Rx buffer
        aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::ConfigureBeaconFilterIeTable( 
    WlanContextImpl& aCtxImpl )
    {
    TUint32 mibLength = WHA::SbeaconFilterIeTable::KHeaderSize;
    
    WHA::SbeaconFilterIeTable* mib 
        = static_cast<WHA::SbeaconFilterIeTable*>(iMibMemoryBuffer);

    // =================================================================
    // track ERP information IE for change
    // =================================================================
    //
    WHA::SDefaultIeFilterTable* filter_table( 
        reinterpret_cast<WHA::SDefaultIeFilterTable*>(mib->iIeTable) );

    filter_table->iIe = E802Dot11ErpInformationIE;
    filter_table->iTreatMeant = WHA::SDefaultIeFilterTable::KTrackChange;

    mibLength += sizeof( WHA::SDefaultIeFilterTable );
    
    // =================================================================
    // track WMM Parameter Element for change
    // =================================================================
    //
    WHA::SIe221FilterTable* filterTableEntry( 
        reinterpret_cast<WHA::SIe221FilterTable*>(filter_table + 1) );

    filterTableEntry->iIe = E802Dot11VendorSpecificIE;    
    filterTableEntry->iTreatMeant = WHA::SIe221FilterTable::KTrackChange;

    os_memcpy( filterTableEntry->iOui, KWmmElemOui, sizeof( KWmmElemOui ) );
    filterTableEntry->iType = KWmmElemOuiType;
    
    // For this element we have to assign both
    // OUI Subtype (1 byte) and Version (1 byte) into the same 
    // iVersion (2 bytes) field of the SIe221FilterTable struct
    // and in such a way that the OUI Subtype is in the lower memory
    // address
    //
    TUint16 version = static_cast<TUint16>(KWmmParamElemVersion) << 8;
    version |= static_cast<TUint16>(KWmmParamElemOuiSubtype);
    WriteHtoUint16( &(filterTableEntry->iVersion), version );
    
    mibLength += sizeof( WHA::SIe221FilterTable ); 
    mib->iNumofElems = 2;   // at this point

    // =================================================================
    // track HT Operation element for change, if relevant
    // =================================================================
    //
    if ( aCtxImpl.HtSupportedByNw() )
        {
        filter_table = reinterpret_cast<WHA::SDefaultIeFilterTable*>(
            filterTableEntry + 1);
    
        filter_table->iIe = E802Dot11HtOperationIE;
        filter_table->iTreatMeant = WHA::SDefaultIeFilterTable::KTrackChange;

        mibLength += sizeof( WHA::SDefaultIeFilterTable );
        ++(mib->iNumofElems);
        }

    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();

    // align length of MIB to 4-byte boundary
    mibLength = Align4( mibLength );
        
    wsa_cmd.Set( 
        aCtxImpl, 
        WHA::KMibBeaconFilterIeTable, 
        mibLength,
        mib );        
   
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );     
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureModeInit::EnableUserData( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11InfrastructureModeInit::EnableUserData"));

    TBool stateChange( EFalse );
    aCtxImpl.iEnableUserData = ETrue;
    OnOidComplete( aCtxImpl );
    aCtxImpl.iUmac.UserDataReEnabled();
    
    if ( iState == EWAIT4USERDATAENABLE )
        {
        // we shall continue travesal
        stateChange = ETrue;
        Fsm( aCtxImpl, EUSERDATAENABLE );
        }

    // signal global state transition event
    return stateChange;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureModeInit::RealScan(
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

    // marker for Exit() method -> see it to believe it
    iFlags |= KScanning;    

    aCtxImpl.iStates.iInfrastructureScanningMode.Set( 
        aMode, aSSID, aScanRate, aChannels,
        aMinChannelTime, aMaxChannelTime, aSplitScan,
        // if we have not yet set the BSS parameters, we need to use 
        // foreground scan instead of background scan (which we normally 
        // use in infrastructure mode). This is to avoid a situation where the
        // WLAN vendor sw would need to send a PS Poll frame, when the AID is
        // not set yet
        iFlags & KBssParamsSet ? WHA::EForcedBgScan : WHA::EFgScan );

    // mark dot11 state backtrack
    aCtxImpl.iStates.iInfrastructureScanningMode.Dot11StateBackTrack();

    ChangeState( aCtxImpl, 
        *this,                                          // prev state
        aCtxImpl.iStates.iInfrastructureScanningMode    // next state
        );                      

    return ETrue; // global statemachine transition will occur 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::ContinueDot11StateTraversal( 
    WlanContextImpl& aCtxImpl )
    {
    // Start Voice over WLAN Call maintenance
    aCtxImpl.StartVoiceOverWlanCallMaintenance();
        
    // start to keep the WLAN connection alive
    aCtxImpl.StartKeepAlive();    

    if ( aCtxImpl.CurrentDot11PwrMgmtMode() != 
         aCtxImpl.DesiredDot11PwrMgmtMode() )
        {
        // dot11 power management mode change is needed 
        
        // initialize the counter which we use to determine if we should 
        // indicate BSS lost if we continuously are unsuccessful in changing the
        // power mgmt mode
        aCtxImpl.InitializeSetPsModeCount();
        
        ChangeState( aCtxImpl,
            *this,
            aCtxImpl.iStates.iPwrMgmtTransitionMode );
        }
    else
        {
        // no power management mode change is needed 
        ChangeState( aCtxImpl,
            *this,
            aCtxImpl.iStates.iInfrastructureNormalMode );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureModeInit::CompleteConnectRequest( 
    WlanContextImpl& aCtxImpl, 
    TInt aCompletionCode ) const
    {
    OnOidComplete( aCtxImpl, aCompletionCode );
    }
