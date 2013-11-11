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
* Description:  Offers API to platform for the rest of the engine
*
*/

/*
* %version: 13 %
*/


#include <e32std.h>
#include "wlmplatform.h"
#include "wlmplatformdata.h"
#include "am_debug.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWlmPlatform::CWlmPlatform( MWlmPlatformCallback& aCallback ) :
    iCallback( aCallback ), 
    iCurrentSystemMode( EWlanSystemStartupInProgress ),
    iPlatformData( NULL ),
    iWlanConnectedBeforeEmergencyCall( EFalse ),
    iSystemTimeHandler( NULL ),
    iPeriodicCacheClearingHandler( NULL )
    {
    DEBUG( "CWlmPlatform::CWlmPlatform()" );
    }

// Symbian 2nd phase constructor can leave.
void CWlmPlatform::ConstructL()
    {
    DEBUG( "CWlmPlatform::ConstructL()" );

    iPlatformData = CWlmPlatformData::NewL( *this );
    iSystemTimeHandler = CWlanSystemTimeHandler::NewL( *this );
    iPeriodicCacheClearingHandler = CWlanPeriodicCacheClearingHandler::NewL ( *this );
    }

// Static constructor.
EXPORT_C  CWlmPlatform* CWlmPlatform::NewL( MWlmPlatformCallback& aCallback )
    {
    DEBUG( "CWlmPlatform::NewL()" );

    CWlmPlatform* self = new(ELeave) CWlmPlatform( aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
EXPORT_C CWlmPlatform::~CWlmPlatform()
    {
    DEBUG( "CWlmPlatform::~CWlmPlatform()" );

    delete iPlatformData;
    delete iSystemTimeHandler;
    delete iPeriodicCacheClearingHandler;
    }

// ---------------------------------------------------------
// CWlmPlatform::Initialize
// ---------------------------------------------------------
//
EXPORT_C void CWlmPlatform::Initialize()
    {
    DEBUG( "CWlmPlatform::Initialize()" );

    iPlatformData->UpdateSystemStatuses();
    }

// ---------------------------------------------------------
// CWlmPlatform::GetScanType
// ---------------------------------------------------------
//
EXPORT_C TWLMScanType CWlmPlatform::GetScanType()
    {
    DEBUG( "CWlmPlatform::GetScanType()" );

    //
    // No scans if WLAN has been disabled or system is just
    // starting up.
    //
    if( iCurrentSystemMode == EWlanSystemDisabled ||
        iCurrentSystemMode == EWlanSystemStartupInProgress )
        {
        return EWLMScanNotAllowed;
        }

    //
    // Scans are passive in PDA/Flight profile until a connection
    // has been made.
    //
    if( iCallback.GetCurrentState() == EWlanStateNotConnected &&
        iCurrentSystemMode == EWlanSystemFlight )
        {
        return EWLMScanForcedPassive;
        }

    return EWLMScanAsRequested;
    }

// ---------------------------------------------------------
// CWlmPlatform::IsWlanDisabled
// ---------------------------------------------------------
//   
EXPORT_C TBool CWlmPlatform::IsWlanDisabled()
    {
    if( iCurrentSystemMode == EWlanSystemDisabled )
        {
        return ETrue;   
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CWlmPlatform::GetCurrentSystemMode
// ---------------------------------------------------------
//
EXPORT_C TWlanSystemMode CWlmPlatform::GetCurrentSystemMode()
    {
    DEBUG( "CWlmPlatform::GetCurrentSystemMode()" );

    return iCurrentSystemMode;
    }

// ---------------------------------------------------------
// CWlmPlatform::SetIconState
// ---------------------------------------------------------
//
EXPORT_C TInt CWlmPlatform::SetIconState( TWlmIconStatus aStatus )
    {
    if ( aStatus != EWlmIconStatusConnected &&
         aStatus != EWlmIconStatusConnectedSecure )
        {
        iWlanConnectedBeforeEmergencyCall = EFalse;
        }
    
    return iPlatformData->SetIconState( aStatus );    
    }

// ---------------------------------------------------------
// CWlmPlatform::GetCurrentOperatorMccL
// ---------------------------------------------------------
//
EXPORT_C void CWlmPlatform::GetCurrentOperatorMccL( TUint& aCountryCode )
    {
    iPlatformData->GetCurrentOperatorMccL( aCountryCode );
    }

// ---------------------------------------------------------
// CWlmPlatform::PublishMacAddress
// ---------------------------------------------------------
//
EXPORT_C TInt CWlmPlatform::PublishMacAddress( TMacAddress& aMacAddr )
    {
    return iPlatformData->PublishMacAddress( aMacAddr );
    }

// ---------------------------------------------------------
// CWlmPlatform::InitializeSystemTimeHandler
// ---------------------------------------------------------
//
EXPORT_C void CWlmPlatform::InitializeSystemTimeHandler()
    {
    DEBUG( "CWlmPlatform::InitializeSystemTimeHandler()" );

    iSystemTimeHandler->StopTimer();
    iPeriodicCacheClearingHandler->StopTimer();
    
    iSystemTimeHandler->StartTimer();
    iPeriodicCacheClearingHandler->StartTimer();
    }

// ---------------------------------------------------------
// CWlmPlatform::PublishBgScanInterval
// ---------------------------------------------------------
//
EXPORT_C void CWlmPlatform::PublishBgScanInterval( TUint32& aInterval )
    {
    iPlatformData->PublishBgScanInterval( aInterval );
    }
    
// ---------------------------------------------------------
// CWlmPlatform::GetWlanOnOffState
// ---------------------------------------------------------
//
EXPORT_C TWlanOnOffState CWlmPlatform::GetWlanOnOffState()
    {
	return iPlatformData->GetWlanOnOffState();
	}

// ---------------------------------------------------------
// CWlmPlatform::SystemModeChanged
// ---------------------------------------------------------
//
void CWlmPlatform::SystemModeChanged(
    TWlanSystemMode aOldMode,
    TWlanSystemMode aNewMode )
    {
    DEBUG( "CWlmPlatform::SystemModeChanged()" );

    if ( aOldMode != aNewMode )
        {
        DEBUG2( "System Mode changed from %u to %u", aOldMode, aNewMode );
        iCurrentSystemMode = aNewMode;

        /**
        * The "order" of System Modes is
        * 1) GSM
        * 2) Flight
        * 3) Disabled
        * 4) Starting
        */
        if ( aOldMode < aNewMode )
            {
            iCallback.ReleaseRequest();
            }
        else if ( aOldMode == EWlanSystemDisabled )
            {
            iCallback.EnableWlan();
            }
        else if ( aOldMode == EWlanSystemStartupInProgress )
            {
            iCallback.StartupComplete();
            }
        }
    }

// ---------------------------------------------------------
// CWlmPlatform::EmergencyCallEstablished
// ---------------------------------------------------------
//
void CWlmPlatform::EmergencyCallEstablished()
    {
    DEBUG( "CWlmPlatform::EmergencyCallEstablished()" );

    iCurrentSystemMode = EWlanSystemNormal;
    iCallback.EnableWlan();
        
    if ( iCallback.GetCurrentState() == EWlanStateInfrastructure ||
         iCallback.GetCurrentState() == EWlanStateIBSS || 
         iCallback.GetCurrentState() == EWlanStateSecureInfra )
        {
        iWlanConnectedBeforeEmergencyCall = ETrue;
        }
    }

// ---------------------------------------------------------
// CWlmPlatform::EmergencyCallCompleted
// ---------------------------------------------------------
//
void CWlmPlatform::EmergencyCallCompleted( TWlanSystemMode aNewMode )
    {
    DEBUG( "CWlmPlatform::EmergencyCallCompleted()" );

        TBool startupComplete;
        iCurrentSystemMode = aNewMode;
        if ( aNewMode == EWlanSystemNormal )
            {
            startupComplete = ETrue;
            iCallback.EmergencyCallComplete( startupComplete );
            } 
        else if ( aNewMode == EWlanSystemStartupInProgress)
            {
            /**
            * Return back to the Startup in Progress mode
            */
            startupComplete = EFalse;
            iCallback.ReleaseRequest();
            iCallback.EmergencyCallComplete( startupComplete );
            }
        else if ( aNewMode == EWlanSystemFlight )
      	    {
       	    /* WLAN connection must be released if WLAN connection 
       	     * was established only for the emergency call 
       	     * (connection did not exist before the emergency call) 
       	     */
       	    if ( !iWlanConnectedBeforeEmergencyCall )
       	        {
       	        iCallback.ReleaseRequest();
       	        }
            startupComplete = ETrue;
            iCallback.EmergencyCallComplete( startupComplete );
       	    }
        else
            {
            iCallback.ReleaseRequest();
            }
    iWlanConnectedBeforeEmergencyCall = EFalse;
    }


// ---------------------------------------------------------
// CWlmPlatform::BtConnectionEstablished
// ---------------------------------------------------------
//
void CWlmPlatform::BtConnectionEstablished()
    {
    iCallback.BtConnectionEstablished();  
    }

// ---------------------------------------------------------
// CWlmPlatform::BtConnectionDisabled
// ---------------------------------------------------------
//
void CWlmPlatform::BtConnectionDisabled()
    {
    iCallback.BtConnectionDisabled();
    }

// ---------------------------------------------------------
// CWlmPlatform::OnSystemTimeChange
// ---------------------------------------------------------
//
void CWlmPlatform::OnSystemTimeChange()
    {
    DEBUG( "CWlmPlatform::OnSystemTimeChange()" );
    
    iCallback.SystemTimeChanged();
    }

// ---------------------------------------------------------
// CWlmPlatform::OnCacheClearTimerExpiration
// ---------------------------------------------------------
//
void CWlmPlatform::OnCacheClearTimerExpiration()
    {
    DEBUG( "CWlmPlatform::OnCacheClearTimerExpiration()" );
    
    iCallback.ClearRegionCache();
    }
    
// ---------------------------------------------------------
// CWlmPlatform::WlanOn
// ---------------------------------------------------------
//
void CWlmPlatform::WlanOn()
    {
	DEBUG( "CWlmPlatform::WlanOn()" );
	
	iCallback.WlanOn();
	}

// ---------------------------------------------------------
// CWlmPlatform::WlanOff
// ---------------------------------------------------------
//
void CWlmPlatform::WlanOff()
    {
	DEBUG( "CWlmPlatform::WlanOff()" );
	
	iCallback.WlanOff();
	}

