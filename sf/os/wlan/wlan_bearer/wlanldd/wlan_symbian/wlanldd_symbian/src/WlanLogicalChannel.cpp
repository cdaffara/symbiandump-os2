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
* Description:   Implementation of the DWlanLogicalChannel class.
*
*/

/*
* %version: 61 %
*/

#include "WlLddWlanLddConfig.h"
#include "wlanlddcommon.h"
#include "WlanLogicalChannel.h"
#include "EthernetFrameMemMngr.h"
#include "EtherCardIoc.h"
#include "Umac.h"
#include "wllddoidmsgstorage.h"
#include "WlanLogicalDevice.h"
#include "osachunk.h"

#ifdef RD_WLAN_DDK
#include "osa.h"
#include "wlanpddiface.h"
#endif

extern TAny* os_alloc( const TUint32 );
extern void os_free( const TAny* );

// Tx memory for frames created by UMAC
const TUint32 KDmaTxMemSize = 4096; // equals to 1 MMU page on most systems

// ---------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// ---------------------------------------------------------------------------
//
#ifndef RD_WLAN_DDK
DWlanLogicalChannel::DWlanLogicalChannel( 
    DWlanLogicalDevice& aParent, 
    Umac& aUmac, 
    TDynamicDfcQue& aDfcQ,
    DMutex& aMutex, 
    DChunk*& aSharedMemoryChunk,
    WlanChunk*& aRxFrameMemoryPool )
#else    
DWlanLogicalChannel::DWlanLogicalChannel( 
    DWlanLogicalDevice& aParent, 
    Umac& aUmac, 
    TDynamicDfcQue& aDfcQ,
    WlanOsa*& aOsa, 
    DChunk*& aSharedMemoryChunk,
    WlanChunk*& aRxFrameMemoryPool )
#endif    
    : 
        iClient( NULL ), 
        iUnit( KUnitMax ), 
        iUmac( aUmac ),
        iDfcQ ( &aDfcQ ),
        iParent( aParent ),
        iWlanGeneralRequestStatus( NULL ), 
		iWlanRequestNotifyStatus( NULL ),
        iWlanReceiveFrameStatus( NULL ), 
        iWlanSendFrameStatus( NULL ),
        iIndicationBuffer( NULL ), 
		iIndicationListHead( NULL ), 
        iFreeIndicationListHead( NULL ), 
		iResumeTxStatus( NULL ), 
        iEthernetReceiveFrameStatus( NULL ), 
		iEthernetFrameMemMngr( NULL ),
        iTimerDfc( TimerDfcDoToggle, this, 0 ),
        iTimer( OnTimeOut, this ),
        iVoiceCallEntryTimer( OnVoiceCallEntryTimerTimeOut, this ),
        iVoiceCallEntryTimerDfc( VoiceCallEntryTimerDfcDoToggle, this, 0 ),
        iNullTimer( OnNullTimerTimeOut, this ),
        iNullTimerDfc( NullTimerDfcDoToggle, this, 0 ),
        iNoVoiceTimer( OnNoVoiceTimerTimeOut, this ),
        iNoVoiceTimerDfc( NoVoiceTimerDfcDoToggle, this, 0 ),
        iKeepAliveTimer( OnKeepAliveTimerTimeOut, this ),
        iKeepAliveTimerDfc( KeepAliveTimerDfcDoToggle, this, 0 ),
        iActiveToLightPsTimer( OnActiveToLightPsTimerTimeOut, this ),
        iActiveToLightPsTimerDfc( ActiveToLightPsTimerDfcDoToggle, this, 0 ),
        iLightPsToActiveTimer( OnLightPsToActiveTimerTimeOut, this ),
        iLightPsToActiveTimerDfc( LightPsToActiveTimerDfcDoToggle, this, 0 ),
        iLightPsToDeepPsTimer( OnLightPsToDeepPsTimerTimeOut, this ),
        iLightPsToDeepPsTimerDfc( LightPsToDeepPsTimerDfcDoToggle, this, 0 ),
        iDfc( DfcDoToggle, this, 0 ),
        iTxTriggerDfc( TxTriggerDfcDoToggle, this, 0 ),
        iFlags( 0 ), 
        iPowerHandler( *this, aDfcQ ), 
        iPoweringDown( EFalse ), 
        iDfcCtx( NULL ), 
#ifndef RD_WLAN_DDK       
        iMutex( aMutex  ),
#else        
        iOsa( aOsa  ),
