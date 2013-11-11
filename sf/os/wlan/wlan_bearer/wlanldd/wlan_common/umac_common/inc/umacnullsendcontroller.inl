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
* Description:   Implementation of WlanNullSendController inline 
*                methods.
*
*/

/*
* %version: 11 %
*/

// Default Voice Call Entry timeout
// This value is used if no other value has been provided
const TUint32 KDefaultVoiceCallEntryTimeout = 300000; // 300 ms

// Default Voice Call Entry Threshold
// This value is used if no other value has been provided
const TUint32 KDefaultVoiceCallEntryThreshold = 5;

// Default Null timeout
// This value is used if no other value has been provided
const TUint32 KDefaultNullTimeout = 20000;      // 20 ms

// Default No Voice timeout
// This value is used if no other value has been provided
const TUint32 KDefaultNoVoiceTimeout = 1000000; // 1 s

// Default Keep Alive timeout
// This value is used if no other value has been provided
const TUint32 KDefaultKeepAliveTimeout = 60000000; // 60 s

// Default value for frame payload length threashold of a potential 
// carrier of downlink Voice over WLAN Call data, which is erroneously 
// tagged as Best Effort
const TUint KDefaultBestEffortVoiceRxLengthThreshold = 400; // bytes

// Default value for the length of the time window within which we
// study the potential carriers of downlink Voice over WLAN Call data, 
// which are erroneously tagged as Best Effort
const TUint KDefaultBestEffortVoiceRxTimeWindow = 375000; // 375 ms

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WlanNullSendController::WlanNullSendController( 
    WlanContextImpl& aWlanCtxImpl,
    MWlanNullSender& aNullSender ) : 
    iFlags( 0 ),
    iVoiceCallEntryTimeout( KDefaultVoiceCallEntryTimeout ),
    iVoiceCallEntryThreshold( KDefaultVoiceCallEntryThreshold ),
    iNullTimeout( KDefaultNullTimeout ),
    iNoVoiceTimeout( KDefaultNoVoiceTimeout ),
    iLatestVoiceRxOrTxInVoiceCallState( 0 ),
    iBestEffortVoiceRxLengthThreshold( 
        KDefaultBestEffortVoiceRxLengthThreshold ),
    iBestEffortVoiceRxInd( 0 ),
    iBestEffortVoiceRxTimeWindow( KDefaultBestEffortVoiceRxTimeWindow ),
    iKeepAliveTimeout( KDefaultKeepAliveTimeout ),
    iLatestTx( 0 ),
    iVoiceCallEntryTxCount( 0 ),
    iVoiceCallEntryRxCount( 0 ),
    iWlanContextImpl( aWlanCtxImpl ),
    iNullSender( aNullSender )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::WlanNullSendController") );
    os_memset( 
        iBestEffortVoiceRxTimeStamp, 
        0, 
        sizeof( iBestEffortVoiceRxTimeStamp ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WlanNullSendController::~WlanNullSendController() 
    { 
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::~WlanNullSendController") );

    // cancel any possibly running timers
    CancelVoiceCallEntryTimeout();
    CancelNullTimeout();
    CancelNoVoiceTimeout();
    CancelKeepAliveTimeout();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanNullSendController::SetParameters(
    TUint32 aVoiceCallEntryTimeout,
    TUint32 aVoiceCallEntryThreshold,
    TUint32 aNullTimeout,
    TUint32 aNoVoiceTimeout,
    TUint32 aKeepAliveTimeout )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::SetParameters: aVoiceCallEntryTimeout: %d"),
        aVoiceCallEntryTimeout );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::SetParameters: aVoiceCallEntryThreshold: %d"),
        aVoiceCallEntryThreshold );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::SetParameters: aNullTimeout: %d"),
        aNullTimeout );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::SetParameters: aNoVoiceTimeout: %d"),
        aNoVoiceTimeout );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::SetParameters: aKeepAliveTimeout: %d"),
        aKeepAliveTimeout );

    iVoiceCallEntryTimeout = aVoiceCallEntryTimeout;
    iVoiceCallEntryThreshold = aVoiceCallEntryThreshold;
    iNullTimeout = aNullTimeout;
    iNoVoiceTimeout = aNoVoiceTimeout;
    iKeepAliveTimeout = aKeepAliveTimeout;
    iBestEffortVoiceRxTimeWindow = 
        ( aVoiceCallEntryTimeout / ( aVoiceCallEntryThreshold - 1 ) ) 
        * KBestEffortVoiceRxTimeStampCnt;

    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::SetParameters: "
         "iBestEffortVoiceRxTimeWindow: %d"), 
        iBestEffortVoiceRxTimeWindow );
    }
        
// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
inline void WlanNullSendController::StopVoiceOverWlanCallMaintenance()
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::StopVoiceOverWlanCallMaintenance") );

    iFlags &= ~KVoiceOverWlanCallMaintenanceStarted;
    
    // cancel relevant possibly running timers
    CancelVoiceCallEntryTimeout();
    CancelNullTimeout();
    CancelNoVoiceTimeout();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanNullSendController::TerminateVoiceOverWlanCallMaintenance()
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::TerminateVoiceOverWlanCallMaintenance") );

    StopVoiceOverWlanCallMaintenance();
    iFlags &= ~KInVoiceCallState;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanNullSendController::StartKeepAlive()
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::StartKeepAlive") );

    iFlags |= KKeepAliveStarted;
    RegisterKeepAliveTimeout( iKeepAliveTimeout );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanNullSendController::StopKeepAlive()
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::StopKeepAlive") );

    iFlags &= ~KKeepAliveStarted;
    // cancel possibly running timer
    CancelKeepAliveTimeout();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanNullSendController::OnNullDataTxCompleted()
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanNullSendController::OnNullDataTxCompleted") );

    if ( iFlags & KKeepAliveStarted )
        {
        // we are still doing Keep Alive and as the previous Null Data, i.e.
        // Keep Alive, Frame has been transmitted, we re-arm the timer for 
        // the next round
        RegisterKeepAliveTimeout( iKeepAliveTimeout );
        
        // There's no need to check for the need to re-arm the NullTimer, because 
        // it won't happen in practice that a Keep Alive (Null Data) frame 
        // transmit would complete when the NullTimer is armed, as we won't need 
        // to send any Keep Alive frames during a Voice Call, i.e. when the 
        // NullTimer is running, because there's frequent frame Tx activity
        // during a Voice Call
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanNullSendController::InVoiceCallState() const
    {
    return static_cast<TBool>(iFlags & KInVoiceCallState);
    }
