/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  WLAN Agent state machine's state implementations
*
*/

/*
* %version: 35 %
*/

#include <connectprog.h>
#include <comms-infras/dbaccess.h>
#include <commdb.h>
#include <wdbifwlansettings.h>
#include <badesca.h>
#include <e32ver.h>
#include <utf.h>
#include "am_debug.h"
#include "genscanlist.h"
#include "genscaninfo.h"
#include "wlanagtstates.h"
#include "wlanagtsm.h"
#include "wlanagthotspotclient.inl"

static const TUint KConnectionEndWaitTime = 2*1000000; // 2 seconds

// -----------------------------------------------------------------------------
// CWlanStateBase::CWlanStateBase
// -----------------------------------------------------------------------------
//
CWlanStateBase::CWlanStateBase( CWlanSM* aWlanSM, RWLMServer& aWLMServer ) :
	CAgentStateBase( *aWlanSM ),
	iWlanSM( aWlanSM ),
	iWLMServer( aWLMServer )
    {
    }

// -----------------------------------------------------------------------------
// CWlanStateBase::StartState
// -----------------------------------------------------------------------------
//
void CWlanStateBase::StartState()
    {
	JumpToRunl( KErrNone );
    }

// Default implementations for the functions from MWLMNotify don't do anything.
// If a state is interested in the notifications, it should 'activate' notifications
// and override these.

// -----------------------------------------------------------------------------
// CWlanStateBase::ConnectionStateChanged
// -----------------------------------------------------------------------------
//
void CWlanStateBase::ConnectionStateChanged( TWlanConnectionState /* aNewState */ )
    {
	DEBUG( "CWlanStateBase::ConnectionStateChanged()" );
    }

// -----------------------------------------------------------------------------
// CWlanStartState::CWlanStartState
// -----------------------------------------------------------------------------
//
CWlanStartState::CWlanStartState( CWlanSM* aWlanSM, RWLMServer& aWLMServer ) :
	CWlanStateBase( aWlanSM, aWLMServer ),
	iNextState( EWlanAgtNextStateUnknown )
    {
	DEBUG( "CWlanStartState constructor" );
    }

// -----------------------------------------------------------------------------
// CWlanStartState::RunL
// -----------------------------------------------------------------------------
//
void CWlanStartState::RunL()
    {
	if( iWlanSM->WlanSettings()->SSID.Length() == 0 )
        {
        PrepareEasyWlanParamsL();
        }
		
	DEBUG( "CWlanStartState: next state: EWlanAgtNextStateJoin" );
	iNextState = EWlanAgtNextStateJoin;
	
	iWlanSM->CompleteState( KErrNone );
    }


