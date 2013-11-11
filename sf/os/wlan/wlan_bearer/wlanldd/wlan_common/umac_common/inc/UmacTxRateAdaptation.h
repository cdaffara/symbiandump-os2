/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanTxRateAdaptation class
*
*/

/*
* %version: 18 %
*/

#ifndef WLANTXRATEADAPTATION_H
#define WLANTXRATEADAPTATION_H

#ifndef RD_WLAN_DDK
#include <wha_types.h>
#else
#include <wlanwha_types.h>
#endif

class WlanContextImpl;

/**
*  Implements dynamic transmit rate adaptation.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanTxRateAdaptation 
    {
    
public:

    /**
    * C++ default constructor.
    */
    WlanTxRateAdaptation();

    /**
    * Destructor.
    */
    virtual ~WlanTxRateAdaptation() {};

    /**
    * Sets the rate adaptation object
    * @since S60 3.1
    * @param aPolicyId id of the Tx rate policy for which the rates
    *        are set
    * @param aRateBitmask contains the rates to be set
    */
    TBool SetRates( TUint8 aPolicyId, WHA::TRate aRateBitmask );

    /**
    * Defines which Tx rate policy should be used when sending via
    * the specified queue
    *
    * @since S60 3.1
    * @param aQueueId id of the Tx queue
    * @param aPolicyId id of the Tx rate policy
    */
    void SetPolicy( WHA::TQueueId aQueueId, TUint8 aPolicyId );
    
    /**
    * Sets the current max Tx rate which should be initially used when sending
    * frames using the specified Tx rate policy.
    * If this rate is not present in the specified rate policy, or if
    * it is not supported either by the nw or by WHA layer, the next possible
    * lower rate will be used instead.
    * However, if the specified rate is lower that any rate in the specified
    * rate policy, the lowest rate in the policy will be used.
    *
    * @since S60 3.1
    * @param aPolicyId id of the Tx rate policy
    * @param aRate initial max Tx rate
    */
    void SetCurrentMaxTxRate( TUint8 aPolicyId, WHA::TRate aRate );
    
    /**
    * Sets the Tx rate adaptation algorithm parameters
    *
    * @since S60 3.1
    * @param aMinStepUpCheckpoint minimum and initial rate increase 
    *        checkpoint in units of frames
    *        Range: [aStepDownCheckpoint,aMaxStepUpCheckpoint]
    * @param aMaxStepUpCheckpoint maximum rate increase checkpoint in units 
    *        of frames
    *        Range: [aStepDownCheckpoint,UCHAR_MAX]
    * @param aStepUpCheckpointFactor StepUpCheckpoint is multiplied with this
    *        value if sending a probe frame fails
    *        Range: [1,aMaxStepUpCheckpoint]
    * @param aStepDownCheckpoint after this many frames the need to decrease
    *        the rate is checked
    *        Range: [2,UCHAR_MAX]
    * @param aMinStepUpThreshold minimum and initial StepUpThreshold 
    *        percentage
    *        Range: [1,aMaxStepUpThreshold]
    * @param aMaxStepUpThreshold maximum StepUpThreshold percentage
    *        Range: [1,100]
    * @param aStepUpThresholdIncrement StepUpThreshold is incremented by this
    *        value if sending a probe frame fails
    *        Range: [0,aMaxStepUpThreshold]
    * @param aStepDownThreshold if the percentage of frames which failed to
    *        be transmitted at the originally requested rate is at least 
    *        aStepDownThreshold at the aStepDownCheckpoint, the rate will
    *        be decreased
    *        Range: [1,100]
    * @param aDisableProbeHandling if EFalse, the rate adaptation algorithm
    *        handles the first frame transmitted after a rate increase in a
    *        special way. Otherwise the special handling is disabled
    */
    void SetAlgorithmParameters( 
        TUint8 aMinStepUpCheckpoint,
        TUint8 aMaxStepUpCheckpoint,
        TUint8 aStepUpCheckpointFactor,
        TUint8 aStepDownCheckpoint,
        TUint8 aMinStepUpThreshold,
        TUint8 aMaxStepUpThreshold,
        TUint8 aStepUpThresholdIncrement,
        TUint8 aStepDownThreshold,
        TBool aDisableProbeHandling );

    /**
    * Gets the current transmit rate & rate policy class to use when sending 
    * via the specified Tx queue
    *
    * @since S60 3.1
    * @param aCtxImpl Global statemachine context
    * @param aQueueId ID of the Tx queue
    * @param aUseSpecialRatePolicy ETrue if use of the special Tx rate
    *        policy is requested for this frame Tx
    * @param aRate Rate to use. Note that this value is not relevant if 
    *        autonomous rate adaptation is being used
    * @param aPolicyId id of the Tx rate policy
    */
    void RatePolicy( 
        const WlanContextImpl& aCtxImpl,
        WHA::TQueueId aQueueId,
        TBool aUseSpecialRatePolicy,
        WHA::TRate& aRate, 
        TUint8& aPolicyId ) const;

    /**
    * Called upon Tx operation complete
    *
    * @since S60 3.1
    * @param aRate the rate which was used to transmit the frame (if success)
    * @param aSuccess did the send succeed
    * @param aQueueId id of the queue via which the frame was transmitted
    * @param aRequestedRate Tx rate that was originally requested
    */
    void OnTxCompleted( 
        WHA::TRate aRate, 
        TBool aSuccess, 
        WHA::TQueueId aQueueId,
        WHA::TRate aRequestedRate );
    
    /**
    * Resets the state of the whole Tx rate adaptation object
    */
    void Reset();

