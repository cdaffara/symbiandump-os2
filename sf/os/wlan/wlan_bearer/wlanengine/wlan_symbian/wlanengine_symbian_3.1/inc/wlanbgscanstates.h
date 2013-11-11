/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class implementing WLAN background scan logic
*
*/

/*
* %version: 4 %
*/

#ifndef WLANBGSCANSTATES_H
#define WLANBGSCANSTATES_H

//#include "wlanbgscan.h"
#include "awsinterface.h"
#include "awsenginebase.h"
#include "wlanscanproviderinterface.h"
#include "wlantimerserviceinterface.h"
#include "wlanbgscanawscommsinterface.h"
#include "wlanbgscancommandlistener.h"

// forward declarations
class CWlanBgScan;
class CWlanTimer;

/**
 *  WLAN Background Scan States
 *  This class implements WLAN Background Scan state machine.
 *  
 *  @since S60 S60 v5.2
 */
NONSHARABLE_CLASS( CWlanBgScanStates )
    {

public:
        
    /**
     * States for WLAN Background Scan.
     */
    enum TWlanBgScanState
        {
        EBgScanInit = 0,
        EBgScanOff,
        EBgScanOn,
        EBgScanAuto,
        EBgScanAutoAws,
        EBgScanSendingAwsCmd,
        EBgScanStateMax
        };
    
    /**
     * Events for state machine.
     */
    enum TWlanBgScanSmEvent
        {
        EBgScanEventAwsCmdComplete = 0,
        EBgScanEventIntervalChanged,
        EBgScanEventAwsStartupComplete,
        EBgScanEventAwsIntervalChanged
        };

    /**
     * Constructor.
     */
    CWlanBgScanStates( CWlanBgScan& aBgScan, MWlanScanResultProvider& aProvider, MWlanTimerServices& aTimerServices );
    
    /**
     * Two-phased constructor.
     */
    void ConstructL();
    
    /**
    * Destructor.
    */
    virtual ~CWlanBgScanStates();
    
    /**
     * Start aggressive background scanning.
     *
     * @since S60 v5.2
     * @param aInterval interval for aggressive BG scanning, in seconds
     * @param aDuration aggressive mode duration, in microseconds
     */
    void StartAggressiveBgScan( TUint32& aInterval, TUint32& aDuration );
    
    /**
     * Stop aggressive scanning.
     *
     * @since S60 v5.2
     * @param aPtr pointer to CWlanBgScanStates instance
     * @return always KErrNone
     */
    static TInt StopAggressiveBgScan( TAny* aPtr );
    
private:
   
    /**
     * Default constructor, no implementation.
     */
    CWlanBgScanStates();
    
    /**
     * State machine for Init state
     *
     * @since S60 v5.2
     * @param aState reference to state
     * @param aEvent event to handle
     */
    void InStateInit( TWlanBgScanState& aState, const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent );
    
    /**
     * State machine for Off state
     *
     * @since S60 v5.2
     * @param aState reference to state
     * @param aEvent event to handle
     */
    void InStateOff( TWlanBgScanState& aState, const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent );
    
    /**
     * State machine for On state
     *
     * @since S60 v5.2
     * @param aState reference to state
     * @param aEvent event to handle
     */
    void InStateOn( TWlanBgScanState& aState, const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent );

    /**
     * State machine for Auto state
     *
     * @since S60 v5.2
     * @param aState reference to state
     * @param aEvent event to handle
     */
    void InStateAuto( TWlanBgScanState& aState, const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent );
    
    /**
     * State machine for Auto with AWS state
     *
     * @since S60 v5.2
     * @param aState reference to state
     * @param aEvent event to handle
     */
    void InStateAutoAws( TWlanBgScanState& aState, const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent );
    
    /**
     * State machine for Sending AWS command state
     *
     * @since S60 v5.2
     * @param aState reference to state
     * @param aEvent event to handle
     */
    void InStateSendingAwsCmd( TWlanBgScanState& aState, const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent );

    /**
     * Handles state transition from SendingAwsCmd state to Off state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     */
    void StateChangeSendingAwsCmdToOff( TWlanBgScanState& aState );
    
    /**
     * Handles state transition from SendingAwsCmd state to On state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     */
    void StateChangeSendingAwsCmdToOn( TWlanBgScanState& aState );

    /**
     * Handles state transition from SendingAwsCmd state to Auto state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     */
    void StateChangeSendingAwsCmdToAuto( TWlanBgScanState& aState );
    
    /**
     * Handles state transition from SendingAwsCmd state to AutoAws state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     */
    void StateChangeSendingAwsCmdToAutoAws( TWlanBgScanState& aState );
    
    /**
     * Handles state transition from Init to Automatic state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     */
    void StateChangeInitToAutoAws( TWlanBgScanState& aState );
    
    /**
     * Handles state transition from AutoAws to On state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     */
    void StateChangeAutoAwsToOn( TWlanBgScanState& aState );
    
    /**
     * Handles state transition from AutoAws to Off state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     */
    void StateChangeAutoAwsToOff( TWlanBgScanState& aState );
    
    /**
     * Handles state transition from Auto to Off state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     */
    void StateChangeAutoToOff( TWlanBgScanState& aState );
    
    /**
     * Handles state transition from Auto to Auto state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     */
    void StateChangeAutoToAuto( TWlanBgScanState& aState );
    
    /**
     * Handles state transition from Auto to On state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     * @param aNewBgScanSetting new background scan setting to be taken into use
     */
    void StateChangeAutoToOn( TWlanBgScanState& aState, TUint32 aNewBgScanSetting );
    
    /**
     * Handles state transition from On to Off state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     */
    void StateChangeOnToOff( TWlanBgScanState& aState );  
    
    /**
     * Handles state transition from On to Automatic state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     */
    void StateChangeOnToAutomatic( TWlanBgScanState& aState );
    
    /**
     * Handles state transition from On to On state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     * @param aNewBgScanSetting new background scan setting to be taken into use
     */
    void StateChangeOnToOn( TWlanBgScanState& aState, TUint32 aNewBgScanSetting );
    
    /**
     * Handles state transition from Off to Automatic state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     */
    void StateChangeOffToAutomatic( TWlanBgScanState& aState );
    
    /**
     * Handles state transition from Off to On state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     * @param aNewBgScanInterval new background scan interval to be taken into use
     */
    void StateChangeOffToOn( TWlanBgScanState& aState, TUint32 aNewBgScanInterval );
    
    /**
     * Handles state transition from Init to Auto state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     * @param aNewBgScanInterval new background scan interval to be taken into use
     */
    void StateChangeInitToAuto( TWlanBgScanState& aState );
    
    /**
     * Handles state transition from Init to Off state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     * @param aNewBgScanInterval new background scan interval to be taken into use
     */
    void StateChangeInitToOff( TWlanBgScanState& aState );
    
    /**
     * Handles state transition from Init to On state.
     *
     * @since S60 v5.2
     * @param aState reference to state
     * @param aNewBgScanInterval new background scan interval to be taken into use
     */
    void StateChangeInitToOn( TWlanBgScanState& aState );
        
    /**
     * Getter for AwsStartupStatus.
     *
     * @since S60 v5.2
     * @return AwsStartupStatus
     */
    inline TInt AwsStartupStatus() const
        {
        return iAwsStartupStatus;
        }
    
    /**
     * Getter for AwsCmdStatus.
     *
     * @since S60 v5.2
     * @return AwsCmdStatus
     */
    inline TInt AwsCmdStatus() const
        {
        return iAwsCmdStatus;
        }
    
    /**
     * Getter for AwsCmd.
     *
     * @since S60 v5.2
     * @return AwsCmd
     */
    inline TInt AwsCmd() const
        {
        return iAwsCmd;
        }
    
    /**
     * Getter for AutoInterval.
     *
     * @since S60 v5.2
     * @return AutoInterval
     */
    inline TInt AutoInterval() const
        {
        return iAutoInterval;
        }
    
    /**
     * Sets background scan interval and refreshes the used
     * interval.
     *
     * @since S60 v5.2
     * @param aInterval new background scan interval to be taken into use
     */
    void SetInterval( TUint32 aInterval );
          
protected:
    
    /**
     * Main state machine
     *
     * @since S60 v5.2
     * @param aEvent event for state machine
     */
    void NextState( const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent );
        
    /**
     * Get current interval.
     *
     * @since S60 v5.2
     * @return current interval in use
     */
    TUint32 GetBgInterval();
    
    /**
     * Interval change is delivered to state machine
     * 
     * @since S60 v5.2
     */
    void IntervalChanged();
    
    /**
     * PSM server mode changed command is given to state
     * machine
     *
     * @since S60 v5.2
     */
    void DeliverPsmServerModeChange();
    
    /**
     * Set AWS startup status.
     *
     * @param aStatus new status
     * @since S60 v5.2
     */
    inline void SetAwsStartupStatus( TInt aStatus );
    
    /**
     * Set AWS command status.
     *
     * @param aCmd new command
     * @param aStatus new status
     * @since S60 v5.2
     */
    inline void SetAwsCmdStatus( MWlanBgScanAwsComms::TAwsCommand aCmd, TInt aStatus );
    
    /**
     * Set Auto interval.
     *
     * @param aAutoInterval new auto interval
     * @since S60 v5.2
     */
    inline void SetAutoInterval( TUint aAutoInterval );
    
    /**
     * Refreshes the used background scan interval.
     *
     * @since S60 v5.2
     */
    void RefreshUsedInterval();
    
private: // data
    
    /**
     * Reference to the Bg Scan
     */
    CWlanBgScan& iBgScan;
    
    /**
     * Used background scan interval, in seconds.
     * This is the smaller one of iBgScanInterval
     * and iAggressiveBgScanInterval. 
     */
    TUint32 iUsedBgScanInterval;
    
    /**
     * Scan interval dictated by AWS component or
     * set (fixed interval) from UI. The value is
     * in seconds.
     */
    TUint32 iBgScanInterval;
    
    /**
     * Aggressive background scan interval, in seconds.
     * Interval used for temporary aggressive BG scanning
     * when requested by the client or when activated by
     * WLAN Engine.
     */
    TUint32 iAggressiveBgScanInterval;
     
    /**
     * Timer guarding when to drop out of aggressive BG scan mode
     */
    CWlanTimer* iAggressiveBgScanTimer; 
     
    /**
     * Next state to enter if AWS command delivery
     * is successful.
     */
    TWlanBgScanState iNextState;
    
    /**
     * AWS startup status code.
     */
    TInt iAwsStartupStatus;
    
    /**
     * AWS command.
     */
    MWlanBgScanAwsComms::TAwsCommand iAwsCmd;
    
    /**
     * AWS command status.
     */
    TInt iAwsCmdStatus;
    
    /**
     * Auto interval to be used in AutoAWS and Auto states
     */
    TUint iAutoInterval;
    
protected: // data
    
    /**
     * Reference to the Scan Result Provider
     */
    MWlanScanResultProvider& iProvider;

    /**
     * Reference to WLAN Timer services. 
     */
    MWlanTimerServices& iTimerServices;

    /**
     * Current background scan state. 
     */
    TWlanBgScanState iBgScanState;
    
    /**
     * Received settings for background scan.
     */
    MWlanBgScanProvider::TWlanBgScanSettings iBgScanSettings;
    
    /**
     * Id of the timer service request regarding 
     * background scan interval change.
     */
    TUint iIntervalChangeRequestId;
    
    /**
     * Completed AWS command.
     */
    MWlanBgScanAwsComms::TAwsCommand iCompletedAwsCommand;
    
    /**
     * Status code of the completed AWS command.
     */
    TInt iAwsCommandCompletionCode;         
    
    /**
     * Current WLAN state. 
     */
    MWlanBgScanProvider::TWlanBgScanState iWlanBgScanState;
    
    };

#include "wlanbgscanstates.inl"

#endif // WLANBGSCANSTATES_H
