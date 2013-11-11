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
* Description:  Wlan agent's state machine
*
*/

/*
* %version: 9 %
*/

#ifndef _WLANSM_H_
#define _WLANSM_H_

#include <comms-infras/cagentsmbase.h>
#include "rwlmserver.h"

struct SWLANSettings;

/**
 * WLAN agent statemachine
 * 
 *  @lib wlanagt.agt
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CWlanSM ): public CAgentSMBase
{
public:

    /**
     * NewL
     *
     * @since S60 v3.0
     * @param aObserver
     * @param aDlgPrc
     * @param aDbAccess
     * @return instance of statemachine
     */
	static CWlanSM* NewL(
	    MAgentNotify& aObserver, 
	    CDialogProcessor* aDlgPrc, 
	    CCommsDbAccess& aDbAccess );	

    /**
     * destructor
     *
     * @since S60 v3.0
     */
	~CWlanSM();

public:	// From CAgentSMBase

    /**
     * GetExcessData
     *
     * @since S60 v3.0
     * @param aBuffer
     * @return
     */
	virtual TInt GetExcessData( TDes8& aBuffer );

    /**
     * Notification
     *
     * @since S60 v3.0
     * @param aEvent
     * @param aInfo
     */
	virtual TInt Notification( TNifToAgentEventType aEvent, TAny* aInfo );


    /**
     * SetOverrideSettingsL
     *
     * @since S60 v3.0
     * @param aWLMOverrideSettings
     */
	void SetOverrideSettingsL( const TWLMOverrideSettings& aWLMOverrideSettings );

    /**
     * WlanSettings
     *
     * @since S60 v3.0
     * @return pointer to SWLANSettings structure
     */
	const SWLANSettings* WlanSettings() const;

    /**
     * SettingsId
     *
     * @since S60 v3.0
     * @return Id of the chosen IAP
     */
	TUint32 SettingsId() const;

    /**
     * IapId
     *
     * @since S60 v3.0
     * @return Id of the chosen IAP
     */
	TUint32 IapId() const;

    /**
     * OverrideSettings
     *
     * @since S60 v3.0
     * @param overridesettings object
     */
	TWLMOverrideSettings* OverrideSettings() const;

    /**
     * SetHotSpotAP
     *
     * @since S60 v3.2
     * @Sets the flag indicating if the AP is HotSpot AP or Normal AP
     */
	void SetHotSpotAP(TBool aHotSpotAP);

    /**
     * IsHotSpotAP
     *
     * @since S60 v3.2
     * @Returns the flag indicating if the AP is HotSpot AP or Normal AP
     */
	TBool IsHotSpotAP() const;


protected:

    /**
     * constructor
     *
     * @since S60 v3.0
     * @param aObserver
     * @param aDlgPrc
     * @param aDbAccess
     */
	CWlanSM(
	    MAgentNotify& aObserver, 
	    CDialogProcessor* aDlgPrc, 
	    CCommsDbAccess& aDbAccess );

    /**
     * ConstructL
     *
     * @since S60 v3.0
     */
	void ConstructL();

private:

    /**
     * DoConstructL
     *
     * @since S60 v3.0
     */
	void DoConstructL();

private:
    /** Client API instance of the WLAN Engine */
	RWLMServer iWLMServer;
    /** WLAN Settings of the IAP from CommsDat*/
	SWLANSettings* iWlanSettings;
    /** An index for accessing IAP specific WLAN settings from CommsDat */
	TUint32 iServiceId;
    /** 
     * Structure that can convey connection parameters which should override
     * the ones in CommsDat
     */
	TWLMOverrideSettings* iWLMOverrideSettings;
    /** Identifier of the IAP */
	TUint32 iIapId;

    /** HotSpot related variable
     */
	TBool   iIsHotSpotAP;
};

#endif // _WLANSM_H_