private:

    struct SBitRate
        {
        /*
        * rates are in ascending order the lowest rate being the first
        */
        WHA::TRate  iBitRate;
        // number of rates
        SBitRate*   iNext;
        SBitRate*   iPrev;
        };

    enum TRateStep
        {
        EKeepCurrent,
        EStepDown,
        EStepUp
        };

    struct SAlgorithmParams
        {
        /*
        * minimum and initial rate increase checkpoint in units of frames
        */
        TUint8  iMinStepUpCheckpoint;
        /*
        * maximum rate increase checkpoint in units of frames
        */
        TUint8  iMaxStepUpCheckpoint;
        /*
        * iStepUpCheckpoint is multiplied with this value if sending a probe 
        * frame fails
        */
        TUint8  iStepUpCheckpointFactor;
        /*
        * after this many frames the need to decrease the rate is checked
        */
        TUint8  iStepDownCheckpoint;
        /*
        * minimum and initial StepUpThreshold percentage
        */
        TUint8  iMinStepUpThreshold;
        /*
        * maximum iStepUpThreshold percentage value
        */
        TUint8  iMaxStepUpThreshold;
        /*
        * iStepUpThreshold is incremented by this value if sending a probe 
        * frame fails
        */
        TUint8  iStepUpThresholdIncrement;        
        /*
        * if the percentage of frames which failed to be transmitted at the 
        * originally requested rate is at least iStepDownThreshold at the 
        * iStepDownCheckpoint, the rate will be decreased
        */
        TUint8  iStepDownThreshold;
        /*
        * if EFalse, the rate adaptation algorithm handles the first frame 
        * transmitted after a rate increase in a special way. Otherwise the
        * special handling is disabled
        */
        TBool   iDisableProbeHandling;
        };

    struct SPolicy
        {
        /** current Tx rate */
        SBitRate*   iCurrentTxRate;
        // first rate
        SBitRate*   iHead;
        // last rate
        SBitRate*   iTail;
        /** number of rate entrys */
        TUint       iNumOfRates;
        /** ETrue for a probe frame */
        TBool       iProbe;
        TUint       iTxCount;
        TUint       iTxFailCount;        
        TUint       iStepUpCheckpoint;
        TUint       iStepUpThreshold;
        };

    /**
    * Resets the state related to the specified rate policy
    * @param aPolicy the policy to reset
    */
    void Reset( SPolicy& aPolicy ) const;

    /**
    * Appends a rate to rate list
    * @param aPolicyIndex index of the Tx rate policy to which to append
    * @param aRate rate to append
    */
    TBool AppendRate( TUint8 aPolicyIndex, WHA::TRate aRate );

    /**
    * Inserts a rate to rate list
    * @param aPolicyIndex index of the Tx rate policy to which to insert
    * @param aBlock rate list item to insert
    */
    void InsertRate( TUint8 aPolicyIndex, SBitRate* aBlock ) const;

    /**
    * Decrements Tx rate for the specified Tx rate policy
    * @param aPolicy the policy
    */
    void RateStepDown( SPolicy& aPolicy ) const;

    /**
    * Increments Tx rate for the specified Tx rate policy
    * @param aPolicy the policy
    * @return ETrue if the operation succeeded
    *         EFalse if already using the highest rate
    */
    TBool RateStepUp( SPolicy& aPolicy ) const;

    /**
    * Called upon Tx operation complete
    * @param aSuccess did the send succeed using the rate origianlly requested
    * @param aPolicy policy which was used when transmitting the related frame
    */
    TRateStep OnTxCompleted( 
        TBool aSuccess,
        SPolicy& aPolicy ) const;

    // Prohibit copy constructor.
    WlanTxRateAdaptation( const WlanTxRateAdaptation& );
    // Prohibit assigment operator.
    WlanTxRateAdaptation& operator= 
        ( const WlanTxRateAdaptation& );
    
private:    // Data
    
    SAlgorithmParams    iAlgorithmParam;

    /** 
    * maps every Tx queue to a Tx policy
    */
    TUint8              iQueue2Policy[WHA::EQueueIdMax];

    /** 
    * maximum number of rate policies in use concurrently.
    * We will use two policies, one for voice priority (voice AC) data
    * and another for everything else, if possible. Otherwise only one policy
    * is used. Parameters for policy 1 are at index 0, for policy 2 at index 1
    * and so on
    */
    enum { KMaxRatePolicyCount = 2 };
    
    SPolicy             iPolicy[KMaxRatePolicyCount];
    };

#endif      // WLANTXRATEADAPTATION_H