// -----------------------------------------------------------------------------
// CWlanStartState::PrepareEasyWlanParamsL
// -----------------------------------------------------------------------------
//
void CWlanStartState::PrepareEasyWlanParamsL()
    {
	DEBUG( "CWlanStartState::PrepareEasyWlanParamsL()" );
    TPckgBuf<TWep> iWep;
    TPtr8 wepPtr( &iWep().KeyMaterial[0], KWLMMaxWEPKeyLength );
	TPckgBuf<TWpaPsk> iPsk;
	TPtr8 pskPtr( &iPsk().KeyMaterial[0], KWLMMaxWpaPskLength );
	TWLMOverrideSettings overrides;
	Mem::FillZ(&overrides, sizeof(overrides));
  
	TBuf8<KMaxSSIDLength> ssid8;
	CnvUtfConverter::ConvertFromUnicodeToUtf8( ssid8, iWlanSM->WlanSettings()->UsedSSID );
	
	overrides.settingsMask = EOverrideSsidMask;
	overrides.ssid.ssidLength = ssid8.Length(); 
	Mem::Copy(overrides.ssid.ssid, ssid8.Ptr(), overrides.ssid.ssidLength);

    if ( (iWlanSM->WlanSettings())->ConnectionMode == Adhoc )
        {
    	DEBUG( "CWlanStartState::PrepareEasyWlanParamsL(), conn mode: Adhoc" );
        overrides.settingsMask |= EOverrideIbssMask;
        }
    
    // Set security mode and parameters
	switch( iWlanSM->WlanSettings()->SecurityMode )
    	{
        case AllowUnsecure:
        	DEBUG( "CWlanStartState::PrepareEasyWlanParamsL(), security mode: Open" );
            break;
        case Wep:
        	DEBUG( "CWlanStartState::PrepareEasyWlanParamsL(), security mode: Wep" );
            overrides.settingsMask |= EOverrideWepMask;                
            
            Mem::Copy(&iWep().KeyMaterial[0], &((iWlanSM->WlanSettings()->WepKey1).KeyMaterial[0]), (iWlanSM->WlanSettings()->WepKey1).KeyLength);
            iWep().KeyIndex = (iWlanSM->WlanSettings()->WepKey1).KeyIndex;
            iWep().KeyLength = (iWlanSM->WlanSettings()->WepKey1).KeyLength;
            
            overrides.wep = iWep();
            break;
        case Wlan8021x:
        case Wpa:
        case Wpa2Only:
        	if ( iWlanSM->WlanSettings()->EnableWpaPsk )
        		{
        		DEBUG1( "CWlanStartState::PrepareEasyWlanParamsL(), security mode: Wpa-psk,len:%d", 
        				iWlanSM->WlanSettings()->WPAKeyLength );
        		
        		overrides.settingsMask |= EOverrideWpaPskMask;
        		pskPtr.Copy( iWlanSM->WlanSettings()->WPAPreSharedKey );
        	    iPsk().KeyLength = iWlanSM->WlanSettings()->WPAKeyLength;
        	    
        	    overrides.wpaPsk = iPsk();        		
        		}
        	else
        		{
        		DEBUG( "CWlanStartState::PrepareEasyWlanParamsL(), security mode: Wpa" );
        		overrides.settingsMask |= EOverrideWpaMask;
        		}
            break;
        default:
        	DEBUG1( "CWlanStartState::PrepareEasyWlanParamsL(), wrong security mode value in commsdat:%d",
        							iWlanSM->WlanSettings()->SecurityMode );
        	User::LeaveIfError(KErrGeneral);
		    return;
    	}
	
	iWlanSM->SetOverrideSettingsL( overrides );
    }


// -----------------------------------------------------------------------------
// CWlanStartState::NextStateL
// -----------------------------------------------------------------------------
//
CAgentStateBase* CWlanStartState::NextStateL(TBool aContinue)
    {
	DEBUG( "CWlanStartState::NextStateL()" );
	
	if( aContinue )
	    {
		switch(iNextState)
		    {
    		case EWlanAgtNextStateJoin:
    			return new (ELeave) CWlanJoinState(iWlanSM, iWLMServer);
    		case EWlanAgtNextStateOpen:
    			return new (ELeave) CWlanOpenState(iWlanSM, iWLMServer);
    		default:
	    		return new(ELeave) CWlanErrorState( iWlanSM, KErrCouldNotConnect );
		    }
	    }
	else
	{
	    return new(ELeave) CWlanErrorState( iWlanSM, KErrCouldNotConnect );
	}
    }

// -----------------------------------------------------------------------------
// CWlanStartState::DoCancel
// -----------------------------------------------------------------------------
//
void CWlanStartState::DoCancel()
    {
    }
    

// -----------------------------------------------------------------------------
// CWlanJoinState::CWlanJoinState
// -----------------------------------------------------------------------------
//
CWlanJoinState::CWlanJoinState( CWlanSM* aWlanSM, RWLMServer& aWLMServer ) :
	CWlanStateBase( aWlanSM, aWLMServer )
    {
    DEBUG( "CWlanJoinState constructor" );
    }

// -----------------------------------------------------------------------------
// CWlanJoinState::~CWlanJoinState
// -----------------------------------------------------------------------------
//
CWlanJoinState::~CWlanJoinState()
    {
	DEBUG( "CWlanJoinState destructor" );
	Cancel();
	if ( iAgtHotSpotClient )
	    {
	    delete iAgtHotSpotClient;
	    }
    }

// -----------------------------------------------------------------------------
// CWlanJoinState::StartState
// -----------------------------------------------------------------------------
//
void CWlanJoinState::StartState()
    {
    DEBUG( "CWlanJoinState::StartState()" );
    TRAPD( ret, DoStartStateL() );
    if( ret != KErrNone )
        {
        iContext = EHotSpotFailure;
        JumpToRunl( ret );
        }
    }

