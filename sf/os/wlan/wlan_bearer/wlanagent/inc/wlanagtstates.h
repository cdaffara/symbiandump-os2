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
* Description:  States for WLAN Agent's state machine
*
*/

/*
* %version: 19 %
*/

#ifndef _WLANSTATES_H_
#define _WLANSTATES_H_

#include <comms-infras/cagentsmbase.h>
#include "rwlmserver.h"
#include "wlanmgmtcommon.h"
#include "wlanagthotspotclient.h"

 // WLAN uids
 
// ID of SelectWLan dialog
const TUid KUidSelectWLanDlg = { 0x10207373 };

// ID of OfflineWlanNote dialog
const TUid KUidCOfflineWlanNoteDlg = { 0x101FD671 };

// ID of OfflineWlanDisabledNote dialog
const TUid KUidCOfflineWlanDisabledNoteDlg = { 0x101FD672 };

// ID of Easy Wep dialog
const TUid KUidEasyWepDlg = { 0x101FD673 };

// ID of Easy Wpa dialog
const TUid KUidEasyWpaDlg = { 0x101FD674 };

// ID of WLANNetworkUnavailableNote dialog
const TUid KUidWLANNetworkUnavailableNoteDlg = { 0x101FD67F };

// ID of Connecting Note dialog
const TUid KUidConnectingNoteDlg = { 0x101FD681 };

const TInt KEasyWepQuery256BitMaxLength = 58;

// Values of TWlanagtProgress are limited by the following (from nifvar.h):
//const TInt KMinAgtProgress           = 2500;
//const TInt KConnectionOpen           = 3500;           // From an Agent
//const TInt KConnectionClosed         = 4500;           // From an Agent
//const TInt KMaxAgtProgress           = 5500;
enum TWlanagtProgress		
    {
	EModeCheck = 3000,
	EEasyConn = 3020,
	EEasyWpa = 3022,
	EEasyWep = 3024,
	EAttemptingToJoin = 3030,
	EOpeningConnection = 3040,
	EDisconnecting = 4000
    };

class CWlanSM;
class ScanInfo;

/**
 * CWlanStateBase is the baseclass of all the WLAN Agent concrete states
 * 
 * @lib wlanagt.agt
 * @since S60 v3.0
 */
NONSHARABLE_CLASS( CWlanStateBase ) : public CAgentStateBase, public MWLMNotify
    {
public:

    /**
     * constructor
     *
     * @since S60 v3.0
     * @param aWlanSM pointer to WLAN Agent's statemachine framework
     * @param aWLMServer reference to WLAN Engine client interface
     */
	CWlanStateBase( CWlanSM* aWlanSM, RWLMServer& aWLMServer );

public:	// From CAgentStateBase

    /**
     * StartState is called by the framework after new state has been instantiated
     *
     * @since S60 v3.0
     */
	virtual void StartState();

public:	// From MWLMNotify

    /**
     * ConnectionStateChanged notification is received when change
     * in connection state occurs
     *
     * @since S60 v3.0
     */
	virtual void ConnectionStateChanged( TWlanConnectionState aNewState );

protected:
    /** pointer to agent's statemachine */
	CWlanSM* iWlanSM;
    /** reference to WLAN Engine client interface */
	RWLMServer& iWLMServer;
    };


/**
 * CWlanStartState is used when starting a connection
 * 
 * @lib wlanagt.agt
 * @since S60 v3.0
 */
NONSHARABLE_CLASS( CWlanStartState ) : public CWlanStateBase
    {
    enum TNextState
        {
    	EWlanAgtNextStateUnknown,
    	EWlanAgtNextStateJoin,
    	EWlanAgtNextStateOpen,
    	EWlanAgtNextStateEasyConn
        };

public:

    /**
     * constructor
     *
     * @since S60 v3.0
     * @param aWlanSM pointer to WLAN Agent's statemachine framework
     * @param aWLMServer reference to WLAN Engine client interface
     */
	CWlanStartState( CWlanSM* aWlanSM, RWLMServer& aWLMServer );

public:	// From CAgentStateBase

    /**
     * NextStateL
     *
     * @since S60 v3.0
     * @param aContinue specifies whether to continue normally or not
     * @return instance of the state that is entered next
     * (depends on the aContinue parameter)
     */
	virtual CAgentStateBase* NextStateL( TBool aContinue );

protected: // From CActive
	
    /**
     * RunL
     *
     * @since S60 v3.0
     */
	virtual void RunL();

    /**
     * DoCancel
     *
     * @since S60 v3.0
     */
	virtual void DoCancel();
	
private:
	/**
	 * PrepareEasyWlanParamsL
	 *
	 * @since S60 v3.2
	 */
	void PrepareEasyWlanParamsL();

private:
    /** specifies the next state */
	TNextState iNextState;
    };






