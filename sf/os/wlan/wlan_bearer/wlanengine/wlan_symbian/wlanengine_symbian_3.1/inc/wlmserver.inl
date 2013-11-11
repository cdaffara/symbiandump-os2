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
* Description:  Inline methods of CWlmServer
*
*/

/*
* %version: 18 %
*/

#include "am_debug.h"

// ---------------------------------------------------------
// CWLMServer::GetScanType
// ---------------------------------------------------------
//
inline TWLMScanType CWlmServer::GetScanType()
    {
    DEBUG( "CWlmServer::GetScanType()" );
    return iPlatform->GetScanType();
    }

// ---------------------------------------------------------
// CWLMServer::SetIconState
// ---------------------------------------------------------
//
inline TInt CWlmServer::SetIconState( TWlmIconStatus aStatus )
    {
    DEBUG1( "CWlmServer::SetIconState( %u )", aStatus );
    return iPlatform->SetIconState( aStatus );
    }

// ---------------------------------------------------------
// CWLMServer::UpdateAvailability
// ---------------------------------------------------------
//
inline void CWlmServer::UpdateAvailability( 
    TBool /*aAvailability*/,
    TBool aNewNetworksDetected,
    TBool aOldNetworksLost )
    {
    TBuf8<1> tmp;
    if( aNewNetworksDetected )
        {
        // Notify subscribees
        for( TInt i = 0; i < iNotificationArray.Count(); i++ )
            {
            iNotificationArray[i]->AddNotification( EWlmNotifyNewNetworksDetected, tmp );
            }
        }
    if( aOldNetworksLost )
        {
        // Notify subscribees
        for( TInt i = 0; i < iNotificationArray.Count(); i++ )
            {
            iNotificationArray[i]->AddNotification( EWlmNotifyOldNetworksLost, tmp );
            }
        }
    }

// ---------------------------------------------------------
// CWlmServer::ReleaseRequest
// ---------------------------------------------------------
//
inline void CWlmServer::ReleaseRequest()
    {
    DEBUG( "CWlmServer::ReleaseRequest()" );
    if( GetCurrentState() != EWlanStateNotConnected )
        {
        // create data for state change notification
        TBuf8<1> state;
        state.SetLength( 1 );
        state[0] = static_cast<TInt8>( EWlanStateNotConnected );

        // create notification for all observing clients
        for ( TInt i = 0; i < iNotificationArray.Count(); i++ )
            {
            iNotificationArray[i]->AddNotification( EWlmNotifyConnectionStateChanged, state );
            }
        }
    else // state == not connected
        {
        if( iPlatform->IsWlanDisabled() )
            {
            // KWlanIntCmdNull means not expecting completion for this request
            iCoreServer->disable_wlan( KWlanIntCmdNull ); 
            }
        }
    }

// ---------------------------------------------------------
// CWlmServer::EnableWlan
// ---------------------------------------------------------
//
inline void CWlmServer::EnableWlan()
    {
    // KWlanIntCmdNull means not expecting completion for this request
    iCoreServer->enable_wlan( KWlanIntCmdNull );
    }

// ---------------------------------------------------------
// CWlmServer::IsRoaming
// ---------------------------------------------------------
//
inline TBool CWlmServer::IsRoaming() const
    {
    return iIsRoaming;
    }

// ---------------------------------------------------------
// CWlmServer::IsSessionActive
// ---------------------------------------------------------
//
inline TBool CWlmServer::IsSessionActive(
    const SRequestMapEntry& aEntry ) const
    {
    return ( aEntry.iSessionId != 0);
    }

// ---------------------------------------------------------
// CWlmServer::CalculateScanStartTime
// ---------------------------------------------------------
//
inline TUint CWlmServer::CalculateScanStartTime(
    const TInt aDelay ) const
    {
    TUint ticks(
        aDelay * KWlanSecsToMicrosecsMultiplier / iSystemTickPeriod );

    return User::TickCount() + ticks; 
    }

// ---------------------------------------------------------
// CWlmServer::FindRequestIndex
// ---------------------------------------------------------
//
inline TUint CWlmServer::FindRequestIndex(
    const TInt aRequestId ) const
    {
    TInt i(0);
    for( i=0; i < iRequestMap.Count(); i++ )
        {
        if( iRequestMap[i].iRequestId == aRequestId )
            {
            return i;
            }
        }
    return i;
    }

// ---------------------------------------------------------
// CWlmServer::IsPendingTimedScanRequest
// ---------------------------------------------------------
//
inline TBool CWlmServer::IsPendingTimedScanRequest(
    const TInt aIndex ) const
    {
    return ( iRequestMap[aIndex].iTime != NULL ) && IsPendingScanRequest( aIndex );
    }

// ---------------------------------------------------------
// CWlmServer::IsPendingScanRequest
// ---------------------------------------------------------
//
inline TBool CWlmServer::IsPendingScanRequest(
    const TInt aIndex ) const
    {
    if( iRequestMap[aIndex].iRequestId == KWlanIntCmdBackgroundScan )
        {
        return ETrue;
        }
    else if( iRequestMap[aIndex].iRequestId >= KWlanExtCmdBase && 
             iRequestMap[aIndex].iFunction == EGetAvailableIaps )
        {
        return ETrue;
        }
    else if( iRequestMap[aIndex].iRequestId >= KWlanExtCmdBase && 
             iRequestMap[aIndex].iFunction == EGetScanResults )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }

    }

// End of File
