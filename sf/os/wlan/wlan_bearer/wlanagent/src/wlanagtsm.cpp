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
* Description:  Implementation of WLAN Agent statemachine framework
*
*/

/*
* %version: 11 %
*/

#include <comms-infras/dbaccess.h>
#include <wdbifwlansettings.h>
#include "am_debug.h"
#include "wlanagtsm.h"
#include "wlanagtstates.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWlanSM::NewL
// -----------------------------------------------------------------------------
//
CWlanSM* CWlanSM::NewL(
    MAgentNotify& aObserver, 
    CDialogProcessor* aDlgPrc, 
    CCommsDbAccess& aDbAccess )
    {
	DEBUG( "CWlanSM::NewL()" );
	CWlanSM* self = new (ELeave) CWlanSM(aObserver, aDlgPrc, aDbAccess);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CWlanSM::CWlanSM
// -----------------------------------------------------------------------------
//
CWlanSM::CWlanSM(
    MAgentNotify& aObserver, 
    CDialogProcessor* aDlgPrc, 
    CCommsDbAccess& aDbAccess ) :
	CAgentSMBase( aObserver, aDlgPrc, aDbAccess ),
	iWlanSettings( NULL ),
	iServiceId( 0 ),
	iWLMOverrideSettings( NULL ),
	iIapId( 0 ),
	iIsHotSpotAP (EFalse)
    {
	DEBUG( "CWlanSM constructor" );
    }
    
// -----------------------------------------------------------------------------
// CWlanSM::ConstructL
// -----------------------------------------------------------------------------
//
void CWlanSM::ConstructL()
    {
	DEBUG( "CWlanSM::ConstructL()" );

	TRAPD( err, DoConstructL() );
	
	if( err == KErrNone )
	    {
	    TWlanConnectionState state = iWLMServer.GetConnectionState();

	    DEBUG1( "CWlanSM::ConstructL() - WLAN engine state is %u", state );
	    
	    if( state == EWlanStateNotConnected )
	        {
	        iState = new(ELeave) CWlanStartState( this, iWLMServer );
	        }
	    else
	    	{
	    	iState = new(ELeave) CWlanWaitConnectionState( this, iWLMServer );
	    	}
	    }
	else
	    {
#ifdef _DEBUG
		User::InfoPrint(_L("wlanagt: state machine creation error"));
#endif
		iState = new(ELeave) CWlanErrorState( this, KErrCouldNotConnect );
	    }
    }

// -----------------------------------------------------------------------------
// CWlanSM::DoConstructL
// -----------------------------------------------------------------------------
//
void CWlanSM::DoConstructL()
    {
	DEBUG( "CWlanSM::DoConstructL()" );
	User::LeaveIfError(iWLMServer.Connect());

	// Check the service type in current IAP, must be LANService.
	TBuf<KCommsDbSvrMaxFieldLength> serviceType;
	Db()->GetDesL( TPtrC( IAP ), TPtrC( IAP_SERVICE_TYPE ), serviceType );
	if( serviceType != TPtrC( LAN_SERVICE ) )
	    {
		User::Leave(KErrCorrupt);
	    }

	// Get the current IAP id
	Db()->GetIntL( TPtrC( IAP ), TPtrC( COMMDB_ID ), iIapId );
	DEBUG1("iap id:%d", iIapId);

	// Get the service ID from the IAP table
	Db()->GetIntL( TPtrC( IAP ), TPtrC( IAP_SERVICE ), iServiceId );
	DEBUG1("service id:%d", iServiceId);

	iWlanSettings = new (ELeave) SWLANSettings;

	CWLanSettings* wlansettings = new (ELeave) CWLanSettings();
	CleanupStack::PushL( wlansettings );
	User::LeaveIfError( wlansettings->Connect() );

	// Get the "IAP specific WLAN settings" for this LAN service ID
	TInt err = wlansettings->GetWlanSettingsForService( iServiceId, *iWlanSettings );
	wlansettings->Disconnect();

	// wlansettings->Disconnect() need to be called also if wlansettings->
	// GetWlanSettingsForService() returns an error. This is why we didn't use 
	// User::LeaveIfError() above directly.
	User::LeaveIfError(err);

	CleanupStack::PopAndDestroy( wlansettings );	
    }

// -----------------------------------------------------------------------------
// CWlanSM::GetExcessData
// -----------------------------------------------------------------------------
//
TInt CWlanSM::GetExcessData(TDes8& /*aBuffer*/)
    {
	DEBUG( "CWlanSM::GetExcessData()" );
	return KErrNone;
    }

// -----------------------------------------------------------------------------
// CWlanSM::Notification
// -----------------------------------------------------------------------------
//
TInt CWlanSM::Notification(TNifToAgentEventType /*aEvent*/, TAny* /*aInfo*/)
    {
	DEBUG( "CWlanSM::Notification()" );
	return KErrNone;
    }

// -----------------------------------------------------------------------------
// CWlanSM::SetOverrideSettingsL
// -----------------------------------------------------------------------------
//
void CWlanSM::SetOverrideSettingsL(const TWLMOverrideSettings& aWLMOverrideSettings)
    {
	if(!iWLMOverrideSettings)
	    {
		iWLMOverrideSettings = new (ELeave) TWLMOverrideSettings;
	    }

	iWLMOverrideSettings->settingsMask = aWLMOverrideSettings.settingsMask;
	iWLMOverrideSettings->ssid = aWLMOverrideSettings.ssid;
	iWLMOverrideSettings->bssid = aWLMOverrideSettings.bssid;
	iWLMOverrideSettings->wep = aWLMOverrideSettings.wep;
	iWLMOverrideSettings->wpaPsk = aWLMOverrideSettings.wpaPsk;
    }

// -----------------------------------------------------------------------------
// CWlanSM::~CWlanSM
// -----------------------------------------------------------------------------
//
CWlanSM::~CWlanSM()
    {
	DEBUG( "CWlanSM destructor" );

	delete iWlanSettings;
	delete iWLMOverrideSettings;

	iWLMServer.Close();

    }

// -----------------------------------------------------------------------------
// CWlanSM::WlanSettings
// -----------------------------------------------------------------------------
//
const SWLANSettings* CWlanSM::WlanSettings() const
    {
	return iWlanSettings;
    }

// -----------------------------------------------------------------------------
// CWlanSM::SettingsId
// -----------------------------------------------------------------------------
//
TUint32 CWlanSM::SettingsId() const
    {
	return iServiceId;
    }

// -----------------------------------------------------------------------------
// CWlanSM::IapId
// -----------------------------------------------------------------------------
//
TUint32 CWlanSM::IapId() const
    {    
	return iIapId;
    }

// -----------------------------------------------------------------------------
// CWlanSM::SetHotSpotAP
// -----------------------------------------------------------------------------
//
void CWlanSM::SetHotSpotAP(TBool aHotSpotAP)
    {    
	iIsHotSpotAP = aHotSpotAP;
    }

// -----------------------------------------------------------------------------
// CWlanSM::IsHotSpotAP
// -----------------------------------------------------------------------------
//
TBool CWlanSM::IsHotSpotAP() const
    {    
	return iIsHotSpotAP;
    }

// -----------------------------------------------------------------------------
// CWlanSM::OverrideSettings
// -----------------------------------------------------------------------------
//
TWLMOverrideSettings* CWlanSM::OverrideSettings() const
    {
	return iWLMOverrideSettings;
    }

