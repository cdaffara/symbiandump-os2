/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanNullSendController class
*
*/

/*
* %version: 16 %
*/

#include "config.h"
#include "umacnullsendcontroller.h"
#include "umacnullsender.h"
#include "UmacContextImpl.h"


// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::StartVoiceOverWlanCallMaintenance()
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::StartVoiceOverWlanCallMaintenance") );

    iFlags |= KVoiceOverWlanCallMaintenanceStarted;
    
    if ( iFlags & KInVoiceCallState )
        {
        RegisterNoVoiceTimeout( iNoVoiceTimeout );
        
        if ( ( iWlanContextImpl.UapsdUsedForVoice() ) &&
             ( iWlanContextImpl.CurrentDot11PwrMgmtMode() == 
               WHA::KPsEnable ) )
            {
            // U-APSD is used for Voice and we are also in PS mode,
            // so initiate QoS Null Frame sending by arming the timer
            RegisterNullTimeout( iNullTimeout );
            }            
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::ResumeQosNullSending()
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::ResumeQosNullSending") );                

    if ( ( iWlanContextImpl.UapsdUsedForVoice() )&&
         ( iFlags & KInVoiceCallState ) &&
         ( iWlanContextImpl.CurrentDot11PwrMgmtMode() == WHA::KPsEnable ) )
        {
        // U-APSD is used for Voice, we are in Voice Call state and in PS
        // mode, so resume QoS null frame sending by re-arming the timer
        RegisterNullTimeout( iNullTimeout );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::OnFrameRx( 
    WHA::TQueueId aQueueId,
    TUint aPayloadLength )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::OnFrameRx entry: iFlags: 0x%08x"),
        iFlags );        

    if ( iFlags & KVoiceOverWlanCallMaintenanceStarted )
        {
        if ( iFlags & KInVoiceCallState )
            {
            // we are already in Voice Call state

            const TInt64 KTimeNow( os_systemTime() );
            
            if ( aQueueId == WHA::EVoice )
                {
                // and this is a Voice priority frame.
                
                iLatestVoiceRxOrTxInVoiceCallState = KTimeNow;
                }
            else if ( ( aQueueId == WHA::ELegacy ) && 
                      ( aPayloadLength < 
                        iBestEffortVoiceRxLengthThreshold ) )
                {
                iBestEffortVoiceRxTimeStamp[iBestEffortVoiceRxInd] = KTimeNow;
                iBestEffortVoiceRxInd = 
                    ( iBestEffortVoiceRxInd == 
                      KBestEffortVoiceRxTimeStampCnt - 1 ) ? 0 :
                      ++iBestEffortVoiceRxInd;
                }
            else
                {
                // no action
                }
            }
        else if ( iFlags & KVoiceCallEntryPending )
            {
            // we are in Voice Call Entry Pending state

            if ( aQueueId == WHA::EVoice )
                {
                // this is a Voice priority frame
                
                ++iVoiceCallEntryRxCount;
                
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanNullSendController::OnFrameRx: iVoiceCallEntryRxCount is now: %d"),
                    iVoiceCallEntryRxCount );
    
                if ( iVoiceCallEntryRxCount >= iVoiceCallEntryThreshold )
                    {
                    // We will enter the Voice Call state
                    EnterVoiceCallState();
                    }
                }            
            }
        else
            {
            // we are neither in Voice Call Entry Pending nor Voice Call state
    
            if ( aQueueId == WHA::EVoice )
                {
                // this is a Voice priority frame. 
                
                // It is the 1st Voice priority frame in the period starting
                // now
                iVoiceCallEntryRxCount = 1;
                iVoiceCallEntryTxCount = 0;
                
                // We enter the Voice Call Entry Pending state by arming
                // the relevant timer
    
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanNullSendController::OnFrameRx: Enter Voice Call Entry pending state") );
    
                RegisterVoiceCallEntryTimeout();
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::OnFrameTx( WHA::TQueueId aQueueId )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::OnFrameTx entry: iFlags: 0x%08x"),
        iFlags );        

    iLatestTx = os_systemTime();
    
    if ( iFlags & KVoiceOverWlanCallMaintenanceStarted )
        {
        if ( iFlags & KInVoiceCallState )
            {
            // we are already in Voice Call state

            if ( aQueueId == WHA::EVoice )
                {
                // and this is a Voice priority frame.
                
                iLatestVoiceRxOrTxInVoiceCallState = iLatestTx;
                }
            }
        else if ( iFlags & KVoiceCallEntryPending )
            {
            // we are in Voice Call Entry Pending state

            if ( aQueueId == WHA::EVoice )
                {
                // this is a Voice priority frame
                
                ++iVoiceCallEntryTxCount;
                
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanNullSendController::OnFrameTx: iVoiceCallEntryTxCount is now: %d"),
                    iVoiceCallEntryTxCount );

                if ( iVoiceCallEntryTxCount >= iVoiceCallEntryThreshold )
                    {
                    // We will enter the Voice Call state
                    EnterVoiceCallState();
                    }
                }            
            }
        else
            {
            // we are neither in Voice Call Entry Pending nor Voice Call state

            if ( aQueueId == WHA::EVoice )
                {
                // this is a Voice priority frame. 
                
                // It is the 1st Voice priority frame in the period starting
                // now
                iVoiceCallEntryTxCount = 1;
                iVoiceCallEntryRxCount = 0;
                                
                // We enter the Voice Call Entry Pending state by arming
                // the relevant timer

                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanNullSendController::OnFrameTx: Enter Voice Call Entry pending state") );

                RegisterVoiceCallEntryTimeout();
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::OnVoiceCallEntryTimerTimeout()
    {
    // as this timer expired we are no more in Voice Call Entry Pending state
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::OnVoiceCallEntryTimerTimeout: exit entry pending (to idle) state") );
    
    iFlags &= ~KVoiceCallEntryPending;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::OnNullTimerTimeout()
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::OnNullTimerTimeout") );
    
    iFlags &= ~KNullTimerArmed;

    if ( iWlanContextImpl.CurrentDot11PwrMgmtMode() == WHA::KPsEnable )
        {        
        // we are still in PS mode (as we were when this timer was 
        // armed)
        
        TUint32 KTimeAfterLatestTx ( 
            static_cast<TUint32>(os_systemTime() - iLatestTx) );
        
        if ( KTimeAfterLatestTx >= iNullTimeout )
            {
            // request a QoS Null Data frame to be sent.
            // However, if there is a frame Tx (of any frame) already pending, 
            // we won't request a new frame to be sent. Note that it can really
            // be any frame, i.e. for any AC, as we currently always set all the
            // ACs as trigger enabled if U-APSD is used
            if ( !iWlanContextImpl.UnsentTxPackets() )
                {
                if ( !iNullSender.TxNullDataFrame( iWlanContextImpl, ETrue ) )
                    {
                    // frame was not sent because we didn't get a Tx buffer.
                    // In this case we'll skip the sending
                    // However, we need to re-arm the timer to trigger the next
                    // QoS Null Data frame sending
                    RegisterNullTimeout( iNullTimeout );
                    }
                }
            else
                {
                // frame Tx already pending, so we don't ask a QoS Null to be sent
                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanNullSendController::OnNullTimerTimeout: frame Tx already pending. New QoS Null Tx request skipped") );
                
                // However, we need to re-arm the timer to trigger the next
                // QoS Null Data frame sending
                RegisterNullTimeout( iNullTimeout );
                }
            }
        else
            {
            // No need to send QoS Null; yet. Re-arm the timer with 
            // a suitable timeout relative to the time of the latest Tx
            // in QoS Null frame sending state
            RegisterNullTimeout( iNullTimeout - KTimeAfterLatestTx );
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::OnQosNullDataTxCompleted()
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::OnQosNullDataTxCompleted entry: iFlags: 0x%08x"),
        iFlags );

    if ( iFlags & KVoiceOverWlanCallMaintenanceStarted &&
         iFlags & KInVoiceCallState &&
         iWlanContextImpl.CurrentDot11PwrMgmtMode() == WHA::KPsEnable )
        {
        // we are doing Voice over WLAN Call maintenance, in Voice Call 
        // state and also in PS mode 

        // as the previous QoS Null Data, i.e. U-APSD Trigger, Frame has been
        // transmitted, re-arm the timer for the next round
        RegisterNullTimeout( iNullTimeout );
        }

    iLatestTx = os_systemTime();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::OnNoVoiceTimerTimeout()
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::OnNoVoiceTimerTimeout entry: iFlags: 0x%08x"),
        iFlags );    

    iFlags &= ~KNoVoiceTimerArmed;
    const TInt64 KTimeNow( os_systemTime() );
    const TUint32 KTimeAfterLatestVoiceRxOrTxInVoiceCallState ( 
        static_cast<TUint32>(KTimeNow - 
                             iLatestVoiceRxOrTxInVoiceCallState) );
    
    if ( KTimeAfterLatestVoiceRxOrTxInVoiceCallState >= iNoVoiceTimeout )
        {
        // no real Voice priority traffic any more.
        
        // check if there still is traffic which may be Voice traffic
        // erroneously tagged as Best Effort priority 
        if ( KTimeNow -
             // the oldest time stamp of these frames that we have in our
             // records - or zero if such a time stamp doesn't exist
             iBestEffortVoiceRxTimeStamp[iBestEffortVoiceRxInd] > 
             iBestEffortVoiceRxTimeWindow )
            {
            // exit Voice Call state
            iFlags &= ~KInVoiceCallState;
        
            // Send voice call state change indication to engine
            iWlanContextImpl.iUmac.OnInDicationEvent( EVoiceCallOff );
    
            OsTracePrint( KUmacDetails, (TUint8*)
                ("UMAC: WlanNullSendController::OnNoVoiceTimerTimeout: Exit Voice Call state") );
            
            // stop the possibly ongoing Null frame sending by canceling the 
            // Null timer
            CancelNullTimeout();
            }
        else
            {
            OsTracePrint( KUmacDetails, (TUint8*)
                ("UMAC: WlanNullSendController::OnNoVoiceTimerTimeout: Continue in Voice Call state") );
            
            RegisterNoVoiceTimeout( iNoVoiceTimeout );
            }
        }
    else
        {
        // don't exit Voice Call state. Re-arm the timer instead with a 
        // suitable timeout relative to the latest Voice priority frame 
        // Rx or Tx in Voice Call state 
        RegisterNoVoiceTimeout( 
            iNoVoiceTimeout - KTimeAfterLatestVoiceRxOrTxInVoiceCallState );        
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::OnKeepAliveTimerTimeout()
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::OnKeepAliveTimerTimeout") );
    
    iFlags &= ~KKeepAliveTimerArmed;

    // request a regular Null Data frame to be sent
    // however, if there is a frame Tx (of any frame) already pending OR
    // we use U-APSD for Voice and we are on a VoWLAN call
    // we won't request a new frame to be sent. 
    if ( !iWlanContextImpl.UnsentTxPackets() &&
         !( iWlanContextImpl.UapsdUsedForVoice() && InVoiceCallState() ) )
        {
        if ( !iNullSender.TxNullDataFrame( iWlanContextImpl, EFalse ) )
            {
            // frame was not sent because we didn't get a Tx buffer.
            // In this case we'll skip the sending
            // However, we need to re-arm the timer to trigger the next
            // Null Data frame sending
            RegisterKeepAliveTimeout( iKeepAliveTimeout );
            }
        }
    else
        {
        // we don't ask a Null Data to be sent
        OsTracePrint( KUmacDetails, (TUint8*)
            ("UMAC: WlanNullSendController::OnKeepAliveTimerTimeout: frame Tx "
             "already pending OR U-APSD used for Voice and we are in VoWLAN"
             "call => Null Data Tx request skipped") );
        
        // However, we need to re-arm the timer to trigger the next
        // Null Data frame sending
        RegisterKeepAliveTimeout( iKeepAliveTimeout );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::RegisterVoiceCallEntryTimeout()
    {
    CancelVoiceCallEntryTimeout();

    iWlanContextImpl.iUmac.RegisterTimeout( 
        iVoiceCallEntryTimeout, 
        EWlanVoiceCallEntryTimer );

    iFlags |= KVoiceCallEntryPending;

    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::RegisterVoiceCallEntryTimeout: timer armed") );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::RegisterNullTimeout( 
    TUint32 aTimeoutInMicroSeconds )
    {
    CancelNullTimeout();

    iWlanContextImpl.iUmac.RegisterTimeout( 
        aTimeoutInMicroSeconds, 
        EWlanNullTimer );

    iFlags |= KNullTimerArmed;

    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::RegisterNullTimeout: timer armed") );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::RegisterNoVoiceTimeout( 
    TUint32 aTimeoutInMicroSeconds )
    {
    CancelNoVoiceTimeout();
    
    iWlanContextImpl.iUmac.RegisterTimeout( aTimeoutInMicroSeconds, 
                                            EWlanNoVoiceTimer );
    iFlags |= KNoVoiceTimerArmed;                                            

    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::RegisterNoVoiceTimeout: timer armed") );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::RegisterKeepAliveTimeout( 
    TUint32 aTimeoutInMicroSeconds )
    {
    CancelKeepAliveTimeout();

    iWlanContextImpl.iUmac.RegisterTimeout( aTimeoutInMicroSeconds, 
                                            EWlanKeepAliveTimer );

    iFlags |= KKeepAliveTimerArmed;

    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::RegisterKeepAliveTimeout: timer armed") );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::CancelVoiceCallEntryTimeout()
    {
    if ( iFlags & KVoiceCallEntryPending )
        {
        iWlanContextImpl.iUmac.CancelTimeout( EWlanVoiceCallEntryTimer );        
        
        iFlags &= ~KVoiceCallEntryPending;

        OsTracePrint( KUmacDetails, (TUint8*)
            ("UMAC: WlanNullSendController::CancelVoiceCallEntryTimeout: timer cancelled") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::CancelNullTimeout()
    {
    if ( iFlags & KNullTimerArmed )
        {
        iWlanContextImpl.iUmac.CancelTimeout( EWlanNullTimer );        
        
        iFlags &= ~KNullTimerArmed;

        OsTracePrint( KUmacDetails, (TUint8*)
            ("UMAC: WlanNullSendController::CancelNullTimeout: timer cancelled") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::CancelNoVoiceTimeout()
    {
    if ( iFlags & KNoVoiceTimerArmed )
        {
        iWlanContextImpl.iUmac.CancelTimeout( EWlanNoVoiceTimer );    
        
        iFlags &= ~KNoVoiceTimerArmed;

        OsTracePrint( KUmacDetails, (TUint8*)
            ("UMAC: WlanNullSendController::CancelNoVoiceTimeout: timer cancelled") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::CancelKeepAliveTimeout()
    {
    if ( iFlags & KKeepAliveTimerArmed )
        {
        iWlanContextImpl.iUmac.CancelTimeout( EWlanKeepAliveTimer );        
        
        iFlags &= ~KKeepAliveTimerArmed;

        OsTracePrint( KUmacDetails, (TUint8*)
            ("UMAC: WlanNullSendController::CancelKeepAliveTimeout: timer cancelled") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanNullSendController::EnterVoiceCallState()
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::EnterVoiceCallState") );

    CancelVoiceCallEntryTimeout();

    iFlags |= KInVoiceCallState;

    // Send voice call state change indication to engine
    iWlanContextImpl.iUmac.OnInDicationEvent( EVoiceCallOn );

    // arm the No Voice timer so that we also exit the Voice Call
    // state at some point
    iLatestVoiceRxOrTxInVoiceCallState = os_systemTime();
    RegisterNoVoiceTimeout( iNoVoiceTimeout );
    
    if ( ( iWlanContextImpl.UapsdUsedForVoice() ) &&
         ( iWlanContextImpl.CurrentDot11PwrMgmtMode() == WHA::KPsEnable ) )
        {
        // U-APSD is used for Voice and we are also in PS mode,
        // so initiate QoS Null Frame sending by arming the timer
        RegisterNullTimeout( iNullTimeout );                        
        }
    }