#endif
        iSharedMemoryChunk( aSharedMemoryChunk ),
        iRxFrameMemoryPool( aRxFrameMemoryPool ),
        iTxActive( EFalse ),
        iAddTxFrameAllowed( ETrue )
    {
    TraceDump(INIT_LEVEL, (("WLANLDD: DWlanLogicalChannel Ctor: 0x%08x"), this));

	iTimerDfc.SetDfcQ( iDfcQ );
	iVoiceCallEntryTimerDfc.SetDfcQ( iDfcQ );
	iNullTimerDfc.SetDfcQ( iDfcQ );
	iNoVoiceTimerDfc.SetDfcQ( iDfcQ );
	iKeepAliveTimerDfc.SetDfcQ( iDfcQ );
	iActiveToLightPsTimerDfc.SetDfcQ( iDfcQ );
	iLightPsToActiveTimerDfc.SetDfcQ( iDfcQ );
	iLightPsToDeepPsTimerDfc.SetDfcQ( iDfcQ );
	iDfc.SetDfcQ( iDfcQ );
	iTxTriggerDfc.SetDfcQ( iDfcQ );

    // store client thread ID
    iClient = &Kern::CurrentThread();

    TraceDump(INIT_LEVEL, (("WLANLDD: current thread 0x%08x"), iClient));

    // open a refernce to client thread, so we don't get out of scope
    ((DObject*)iClient)->Open();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
DWlanLogicalChannel::~DWlanLogicalChannel()
    {
    TraceDump(INIT_LEVEL, (("WLANLDD: DWlanLogicalChannel Dtor: 0x%08x"), this));

    if ( iUnit == KUnitWlan )
        {
        iTimerDfc.Cancel();
        iVoiceCallEntryTimerDfc.Cancel();
        iNullTimerDfc.Cancel();
        iNoVoiceTimerDfc.Cancel();
        iKeepAliveTimerDfc.Cancel();
        iActiveToLightPsTimerDfc.Cancel(); 
        iLightPsToActiveTimerDfc.Cancel(); 
        iLightPsToDeepPsTimerDfc.Cancel(); 
        iDfc.Cancel();

        // if there are any indication items currently pending, free them by
        // moving them to the free list
        PurgeIndicationList();
        // delete all indication items from the free list
        FreeIndicationList();
        // detach management side callback
        TraceDump(INIT_LEVEL, ("WLANLDD: detach managemement side callback from UMAC"));
        iUmac.Detach();

        if ( iFlags & KPowerHandlerRegistered )
            {
            // we have registered our power handler
            
            // if we stopping operations because of a power down request from power
            // manager, acknowledge back to power manager that we are now done
            if ( iPoweringDown )
                {
                TraceDump(INIT_LEVEL, 
                    (("WLANLDD: DWlanLogicalChannel::~DWlanLogicalChannel(): signalling powerDownDone")));
                iPowerHandler.OnPowerDownDone();        
                }

            // de-register our power handler from the power manager
            TraceDump(INIT_LEVEL, 
                (("WLANLDD: DWlanLogicalChannel::~DWlanLogicalChannel(): de-register our powerhandler")));
            iPowerHandler.Remove();
            
            // destroy UMAC internal Tx memory buffer
            iDmaTxMemory.Finit();            
            }
        }
    else if ( iUnit == KUnitEthernet )
        {
        iTxTriggerDfc.Cancel();
        }

    if ( iEthernetFrameMemMngr )
        {
        TraceDump(INIT_LEVEL, ("WLANLDD: deallocate ethernet frame memory pool"));
        
        // normally the user side client will do this, but just to be on 
        // the safe side, make sure it really is done before proceeding with
        // frame memory mgr destruction
        OnReleaseEthernetFrameBuffers();
        
        TraceDump(MEMORY, (("WLANLDD: delete DEthernetFrameMemMngr: 0x%08x"), 
        reinterpret_cast<TUint32>(iEthernetFrameMemMngr)));        
    
        delete iEthernetFrameMemMngr;
        iEthernetFrameMemMngr = NULL;
        }

    iDfcQ = NULL; // destroyed elsewhere (by the owner)
    
    // close our refernce to client thread as the very last thing we do
    Kern::SafeClose( reinterpret_cast<DObject*&>(iClient), NULL);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
DChunk*& DWlanLogicalChannel::SharedMemoryChunk()
    {
    return iSharedMemoryChunk;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::SetRxBufAlignmentPadding( 
    TInt aRxBufAlignmentPadding )
    {
    iParent.SetRxBufAlignmentPadding( aRxBufAlignmentPadding );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalChannel::RxBufAlignmentPadding() const
    {
    return iParent.RxBufAlignmentPadding();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool DWlanLogicalChannel::InitIndicationListEntries()
    {
    // Initialize indication list entries
    TBool ret( ETrue );     // true for success     

    for ( TUint32 i = 0 ; i < KMaxIndicationListEntries; ++i )
        {
        TIndicationListEntry* tmp = new TIndicationListEntry;

        if ( tmp )
            {
            TraceDump(MEMORY, (("WLANLDD: new TIndicationListEntry: 0x%08x"), 
            reinterpret_cast<TUint32>(tmp)));
            
            // The ReleaseIndicationListEntry 
            // function can also be used for initialization
            ReleaseIndicationListEntry( tmp );
            }
        else
            {
            // alloc failed no use to continue
            ret = EFalse;
            break;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
//
// If an error occurs in this method, we set iPdd to NULL to prevent 
// PDD object destruction in base class (DLogicalChannelBase) destructor.
// DLogicalChannelBase destructor gets called as this logical channel instance
// gets destoyed when this method returns with an error.
// The PDD object gets destructed elsewhere.
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalChannel::DoCreate(
    TInt aUnit, 
    const TDesC8* /*aInfo*/, 
    const TVersion& /*aVer*/ )
    {
    TraceDump(INIT_LEVEL, ("WLANLDD: DWlanLogicalChannel::DoCreate"));
    TraceDump(INIT_LEVEL, 
        (("WLANLDD: current thread 0x%08x"), &Kern::CurrentThread()));
    TraceDump(INIT_LEVEL, (("WLANLDD: aUnit %d"), aUnit));

    if ( !Kern::CurrentThreadHasCapability( 
             ECapabilityCommDD,
             __PLATSEC_DIAGNOSTIC_STRING( "Checked by wlan.ldd")))
        {
        iPdd = NULL;
        return KErrPermissionDenied;
        }

    // enable reception of requests from user mode
    SetDfcQ(iDfcQ);
    iMsgQ.Receive();

    // length of the allocation unit to be used for OSA interconnect 
    // (DMA capable) memory and frame Rx/Tx buffers. 
    // If WLAN DDK is used and the WLAN PDD capabilities query below 
    // returns a non-NULL response and the PDD indicates support for
    // cached memory, we use the length of the processor's cache line 
    // - returned in that response - as the allocation unit length.
    // Otherwise we use this default value
    TInt allocationUnit ( 32 );
    
    iUnit = aUnit;  // store the unit

    // PDD has been loaded and created by the framework at this point

    if ( iUnit == KUnitWlan )
        {
        if ( !InitIndicationListEntries() )
            {
            iPdd = NULL;
            return KErrNoMemory;
            }

        // attach management side callback
        TraceDump(INIT_LEVEL, ("WLANLDD: attach managemement side callback to UMAC"));
        iUmac.Attach( *this );

#ifndef RD_WLAN_DDK
        WHA::Wha* wha( reinterpret_cast<WHA::Wha*>(iPdd) );
        // attach the WHA object to UMAC
        TraceDump(INIT_LEVEL, ("WLANLDD: attach WHA object to UMAC"));
        iUmac.AttachWsa( wha );
#else

        MWlanPddIface* pdd( reinterpret_cast<MWlanPddIface*>(iPdd) );

        TraceDump(INIT_LEVEL, 
            (("WLANLDD: DWlanLogicalChannel::DoCreate: wlanpdd addr: 0x%08x"), 
            pdd));
        
        // determine PDD capabilities
            
        MWlanPddIface::SCapabilities* pddCapabilities ( NULL );

        pdd->GetCapabilities( pddCapabilities );

        TraceDump(INIT_LEVEL, 
            (("WLANLDD: DWlanLogicalChannel::DoCreate: pddCapabilities ptr: 0x%08x"), 
            reinterpret_cast<TUint32>(pddCapabilities)));

        if ( pddCapabilities )
            {
            TraceDump(INIT_LEVEL, 
                (("WLANLDD: DWlanLogicalChannel::DoCreate: wlanpdd iCapabilities: 0x%08x"), 
                pddCapabilities->iCapabilities));
            
            if ( pddCapabilities->iCapabilities & 
                 MWlanPddIface::SCapabilities::KCachedMemory )
                {
                TraceDump(INIT_LEVEL, 
                    ("WLANLDD: DWlanLogicalChannel::DoCreate: use cached memory"));

                iParent.UseCachedMemory( ETrue );
                    
                TraceDump(INIT_LEVEL, 
                    (("WLANLDD: DWlanLogicalChannel::DoCreate: wlanpdd iCacheLineLength: %d"), 
                    pddCapabilities->iCacheLineLength));

                allocationUnit = pddCapabilities->iCacheLineLength; 
                }
            delete pddCapabilities;
            pddCapabilities = NULL;
            }
        
        // initialize OSA
        if ( !iOsa->Initialize( 
                iParent.UseCachedMemory(), 
                allocationUnit,
                iDfcQ ) )
            {
            // init failed. We cannot continue
            
            TraceDump(ERROR_LEVEL, 
                ("WLANLDD: DWlanLogicalChannel::DoCreate: osa init failed, abort"));

            iPdd = NULL;
            return KErrGeneral;
            }
        
        TraceDump(INIT_LEVEL, 
            ("WLANLDD: DWlanLogicalChannel::DoCreate: attach osa and osaext to wlanpdd"));
        TraceDump(INIT_LEVEL, 
            (("WLANLDD: DWlanLogicalChannel::DoCreate: osa and osaext addr: 0x%08x"), 
            reinterpret_cast<TUint32>(iOsa)));

        // pass interfaces to PDD
        const TBool ret( pdd->Attach( *iOsa, *iOsa ) );
        if ( ret )
            {
            TraceDump(INIT_LEVEL, 
                ("WLANLDD: DWlanLogicalChannel::DoCreate: extract WHA and attach it to UMAC"));
            // extract wha and attach it to UMAC
            iUmac.AttachWsa( &(pdd->Extract()) );
            }
        else
            {
            // attach failure abort
            TraceDump(WARNING_LEVEL, ("WLANLDD: PDD attach failure -> abort"));
            iPdd = NULL;
            return KErrNoMemory;
            }
#endif
        
#ifndef RD_WLAN_DDK
        // pass the mutex to WHA
        wha->Mutex( reinterpret_cast<WHA::TMutexHandle>(&iMutex) );            
#endif

        // register our power handler with the power manager
        TraceDump(INIT_LEVEL, 
            (("WLANLDD: DWlanLogicalChannel::DoCreate: register our powerhandler")));
        iPowerHandler.Add();
        iFlags |= KPowerHandlerRegistered; // note that we have registered
        
        // create UMAC internal Tx memory buffer
        if ( iDmaTxMemory.Init( KDmaTxMemSize, iParent.UseCachedMemory() ) 
             == KDmaTxMemSize )
            {
            // success; no further action needed
            }
        else
            {
            // failure
            iPdd = NULL;
            return KErrNoMemory;
            }
        }
    else if ( iUnit == KUnitEthernet )
        {
        // nothing for now
        }
    else
        {
        // unknown unit
        
        TraceDump(ERROR_LEVEL, 
            (("WLANLDD: DWlanLogicalChannel::DoCreate: ERROR: unknown unit: %d"), 
            iUnit));
        iPdd = NULL;
        return KErrPermissionDenied;
        }

    // init ethernet frame memory pool manager
    iEthernetFrameMemMngr = DEthernetFrameMemMngr::Init( 
        aUnit, 
        *this, 
        iRxFrameMemoryPool,
        iParent.UseCachedMemory(),
        allocationUnit );

    if ( iEthernetFrameMemMngr )
        {
        // init success -> continue
        TraceDump(INIT_LEVEL, (("WLANLDD: ethernet memory pool allocated: 0x%08x"),
            iEthernetFrameMemMngr));
        }
    else
        {
        // init failure no use to continue
        TraceDump(WARNING_LEVEL, 
            ("WLANLDD: ethernet memory pool allocation failure"));

        iPdd = NULL;
        return KErrNoMemory;
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::HandleMsg(TMessageBase* aMsg)
    {
    TraceDump(WLM_CMD, ("WLANLDD: DWlanLogicalChannel::HandleMsg"));

    // we are now executing in a DFC thread context
    // acquire mutex
#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif
    
    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::HandleMsg: mutex acquired")));

    TThreadMessage& m = *reinterpret_cast<TThreadMessage*>( aMsg );

    TraceDump(WLM_CMD_DETAILS, (("WLANLDD: current thread 0x%08x"), 
        &Kern::CurrentThread()));       
    TraceDump(WLM_CMD_DETAILS, (("WLANLDD: client thread 0x%08x"), m.Client()));    
    TraceDump(WLM_CMD_DETAILS, (("WLANLDD: channel creator thread 0x%08x"), 
        iClient));

    // Get message type
    TInt id = m.iValue;

    TraceDump(WLM_CMD_DETAILS, (("WLANLDD: message id: %d"), id));

    // Decode the message type and dispatch it to the relevent handler function...
    
    // A logical channel can be closed either explicitly by its user-side client,
    // or implicitly if the client thread dies. In the latter case, the channel
    // is closed in the context of the kernel supervisor thread. 
    if ( id == ECloseMsg )
        {
        TraceDump(WLM_CMD_DETAILS, ("WLANLDD: close message"));

        if( m.Client() == iClient )
            {
            // channel closed by client

            // trick to disable PDD object dealloc
            // as this logical channel gets destoyed after this
            // and iPdd is deallocated in DLogicalChannelBase
            iPdd = NULL;    
            }

        // release mutex
        // Note that in this case we release the mutex before completing the
        // client message. Reason: When SMP is used, doing these actions in
        // the reverse order could, in principle, lead to a situation where
        // the channel instance has already been destroyed (by another thread)
        // when the mutex is tried to be released.

#ifndef RD_WLAN_DDK
        Kern::MutexSignal( iMutex );
#else
        iOsa->MutexRelease();
#endif    
        
        TraceDump(MUTEX, 
            (("WLANLDD: DWlanLogicalChannel::HandleMsg: mutex released")));
        
        m.Complete( KErrNone, EFalse );
        
        return;
        }

    // For all other message types, we check that the message is from the thread
    // that created us.
    if ( m.Client() != iClient )
        {
        TraceDump(SERIOUS_LEVEL, (("WLANLDD: ERROR: requesting thread: 0x%08x"), 
            m.Client()));
        TraceDump(SERIOUS_LEVEL, ("WLANLDD: ERROR: request from wrong thread"));

        m.Complete( KErrPermissionDenied, ETrue );
        }
    else if ( id == KMaxTInt )
        {
        TraceDump(WLM_CMD_DETAILS, ("WLANLDD: DoCancel"));

        // DoCancel
        DoCancel( m.Int0() );
        m.Complete( KErrNone,ETrue );
        }
    else if ( id < 0 )
        {
        TraceDump(WLM_CMD_DETAILS, ("WLANLDD: DoRequest"));

        // DoRequest
        TRequestStatus* req_status = reinterpret_cast<TRequestStatus*>( m.Ptr0() );
        const TInt r = DoRequest( ~id, req_status, m.Ptr1(), m.Ptr2() );
        if ( r != KErrNone)
            {
            // this branch means that an error of somekind has occurred
            // we complete the request synchronously
            Kern::RequestComplete( m.Client(), req_status, r );
            }

        m.Complete( KErrNone, ETrue );
        }
    else
        {
        TraceDump(WLM_CMD_DETAILS, ("WLANLDD: DoControl"));

        // DoControl
        const TInt r = DoControl( id, m.Ptr0(), m.Ptr1() );
        m.Complete( r, ETrue );
        }

    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::HandleMsg: mutex released")));
    }
    

// ---------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// ---------------------------------------------------------------------------
//
TAny* DWlanLogicalChannel::DoControlFast( TInt aFunction, TAny* aParam )
    {
    TAny* ret( NULL );
    TBool triggerTx ( EFalse );    
    
    TraceDump(WLM_CMD_DETAILS, 
        (("WLANLDD: DWlanLogicalChannel::DoControlFast: current thread 0x%08x"), 
        &Kern::CurrentThread()));       
    TraceDump(WLM_CMD_DETAILS, (("WLANLDD: channel creator thread: 0x%08x"), 
        iClient));
    TraceDump(WLM_CMD_DETAILS, (("WLANLDD: function: 0x%x"), aFunction));
    
    // acquire mutex
    // Enter critical section before requesting the mutex as
    // we are executing in the context of a user mode thread
    NKern::ThreadEnterCS();    
#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif
    
    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::DoControlFast: mutex acquired")));
    
    if ( iUnit == KUnitWlan )
        {
        ret = OnMgmtSideControlFast( aFunction, aParam );
        }
    else if ( iUnit == KUnitEthernet )
        {
        ret = OnEthernetSideControlFast( aFunction, aParam, triggerTx );
        }
    else
        {
        // unknown unit
#ifndef NDEBUG            
        TraceDump(ERROR_LEVEL, 
            ("WLANLDD: DWlanLogicalChannel::DoControlFast: unknown unit"));
        TraceDump(ERROR_LEVEL, (("WLANLDD: aFunction: %d"), aFunction));
        TraceDump(ERROR_LEVEL, (("WLANLDD: unit: %d"), iUnit));
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#endif
        }
    
    // release mutex
#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif 
    // and exit from critical section
    NKern::ThreadLeaveCS();
    
    TraceDump(MUTEX, 
        ("WLANLDD: DWlanLogicalChannel::DoControlFast: mutex released"));

    if ( triggerTx )
        {
        // Trigger a new Tx - via a DFC. 
        // Note that we do this outside of mutex protection, as this block of 
        // code is executed by a lower priority (user side) thread, and the 
        // DFC by a higher priority (kernel side) thread, and the latter will 
        // also need to acquire the same mutex before it can proceed. So, as 
        // the lower priority thread will get paused and the higher priority 
        // thread will get scheduled (to execute the DFC), we don't want the 
        // higher priority thread to need to wait for the mutex. So we 
        // released the mutex first in this code block and after that enqueue 
        // the DFC request.
        if ( !( iFlags & KTxTriggerArmed ) )
            {
            iFlags |= KTxTriggerArmed;
            iTxTriggerDfc.Enque();
            }
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// ---------------------------------------------------------------------------
//
TAny* DWlanLogicalChannel::OnMgmtSideControlFast( 
    TInt aFunction, 
    TAny* aParam )
    {
    TAny* ret( NULL );
    
    switch ( aFunction )
        {
        // ==========================================================
        // Get Rx frame
        // ==========================================================        
        case RWlanLogicalChannel::EWlanControlFastGetRxFrame:
            if ( iEthernetFrameMemMngr )
                {
                ret = iEthernetFrameMemMngr->GetRxFrame( 
                        reinterpret_cast<TDataBuffer*>(aParam) );
                }
            break;
        // ==========================================================
        // Unknown request
        // ==========================================================        
        default:
#ifndef NDEBUG
            TraceDump(ERROR_LEVEL, (("WLANLDD: unknown request: %d"), 
                aFunction));
            os_assert( 
                (TUint8*)("WLANLDD: panic"), 
                (TUint8*)(WLAN_FILE), 
                __LINE__ );
#endif
            break;
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// ---------------------------------------------------------------------------
//
TAny* DWlanLogicalChannel::OnEthernetSideControlFast( 
    TInt aFunction, 
    TAny* aParam,
    TBool& aTriggerTx )
    {
    TAny* ret( NULL );    
    aTriggerTx = EFalse;
    
    switch ( aFunction )
        {
        // ==========================================================
        // Alloc Tx buffer
        // ==========================================================        
        case RPcmNetCardIf::EControlFastAllocTxBuffer:
            if ( iEthernetFrameMemMngr )
                {
                ret = iEthernetFrameMemMngr->AllocTxBuffer(
                    reinterpret_cast<TUint>(aParam) );
                }
            
            if ( !ret && iAddTxFrameAllowed )
                {
                iAddTxFrameAllowed = EFalse;
                
                TraceDump( NWSA_TX, 
                    ("WLANLDD: DWlanLogicalChannel::OnEthernetSideControlFast: "
                     "stop flow from protocol stack") );        
                }
            break;            
        // ==========================================================
        // Add Tx frame
        // ==========================================================        
        case RPcmNetCardIf::EControlFastAddTxFrame:
            {
#ifndef NDEBUG
            if ( !iAddTxFrameAllowed )
                {
                TraceDump(ERROR_LEVEL, 
                    ("WLANLDD: DWlanLogicalChannel::OnEthernetSideControlFast: "
                     "WARNING: AddTxFrame req. when flow ctrl is on"));
                }
#endif
            if ( iEthernetFrameMemMngr && aParam )
                {
                if ( iEthernetFrameMemMngr->AllTxQueuesEmpty() )
                    {
                    aTriggerTx = ETrue;
                    }
                
                TDataBuffer* discardFrame ( NULL );
                
                ret = reinterpret_cast<TAny*>(
                          iEthernetFrameMemMngr->AddTxFrame( 
                              reinterpret_cast<TDataBuffer*>(aParam),
                              discardFrame,
                              iUmac.UserDataTxEnabled() ));
                
                if ( discardFrame )
                    {
                    TraceDump( NWSA_TX_DETAILS, 
                        (("WLANLDD: DWlanLogicalChannel::OnEthernetSideControlFast: "
                          "have to drop tx frame of UP: %d"),
                        reinterpret_cast<TDataBuffer*>(
                            aParam)->UserPriority()) );
                    
                    iEthernetFrameMemMngr->FreeTxPacket( discardFrame );
                    aTriggerTx = EFalse;
                    }
                }
                
            if ( !ret )
                {
                iAddTxFrameAllowed = EFalse;

                TraceDump( NWSA_TX, 
                    ("WLANLDD: DWlanLogicalChannel::OnEthernetSideControlFast: "
                     "stop flow from protocol stack") );
                }
            break;
            }            
        // ==========================================================
        // Get Rx frame
        // ==========================================================        
        case RPcmNetCardIf::EControlFastGetRxFrame:
            {
            if ( iEthernetFrameMemMngr )
                {
                ret = iEthernetFrameMemMngr->GetRxFrame( 
                        reinterpret_cast<TDataBuffer*>(aParam) );
                }
            }
            break;
        // ==========================================================
        // Unknown request
        // ==========================================================        
        default:
#ifndef NDEBUG
            TraceDump(ERROR_LEVEL, (("WLANLDD: unknown request: %d"), 
                aFunction));
            os_assert( 
                (TUint8*)("WLANLDD: panic"), 
                (TUint8*)(WLAN_FILE), 
                __LINE__ );            
#endif
            break;
        } // switch
    
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::DoCancel( TInt aMask )
    {
    if ( iUnit == KUnitWlan )
        {
        if ( aMask & ( 1 << RWlanLogicalChannel::EWlanRequestNotify ) )
            {
            TraceDump(INFO_LEVEL, 
                ("WLANLDD: DWlanLogicalChannel::DoCancel: mgmt side notify cancel"));

            CancelIndicationRequest();
            Kern::RequestComplete( 
                iClient, iWlanRequestNotifyStatus, KErrServerTerminated );
            iWlanRequestNotifyStatus = NULL;
            }        
        else if ( aMask & ( 1 << RWlanLogicalChannel::EWlanRequestFrame ) )
            {
            TraceDump(INFO_LEVEL, 
                ("WLANLDD: DWlanLogicalChannel::DoCancel: mgmt side frame read cancel"));

            Kern::RequestComplete( 
                iClient, iWlanReceiveFrameStatus, KErrServerTerminated );
            iWlanReceiveFrameStatus = NULL;
            }
        else
            {
            TraceDump(ERROR_LEVEL, 
                (("WLANLDD: DWlanLogicalChannel::DoCancel: mgmt side "
                  "unhandled mask panic: 0x%08x"), 
                aMask));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            }        
        }
    else if ( iUnit == KUnitEthernet )
        {
        if ( aMask & ( 1 << RPcmNetCardIf::EControlResumeTxCancel ) )
            {
            TraceDump(INFO_LEVEL, 
                ("WLANLDD: DWlanLogicalChannel::DoCancel: user side resume Tx cancel"));

            Kern::RequestComplete( 
                iClient, iResumeTxStatus, KErrServerTerminated );
            iResumeTxStatus = NULL;
            }        
        else if ( aMask & ( 1 << RPcmNetCardIf::EControlReadCancel ) )
            {
            TraceDump(INFO_LEVEL, 
                ("WLANLDD: DWlanLogicalChannel::DoCancel: user side frame read cancel"));

            Kern::RequestComplete( 
                iClient, iEthernetReceiveFrameStatus, KErrServerTerminated );
            iEthernetReceiveFrameStatus = NULL;
            }
        else
            {
            TraceDump(ERROR_LEVEL, 
                (("WLANLDD: DWlanLogicalChannel::DoCancel: user side "
                  "unhandled mask panic: 0x%08x"), 
                aMask));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            }        
        }
    else
        {
        TraceDump(ERROR_LEVEL, (("WLANLDD: DoCancel unknown unit panic: %d"), 
            iUnit));
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool DWlanLogicalChannel::ProtocolStackDataReceiveComplete( 
    TDataBuffer*& aBufferStart,
    TUint32 aNumOfBuffers )
    {
    if ( iEthernetFrameMemMngr && 
         iEthernetFrameMemMngr->OnEthernetFrameRxComplete( 
             aBufferStart, 
             aNumOfBuffers ) )
        {
        Kern::RequestComplete( iClient, iEthernetReceiveFrameStatus, KErrNone );
        iEthernetReceiveFrameStatus = NULL;
        }
        
    return ETrue; // indicate always successful processing
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool DWlanLogicalChannel::ProtocolStackSideClientReady() const
    {
    TBool status ( EFalse );
    
    if ( iEthernetFrameMemMngr && iEthernetFrameMemMngr->IsMemInUse() )
        {
        status = ETrue;
        }
    
    return status;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::UserDataReEnabled()
    {
    if ( ProtocolStackSideClientReady() &&
         !( iFlags & KTxTriggerArmed ) )
        {
        iFlags |= KTxTriggerArmed;
        iTxTriggerDfc.Enque();
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint8* DWlanLogicalChannel::DmaPrivateTxMemory()
    {
    return reinterpret_cast<TUint8*>(iDmaTxMemory.Addr());
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::SetProtocolStackTxOffset( 
    TUint32 aEthernetFrameTxOffset )
    {
    const TUint32 KNotRelevant ( 0 );
    
    if ( iEthernetFrameMemMngr )
        {
        TraceDump(NWSA_TX_DETAILS, 
            (("WLANLDD: DWlanLogicalChannel::SetProtocolStackTxOffset: aEthernetFrameTxOffset: %d"),
            aEthernetFrameTxOffset ));

        iEthernetFrameMemMngr->SetTxOffsets( 
            aEthernetFrameTxOffset,
            KNotRelevant,
            KNotRelevant );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnTxProtocolStackDataComplete( 
    TInt aCompletionCode,
    TDataBuffer* aMetaHeader )
    {
    TraceDump(UMAC_PROTO_CALLBACK, 
        ("WLANLDD: DWlanLogicalChannel::OnTxProtocolStackDataComplete"));
    TraceDump(UMAC_PROTO_CALLBACK, 
        (("WLANLDD: aCompletionCode: %d"), aCompletionCode));

    if ( iEthernetFrameMemMngr )
        {
        iEthernetFrameMemMngr->FreeTxPacket( aMetaHeader );
        }

    TxProtocolStackData();
    
    if ( !iAddTxFrameAllowed )
        {
        if ( iResumeTxStatus &&
             iEthernetFrameMemMngr &&
             iEthernetFrameMemMngr->ResumeClientTx( 
                 iUmac.UserDataTxEnabled() ) )
            {
            // resume Tx flow from protocol stack
            
            TraceDump(NWSA_TX_DETAILS, 
                ("WLANLDD: DWlanLogicalChannel::OnTxProtocolStackDataComplete: complete client Tx resume req"));        
    
            // complete the pending request to user mode
            Kern::RequestComplete( 
                iClient, iResumeTxStatus, aCompletionCode );
            // mark request as non-pending
            iResumeTxStatus = NULL;
            
            iAddTxFrameAllowed = ETrue;
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnOtherTxDataComplete()
    {
    TraceDump(UMAC_PROTO_CALLBACK, 
        ("WLANLDD: DWlanLogicalChannel::OnOtherTxDataComplete"));
    
    // We try to submit Protocol Stack side frames also when this event occurs
    // a) to speed up new Tx submission in the case that a frame submitted by
    // someone else than the Protocol Stack Side Client unblocks the WHA Tx
    // pipeline and
    // b) to avoid the Tx pipe from getting stuck even temporarily
    if ( ProtocolStackSideClientReady() &&
         !( iFlags & KTxTriggerArmed ) )
        {
        iFlags |= KTxTriggerArmed;
        iTxTriggerDfc.Enque(); // we do this via a DFC
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnTxDataSent()
    {
    TraceDump(UMAC_PROTO_CALLBACK, 
        ("WLANLDD: DWlanLogicalChannel::OnTxDataSent"));

    // We try to submit Protocol Stack side frames also when this event occurs
    // a) to speed up new Tx submission in the case that a frame, submitted by
    // someone else than the Protocol Stack Side Client, gets transmitted &
    // makes a WHA Txqueue - for which there are packets pending in our Tx 
    // queues - non-full
    // b) to avoid the Tx pipe from getting stuck even temporarily
    if ( ProtocolStackSideClientReady() &&
         !( iFlags & KTxTriggerArmed ) )
        {
        iFlags |= KTxTriggerArmed;
        iTxTriggerDfc.Enque(); // we do this via a DFC
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::TxManagementData()
    {
    TDataBuffer* buffer( NULL );
    
    if ( iEthernetFrameMemMngr )
        {
        buffer = iEthernetFrameMemMngr->OnWriteEthernetFrame();
        }

    if ( buffer )
        {
        iUmac.WriteMgmtFrame( *buffer );
        }
    else
        {
#ifndef NDEBUG
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#endif
        Kern::RequestComplete( 
            iClient, iWlanSendFrameStatus, KErrGeneral );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::TxProtocolStackData()
    {
#ifndef NDEBUG
    TUint packetsSubmitted ( 0 );
#endif
    
    if ( !iTxActive )
        {
        TWhaTxQueueState txQueueState;
        TBool morePackets ( EFalse );
        iTxActive = ETrue;
        
        while ( iEthernetFrameMemMngr && iUmac.TxPermitted( txQueueState ) )
            {
            TDataBuffer* metaHeader = 
                iEthernetFrameMemMngr->GetTxFrame( txQueueState, morePackets );
            
            if ( metaHeader )
                {
                TraceDump( NWSA_TX_DETAILS, 
                    (("WLANLDD: DWlanLogicalChannel::TxProtocolStackData: submitting packet %d"),
                    ++packetsSubmitted) );

                iUmac.TxProtocolStackData( 
                    *metaHeader,
                    morePackets );
                }
            else
                {
                TraceDump(NWSA_TX_DETAILS, 
                    ("WLANLDD: DWlanLogicalChannel::TxProtocolStackData: no packet that could be submitted"));
                
                break;
                }
            }
        
        iTxActive = EFalse;
        }
    
    TraceDump( NWSA_TX_DETAILS, 
        (("WLANLDD: DWlanLogicalChannel::TxProtocolStackData: %d packet(s) submitted"),
        packetsSubmitted) );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool DWlanLogicalChannel::OnReadEthernetFrameRequest()
    {
    TBool ret( EFalse );
    
    if ( iEthernetFrameMemMngr )
        {
        ret = iEthernetFrameMemMngr->OnReadRequest();
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalChannel::OnEthernetSideRequest( 
    TInt aReqNo, 
    TRequestStatus* aStatus, 
    TAny* /*a1*/, 
    TAny* /*a2*/ )
    {
    const TInt ret( KErrNone );

    switch (aReqNo)
        {
        case RPcmNetCardIf::ERequestRead:
            // Read request
            if ( OnReadEthernetFrameRequest() )
                {
                // rx data to be completed exists
                // complete it directly
                Kern::RequestComplete( 
                    iClient, 
                    aStatus, 
                    KErrNone );
                }
            else
                {
                // no rx data for completion exists
                // store the pending request
                iEthernetReceiveFrameStatus = aStatus;
                }
            break;
        case RPcmNetCardIf::EResumeTx:
            // request to get notified once Tx can again be resumed
            iResumeTxStatus = aStatus;
            break;
        default:
            // unknown request
            
            TraceDump(ERROR_LEVEL, 
                ("WLANLDD: unknown protocol stack request"));
            TraceDump(ERROR_LEVEL, (("WLANLDD: request id: %d"), aReqNo));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::FinitSystem()
    {
    TraceDump(INIT_LEVEL, ("WLANLDD * finit system"));
    iUmac.FinitSystem();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::InitSystem( 	
    TAny* aInputBuffer,
	TUint aInputLength )
    {    
    // read the USER mode parameters to internal storage
    TInt ret = Kern::ThreadRawRead( 
        iClient, 
        aInputBuffer,
        &iOpenParam,
        aInputLength );
    if ( ret != KErrNone )
        {
        TraceDump(ERROR_LEVEL, 
            ("WLANLDD: DWlanLogicalChannel::InitSystem(): ThreadRawRead panic"));
        TraceDump(ERROR_LEVEL, (("WLANLDD: ret: %d"), ret));
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    // backup pointers
    const TAny* pda( iOpenParam.iPda );
    const TAny* fw( iOpenParam.iFirmWare );

    // allocate kernel memory for the pda and firmware
    iOpenParam.iPda = os_alloc( iOpenParam.iPdaLength );
    iOpenParam.iFirmWare = os_alloc( iOpenParam.iFirmWareLength );
    
    if ( iOpenParam.iPda && iOpenParam.iFirmWare )
        {
        // allocation success
        // read the parameters to allocated storage
        ret = Kern::ThreadRawRead( 
            iClient, 
            pda,
            iOpenParam.iPda,
            iOpenParam.iPdaLength );
        if ( ret != KErrNone )
            {
            TraceDump(ERROR_LEVEL, 
                ("WLANLDD: DWlanLogicalChannel::InitSystem(): ThreadRawRead panic"));
            TraceDump(ERROR_LEVEL, (("WLANLDD: ret: %d"), ret));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            }
        ret = Kern::ThreadRawRead( 
            iClient, 
            fw,
            iOpenParam.iFirmWare,
            iOpenParam.iFirmWareLength );
        if ( ret != KErrNone )
            {
            TraceDump(ERROR_LEVEL, 
                ("WLANLDD: DWlanLogicalChannel::InitSystem(): ThreadRawRead panic"));
            TraceDump(ERROR_LEVEL, (("WLANLDD: ret: %d"), ret));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            }
        
        // check this in completion method
        iFlags |= KFreeOpenParamsMask;

        iUmac.BootUp( 
            static_cast<TUint8*>(iOpenParam.iPda), 
            iOpenParam.iPdaLength, 
            static_cast<TUint8*>(iOpenParam.iFirmWare), 
            iOpenParam.iFirmWareLength );
        }
    else
        {
        // allocation failure
        // make sure all is released
        os_free( iOpenParam.iPda );
        os_free( iOpenParam.iFirmWare );
        
        TraceDump(ERROR_LEVEL, 
            ("WLANLDD: DWlanLogicalChannel::InitSystem: ERROR: PDA and/or firmware memory allocation failure"));

        // complete the pending request with error code to user mode
        Kern::RequestComplete( 
            iClient, 
            iWlanGeneralRequestStatus, 
            KErrNoMemory );
        // mark request as non pending
        iWlanGeneralRequestStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool DWlanLogicalChannel::HandlePointerParameters()
    {
    TraceDump(WLM_CMD_DETAILS, 
        (("WLANLDD: DWlanLogicalChannel::HandlePointerParameters")));

    if ( !HandleScanResponseFrameBodyCase() )
        {
        // memory allocation failure
        
        return EFalse;        
        }

    if ( !CheckAndHandleIeDataCase() )
        {
        // memory allocation failure

        // free memory allocated in the previous step
        FreeScanResponseFramebody();
        
        return EFalse;        
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool DWlanLogicalChannel::HandleScanResponseFrameBodyCase()
    {
    TBool ret( ETrue );

    // handle scan response frame body case
    if ( (reinterpret_cast<const TOIDHeader*>
        (&iOidMsgStorage))->oid_id == E802_11_CONNECT )
        {
        if ( iOidMsgStorage.iOidMsgs.iConnectMsg.scanResponseFrameBody )
            {
            // scan response frame body exists
            // we must allocate kernel storage for it here

            TraceDump(WLM_CMD_DETAILS, 
                (("WLANLDD: DWlanLogicalChannel::HandleScanResponseFrameBodyCase: allocate memory & copy data; length: %d"),
                iOidMsgStorage.iOidMsgs
                    .iConnectMsg.scanResponseFrameBodyLength ));

            const TUint8* user_mode_address( 
                iOidMsgStorage.iOidMsgs.iConnectMsg.scanResponseFrameBody );

            iOidMsgStorage.iOidMsgs.iConnectMsg.scanResponseFrameBody 
                = static_cast<TUint8*>(os_alloc( 
                    iOidMsgStorage.iOidMsgs
                        .iConnectMsg.scanResponseFrameBodyLength ));
                
            if ( iOidMsgStorage.iOidMsgs.iConnectMsg.scanResponseFrameBody )
                {
                // allocation success
                // read the parameters to the allocated storage
                const TInt local( Kern::ThreadRawRead( 
                    iClient, 
                    user_mode_address,
                    const_cast<TUint8*>(iOidMsgStorage.iOidMsgs
                        .iConnectMsg.scanResponseFrameBody),
                    iOidMsgStorage.iOidMsgs
                        .iConnectMsg.scanResponseFrameBodyLength ) );
                    
                if ( local != KErrNone )
                    {
                    TraceDump(ERROR_LEVEL, 
                        ("WLANLDD: ThreadRawRead panic"));
                    TraceDump(ERROR_LEVEL, (("WLANLDD: local: %d"), ret));
                    os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
                    }

                // mark memory deallocation event
                iFlags |= KFreeScanResponseFramebodyMask;
                }
            else
                {
                // allocation failure
                ret = EFalse;
                }
            }
        else    // --- scan response frame body does not exist ---
            {
            // this is a usage error
            
            TraceDump(ERROR_LEVEL, 
                ("WLANLDD: scan response frame body is null in connect OID"));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            }
        }
    else    // --- connect not issued ---
        {
        // do nothing
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool DWlanLogicalChannel::CheckAndHandleIeDataCase()
    {
    TBool ret( ETrue );

    // handle Tx IE case
    if ( (reinterpret_cast<const TOIDHeader*>
        (&iOidMsgStorage))->oid_id == E802_11_CONNECT )
        {
        if ( iOidMsgStorage.iOidMsgs.iConnectMsg.ieData )
            {
            // Tx IE exists
            // we must allocate kernel storage for it here

            TraceDump(WLM_CMD_DETAILS, 
                (("WLANLDD: DWlanLogicalChannel::CheckAndHandleIeDataCase: allocate memory & copy data; length: %d"),
                iOidMsgStorage.iOidMsgs.iConnectMsg.ieDataLength ));

            const TUint8* user_mode_address( 
                iOidMsgStorage.iOidMsgs.iConnectMsg.ieData );

            iOidMsgStorage.iOidMsgs.iConnectMsg.ieData 
                = static_cast<TUint8*>(os_alloc( 
                    iOidMsgStorage.iOidMsgs.iConnectMsg.ieDataLength ));
                    
            if ( iOidMsgStorage.iOidMsgs.iConnectMsg.ieData )
                {
                // allocation success
                // read the parameters to the allocated storage
                const TInt local( Kern::ThreadRawRead( 
                    iClient, 
                    user_mode_address,
                    const_cast<TUint8*>(iOidMsgStorage.iOidMsgs
                        .iConnectMsg.ieData),
                    iOidMsgStorage.iOidMsgs
                        .iConnectMsg.ieDataLength ) );
                    
                if ( local != KErrNone )
                    {
                    TraceDump(ERROR_LEVEL, 
                        ("WLANLDD: ThreadRawRead panic"));
                    TraceDump(ERROR_LEVEL, (("WLANLDD: local: %d"), ret));
                    os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
                    }

                // mark memory deallocation event
                iFlags |= KFreeIeDataMask;
                }
            else
                {
                // allocation failure
                ret = EFalse;
                }
            }
        else    // --- RSN IE does not exists ---
            {
            // do nothing
            }
        }
    else    // --- connect not issued ---
        {
        // do nothing
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::FreeScanResponseFramebody()
    {
    if ( iFlags & KFreeScanResponseFramebodyMask )
        {
        // free scan response frame

        TraceDump(WLM_CMD_DETAILS, 
            (("WLANLDD: DWlanLogicalChannel::FreeScanResponseFramebody: freeing memory")));
        
        os_free( iOidMsgStorage.iOidMsgs.iConnectMsg.scanResponseFrameBody );
        iOidMsgStorage.iOidMsgs.iConnectMsg.scanResponseFrameBody = NULL;
        iFlags &= ~KFreeScanResponseFramebodyMask;
        }    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::FreeIeData()
    {    
    if ( iFlags & KFreeIeDataMask )
        {
        // free Tx IE

        TraceDump(WLM_CMD_DETAILS, 
            (("WLANLDD: DWlanLogicalChannel::FreeIeData: freeing memory")));
        
        os_free( iOidMsgStorage.iOidMsgs.iConnectMsg.ieData ); 
        iOidMsgStorage.iOidMsgs.iConnectMsg.ieData = NULL;
        iFlags &= ~KFreeIeDataMask;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalChannel::OnMgmtSideDoRequest( 
    TInt aReqNo, 
    TRequestStatus* aStatus, 
    TAny* a1, 
    TAny* a2 )
    {
    TraceDump(WLM_CMD, 
        (("WLANLDD: DWlanLogicalChannel::OnMgmtSideDoRequest(): request: %d"), 
        aReqNo));
    TraceDump(WLM_CMD_DETAILS, (("WLANLDD: a1 0x%08x"), a1));
    TraceDump(WLM_CMD_DETAILS, (("WLANLDD: a2 0x%08x"), a2));

    TInt ret( KErrNone );

    SOutputBuffer output_buffer = { NULL, 0 };
    if ( a2 )
        {
        // read the USER mode output parameters to internal storage
        ret = Kern::ThreadRawRead( 
            iClient, 
            a2,
            &output_buffer,
            sizeof(output_buffer) );
        if ( ret != KErrNone )
            {
            TraceDump(ERROR_LEVEL, 
                ("WLANLDD: OnMgmtSideDoRequest(): ThreadRawRead panic"));
            TraceDump(ERROR_LEVEL, (("WLANLDD: ret: %d"), ret));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            }
        }

    switch ( aReqNo )
        {
        case RWlanLogicalChannel::EWlanInitSystem:
            // bootup the chip and the system
            iWlanGeneralRequestStatus = aStatus; 
            InitSystem( a1, sizeof(TOpenParam) );
            break;
        case RWlanLogicalChannel::EWlanFinitSystem:
            // power down the chip and the system
            iWlanGeneralRequestStatus = aStatus; 
            FinitSystem();
            break;
        case RWlanLogicalChannel::EWlanCommand:
            // management command
            iWlanGeneralRequestStatus = aStatus; 

            // read the USER mode parameters to internal storage
            ret = Kern::ThreadRawRead( 
                iClient, 
                a1,
                &iOidMsgStorage,
                sizeof(iOidMsgStorage) );
            if ( ret != KErrNone )
                {
                TraceDump(ERROR_LEVEL, 
                    ("WLANLDD: DWlanLogicalChannel::OnMgmtSideDoRequest(): ThreadRawRead panic"));
                TraceDump(ERROR_LEVEL, (("WLANLDD: ret: %d"), ret));
                os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
                }

            // read also parameters pointed to by pointers from USER mode to 
            // internal storage; when such parameters exist
            if ( !HandlePointerParameters() )
                {
                // memory allocation failure
                // complete the pending request with error code to user mode
                Kern::RequestComplete( 
                    iClient, 
                    iWlanGeneralRequestStatus, 
                    KErrNoMemory );
                // mark request as non pending
                iWlanGeneralRequestStatus = NULL;
                break;
                }

            iUmac.HandleOid( 
                reinterpret_cast<const TOIDHeader*>(&iOidMsgStorage), 
                // we can pass output buffer pointer as is
                // because it is not accessed by UMAC and the 
                // actual user mode writing is done by OnOidCompleted method
                output_buffer.iData, 
                output_buffer.iLen );
            break;
        case RWlanLogicalChannel::EWlanRequestNotify:
            // store the USER mode indication address;
            iIndicationBuffer = static_cast<TIndication*>(a1);
            iWlanRequestNotifyStatus = aStatus;
            IndicationRequest( static_cast<TIndication*>(a1) );
            break;
        case RWlanLogicalChannel::EWlanRequestFrame:
            if ( OnReadEthernetFrameRequest() )
                {
                // rx data to be completed exists
                // complete it directly
                Kern::RequestComplete( 
                    iClient, 
                    aStatus, 
                    KErrNone );
                }
            else
                {
                // no rx data for completion exists
                // store the pending request
                iWlanReceiveFrameStatus = aStatus;
                }
            break;
        case RWlanLogicalChannel::EWlanRequestSend:
            iWlanSendFrameStatus = aStatus;

            TxManagementData();
            break;
        default:
            // unknown request
            TraceDump(ERROR_LEVEL, 
                ("WLANLDD: unhandled management request panic"));
            TraceDump(ERROR_LEVEL, (("WLANLDD: request id: %d"), aReqNo));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalChannel::DoRequest( 
    TInt aReqNo, 
    TRequestStatus* aStatus, 
    TAny* a1, 
    TAny* a2 )
    {
    TInt ret( KErrNone );
    if ( iUnit == KUnitWlan )
        {
        ret = OnMgmtSideDoRequest( aReqNo, aStatus, a1, a2 );
        }
    else if ( iUnit == KUnitEthernet )
        {
        ret = OnEthernetSideRequest( aReqNo, aStatus, a1, a2 );
        }
    else
        {
        // unknown unit
        
        TraceDump(ERROR_LEVEL, 
            ("WLANLDD: DWlanLogicalChannel::DoRequest(): unknown request unit panic"));
        TraceDump(ERROR_LEVEL, (("WLANLDD: request id: %d"), aReqNo));
        TraceDump(ERROR_LEVEL, (("WLANLDD: unit: %d"), iUnit));
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalChannel::OnInitialiseEthernetFrameBuffers( 
    TSharedChunkInfo* aSharedChunkInfo )
    {
    TInt ret( KErrNoMemory );

    if ( iEthernetFrameMemMngr )
        {
        TUint8 notRelevant( 0 );
        TUint8 vendorTxHdrLen ( 0 );
        TUint8 vendorTxTrailerLen ( 0 );
        
        iUmac.GetFrameExtraSpaceForVendor( 
            notRelevant,
            vendorTxHdrLen,
            vendorTxTrailerLen );
        
        ret = iEthernetFrameMemMngr->OnInitialiseMemory( 
            *iClient, 
            aSharedChunkInfo,
            vendorTxHdrLen,
            vendorTxTrailerLen );
        
        if ( ( iUnit == KUnitEthernet ) && 
             ( ret == KErrNone ) )
            {
            // attach protocol stack side callback
            TraceDump(INIT_LEVEL, 
                ("WLANLDD: attach protocol stack side callback to UMAC"));
            iUmac.AttachProtocolStackSideUmacCb( *this );
    
            // set frame Tx offset for protocol stack side 
            iUmac.SetTxOffset();
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnReleaseEthernetFrameBuffers()
    {
    if ( iEthernetFrameMemMngr )
        {
        iEthernetFrameMemMngr->OnReleaseMemory( *iClient );    
        }

    if ( iUnit == KUnitEthernet )
        {
        // detach protocol stack side callback
        TraceDump(INIT_LEVEL, 
            ("WLANLDD: detach protocol stack side callback from UMAC"));
        iUmac.DetachProtocolStackSideUmacCb();
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalChannel::OnEthernetSideControl( 
    TInt aFunction, 
    TAny* a1, 
    TAny* /*a2*/ )
    {
    TInt ret(KErrNone);

    switch (aFunction)
        {
        case RPcmNetCardIf::EControlGetConfig:
            {
            const TMacAddress& mac = iUmac.StationId();            
            TEtherConfig ethercfg = { NULL };
	
            // 0x00 - card is ready, 0xff - card not ready.
            ethercfg.iStatus			= 0;                
            // Speed setting (0x00 to 0x03).
            ethercfg.iEthSpeed 		= 0;				
            // Duplex setting (0x00 to 0x03)
            ethercfg.iEthDuplex		= 0;				

            // set STA MAC
            os_memcpy( ethercfg.iEthAddress, &mac, KMacAddressLength );

            // write config to USER mode
            ret = Kern::ThreadRawWrite( 
                iClient, 
                a1, 
                &ethercfg,
                sizeof(ethercfg) );
            if ( ret != KErrNone )
                {
                TraceDump(ERROR_LEVEL, ("WLANLDD: ThreadRawWrite panic"));
                TraceDump(ERROR_LEVEL, (("WLANLDD: ret: %d"), ret));
                os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
                }
            break;
            }
        case RPcmNetCardIf::ESvControlInitBuffers:
            // instruct initiliaze buffers for data xfer
            if ( a1 )
                {
                // call
                TraceDump(INIT_LEVEL, 
                    ("WLANLDD * init protocol shared IO-buffer memory pool"));
                
                TraceDump(INIT_LEVEL, 
                    (("WLANLDD: DWlanLogicalChannel::OnEthernetSideControl: chunk info addr: 0x%08x"), 
                    a1));
                ret = OnInitialiseEthernetFrameBuffers( 
                    static_cast<TSharedChunkInfo*>(a1) );
                }
            else
                {
                TraceDump(ERROR_LEVEL, 
                    ("WLANLDD: DWlanLogicalChannel::OnEthernetSideControl(): init protocol stack buffer pool panic"));
                os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
                }
            break;
        case RPcmNetCardIf::ESvControlFreeBuffers:
            {
            // instruct free buffers for data xfer
            OnReleaseEthernetFrameBuffers();
            break;
            }
        default:
            // unhadled panic
            TraceDump(ERROR_LEVEL, 
                ("WLANLDD: DWlanLogicalChannel::OnEthernetSideControl(): protocol stack side control panic"));
            TraceDump(ERROR_LEVEL, (("function id: %d"), aFunction));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalChannel::OnMgmtSideControl( 
    TInt aFunction, 
    TAny* a1, 
    TAny* /*a2*/ )
    {
    TInt ret( KErrNone );
    if ( aFunction == RWlanLogicalChannel::EWlanSvControlInitBuffers )
        {
        // initiliaze buffers for wlan mgmt client data xfer
        if ( a1 )
            {
            TraceDump(INIT_LEVEL, 
                ("WLANLDD: DWlanLogicalChannel::OnMgmtSideControl(): init management client shared IO-buffer memory pool"));
            
            TraceDump(INIT_LEVEL, 
                (("WLANLDD: DWlanLogicalChannel::OnMgmtSideControl: chunk info addr: 0x%08x"), 
                a1));
            ret = OnInitialiseEthernetFrameBuffers( 
                static_cast<TSharedChunkInfo*>(a1) );
            }
        else
            {            
            TraceDump(ERROR_LEVEL, 
                ("WLANLDD: DWlanLogicalChannel::OnMgmtSideControl(): init management client buffer pool panic"));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            }
        }
    else if ( aFunction == RWlanLogicalChannel::EWlanSvControlFreeBuffers )
        {
        // free wlan mgmt client data xfer buffers
        OnReleaseEthernetFrameBuffers();        
        }
    else
        {
        // unknown request
        
        TraceDump(ERROR_LEVEL, 
            ("WLANLDD: DWlanLogicalChannel::OnMgmtSideControl(): management side control panic"));
        TraceDump(ERROR_LEVEL, (("WLANLDD: function id: %d"), aFunction));
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalChannel::DoControl( TInt aFunction, TAny* a1, TAny* a2)
    {
    TInt ret( KErrNone );
    if ( iUnit == KUnitWlan )
        {
        ret = OnMgmtSideControl( aFunction, a1, a2 );
        }
    else if ( iUnit == KUnitEthernet )
        {
        ret = OnEthernetSideControl( aFunction, a1, a2 );
        }
    else
        {
        // unknown unit
        
        TraceDump(ERROR_LEVEL, ("WLANLDD: unhandled control unit panic"));
        TraceDump(ERROR_LEVEL, (("WLANLDD: function id: %d"), aFunction));
        TraceDump(ERROR_LEVEL, (("WLANLDD: unit: %d"), iUnit));
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnOidCompleted( 
    TInt aReason,
    SOidOutputData& OidOutputData )
    {
    if ( iFlags & KFreeOpenParamsMask )
        {
        // free open params
        os_free( iOpenParam.iPda );
        iOpenParam.iPda = NULL;
        os_free( iOpenParam.iFirmWare );
        iOpenParam.iFirmWare = NULL;
        iFlags &= ~KFreeOpenParamsMask;
        }
    else
        { 
        // check if there are other command parameter related memory blocks
        // to be released
        // Note! We check the flags already here to avoid unnecessary
        // FreeXXX function calls
        
        if ( iFlags & KFreeScanResponseFramebodyMask )
            {
            // free scan response frame body
            FreeScanResponseFramebody();
            
            // the following flags can be set only at the same time with
            // KFreeScanResponseFramebodyMask
            
            if ( iFlags & KFreeIeDataMask )
                {
                // free IE(s)
                FreeIeData();
                }
            }
        }

    TraceDump(UMAC_MGMT_CALLBACK, 
        ("WLANLDD: DWlanLogicalChannel::OnOidCompleted()"));
    TraceDump(UMAC_MGMT_CALLBACK, (("WLANLDD: OID: 0x%08x"), 
        OidOutputData.iOidId));
    TraceDump(UMAC_MGMT_CALLBACK, (("WLANLDD: aReason: %d"), aReason));

    if ( iWlanGeneralRequestStatus )
        {
        // write data to user mode if this happened to be a read request
        if ( OidOutputData.iOidData )
            {
            TraceDump(UMAC_MGMT_CALLBACK, ("WLANLDD: write data to USER mode"));
            TraceDump(UMAC_MGMT_CALLBACK, (("WLANLDD: length: %d"), 
                OidOutputData.iLengthOfDataInBytes));

            const TInt ret 
                = Kern::ThreadRawWrite( 
                iClient, 
                OidOutputData.iBufferSupplied, 
                OidOutputData.iOidData,
                OidOutputData.iLengthOfDataInBytes );

            if ( ret != KErrNone )
                {
                TraceDump(ERROR_LEVEL, ("WLANLDD: ThreadRawWrite panic"));
                TraceDump(ERROR_LEVEL, (("WLANLDD: ret: %d"), ret));
                os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
                }
            }

        // complete the pending request to user mode
        Kern::RequestComplete( iClient, iWlanGeneralRequestStatus, aReason );
        // mark request as non pending
        iWlanGeneralRequestStatus = NULL;
        }
    else    // iReqStatus
        {
        // no pending request exists, so it must have been cancelled
        // Nothing more to do here
        TraceDump(UMAC_MGMT_CALLBACK, 
            ("WLANLDD: DWlanLogicalChannel::OnOidCompleted(): no pending request"));
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::SetMgmtSideTxOffsets( 
    TUint32 aEthernetFrameTxOffset,
    TUint32 aDot11FrameTxOffset,
    TUint32 aSnapFrameTxOffset )
    {
    if ( iEthernetFrameMemMngr )
        {
        TraceDump(NWSA_TX_DETAILS, 
            (("WLANLDD: DWlanLogicalChannel::SetMgmtSideTxOffsets: "
              "aEthernetFrameTxOffset: %d"),
            aEthernetFrameTxOffset ));
        TraceDump(NWSA_TX_DETAILS, 
            (("WLANLDD: DWlanLogicalChannel::SetMgmtSideTxOffsets: "
              "aDot11FrameTxOffset: %d"),
            aDot11FrameTxOffset ));
        TraceDump(NWSA_TX_DETAILS, 
            (("WLANLDD: DWlanLogicalChannel::SetMgmtSideTxOffsets: "
              "aSnapFrameTxOffset: %d"),
            aSnapFrameTxOffset ));

        iEthernetFrameMemMngr->SetTxOffsets( 
            aEthernetFrameTxOffset, 
            aDot11FrameTxOffset,
            aSnapFrameTxOffset );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint8* DWlanLogicalChannel::GetBufferForRxData( 
    TUint aLengthinBytes )
    {
    TUint8* ret ( NULL );
    if ( iEthernetFrameMemMngr )
        {
        ret = iEthernetFrameMemMngr->OnGetEthernetFrameRxBuffer( 
            aLengthinBytes );
        }
    else
        {
         TraceDump(RX_FRAME | WARNING_LEVEL, 
            ("WLANLDD: DWlanLogicalChannel::GetBufferForRxData: WARNING: no frame mgr => failed"));       
        }

    return ret;
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::MgmtDataReceiveComplete( 
    TDataBuffer*& aBufferStart, 
    TUint32 aNumOfBuffers )
    {
    if ( iEthernetFrameMemMngr && 
         ( iEthernetFrameMemMngr->OnEthernetFrameRxComplete( 
               aBufferStart, 
               aNumOfBuffers ) ) )
        {
        Kern::RequestComplete( iClient, iWlanReceiveFrameStatus, KErrNone );
        iWlanReceiveFrameStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::MgmtPathWriteComplete( TInt aErr )
    {
    TraceDump(UMAC_MGMT_CALLBACK, 
        ("WLANLDD: DWlanLogicalChannel::MgmtPathWriteComplete()"));
    TraceDump(UMAC_MGMT_CALLBACK, 
        (("WLANLDD: aCompletionCode: %d"), aErr));

    if ( iWlanSendFrameStatus )
        {
        // complete the pending request to user mode
        Kern::RequestComplete( 
            iClient, iWlanSendFrameStatus, aErr );
        // mark request as non pending
        iWlanSendFrameStatus = NULL;
        }
    else    // iReqStatus
        {
        // no pending request exists, so it must have been cancelled
        // Nothing to do here
        TraceDump(UMAC_MGMT_CALLBACK, 
            ("WLANLDD: DWlanLogicalChannel::MgmtPathWriteComplete(): no pending request"));
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
DWlanLogicalChannel::TIndicationListEntry* 
DWlanLogicalChannel::GetFreeIndicationListEntry()
    {
    TIndicationListEntry* newEntry = NULL;

    if ( iFreeIndicationListHead )
        {
        newEntry = iFreeIndicationListHead;
        iFreeIndicationListHead = newEntry->next;
        newEntry->next = NULL;
        }

    return newEntry;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::StoreIndication( TIndication aIndication )
    {
    TIndicationListEntry* newEntry = GetFreeIndicationListEntry();

    if ( !newEntry )
        {
        return;
        }

    newEntry->indication = aIndication;
    newEntry->next = NULL;

    if ( !iIndicationListHead )
        {
        iIndicationListHead = newEntry;
        }
    else
        {
        TIndicationListEntry *tmp = iIndicationListHead;

        while ( tmp->next )
            {
            tmp = tmp->next;
            }

        tmp->next = newEntry;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::IndicationComplete()
    {
    Kern::RequestComplete( 
        iClient, iWlanRequestNotifyStatus, KErrNone );
    iWlanRequestNotifyStatus = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool DWlanLogicalChannel::GetStoredIndication()
    {
    TIndicationListEntry *tmp = NULL;

    if ( !iIndicationListHead )
        {
        return EFalse;
        }

    TraceDump(WLM_INDICATION, 
        (("WLANLDD: DWlanLogicalChannel::GetStoredIndication(): indication complete: %d"), 
        iIndicationListHead->indication));

    const TInt ret = Kern::ThreadRawWrite( iClient, iIndicationBuffer, 
        &( iIndicationListHead->indication ), 
        sizeof(iIndicationListHead->indication) );
    
    if ( ret != KErrNone )
        {
        TraceDump(ERROR_LEVEL, ("WLANLDD: ThreadRawWrite panic"));
        TraceDump(ERROR_LEVEL, (("WLANLDD: ret: %d"), ret));
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    tmp = iIndicationListHead;
    iIndicationListHead = tmp->next;

    ReleaseIndicationListEntry( tmp );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::TryToCompleteIndicationRequest()
    {
    if ( iIndicationBuffer )
        {
        if ( GetStoredIndication() )
            {
            IndicationComplete();
            iIndicationBuffer = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnInDicationEvent( TIndication aIndication )
    {
    StoreIndication( aIndication );
    TryToCompleteIndicationRequest();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::FreeIndicationList()
    {
    // Free the indication list entries
    TIndicationListEntry *iter = iFreeIndicationListHead;
    TIndicationListEntry *next = NULL;

    while ( iter )
        {
        next = iter->next;

        TraceDump(MEMORY, (("WLANLDD: delete TIndicationListEntry: 0x%08x"), 
        reinterpret_cast<TUint32>(iter)));        

        delete iter;

        iter = next;
        }

    iFreeIndicationListHead = NULL;
    iIndicationListHead = NULL;
    iIndicationBuffer = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::CancelIndicationRequest()
    {
    iIndicationBuffer = NULL;
    PurgeIndicationList();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::PurgeIndicationList()
    {
    TIndicationListEntry* iter = iIndicationListHead;
    TIndicationListEntry* next = NULL;

    while ( iter )
        {
        next = iter->next;

        ReleaseIndicationListEntry( iter );

        iter = next;
        }

    iIndicationListHead = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::IndicationRequest( TIndication* aBuffer )
    {
    iIndicationBuffer = aBuffer;
    TryToCompleteIndicationRequest();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::ReleaseIndicationListEntry(
    TIndicationListEntry* aEntry )
    {
    if ( aEntry )
        {
        aEntry->next = NULL;
    
        if ( !iFreeIndicationListHead )
            {
            iFreeIndicationListHead = aEntry;
            }
        else
            {
            TIndicationListEntry* tmp = iFreeIndicationListHead;
    
            while ( tmp->next )
                {
                tmp = tmp->next;
                }
    
            tmp->next = aEntry;
            }
        }
#ifndef NDEBUG
    else
        {
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );        
        }
#endif        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::MarkRxBufFree( TUint8* aBufferToFree )
    {
    if ( iEthernetFrameMemMngr )
        {
        iEthernetFrameMemMngr->DoMarkRxBufFree( aBufferToFree );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::RealTimerDfc()
    {
    // acquire mutex

#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealTimerDfc(): mutex acquired")));

    iUmac.OnTimeout( EWlanDefaultTimer );

    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealTimerDfc(): mutex released")));    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::TimerDfcDoToggle( TAny* aPtr )
    {
    DWlanLogicalChannel* ptr = static_cast<DWlanLogicalChannel*>(aPtr);
    ptr->RealTimerDfc();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::RealDfc()
    {
    // acquire mutex

#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealDfc: mutex acquired")));

    if ( !(iFlags & KDfcCancelledMask) )
        {
        iUmac.OnDfc( iDfcCtx );
        }

    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealDfc: mutex released")));    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::DfcDoToggle( TAny* aPtr )
    {
    DWlanLogicalChannel* ptr( static_cast<DWlanLogicalChannel*>(aPtr) );
    ptr->RealDfc();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnTimeOut( TAny *aPtr )
    {
    DWlanLogicalChannel* p = static_cast<DWlanLogicalChannel*>(aPtr);

    // queue to a DFC for completion
    p->iTimerDfc.Add();
    }
   
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::RealVoiceCallEntryTimerDfc()
    {
    // acquire mutex

#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealVoiceCallEntryTimerDfc(): mutex acquired")));

    iUmac.OnTimeout( EWlanVoiceCallEntryTimer );

    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealVoiceCallEntryTimerDfc(): mutex released")));    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::VoiceCallEntryTimerDfcDoToggle( TAny* aPtr )
    {
    DWlanLogicalChannel* ptr = static_cast<DWlanLogicalChannel*>(aPtr);
    ptr->RealVoiceCallEntryTimerDfc();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnVoiceCallEntryTimerTimeOut( TAny *aPtr )
    {
    DWlanLogicalChannel* p = static_cast<DWlanLogicalChannel*>(aPtr);

    // queue to a DFC for completion
    p->iVoiceCallEntryTimerDfc.Add();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::RealNullTimerDfc()
    {
    // acquire mutex

#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealNullTimerDfc(): mutex acquired")));

    iUmac.OnTimeout( EWlanNullTimer );

    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealNullTimerDfc(): mutex released")));    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::NullTimerDfcDoToggle( TAny* aPtr )
    {
    DWlanLogicalChannel* ptr = static_cast<DWlanLogicalChannel*>(aPtr);
    ptr->RealNullTimerDfc();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnNullTimerTimeOut( TAny *aPtr )
    {
    DWlanLogicalChannel* p = static_cast<DWlanLogicalChannel*>(aPtr);

    // queue to a DFC for completion
    p->iNullTimerDfc.Add();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::RealNoVoiceTimerDfc()
    {
    // acquire mutex

#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealNoVoiceTimerDfc: mutex acquired")));

    iUmac.OnTimeout( EWlanNoVoiceTimer );

    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealNoVoiceTimerDfc: mutex released")));    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::NoVoiceTimerDfcDoToggle( TAny* aPtr )
    {
    DWlanLogicalChannel* ptr = static_cast<DWlanLogicalChannel*>(aPtr);
    ptr->RealNoVoiceTimerDfc();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnNoVoiceTimerTimeOut( TAny *aPtr )
    {
    DWlanLogicalChannel* p = static_cast<DWlanLogicalChannel*>(aPtr);

    // queue to a DFC for completion
    p->iNoVoiceTimerDfc.Add();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::RealKeepAliveTimerDfc()
    {
    // acquire mutex

#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealKeepAliveTimerDfc: mutex acquired")));

    iUmac.OnTimeout( EWlanKeepAliveTimer );

    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealKeepAliveTimerDfc: mutex released")));    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::KeepAliveTimerDfcDoToggle( TAny* aPtr )
    {
    DWlanLogicalChannel* ptr = static_cast<DWlanLogicalChannel*>(aPtr);
    ptr->RealKeepAliveTimerDfc();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnKeepAliveTimerTimeOut( TAny *aPtr )
    {
    DWlanLogicalChannel* p = static_cast<DWlanLogicalChannel*>(aPtr);

    // queue to a DFC for completion
    p->iKeepAliveTimerDfc.Add();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::RealActiveToLightPsTimerDfc()
    {
    // acquire mutex

#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealActiveToLightPsTimerDfc: mutex acquired")));

    iUmac.OnTimeout( EWlanActiveToLightPsTimer );

    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealActiveToLightPsTimerDfc: mutex released")));    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::ActiveToLightPsTimerDfcDoToggle( TAny* aPtr )
    {
    DWlanLogicalChannel* ptr = static_cast<DWlanLogicalChannel*>(aPtr);
    ptr->RealActiveToLightPsTimerDfc();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnActiveToLightPsTimerTimeOut( TAny *aPtr )
    {
    DWlanLogicalChannel* p = static_cast<DWlanLogicalChannel*>(aPtr);

    // queue to a DFC for completion
    p->iActiveToLightPsTimerDfc.Add();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::RealLightPsToActiveTimerDfc()
    {
    // acquire mutex

#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealLightPsToActiveTimerDfc: mutex acquired")));

    iUmac.OnTimeout( EWlanLightPsToActiveTimer );

    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealLightPsToActiveTimerDfc: mutex released")));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::LightPsToActiveTimerDfcDoToggle( TAny* aPtr )
    {
    DWlanLogicalChannel* ptr = static_cast<DWlanLogicalChannel*>(aPtr);
    ptr->RealLightPsToActiveTimerDfc();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnLightPsToActiveTimerTimeOut( TAny *aPtr )
    {
    DWlanLogicalChannel* p = static_cast<DWlanLogicalChannel*>(aPtr);

    // queue to a DFC for completion
    p->iLightPsToActiveTimerDfc.Add();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::RealLightPsToDeepPsTimerDfc()
    {
    // acquire mutex

#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealLightPsToDeepPsTimerDfc: mutex acquired")));

    iUmac.OnTimeout( EWlanLightPsToDeepPsTimer );

    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::RealLightPsToDeepPsTimerDfc: mutex released")));    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::LightPsToDeepPsTimerDfcDoToggle( TAny* aPtr )
    {
    DWlanLogicalChannel* ptr = static_cast<DWlanLogicalChannel*>(aPtr);
    ptr->RealLightPsToDeepPsTimerDfc();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnLightPsToDeepPsTimerTimeOut( TAny *aPtr )
    {
    DWlanLogicalChannel* p = static_cast<DWlanLogicalChannel*>(aPtr);

    // queue to a DFC for completion
    p->iLightPsToDeepPsTimerDfc.Add();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::RegisterTimeout( 
    TUint32 aTimeoutInMicroSeconds,
    TWlanTimer aTimer )
    {
    TraceDump(INFO_LEVEL, 
        (("WLANLDD: DWlanLogicalChannel::RegisterTimeout: timer: %d"),
        aTimer));    
    TraceDump(INFO_LEVEL, 
        (("WLANLDD: DWlanLogicalChannel::RegisterTimeout: aTimeoutInMicroSeconds: %d"),
        aTimeoutInMicroSeconds));    

    const TInt KThousand = 1000;
    
    switch ( aTimer )
        {
        case EWlanDefaultTimer:
            iTimer.OneShot( 
                // convert timeout 1st to milliseconds & then to NKern ticks
                NKern::TimerTicks( aTimeoutInMicroSeconds / KThousand ), 
                // request completion in ISR context
                EFalse );
            break;
        case EWlanVoiceCallEntryTimer:
            iVoiceCallEntryTimer.OneShot( 
                // convert timeout 1st to milliseconds & then to NKern ticks
                NKern::TimerTicks( aTimeoutInMicroSeconds / KThousand ), 
                // request completion in ISR context
                EFalse );
            break;
        case EWlanNullTimer:
            iNullTimer.OneShot( 
                // convert timeout 1st to milliseconds & then to NKern ticks
                NKern::TimerTicks( aTimeoutInMicroSeconds / KThousand ), 
                // request completion in ISR context
                EFalse );
            break;
        case EWlanNoVoiceTimer:
            iNoVoiceTimer.OneShot( 
                // convert timeout 1st to milliseconds & then to NKern ticks
                NKern::TimerTicks( aTimeoutInMicroSeconds / KThousand ), 
                // request completion in ISR context
                EFalse );        
            break;
        case EWlanKeepAliveTimer:
            iKeepAliveTimer.OneShot( 
                // convert timeout 1st to milliseconds & then to NKern ticks
                NKern::TimerTicks( aTimeoutInMicroSeconds / KThousand ), 
                // request completion in ISR context
                EFalse );        
            break;
        case EWlanActiveToLightPsTimer:
            iActiveToLightPsTimer.OneShot( 
                // convert timeout 1st to milliseconds & then to NKern ticks
                NKern::TimerTicks( aTimeoutInMicroSeconds / KThousand ), 
                // request completion in ISR context
                EFalse );        
            break;
        case EWlanLightPsToActiveTimer:
            iLightPsToActiveTimer.OneShot( 
                // convert timeout 1st to milliseconds & then to NKern ticks
                NKern::TimerTicks( aTimeoutInMicroSeconds / KThousand ), 
                // request completion in ISR context
                EFalse );        
            break;
        case EWlanLightPsToDeepPsTimer:
            iLightPsToDeepPsTimer.OneShot( 
                // convert timeout 1st to milliseconds & then to NKern ticks
                NKern::TimerTicks( aTimeoutInMicroSeconds / KThousand ), 
                // request completion in ISR context
                EFalse );        
            break;
        default:
            TraceDump(ERROR_LEVEL, 
                (("WLANLDD: DWlanLogicalChannel::RegisterTimeout: panic, unknown timer: %d"), 
                aTimer));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );            
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::CancelTimeout( TWlanTimer aTimer )
    {
    TraceDump(INFO_LEVEL, 
        (("WLANLDD: DWlanLogicalChannel::CancelTimeout: timer: %d"),
        aTimer));        

    switch ( aTimer )
        {
        case EWlanDefaultTimer:
            iTimer.Cancel();
            iTimerDfc.Cancel();
            break;
        case EWlanVoiceCallEntryTimer:
            iVoiceCallEntryTimer.Cancel();
            iVoiceCallEntryTimerDfc.Cancel();
            break;
        case EWlanNullTimer:
            iNullTimer.Cancel();
            iNullTimerDfc.Cancel();
            break;
        case EWlanNoVoiceTimer:
            iNoVoiceTimer.Cancel();
            iNoVoiceTimerDfc.Cancel();
            break;
        case EWlanKeepAliveTimer:
            iKeepAliveTimer.Cancel();
            iKeepAliveTimerDfc.Cancel();
            break;
        case EWlanActiveToLightPsTimer:
            iActiveToLightPsTimer.Cancel(); 
            iActiveToLightPsTimerDfc.Cancel(); 
            break;
        case EWlanLightPsToActiveTimer:
            iLightPsToActiveTimer.Cancel(); 
            iLightPsToActiveTimerDfc.Cancel(); 
            break;
        case EWlanLightPsToDeepPsTimer:
            iLightPsToDeepPsTimer.Cancel(); 
            iLightPsToDeepPsTimerDfc.Cancel();
            break;
        default:
            TraceDump(ERROR_LEVEL, 
                (("WLANLDD: DWlanLogicalChannel::CancelTimeout: panic, unknown timer: %d"), 
                aTimer));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::RegisterDfc( TAny* aCntx )
    {
    iFlags &= ~KDfcCancelledMask;
    iDfcCtx = aCntx;
    iDfc.Enque();    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::CancelDfc()
    {
    iFlags |= KDfcCancelledMask;
    iDfc.Cancel();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::RealTxTriggerDfc()
    {
    // acquire mutex

#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif

    TraceDump(MUTEX, 
        ("WLANLDD: DWlanLogicalChannel::RealTxTriggerDfc: mutex acquired"));

    iFlags &= ~KTxTriggerArmed;
    TxProtocolStackData();

    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        ("WLANLDD: DWlanLogicalChannel::RealTxTriggerDfc: mutex released"));    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::TxTriggerDfcDoToggle( TAny* aPtr )
    {
    DWlanLogicalChannel* ptr( static_cast<DWlanLogicalChannel*>(aPtr) );
    ptr->RealTxTriggerDfc();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDataBuffer* DWlanLogicalChannel::GetRxFrameMetaHeader()
    {
    TDataBuffer* metaHdr ( NULL );
    if ( iEthernetFrameMemMngr )
        {
        metaHdr = iEthernetFrameMemMngr->GetRxFrameMetaHeader();
        }
    else
        {
         TraceDump(RX_FRAME | WARNING_LEVEL, 
            ("WLANLDD: DWlanLogicalChannel::GetRxFrameMetaHeader: WARNING: no frame mgr => failed"));       
        }

    return metaHdr;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::FreeRxFrameMetaHeader( TDataBuffer* aMetaHeader )
    {
    if ( iEthernetFrameMemMngr )
        {
        iEthernetFrameMemMngr->FreeRxFrameMetaHeader( aMetaHeader );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnPowerUp()
    {
    // acquire mutex

#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::OnPowerUp: mutex acquired")));

    // No real action in this case; so just inform that we are done
    iPowerHandler.OnPowerUpDone(); 

    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::OnPowerUp: mutex released")));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalChannel::OnPowerDown()
    {
    TraceDump(INIT_LEVEL, 
        (("WLANLDD: DWlanLogicalChannel::OnPowerDown")));
        
    iPoweringDown = ETrue;
    
    // acquire mutex

#ifndef RD_WLAN_DDK
    Kern::MutexWait( iMutex );
#else
    iOsa->MutexAcquire();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::OnPowerDown: mutex acquired")));
    
    // Trigger power down indication to wlanengine
    iUmac.OnInDicationEvent( EOsPowerStandby );    
    
    // release mutex

#ifndef RD_WLAN_DDK
    Kern::MutexSignal( iMutex );
#else
    iOsa->MutexRelease();
#endif    

    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalChannel::OnPowerDown: mutex released")));
    }
