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
* Description:   Declaration of the WlanNullSendController class
*
*/

/*
* %version: 15 %
*/

#ifndef WLAN_NULL_SEND_CONTROLLER
#define WLAN_NULL_SEND_CONTROLLER

#ifndef RD_WLAN_DDK
#include <wha_types.h>
#else
#include <wlanwha_types.h>
#endif

class WlanContextImpl;
class MWlanNullSender;

/*
* Encapsulates periodic NULL or QoS NULL frame sending functionality
*/
class WlanNullSendController
    {    

public:
    
    /** Ctor */
    explicit inline WlanNullSendController( 
        WlanContextImpl& aWlanCtxImpl,
        MWlanNullSender& aNullSender );
    
    /** Dtor */
    inline ~WlanNullSendController();

    /**
    * Sets the null frame sending parameters
    *
    * @since S60 3.2
    * @param aVoiceCallEntryTimeout when we are not in Voice over WLAN Call
    *        state and we transmit at least aVoiceCallEntryTxThreshold
    *        Voice priority frames during the time period (microseconds)
    *        denoted by this parameter, we enter Voice over WLAN Call state
    * @param aVoiceCallEntryThreshold Threshold value for the number of 
    *        Voice priority frames to enter Voice over WLAN Call state    
    * @param aNullTimeout NULL frame sending interval
    * @param aNoVoiceTimeout after this long time of no Voice priority data 
    *                        Tx, exit voice call state
    * @param aKeepAliveTimeout Keep Alive frame sending interval
    */
    inline void SetParameters(
        TUint32 aVoiceCallEntryTimeout,
        TUint32 aVoiceCallEntryThreshold,
        TUint32 aNullTimeout,
        TUint32 aNoVoiceTimeout,
        TUint32 aKeepAliveTimeout );

    /**
    * Starts Voice over WLAN Call maintenance
    *
    * @since S60 3.2
    */
    void StartVoiceOverWlanCallMaintenance();

    /**
    * Stops Voice over WLAN Call maintenance
    * Note! Does not reset the Voice over WLAN Call state!
    *
    * @since S60 3.2
    */
    inline void StopVoiceOverWlanCallMaintenance();

    /**
    * Terminates Voice over WLAN Call maintenance
    */
    inline void TerminateVoiceOverWlanCallMaintenance();

    /**
    * Resumes QoS Null Data frame sending when necessary.
    * Doesn't change the Voice Call state
    *
    * @since S60 3.2
    */
    void ResumeQosNullSending();
                    
    /**
    * Starts Keep Alive frame sending
    *
    * @since S60 3.2
    */
    inline void StartKeepAlive();
    
    /**
    * Stops Keep Alive frame sending
    *
    * @since S60 3.2
    */
    inline void StopKeepAlive();

    /**
    * To be called upon every Data frame Rx (other than Null and QoS Null Data) 
    *
    * @since S60 3.2
    * @param aQueueId Id of the queue/AC via which the frame was transmitted
    * @param aPayloadLength length of the ethernet frame payload
    */
    void OnFrameRx( 
        WHA::TQueueId aQueueId,
        TUint aPayloadLength );
    
    /**
    * To be called upon every Data frame (other than Null and QoS Null Data) 
    * send completion
    *
    * @since S60 3.2
    * @param aQueueId Id of the queue/AC via which the frame was transmitted
    */
    void OnFrameTx( WHA::TQueueId aQueueId );

    /**
    * To be called upon Voice Call Entry Timer timeout
    *
    * @since S60 3.2
    */
    void OnVoiceCallEntryTimerTimeout();

    /**
    * To be called upon Null Timer timeout
    *
    * @since S60 3.2
    */
    void OnNullTimerTimeout();

    /**
    * To be called upon every QoS Null Data frame send completion
    *
    * @since S60 3.2
    */
    void OnQosNullDataTxCompleted();

    /**
    * To be called upon every Null Data frame send completion
    *
    * @since S60 3.2
    */
    inline void OnNullDataTxCompleted();

    /**
    * To be called upon Voice Timer timeout
    *
    * @since S60 3.2
    */
    void OnNoVoiceTimerTimeout();
    
    /**
    * To be called upon Keep Alive Timer timeout
    *
    * @since S60 3.2
    */
    void OnKeepAliveTimerTimeout();
    
    /** 
    * Are we currently in Voice over WLAN Call state
    * 
    * @return ETrue if we are
    *         EFalse if we are not
    */
    inline TBool InVoiceCallState() const;

private:
    
    /** 
    * Arms the Voice Call Entry timer
    *
    * @since S60 3.2
    */
    void RegisterVoiceCallEntryTimeout();

    /** 
    * Arms the Null timer
    *
    * @since S60 3.2
    * @param aTimeoutInMicroSeconds Timeout in microseconds
    */
    void RegisterNullTimeout( TUint32 aTimeoutInMicroSeconds );

    /**
    * Arms the No Voice timer
    *
    * @since S60 3.2
    * @param aTimeoutInMicroSeconds Timeout in microseconds
    */
    void RegisterNoVoiceTimeout( TUint32 aTimeoutInMicroSeconds );

    /**
    * Arms the Keep Alive timer
    *
    * @since S60 3.2
    * @param aTimeoutInMicroSeconds Timeout in microseconds
    */
    void RegisterKeepAliveTimeout( TUint32 aTimeoutInMicroSeconds );

    /** 
    * Cancels the Voice Call Entry Timer
    *
    * @since S60 3.2
    */
    void CancelVoiceCallEntryTimeout();
    
    /** 
    * Cancels the Null Timer
    *
    * @since S60 3.2
    */
    void CancelNullTimeout();

    /** 
    * Cancels the No Voice Timer
    *
    * @since S60 3.2
    */
    void CancelNoVoiceTimeout();
    
    /** 
    * Cancels the Keep Alive Timer
    *
    * @since S60 3.2
    */
    void CancelKeepAliveTimeout();
        
    /** 
    * Enters the Voice Call state
    */
    void EnterVoiceCallState();
    
    // Prohibit copy constructor
    WlanNullSendController( 
        const WlanNullSendController& );
    // Prohibit assigment operator
    WlanNullSendController& operator= 
        ( const WlanNullSendController& );

private:    // Data
        
    /** stores the flags defined below */
    TUint32                     iFlags;
    /** 
    * this flag is set when Voice over WLAN Call maintenance has been started
    */
    static const TUint32 KVoiceOverWlanCallMaintenanceStarted = ( 1 << 0 );
    /** 
    * this flag is set when we are in voice call state
    */
    static const TUint32 KVoiceCallEntryPending = ( 1 << 1 );
    /** 
    * this flag is set when we are in voice call state
    */
    static const TUint32 KInVoiceCallState = ( 1 << 2 );
    /** 
    * this flag is set when the Null Timer has been armed
    */
    static const TUint32 KNullTimerArmed = ( 1 << 3 );
    /** 
    * this flag is set when the No Voice Timer has been armed
    */
    static const TUint32 KNoVoiceTimerArmed = ( 1 << 4 );
    /** 
    * this flag is set when the Keep Alive Timer has been armed
    */
    static const TUint32 KKeepAliveStarted = ( 1 << 5 );
    /** 
    * this flag is set when the Keep Alive Timer has been armed
    */
    static const TUint32 KKeepAliveTimerArmed = ( 1 << 6 );
    /** 
    * how many time stamps of potential Voice over WLAN downlink frames
    * we will store and investigate when determining if we should continue
    * in Voice over WLAN call state based on them - in the absence of actual
    * Voice priority traffic
    */
    static const TUint KBestEffortVoiceRxTimeStampCnt = 5;
    /** 
    * when we are not in Voice over WLAN Call
    * state and we transmit at least iVoiceCallEntryTxThreshold
    * Voice priority frames during the time period (microseconds)
    * denoted by this attribute, we enter Voice over WLAN Call state
    */
    TUint32                     iVoiceCallEntryTimeout;
    /*
    * threshold value for the number of Voice priority Tx/Rx frames to enter
    * Voice over WLAN Call state    
    */
    TUint32                     iVoiceCallEntryThreshold;
    /** 
    * after this long time (microseconds) of no transmitted frames,
    * a NULL frame needs to be transmitted
    */
    TUint32                     iNullTimeout;
    /** 
    * after this long time (microseconds) of no transmitted Voice priority 
    * frames, exit voice call state
    */
    TUint32                     iNoVoiceTimeout;
    /* 
    * time stamp (microseconds) of the latest Voice priority frame Rx or Tx 
    * in Voice Call state 
    */
    TInt64                      iLatestVoiceRxOrTxInVoiceCallState;
    /** 
    * if the Ethernet payload length (in bytes) of a Best Effort frame
    * received during Voice over WLAN call state in WMM nw is shorter 
    * than this threshold, it's a potential carrier of downlink Voice 
    * over WLAN Call data, which is erroneously tagged as Best Effort
    */
    TUint                       iBestEffortVoiceRxLengthThreshold;
    /* 
    * time stamps of the latest potential Voice over WLAN downlink frames
    * erroneously taggead as Best Effort.  
    * Stored as a ring buffer. 
    */
    TInt64 iBestEffortVoiceRxTimeStamp[KBestEffortVoiceRxTimeStampCnt];
    /** 
    * index where to store the time stamp of the next potential Voice over
    * WLAN downlink frame. 
    * Note that as the time stamps are stored as a ring buffer, this is
    * also the location of the oldest stored time stamp. 
    */
    TUint                       iBestEffortVoiceRxInd;
    /** 
    * size of the time window (in microseconds) during which we need
    * to receive KBestEffortVoiceRxTimeStampCnt potential Voice over WLAN
    * frames in order to continue in Voice over WLAN call state even if
    * there's no actual Voice priority traffic 
    */
    TUint                       iBestEffortVoiceRxTimeWindow;
    /** 
    * after this long time (microseconds) of no transmitted frames,
    * a keep alive frame needs to be transmitted
    */
    TUint32                     iKeepAliveTimeout;    
    /* time stamp of the latest frame Tx */
    TInt64                      iLatestTx;
    /** 
    * counter for our Voice priority Tx frames in Voice Call State Entry
    * pending state 
    */
    TUint                       iVoiceCallEntryTxCount;
    /** 
    * counter for Voice priority Rx frames in Voice Call State Entry
    * pending state 
    */
    TUint                       iVoiceCallEntryRxCount;
    /** global state machine context reference */
    WlanContextImpl&            iWlanContextImpl;
    /** Null Data Frame Sender reference */
    MWlanNullSender&            iNullSender;
    };

#include "umacnullsendcontroller.inl"

#endif // WLAN_NULL_SEND_CONTROLLER
