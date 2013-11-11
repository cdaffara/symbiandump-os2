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
* %version: 12 %
*/

#ifndef WLANBGSCAN_H
#define WLANBGSCAN_H

#include "awsinterface.h"
#include "awsenginebase.h"
#include "wlanscanproviderinterface.h"
#include "wlantimerserviceinterface.h"
#include "wlanbgscanawscommsinterface.h"
#include "wlanbgscancommandlistener.h"
#include "wlanbgscanstates.h"



/**
 *  WLAN Background Scan
 *  This class implements WLAN Background Scan logic.
 *  
 *  @since S60 S60 v5.2
 */
NONSHARABLE_CLASS( CWlanBgScan ) :
    public MWlanBgScanProvider,
    public MWlanTimerServiceCallback,
    public MWlanBgScanCommandListener,
    public CWlanBgScanStates
    {

public:

    /**
     * Describes relation of time to time range.
     */
    enum TRelation
        {
        ESmaller,
        EInsideRange,
        EGreater
        };
    
    /**
     * Possible statuses for AWS presence.
     */
    enum TAwsPresence
        {
        EAwsPresent,
        EAwsNotPresent,
        EAwsStarting
        };

    /**
     * Two-phased constructor.
     */
    static CWlanBgScan* NewL( MWlanScanResultProvider& aProvider, MWlanTimerServices& aTimerServices );
    
    /**
    * Destructor.
    */
    virtual ~CWlanBgScan();

    /**
     * From MWlanBgScanProvider.
     * Called when Scan has been completed.
     *
     * @since S60 v5.2
     */
    void ScanComplete();
        
    /**
     * From MWlanBgScanProvider.
     * Called to indicate that WLAN state has changed.
     *
     * @since S60 v5.2
     * 
     * @param aState New WLAN state
     */
    void SetBgScanState( const MWlanBgScanProvider::TWlanBgScanState &aState );
    
    /**
     * From MWlanBgScanProvider.
     * Whether background scan is enabled.
     *
     * @since S60 v5.2
     * @return True if background scan is enabled,
     *         False otherwise.
     */
    TBool IsBgScanEnabled();
        
    /**
     * From MWlanBgScanProvider.
     * Notification about changed settings.
     *
     * @since S60 v5.2
     */
    void NotifyChangedSettings( MWlanBgScanProvider::TWlanBgScanSettings& aSettings );
    
    /**
     * From MWlanTimerServiceCallback.
     * OnTimeout.
     * Requested time has elapsed.
     *
     * @since S60 v5.2
     */
    void OnTimeout();
       
    /**
     * From MWlanBgScanCommandListener.
     * DoSetInterval.
     * This method is called by the command queue when
     * interval change request is processed.
     * @param aNewInterval new interval to be taken into use
     */
    void DoSetInterval( TUint32 aNewInterval );
    
    /**
     * This method is called by the command queue when
     * AWS startup has been completed.
     * 
     * @since S60 v5.2
     */
    void AwsStartupComplete( TInt aStatus );
    
    /**
     * This method is called by the command queue when
     * AWS command has been completed.
     * 
     * @param aCommand completed command
     * @param aStatus completion code of the command
     * 
     * @since S60 v5.2
     */
    void AwsCommandComplete( MWlanBgScanAwsComms::TAwsCommand& aCommand, TInt aStatus );
    
    /**
     * Send AWS command
     * 
     * @param aCommand command to be sent
     * 
     * @since S60 v5.2
     */
    void AwsCommand( MWlanBgScanAwsComms::TAwsMessage& aCommand );
    
    /**
     * AWS presence status.
     *
     * @since S60 v5.2
     * @return EAwsPresent if AWS is available,
     *         EAwsNotPresent if AWS is not available,
     *         EAwsStarting if AWS is still starting up 
     *         
     */
    CWlanBgScan::TAwsPresence AwsPresence();
    
    /**
     * Request callback when interval change should take place.
     *
     * @since S60 v5.2
     */
    void ScheduleAutoIntervalChange();
    
    /**
     * Get current Auto Background Scan interval.
     *
     * @since S60 v5.2
     * 
     * @return interval
     */
    TUint32 CurrentAutoInterval();
  
    /**
     * Start aggressive background scanning.
     *
     * @since S60 v5.2
     * @param aInterval scan interval for aggressive mode, in seconds
     * @param aTimeout aggressive mode duration, in microseconds
     */
    void StartAggressiveBgScan( TUint32& aInterval, TUint32& aTimeout );

private:
    
    /**
     * Constructor.
     */
    CWlanBgScan( MWlanScanResultProvider& aProvider, MWlanTimerServices& aTimerServices );
    
    /**
     * Default constructor, no implementation.
     */
    CWlanBgScan();
    
    /**
     * Two-phased constructor.
     */
    void ConstructL();
        
    /**
     * Get next time when to change Auto interval.
     *
     * @since S60 v5.2
     * 
     * @return time to change interval
     */
    TTime AutoIntervalChangeAt();
    
    /**
     * See how time is related to a time range.
     *
     * @param aTime time to be checked
     * @param aStart start of range
     * @param aEnd end of range
     * @return relation
     * @since S60 v5.2
     */
    TRelation TimeRelationToRange( const TTime& aTime, TUint aRangeStart, TUint aRangeEnd ) const;
    
    /**
     * Check the proposed settings are valid.
     *
     * @since S60 v5.2
     *
     * @param aSettingsToUse settings that can be used [OUT]
     * @param aProposedSettings proposed settings [IN]
     */
    void CheckSettings(
            MWlanBgScanProvider::TWlanBgScanSettings& aSettingsToUse,
            const MWlanBgScanProvider::TWlanBgScanSettings& aProposedSettings ) const;
    
    /**
     * Is interval change needed.
     *
     * @since S60 v5.2
     * @return ETrue if interval change is needed,
     *         EFalse otherwise.
     */
    TBool IsIntervalChangeNeeded();
    
private: // data
    
    /**
     * Reference to the Scan Result Provider
     */
    MWlanScanResultProvider& iProvider;
    
    /**
     * Interface to AWS comms object. 
     */
    MWlanBgScanAwsComms* iAwsComms;
    
    /**
     * Whether AWS is ok or not. 
     */
    TBool iAwsOk;
    
    /**
     * Current PSM server mode. 
     */
    TUint iCurrentPsmServerMode;
            
    };

#endif // WLANBGSCAN_H
