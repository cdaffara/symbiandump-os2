/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDynamicPowerModeMgmtCntx class
*
*/

/*
* %version: 14 %
*/

#ifndef WLAN_DYNAMIC_POWER_MODE_MGMT_CNTX
#define WLAN_DYNAMIC_POWER_MODE_MGMT_CNTX

#include "umacinternaldefinitions.h"
#include "umacactivemodepowermodemgr.h"
#include "umaclightpsmodepowermodemgr.h"
#include "umacdeeppsmodepowermodemgr.h"

class WlanContextImpl;
class WlanPowerModeMgrBase;

/*
* Context for implementing dynamic power mode transition algorithm in 
* infrastructure mode between PS and active power management mode
*/
class WlanDynamicPowerModeMgmtCntx
    {    

public:
    
    /** Ctor */
    explicit WlanDynamicPowerModeMgmtCntx( WlanContextImpl& aWlanCtxImpl );
    
    /** Dtor */
    ~WlanDynamicPowerModeMgmtCntx();

    /**
    * Starts dynamic power mode management
    *
    * @since S60 3.1
    */
    void StartPowerModeManagement();

    /**
    * Stops dynamic power mode management
    *
    * @since S60 3.1
    */
    void StopPowerModeManagement();
                    
    /**
    * To be called before frame Tx
    * Determines the need to make a power mode transition
    *
    * @since S60 3.1
    * @param aQueueId Id of the queue/AC via which the frame will be transmitted
    * @param aEtherType Ethernet type of the frame
    * @param aDot11FrameType 802.11 frame type
    * @return To which power management mode to change; if any at all
    */
    TPowerMgmtModeChange OnFrameTx( 
        WHA::TQueueId aQueueId, 
        TUint16 aEtherType,
        T802Dot11FrameControlTypeMask aDot11FrameType );

    /** 
    * To be called when accepting an Rx frame
    *
    * @since S60 3.1
    * @param aAccessCategory AC/Queue via which the frame has beeen transmitted
    * @param aEtherType Ethernet type of the received frame
    * @param aPayloadLength length of the ethernet frame payload
    * @param aDaType DA type (Unicast, Multicast or Broadcast) of the frame
    * @return To which power management mode to change; if any at all
    */
    TPowerMgmtModeChange OnFrameRx(
        WHA::TQueueId aAccessCategory,
        TUint16 aEtherType,
        TUint aPayloadLength,
        TDaType aDaType );
                        
    /**
    * To be called upon receiving the PS Mode Error indication
    * Determines the need to make a power mode transition
    *
    * @return To which power management mode to change; if any at all
    */
    TPowerMgmtModeChange OnPsModeErrorIndication();
    
    /**
    * To be called upon Active to Light PS timer timeout
    *
    * @since S60 v5.1
    * @return ETrue if power mode transition should be done, 
    *         EFalse otherwise
    */
    TBool OnActiveToLightPsTimerTimeout();

    /**
    * To be called upon Light PS to Active timer timeout
    *
    * @since S60 v5.1
    * @return ETrue if power mode transition should be done, 
    *         EFalse otherwise
    */
    TBool OnLightPsToActiveTimerTimeout();

    /**
    * To be called upon Light PS to Deep PS timer timeout
    *
    * @since S60 v5.1
    * @return ETrue if power mode transition should be done, 
    *         EFalse otherwise
    */
    TBool OnLightPsToDeepPsTimerTimeout();
    
    /**
    * Sets the dynamic power mode transition algorithm parameters
    *
    * @since S60 3.1
    * @param aToLightPsTimeout time interval in microseconds after which 
    *        transition from Active mode to Light PS mode is considered
    * @param aToLightPsFrameThreshold frame count threshold used when 
    *        considering transition from Active to Light PS mode 
    * @param aToActiveTimeout time interval in microseconds after which the 
    *        frame counter used when considering transition from Light PS 
    *        to Active mode is reset
    * @param aToActiveFrameThreshold frame count threshold used when 
    *        considering transition from Light PS to Active mode
    * @param aToDeepPsTimeout time interval in microseconds after which 
    *        transition from Light PS mode to Deep PS mode is considered
    * @param aToDeepPsFrameThreshold frame count threshold used when 
    *        considering transition from Light PS to Deep PS mode
    * @param aUapsdRxFrameLengthThreshold received frame 
    *        payload length (in bytes) threshold in U-APSD network for
    *        Best Effort Access Category
    */
    void SetParameters(
        TUint32 aToLightPsTimeout,
        TUint16 aToLightPsFrameThreshold,
        TUint32 aToActiveTimeout,
        TUint16 aToActiveFrameThreshold,
        TUint32 aToDeepPsTimeout,
        TUint16 aToDeepPsFrameThreshold,
        TUint16 aUapsdRxFrameLengthThreshold );

    /**
    * Configures dynamic power mode management traffic override
    *
    * The settings here become effective once using the PS mode has been 
    * allowed by WLAN Mgmt Client.
    * When a setting below is ETrue, any amount of Rx or Tx traffic via
    * the AC in question won't cause a change from PS to CAM mode once PS
    * mode has been entered, and traffic via that AC won't make us to 
    * stay in CAM either.
    * Every AC has a separate setting for U-APSD and legacy PS.
    * The U-APSD setting is used if U-APSD is used for the AC in question.
    * Otherwise the corresponding legacy setting is used.
    *    
    * @since S60 3.2
    * @param aCtxImpl statemachine context
    * @param aStayInPsDespiteUapsdVoiceTraffic U-APSD Voice AC setting
    * @param aStayInPsDespiteUapsdVideoTraffic U-APSD Video AC setting
    * @param aStayInPsDespiteUapsdBestEffortTraffic U-APSD Best Effort AC 
    *                                               setting
    * @param aStayInPsDespiteUapsdBackgroundTraffic U-APSD Background AC 
    *                                               setting
    * @param aStayInPsDespiteLegacyVoiceTraffic legacy Voice AC setting
    * @param aStayInPsDespiteLegacyVideoTraffic legacy Video AC setting
    * @param aStayInPsDespiteLegacyBestEffortTraffic legacy Best Effort AC 
    *                                                setting
    * @param aStayInPsDespiteLegacyBackgroundTraffic legacy Background AC 
    *                                                setting
    * @return ETrue if a state change occurred
    *         EFalse otherwise
    */
    void ConfigureTrafficOverride( 
        TBool aStayInPsDespiteUapsdVoiceTraffic,
        TBool aStayInPsDespiteUapsdVideoTraffic,
        TBool aStayInPsDespiteUapsdBestEffortTraffic, 
        TBool aStayInPsDespiteUapsdBackgroundTraffic,
        TBool aStayInPsDespiteLegacyVoiceTraffic,
        TBool aStayInPsDespiteLegacyVideoTraffic,
        TBool aStayInPsDespiteLegacyBestEffortTraffic,
        TBool aStayInPsDespiteLegacyBackgroundTraffic );

    /**
    * Freezes the dynamic power mode management traffic override settings 
    * provided earlier with the ConfigureTrafficOverride() method and based 
    * on whether U-APSD is used for the different ACs/Tx queues
    *
    * @since S60 3.2
    */
    void FreezeTrafficOverride();

    /**
    * Restores the Active mode parameters back to their WLAN Mgmt Client 
    * provided values
    */
    inline void RestoreActiveModeParameters();
    
private:
    
    /** 
    * Starts ToLightPsTimer 
    *
    * @since S60 5.1
    * @param aTimeout timeout in microseconds
    */
    void RegisterToLightPsTimeout();

    /** 
    * Starts ToActiveTimer 
    *
    * @since S60 5.1
    * @param aTimeout timeout in microseconds
    */
    void RegisterToActiveTimeout();

    /** 
    * Starts ToDeepPsTimer 
    *
    * @since S60 5.1
    * @param aTimeout timeout in microseconds
    */
    void RegisterToDeepPsTimeout();

    /** 
    * Cancels ToLightPsTimer 
    *
    * @since S60 5.1
    * @param aTimeout timeout in microseconds
    */
    void CancelToLightPsTimeout();

    /** 
    * Cancels ToActiveTimer 
    *
    * @since S60 5.1
    * @param aTimeout timeout in microseconds
    */
    void CancelToActiveTimeout();
    
    /** 
    * Cancels ToDeepPsTimer 
    *
    * @since S60 5.1
    * @param aTimeout timeout in microseconds
    */
    void CancelToDeepPsTimeout();
    
    /** 
    * Cancels all running timers
    *
    * @since S60 5.1
    */
    inline void CancelTimeouts();
    
    /** 
    * Returns data traffic override setting for Voice AC
    * regarding U-APSD traffic
    *
    * @since S60 3.1
    * @return ETrue if Voice AC traffic shall be ignored
    */
    inline TBool StayInPsDespiteUapsdVoiceTraffic() const;
    
    /** 
    * Returns data traffic override setting for Video AC
    * regarding U-APSD traffic
    *
    * @since S60 3.1
    * @return ETrue if Video AC traffic shall be ignored
    */
    inline TBool StayInPsDespiteUapsdVideoTraffic() const;

    /** 
    * Returns data traffic override setting for Best Effort AC
    * regarding U-APSD traffic
    *
    * @since S60 3.1
    * @return ETrue if Best Effort AC traffic shall be ignored
    */
    inline TBool StayInPsDespiteUapsdBestEffortTraffic() const;

    /** 
    * Returns data traffic override setting for Background AC
    * regarding U-APSD traffic
    *
    * @since S60 3.1
    * @return ETrue if Background AC traffic shall be ignored
    */
    inline TBool StayInPsDespiteUapsdBackgroundTraffic() const;

    /** 
    * Returns data traffic override setting for Voice AC
    * regarding legacy, i.e. non-U-APSD, traffic
    *
    * @since S60 3.1
    * @return ETrue if Voice AC traffic shall be ignored
    */
    inline TBool StayInPsDespiteLegacyVoiceTraffic() const;
    
    /** 
    * Returns data traffic override setting for Video AC
    * regarding legacy, i.e. non-U-APSD, traffic
    *
    * @since S60 3.1
    * @return ETrue if Video AC traffic shall be ignored
    */
    inline TBool StayInPsDespiteLegacyVideoTraffic() const;

    /** 
    * Returns data traffic override setting for Best Effort AC
    * regarding legacy, i.e. non-U-APSD, traffic
    *
    * @since S60 3.1
    * @return ETrue if Best Effort AC traffic shall be ignored
    */
    inline TBool StayInPsDespiteLegacyBestEffortTraffic() const;

    /** 
    * Returns data traffic override setting for Background AC
    * regarding legacy, i.e. non-U-APSD, traffic
    *
    * @since S60 3.1
    * @return ETrue if Background AC traffic shall be ignored
    */
    inline TBool StayInPsDespiteLegacyBackgroundTraffic() const;
    
    /** 
    * Sets special parameters for the Active mode.
    * To be used when switching to Active upen receiving PsModeError 
    * indication from WHA layer.
    * The Wlan Mgmt Client provided Active mode parameters can be restored
    * with RestoreActiveModeParameters()
    */
    inline void SetPsModeErrorActiveModeParameters();

    /** 
    * Sets special parameters for the Active mode.
    * To be used when switching to Active upon transmitting our keep alive 
    * frame or an ARP frame.
    * The Wlan Mgmt Client provided Active mode parameters can be restored
    * with RestoreActiveModeParameters()
    */
    inline void SetKeepAliveActiveModeParameters();
    
    // Prohibit copy constructor
    WlanDynamicPowerModeMgmtCntx( 
        const WlanDynamicPowerModeMgmtCntx& );
    // Prohibit assigment operator
    WlanDynamicPowerModeMgmtCntx& operator= 
        ( const WlanDynamicPowerModeMgmtCntx& );

private:    // Data

    /** for backing up the parameters related to Active mode */
    struct SActiveParamsBackup
        {
        TUint32 iToLightPsTimeout;
        TUint16 iToLightPsFrameThreshold;
        TUint16 iUapsdRxFrameLengthThreshold;
        };
    
    /** flag value to store state transition need internally */
    TBool                       iStateChange; 
    /** stores the flags defined below */
    TUint32                     iFlags;
    /** 
    * any amount of U-APSD Voice AC Rx or Tx traffic won't cause a 
    * change from PS to CAM mode once PS mode has been entered 
    */
    static const TUint32 KStayInPsDespiteUapsdVoiceTraffic = ( 1 << 0 );
    /** 
    * any amount of U-APSD Video AC Rx or Tx traffic won't cause a 
    * change from PS to CAM mode once PS mode has been entered 
    */
    static const TUint32 KStayInPsDespiteUapsdVideoTraffic = ( 1 << 1 );
    /** 
    * any amount of U-APSD Best Effort AC Rx or Tx traffic won't cause a 
    * change from PS to CAM mode once PS mode has been entered 
    */
    static const TUint32 KStayInPsDespiteUapsdBestEffortTraffic = ( 1 << 2 );
    /** 
    * any amount of U-APSD Background AC Rx or Tx traffic won't cause a 
    * change from PS to CAM mode once PS mode has been entered 
    */
    static const TUint32 KStayInPsDespiteUapsdBackgroundTraffic = ( 1 << 3 );
    /** 
    * any amount of legacy, i.e. non U-APSD, Voice AC Rx or Tx traffic 
    * won't cause a change from PS to CAM mode once PS mode has been entered
    */
    static const TUint32 KStayInPsDespiteLegacyVoiceTraffic = ( 1 << 4 );
    /** 
    * any amount of legacy, i.e. non U-APSD, Video AC Rx or Tx traffic 
    * won't cause a change from PS to CAM mode once PS mode has been entered
    */
    static const TUint32 KStayInPsDespiteLegacyVideoTraffic = ( 1 << 5 );
    /** 
    * any amount of legacy, i.e. non U-APSD, Best Effort AC Rx or Tx traffic 
    * won't cause a change from PS to CAM mode once PS mode has been entered
    */
    static const TUint32 KStayInPsDespiteLegacyBestEffortTraffic = ( 1 << 6 );
    /** 
    * any amount of legacy, i.e. non U-APSD, Background AC Rx or Tx traffic 
    * won't cause a change from PS to CAM mode once PS mode has been entered
    */
    static const TUint32 KStayInPsDespiteLegacyBackgroundTraffic = ( 1 << 7 );
    /** ToLightPsTimer started */
    static const TUint32 KToLightPsTimerStarted = ( 1 << 8 );
    /** ToActiveTimer started */
    static const TUint32 KToActiveTimerStarted = ( 1 << 9 );
    /** ToDeepPsTimer started */
    static const TUint32 KToDeepPsTimerStarted = ( 1 << 10 );
    
    /** 
    * time interval in microseconds after which transition from Active
    * mode to Light PS mode is considered.
    */ 
    TUint32 iToLightPsTimeout;

    /**
    * time interval in microseconds after which the frame counter
    * used when considering transition from Light PS to Active mode is reset.
    */
    TUint32 iToActiveTimeout;

    /**
    * time interval in microseconds after which transition from Light PS
    * mode to Deep PS mode is considered.
    */ 
    TUint32 iToDeepPsTimeout;
    
    /** for backing up the parameters related to Active mode */
    SActiveParamsBackup iActiveParamsBackup;
    
    /** currently active power management mode context */
    WlanPowerModeMgrBase*       iActiveCntx;
    /** context for active mode */
    WlanActiveModePowerModeMgr  iActiveModeCntx;
    /** context for Light PS mode */
    WlanLightPsModePowerModeMgr iLightPsModeCntx;
    /** context for Deep PS mode */
    WlanDeepPsModePowerModeMgr  iDeepPsModeCntx;
    /** global state machine context reference */
    WlanContextImpl&            iWlanContextImpl;
    /** 
    * the frozen Rx & Tx traffic ignoration setting for every AC
    */
    TBool iIgnoreTraffic[WHA::EQueueIdMax];    
    };

#include "umacdynamicpowermodemgmtcntx.inl"

#endif // WLAN_DYNAMIC_POWER_MODE_MGMT_CNTX