/**
 * CWlanJoinState handles the connection attempt to WlanEngine
 * 
 * @lib wlanagt.agt
 * @since S60 v3.0
 */
NONSHARABLE_CLASS( CWlanJoinState ): public CWlanStateBase
    {

    enum TActiveObjectContext
        {
    	EHotSpotInProgress,
    	EHotSpotFailure,
    	EHotSpotNormalAP,
    	EHotSpotWLMServerHS,
    	EHotSpotWLMServerNormal
        };

public:

    /**
     * constructor
     *
     * @since S60 v3.0
     * @param aWlanSM pointer to WLAN Agent's statemachine framework
     * @param aWLMServer reference to WLAN Engine client interface
     */
	CWlanJoinState( CWlanSM* aWlanSM, RWLMServer& aWLMServer );

	// Destructor
	~CWlanJoinState();

	// From CAgentStateBase
    /**
     * StartState is called by the framework after new state has been instantiated
     *
     * @since S60 v3.0
     */
	virtual void StartState();

    /**
     * NextStateL
     *
     * @since S60 v3.0
     * @param aContinue specifies whether to continue normally or not
     * @return instance of the state that is entered next
     * (depends on the aContinue parameter)
     */
	virtual CAgentStateBase* NextStateL( TBool aContinue );


protected: // From CActive

    /**
     * RunL
     *
     * @since S60 v3.0
     */
	virtual void RunL();

    /**
     * DoCancel
     *
     * @since S60 v3.0
     */
	virtual void DoCancel();

private:

    /**
     * DoStartStateL
     *
     * @since S60 v3.0
     */
	void DoStartStateL();

private:

	//indicates the Active Object context of the state
	TActiveObjectContext iContext;

	// pointer to ECOM plug-in instance
	CWlanAgtHotSpotClient  *iAgtHotSpotClient;
    };

/**
 * CWlanOpenState is active when there is an active connection
 * 
 * @lib wlanagt.agt
 * @since S60 v3.0
 */
NONSHARABLE_CLASS( CWlanOpenState ): public CWlanStateBase
    {
	enum TSubState
    	{
		EWlanOpenSubStateOpening,
		EWlanOpenSubStateOpened,
		EWlanOpenSubStateRenewingDHCP
    	};

public:

    /**
     * constructor
     *
     * @since S60 v3.0
     * @param aWlanSM pointer to WLAN Agent's statemachine framework
     * @param aWLMServer reference to WLAN Engine client interface
     */
	CWlanOpenState( CWlanSM* aWlanSM, RWLMServer& aWLMServer );

    /**
     * destructor
     *
     * @since S60 v3.0
     */
	virtual ~CWlanOpenState();

public:	// From CAgentStateBase
    /**
     * StartState is called by the framework after new state has been instantiated
     *
     * @since S60 v3.0
     */
	virtual void StartState();

    /**
     * NextStateL
     *
     * @since S60 v3.0
     * @param aContinue specifies whether to continue normally or not
     * @return instance of the state that is entered next
     * (depends on the aContinue parameter)
     */
	virtual CAgentStateBase* NextStateL( TBool aContinue );

public:	// From MWLMNotify
    /**
     * ConnectionStateChanged notification tells about chnages in the connection state
     *
     * @since S60 v3.0
     * @param aNewState specifies the new state
     */
	virtual void ConnectionStateChanged( TWlanConnectionState aNewState );

protected: // From CActive

    /**
     * RunL
     *
     * @since S60 v3.0
     */
	virtual void RunL();

    /**
     * DoCancel
     *
     * @since S60 v3.0
     */
	virtual void DoCancel();

private:

    /**
     * DoStartStateL
     *
     * @since S60 v3.0
     */
	void DoStartStateL();

    /**
     * DisconnectCb
     *
     * @since S60 v3.0
     * @param aThisPtr pointer to 'this'
     */
	static TInt DisconnectCb( TAny* aThisPtr );

private:
    /** substate of the connection */
	TSubState iSubState;
    /** callback pointer */
	CAsyncCallBack* iDisconnectCb;
	//Interfce to Notifier
	//RNotifier iNotifier;
    };

