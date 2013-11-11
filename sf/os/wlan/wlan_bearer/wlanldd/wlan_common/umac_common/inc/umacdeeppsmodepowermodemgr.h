/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDeepPsModePowerModeMgr class
*
*/

/*
* %version: 4 %
*/

#ifndef WLAN_DEEP_PS_MODE_POWER_MODE_MGR
#define WLAN_DEEP_PS_MODE_POWER_MODE_MGR

#include "umacpowermodemgrbase.h"

class WlanContextImpl;

/**
* Class implementing infrastructure mode dynamic power mode management 
* algorithm for Deep PS mode
*/
class WlanDeepPsModePowerModeMgr : public WlanPowerModeMgrBase
    {  

public:

    /** Ctor */
    WlanDeepPsModePowerModeMgr();

    /** Dtor */
    virtual ~WlanDeepPsModePowerModeMgr();
    
    /**
    * Sets the dynamic power mode transition algorithm parameters
    *
    * @param aUapsdRxFrameLengthThreshold received frame 
    *        payload length (in bytes) threshold in U-APSD network
    */
    void SetParameters( TUint16 aUapsdRxFrameLengthThreshold );
    
    /**
    * To be called when transmitting a frame
    *
    * @since S60 5.1
    * Determines the need to make a power mode transition
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
    * @since S60 5.1
    * @param aCtxImpl global statemachine context
    * @param aAccessCategory AC/Queue via which the frame has beeen transmitted
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
    * To be called upon receiving the PS Mode Error indication
    * Determines the need to make a power mode transition
    *
    * @return To which power management mode to change; if any at all
    */
    virtual TPowerMgmtModeChange OnPsModeErrorIndication();
    
    /** 
    * From WlanPowerModeMgrBase
    * Resets the state of the object 
    *
    * @since S60 5.1
    */
    virtual void DoReset() {};

private:    // Methods

    // Prohibit copy constructor
    WlanDeepPsModePowerModeMgr( const WlanDeepPsModePowerModeMgr& );
    // Prohibit assigment operator
    WlanDeepPsModePowerModeMgr& operator= ( 
        const WlanDeepPsModePowerModeMgr& );    

private:    // Data

};

#endif // WLAN_DEEP_PS_MODE_POWER_MODE_MGR
