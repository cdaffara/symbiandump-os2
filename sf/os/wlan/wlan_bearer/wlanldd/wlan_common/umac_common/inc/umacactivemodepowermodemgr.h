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
* Description:   Declaration of the WlanActiveModePowerModeMgr class
*
*/

/*
* %version: 10 %
*/

#ifndef WLAN_ACTIVE_MODE_POWER_MODE_MGR
#define WLAN_ACTIVE_MODE_POWER_MODE_MGR

#include "umacpowermodemgrbase.h"

class WlanContextImpl;


/**
* Class implementing infrastructure mode dynamic power mode management 
* algorithm for Active mode (CAM)
*/
class WlanActiveModePowerModeMgr : public WlanPowerModeMgrBase
    {
    
public:
    /** 
    * Default Rx/Tx frame count threshold for considering change to Light 
    * PS mode.
    * This value is used if another value hasn't been provided
    */
    static const TUint KDefaultToLightPsFrameThreshold = 1;
    
public:
    
    /** Ctor */
    WlanActiveModePowerModeMgr();   
        
    /** Dtor */
    virtual ~WlanActiveModePowerModeMgr();

    /**
    * Sets the dynamic power mode transition algorithm parameters
    *
    * @since S60 5.1
    * @param aToLightPsFrameThreshold frame count threshold for changing to 
    *        Light PS mode
    * @param aUapsdRxFrameLengthThreshold received frame 
    *        payload length (in bytes) threshold in U-APSD network
    */
    inline void SetParameters(
        TUint aToLightPsFrameThreshold,
        TUint16 aUapsdRxFrameLengthThreshold );

    /**
    * To be called when transmitting a frame
    * Determines the need to make a power mode transition
    *
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    * @param aQueueId Id of the queue/AC via which the frame will be transmitted
    * @param aEtherType Ethernet type of the frame
    * @param aDot11FrameType 802.11 frame type
    * @param aIgnoreThisFrame shall this frame be ignored from dynamic power 
    *                         mode management perspective
    * @return To which power management mode to change; if any at all
    */
    virtual TPowerMgmtModeChange OnFrameTx( 
        WlanContextImpl& aCtxImpl, 
        WHA::TQueueId aQueueId,
        TUint16 aEtherType,
        T802Dot11FrameControlTypeMask aDot11FrameType,
        TBool aIgnoreThisFrame );

    /** 
    * To be called when accepting an Rx frame
    *
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    * @param aAccessCategory AC/Queue via which the frame was transmitted
    * @param aEtherType Ethernet type of the received frame
    * @param aIgnoreThisFrame shall this frame be ignored from dynamic power 
    *                         mode management perspective
    * @param aPayloadLength length of the ethernet frame payload
    * @param aDaType DA type (Unicast, Multicast or Broadcast) of the frame
    * @return To which power management mode to change; if any at all
    */
    virtual TPowerMgmtModeChange OnFrameRx(
        WlanContextImpl& aCtxImpl,
        WHA::TQueueId aAccessCategory,
        TUint16 aEtherType,
        TBool aIgnoreThisFrame,
        TUint aPayloadLength,
        TDaType aDaType );

    /**
    * From WlanPowerModeMgrBase
    * To be called upon Active to Light PS timer timeout
    *
    * @since S60 5.1
    * @return ETrue if power mode transition should be done, 
    *         EFalse otherwise
    */
    virtual TBool OnActiveToLightPsTimerTimeout();
    
    /** 
    * From WlanPowerModeMgrBase
    * Resets the state of the object 
    *
    * @since S60 3.1
    */
    virtual void DoReset();
        
private:

    // Prohibit copy constructor
    WlanActiveModePowerModeMgr( const WlanActiveModePowerModeMgr& );
    // Prohibit assignment operator
    WlanActiveModePowerModeMgr& operator= ( 
        const WlanActiveModePowerModeMgr& );   
    
private:    // Data

    /**
    * Rx/Tx frame counter used when considering change to 
    * Light PS mode
    */
    TUint iToLightPsFrameCount;

    /**
    * Rx/Tx frame count threshold used when considering change to 
    * Light PS mode
    */
    TUint iToLightPsFrameThreshold;
    };

#include "umacactivemodepowermodemgr.inl"

#endif // WLAN_ACTIVE_MODE_POWER_MODE_MGR