/**
 * CWlanDisconnectState initiaes disconnection towards WlanEngine
 * 
 * @lib wlanagt.agt
 * @since S60 v3.0
 */
NONSHARABLE_CLASS( CWlanDisconnectState ): public CWlanStateBase
    {
public:

    /**
     * constructor
     *
     * @since S60 v3.0
     * @param aWlanSM pointer to WLAN Agent's statemachine framework
     * @param aWLMServer reference to WLAN Engine client interface
     */
	CWlanDisconnectState(CWlanSM* aWlanSM, RWLMServer& aWLMServer);

    /**
     * destructor
     *
     * @since S60 v3.0
     */
	virtual ~CWlanDisconnectState();

	// From CAgentStateBase
	//
     /* StartState is called by the framework after new state has been instantiated
     *
     * @since S60 v3.0
     */
	virtual void StartState();

    /**
     * NextStateL
     *
     * @since S60 v3.0
     * @param aContinue specifies whether to continue normally or not
     * @return instance of the state that is entered next
     * (depends on the aContinue parameter)
     */
	virtual CAgentStateBase* NextStateL(TBool aContinue);

protected: // From CActive

    /**
     * RunL
     *
     * @since S60 v3.0
     */
	virtual void RunL();

    /**
     * DoCancel
     *
     * @since S60 v3.0
     */
	virtual void DoCancel();

private:

    /** pointer to ECOM plug-in instance */
        CWlanAgtHotSpotClient  *iAgtHotSpotClient;
    };

/**
 * CWlanErrorState is a "Dummy state" used when the CWlanSM cannot
 * be constructed properly.
 * 
 * @lib wlanagt.agt
 * @since S60 v3.0
 */
NONSHARABLE_CLASS( CWlanErrorState ): public CAgentStateBase
    {
public:

    /**
     * constructor
     *
     * @since S60 v3.0
     * @param aWlanSM pointer to WLAN Agent's statemachine framework
     * @param aWLMServer reference to WLAN Engine client interface
     */
	CWlanErrorState( CWlanSM* aWlanSM, TInt aError );

	// From CAgentStateBase
    /**
     * StartState is called by the framework after new state has been instantiated
     *
     * @since S60 v3.0
     */
	virtual void StartState();

    /**
     * NextStateL
     *
     * @since S60 v3.0
     * @param aContinue specifies whether to continue normally or not
     * @return instance of the state that is entered next
     * (depends on the aContinue parameter)
     */
	virtual CAgentStateBase* NextStateL(TBool aContinue);

protected: // From CActive

    /**
     * RunL
     *
     * @since S60 v3.0
     */
	virtual void RunL();

    /**
     * DoCancel
     *
     * @since S60 v3.0
     */
	virtual void DoCancel();

protected:
    /** pointer to WLAN agent's state machine */
	CWlanSM* iWlanSM;
	
private:
    /** error code */
	TInt iError;
    };

/**
 * CWlanWaitConnectionState is used for waiting previous connection to close down.
 * 
 * @lib wlanagt.agt
 * @since S60 v3.0
 */
NONSHARABLE_CLASS( CWlanWaitConnectionState ): public CWlanStateBase
    {
public:
    
    /**
     * constructor
     *
     * @since S60 v3.0
     * @param aWlanSM pointer to WLAN Agent's statemachine framework
     * @param aWLMServer reference to WLAN Engine client interface
     */
    CWlanWaitConnectionState( CWlanSM* aWlanSM, RWLMServer& aWLMServer );

    /**
     * destructor
     *
     * @since S60 v3.0
     */
    virtual ~CWlanWaitConnectionState();
    
public:	// From CAgentStateBase

    /**
     * StartState is called by the framework after new state has been instantiated
     *
     * @since S60 v3.0
     */
	void StartState();

    /**
     * NextStateL
     *
     * @since S60 v3.0
     * @param aContinue specifies whether to continue normally or not
     * @return instance of the state that is entered next
     * (depends on the aContinue parameter)
     */
	CAgentStateBase* NextStateL(TBool aContinue);

protected: // From CActive

    /**
     * RunL
     *
     * @since S60 v3.0
     */
	void RunL();

    /**
     * DoCancel
     *
     * @since S60 v3.0
     */
	void DoCancel();
	
private:
    /** Timer for waiting */
    RTimer iTimer;
    };

#endif // _WLANSTATES_H_