// -----------------------------------------------------------------------------
// CWlanJoinState::DoStartStateL
// -----------------------------------------------------------------------------
//
void CWlanJoinState::DoStartStateL()
    {
    DEBUG( "CWlanJoinState::DoStartStateL()" );

    // check if plug-in is available
    TRAPD( ret, iAgtHotSpotClient = CWlanAgtHotSpotClient::NewL() );

    if ( ret == KErrNone )
        {
        DEBUG( "CWlanJoinState::DoStartStateL(), AgtHotSpot plugin available" );

        //Assume that AP is HotSpot AP. Later if HS Server says its not HS AP, then it will be set as normal AP
        iWlanSM->SetHotSpotAP( ETrue );

        // talk to hotspot server	
        DEBUG( "CWlanJoinState::DoStartStateL(), Sending Start message to HotSpot Server" );
        iAgtHotSpotClient->HotSpotStart( iWlanSM->IapId(), iStatus );
        iContext = EHotSpotInProgress;
        SetActive();
        }
    else
        {
        iAgtHotSpotClient = NULL;
        DEBUG(  "CWlanJoinState::DoStartStateL(), Couldn't find AgtHotSpot plug-in,defaulting to normal procedure" );

        iWLMServer.Join( iStatus, iWlanSM->SettingsId(), iWlanSM->OverrideSettings() );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CWlanJoinState::NextStateL
// -----------------------------------------------------------------------------
//
CAgentStateBase* CWlanJoinState::NextStateL( TBool aContinue )
    {
	DEBUG( "CWlanJoinState::NextStateL()" );

	if( aContinue )
	    {
	    return new (ELeave) CWlanOpenState( iWlanSM, iWLMServer );
	    }
   	else if( iStatus.Int() == KErrWlanConnAlreadyActive )
        {
        return new(ELeave) CWlanErrorState( iWlanSM, KErrWlanConnAlreadyActive );
        }
   	else
        {
        return new(ELeave) CWlanErrorState( iWlanSM, KErrCouldNotConnect );
        }
    }

// -----------------------------------------------------------------------------
// CWlanJoinState::RunL
// -----------------------------------------------------------------------------
//
void CWlanJoinState::RunL()
    {
    DEBUG1( "CWlanJoinState::RunL(), iStatus = %i", iStatus.Int() );

    if( iAgtHotSpotClient )
	{
	switch( iContext )
	    {
	    case EHotSpotInProgress:
		if( iStatus != KErrNone )
		    {
		    if( iStatus == KErrNotSupported )
		        {
                DEBUG( "CWlanJoinState::RunL(), HotSpot Server returned KErrNotSupported, treating it as normal AP" );
                iWlanSM->SetHotSpotAP( EFalse );
                iContext = EHotSpotNormalAP;
                }
		    else
		        {
		        DEBUG( "CWlanJoinState::RunL(), Error returned by HotSpot Server" );
		        iContext = EHotSpotFailure;
		        }
		    iAgtHotSpotClient->HotSpotCloseConnection( iWlanSM->IapId(), iStatus );
		    SetActive();
		    }
		else
		    {
		    // send Join request to WLM server
		    DEBUG( "CWlanJoinState::RunL(), Sending Join request to Engine" );
		    iWLMServer.Join( iStatus, iWlanSM->SettingsId(), iWlanSM->OverrideSettings() );
		    iContext = EHotSpotWLMServerHS;
		    SetActive();
		    }
		break;
    
	    case EHotSpotWLMServerHS:
		if(iStatus != KErrNone)
		    {
		    DEBUG( "CWlanJoinState::RunL(), Join failed, Continue trying with HOTSPOT Server" );
		    iAgtHotSpotClient->HotSpotStartAgain( iWlanSM->IapId(), iStatus );
		    iContext = EHotSpotInProgress;
		    SetActive();
		    }
		else
		    {
		    DEBUG( "CWlanJoinState::RunL(), Successfully joined to network,  CloseConnection will be called from Disconnect State");
		    DEBUG( "CWlanJoinState::RunL(), Sending ServiceStarted indication to NIFMAN" );
		    iSMObserver->ServiceStarted();
		    iWlanSM->CompleteState( KErrNone );
		    }
		break;

	    case EHotSpotNormalAP:
		// send Join request to WLM server
		DEBUG( "CWlanJoinState::RunL(), Sending Join request to Engine" );
		iWLMServer.Join( iStatus, iWlanSM->SettingsId(), iWlanSM->OverrideSettings() );
		iContext = EHotSpotWLMServerNormal;
		SetActive();
		break;

	    case EHotSpotWLMServerNormal:
		if(iStatus != KErrNone)
		    {   
		    DEBUG1( "CWlanJoinState::RunL(), Join failed(%d), not retrying with HotSpot Server, as this is normal AP", iStatus.Int() );
		    iSMObserver->ConnectionComplete( EAttemptingToJoin, iStatus.Int() );
		    }
		else
		    {
		    DEBUG( "CWlanJoinState::RunL(), Successfully joined to network,Sending ServiceStarted indication to NIFMAN" );
		    iSMObserver->ServiceStarted();
		    iWlanSM->CompleteState( KErrNone );
		    }
		break;

	    case EHotSpotFailure:
		DEBUG( "CWlanJoinState::RunL(), Cancelling the Join operation" );
		iSMObserver->ConnectionComplete( EAttemptingToJoin, KErrCouldNotConnect );
		break;

	    default:
		DEBUG( "CWlanJoinState::RunL(), Unknown Active Object context, Panicing.." );
    		User::Panic( _L("wlanagt"), KErrGeneral );
    		break;
	    }
	}

    else
	{
	//Normal AP
	if(iStatus != KErrNone)
	    {
	    iSMObserver->ConnectionComplete( EAttemptingToJoin, iStatus.Int() );
	    return;
	    }
	else
	    {
	    DEBUG( "CWlanJoinState::RunL(), Successfully joined to network" );

	    // Nifman loads the NIF after a ServiceStarted() call
	    iSMObserver->ServiceStarted();
	    iWlanSM->CompleteState( KErrNone );
	    }
	}

    }

// CWlanJoinState::DoCancel
// -----------------------------------------------------------------------------
//
void CWlanJoinState::DoCancel()
    {
    DEBUG( "CWlanJoinState::DoCancel()" );
    if ( iAgtHotSpotClient )
        {
        //Cancel is sent to HSS even if there is no pending request, so that CloseConnection is not 
        //required to be called seperately
        DEBUG( "CWlanJoinState::DoCancel(), Sending Cancel to HotSpot Server" );
        iAgtHotSpotClient->HotSpotCancel( iWlanSM->IapId() );
        }

    // If Join request is sent to Engine, cancel it by sending Release request
    if ( !(iAgtHotSpotClient) || iContext == EHotSpotWLMServerHS || 
		iContext == EHotSpotWLMServerNormal )
        {
        DEBUG( "CWlanJoinState::DoCancel(), Sending Release to WLM Server" );
        iWLMServer.Release();
        }
    }

// -----------------------------------------------------------------------------
// CWlanOpenState::CWlanOpenState
// -----------------------------------------------------------------------------
//
CWlanOpenState::CWlanOpenState( CWlanSM* aWlanSM, RWLMServer& aWLMServer ) :
	CWlanStateBase( aWlanSM, aWLMServer ),
	iSubState( EWlanOpenSubStateOpening )
    {
	DEBUG( "CWlanOpenState constructor" );
    }

// -----------------------------------------------------------------------------
// CWlanOpenState::StartState
// -----------------------------------------------------------------------------
//
void CWlanOpenState::StartState()
    {
	DEBUG( "CWlanOpenState::StartState()" );
	TRAPD( ret, DoStartStateL() );
	JumpToRunl( ret );
    }

// -----------------------------------------------------------------------------
// CWlanOpenState::DoStartStateL
// -----------------------------------------------------------------------------
//
void CWlanOpenState::DoStartStateL()
    {
	DEBUG( "CWlanOpenState::DoStartStateL()" );

    // @to do: whether this callback system is needed anymore or not,
    // should be checked.
    
	// Create a callback for handling the "disconnect" notification from 
	// WLM. We must use async callback to avoid deleting iNotify of RWLMServer 
	// "too early" (ConnectionStateChanged() notification call from WLM must 
	// not result to calling CancelNotifies() of RWLMServer synchronously).
	TCallBack callback( DisconnectCb, this );
	iDisconnectCb = new (ELeave) CAsyncCallBack( callback, CActive::EPriorityStandard );

    }

// -----------------------------------------------------------------------------
// CWlanOpenState::NextStateL
// -----------------------------------------------------------------------------
//
CAgentStateBase* CWlanOpenState::NextStateL( TBool /*aContinue*/ )
    {
	DEBUG( "CWlanOpenState::NextStateL(), calling RWLMServer::CancelNotifies()" );
	iWLMServer.CancelNotifies();
	return new (ELeave) CWlanDisconnectState( iWlanSM, iWLMServer );
    }

// -----------------------------------------------------------------------------
// CWlanOpenState::RunL
// -----------------------------------------------------------------------------
//
void CWlanOpenState::RunL()
    {
	DEBUG( "CWlanOpenState::RunL()" );
	
	switch( iSubState )
	    {
	    case EWlanOpenSubStateOpening:
		    if( iStatus != KErrNone )
		        {
			    iSMObserver->ConnectionComplete( EOpeningConnection, iStatus.Int() );
			    return;
		        }
		    DEBUG( "CWlanOpenState::RunL(), Connection Complete with KErrNone" );
    		iSMObserver->ConnectionComplete( KConnectionOpen, KErrNone );
    		iSubState = EWlanOpenSubStateOpened;

    		DEBUG( "CWlanOpenState::RunL(), calling RWLMServer::ActivateNotifiesL()" );
    		iWLMServer.ActivateNotifiesL( *this, EWlmNotifyConnectionStateChanged );
    		break;
    	case EWlanOpenSubStateOpened:
    		User::Panic( _L("wlanagt"), KErrGeneral );
    		break;
    	default:
    		User::Panic( _L("wlanagt"), KErrGeneral );
    		break;
	    }
    }

// -----------------------------------------------------------------------------
// CWlanOpenState::DoCancel
// -----------------------------------------------------------------------------
//
void CWlanOpenState::DoCancel()
    {
	DEBUG( "CWlanOpenState::DoCancel()" );
    }

// -----------------------------------------------------------------------------
// CWlanOpenState::DisconnectCb
// -----------------------------------------------------------------------------
//
TInt CWlanOpenState::DisconnectCb( TAny* aThisPtr )
    {
	DEBUG( "CCWlanOpenState::DisconnectCb()" );

	CWlanOpenState* self = static_cast<CWlanOpenState*>( aThisPtr );

	// Send a "vendor specific" notification to wlannif. This is the only 
	// agent-to-nif notification we use, and always means that interface should 
	// be stopped.
	self->iSMObserver->Notification( EAgentToNifEventVendorSpecific, NULL );
	return KErrNone;
    }

// -----------------------------------------------------------------------------
// CWlanOpenState::ConnectionStateChanged
// -----------------------------------------------------------------------------
//
void CWlanOpenState::ConnectionStateChanged( TWlanConnectionState aNewState )
    {
	DEBUG1( "CWlanOpenState::ConnectionStateChanged(), new state %i", aNewState );

	// EWlanStateNotConnected is the only connection state we are interested in.
	if( aNewState == EWlanStateNotConnected )
    	{
		DEBUG( "CWlanOpenState::ConnectionStateChanged(), calling iDisconnectCb->CallBack()" );
	    	    
	    iDisconnectCb->CallBack();
        }
    }

// -----------------------------------------------------------------------------
// CWlanOpenState::~CWlanOpenState
// -----------------------------------------------------------------------------
//
CWlanOpenState::~CWlanOpenState()
    {
	DEBUG( "CWlanOpenState destructor" );
	delete iDisconnectCb;
    }

// -----------------------------------------------------------------------------
// CWlanDisconnectState::CWlanDisconnectState
// -----------------------------------------------------------------------------
//
CWlanDisconnectState::CWlanDisconnectState(CWlanSM* aWlanSM, RWLMServer& aWLMServer) :
	CWlanStateBase(aWlanSM, aWLMServer)
    {
    DEBUG( "CWlanDisconnectState constructor" );
    }

// -----------------------------------------------------------------------------
// CWlanDisconnectState::~CWlanDisconnectState
// -----------------------------------------------------------------------------
//
CWlanDisconnectState::~CWlanDisconnectState()
    {
	DEBUG( "CWlanDisconnectState destructor" );
	Cancel();
	if ( iAgtHotSpotClient )
	    {
	    delete iAgtHotSpotClient;
	    }
    }

// -----------------------------------------------------------------------------
// CWlanDisconnectState::StartState
// -----------------------------------------------------------------------------
//
void CWlanDisconnectState::StartState()
    {
    DEBUG( "CWlanDisconnectState::StartState()" );
    // check if plug-in is available
   TRAPD( ret, iAgtHotSpotClient = CWlanAgtHotSpotClient::NewL() );

    if( ret == KErrNone )
	{
	DEBUG( "CWlanDisconnectState::StartState(), AgtHotSpot plugin is available" );

	if ( iWlanSM->IsHotSpotAP() )
	    {
	    // talk to hospot server	
	    DEBUG( "CWlanDisconnectState::StartState(), Sending CloseConnection message to HotSpot Server" );
	    iAgtHotSpotClient->HotSpotCloseConnection( iWlanSM->IapId(), iStatus );
	    SetActive();
	    return;
	    }
	}
    else 
	{
	iAgtHotSpotClient = NULL;
	}
    JumpToRunl( KErrNone );
    }


// -----------------------------------------------------------------------------
// CWlanDisconnectState::NextStateL
// -----------------------------------------------------------------------------
//
CAgentStateBase* CWlanDisconnectState::NextStateL( TBool /*aContinue*/ )
    {
	DEBUG( "CWlanDisconnectState::NextStateL()" );
	return new (ELeave) CWlanDisconnectState( iWlanSM, iWLMServer );
    }

// -----------------------------------------------------------------------------
// CWlanDisconnectState::RunL
// -----------------------------------------------------------------------------
//
void CWlanDisconnectState::RunL()
    {
	DEBUG( "CWlanDisconnectState::RunL()" );
	iSMObserver->UpdateProgress( EDisconnecting, KErrNone );
	DEBUG( "CWlanDisconnectState::RunL(), calling RWLMServer::Release()" );

	// Only call iWLMServer.Release() and assume that disconnect is complete.
	iWLMServer.Release();
	iSMObserver->DisconnectComplete();
    }

// -----------------------------------------------------------------------------
// CWlanDisconnectState::DoCancel
// -----------------------------------------------------------------------------
//
void CWlanDisconnectState::DoCancel()
    {
    DEBUG( "CWlanDisconnectState::DoCancel()" );
    if( iAgtHotSpotClient )
	{
	iAgtHotSpotClient->HotSpotCancel( iWlanSM->IapId() );
	}
    }

// -----------------------------------------------------------------------------
// CWlanErrorState::CWlanErrorState
// -----------------------------------------------------------------------------
//
// CWlanErrorState
//
// "Dummy state" that only reports an error.
// Currently only used when the CWlanSM cannot be constructed properly.
//
CWlanErrorState::CWlanErrorState( CWlanSM* aWlanSM, TInt aError ) :
	CAgentStateBase( *aWlanSM ),
	iWlanSM( aWlanSM ),
	iError( aError )
    {
	DEBUG( "CWlanErrorState constructor" );
    }

// -----------------------------------------------------------------------------
// CWlanErrorState::StartState
// -----------------------------------------------------------------------------
//
void CWlanErrorState::StartState()
    {
	DEBUG( "CWlanErrorState::StartState()" );
	JumpToRunl( KErrNone );
    }

// -----------------------------------------------------------------------------
// CWlanErrorState::NextStateL
// -----------------------------------------------------------------------------
//
CAgentStateBase* CWlanErrorState::NextStateL( TBool /*aContinue*/ )
    {
	DEBUG( "CWlanErrorState::NextStateL()" );
	return new (ELeave) CWlanErrorState( iWlanSM, iError );
    }

// -----------------------------------------------------------------------------
// CWlanErrorState::RunL
// -----------------------------------------------------------------------------
//
void CWlanErrorState::RunL()
    {
	DEBUG( "CWlanErrorState::RunL()" );
	iWlanSM->CompleteState( iError );
    }

// -----------------------------------------------------------------------------
// CWlanErrorState::DoCancel
// -----------------------------------------------------------------------------
//
void CWlanErrorState::DoCancel()
    {
	DEBUG( "CWlanErrorState::DoCancel" );
    }

// -----------------------------------------------------------------------------
// CWlanWaitConnectionState::CWlanWaitConnectionState
// -----------------------------------------------------------------------------
//
CWlanWaitConnectionState::CWlanWaitConnectionState(
    CWlanSM* aWlanSM, 
    RWLMServer& aWLMServer ) :
    CWlanStateBase( aWlanSM, aWLMServer )
    {
    DEBUG(  "CWlanWaitConnectionState::CWlanWaitConnectionState()"  );
    }

// -----------------------------------------------------------------------------
// CWlanWaitConnectionState::~CWlanWaitConnectionState
// -----------------------------------------------------------------------------
//
CWlanWaitConnectionState::~CWlanWaitConnectionState()
    {
    DEBUG( "CWlanWaitConnectionState::~CWlanWaitConnectionState()" );
    iTimer.Close();
    }

// -----------------------------------------------------------------------------
// CWlanWaitConnectionState::StartState
// -----------------------------------------------------------------------------
//
void CWlanWaitConnectionState::StartState()
    {
    DEBUG( "CWlanWaitConnectionState::StartState()" );
    
    DEBUG1( "CWlanWaitConnectionState::StartState() - setting the timer to %u microseconds",
        KConnectionEndWaitTime );
    
    TInt ret = iTimer.CreateLocal();
    if ( ret != KErrNone )
        {
        DEBUG1( "CWlanWaitConnectionState::StartState() - unable to create the timer (%d)",
            ret );
            
        JumpToRunl( ret );
        }

    iTimer.After( iStatus, KConnectionEndWaitTime );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CWlanWaitConnectionState::NextStateL
// -----------------------------------------------------------------------------
//
CAgentStateBase* CWlanWaitConnectionState::NextStateL(TBool aContinue)
    {
    DEBUG( "CWlanWaitConnectionState::NextStateL()"  );
    DEBUG1( "CWlanWaitConnectionState::NextStateL() - aContinue %u",
        aContinue );

	if( aContinue )
	{ 
		return new (ELeave) CWlanStartState( iWlanSM, iWLMServer );   
	}

	return new (ELeave) CWlanErrorState( iWlanSM, KErrWlanConnAlreadyActive );
    }

// -----------------------------------------------------------------------------
// CWlanWaitConnectionState::RunL
// -----------------------------------------------------------------------------
//  
void CWlanWaitConnectionState::RunL()
    {
    DEBUG( "CWlanWaitConnectionState::RunL()"  );
    DEBUG1( "CWlanWaitConnectionState::RunL() - iStatus %d" , iStatus.Int() );

	if ( iStatus.Int()!= KErrNone )
	    {
		iSMObserver->ConnectionComplete( EAttemptingToJoin, iStatus.Int() );
	    }
	else
	    {
	    TWlanConnectionState state = iWLMServer.GetConnectionState();

	    DEBUG1(  "CWlanWaitConnectionState::RunL() - WLAN engine state is %u",
            state );
	    
	    if ( state != EWlanStateNotConnected )
	        {
	        DEBUG( "CWlanWaitConnectionState::RunL() - connection is still open" );
	        DEBUG( "CWlanWaitConnectionState::RunL() - completing request with KErrWlanConnAlreadyActive" );

		      iSMObserver->ConnectionComplete( EAttemptingToJoin, KErrWlanConnAlreadyActive );		
	        }
            else
            	{
            	DEBUG( "CWlanWaitConnectionState::RunL() - connection has closed, continuing with request" );
            	iWlanSM->CompleteState( KErrNone );
            	}
	    }
    }

// -----------------------------------------------------------------------------
// CWlanWaitConnectionState::DoCancel
// -----------------------------------------------------------------------------
//     
void CWlanWaitConnectionState::DoCancel()
    {
    DEBUG( "CWlanWaitConnectionState::DoCancel()" );
    iTimer.Cancel();
    }
