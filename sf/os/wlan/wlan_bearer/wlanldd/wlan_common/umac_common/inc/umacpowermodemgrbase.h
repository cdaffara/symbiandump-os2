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
* Description:   Declaration of the WlanPowerModeMgrBase class
*
*/

/*
* %version: 13 %
*/

#ifndef WLAN_POWER_MODE_MGR_BASE_H
#define WLAN_POWER_MODE_MGR_BASE_H

#ifndef RD_WLAN_DDK
#include <wha_types.h>
#else
#include <wlanwha_types.h>
#endif

#include "umacinternaldefinitions.h"

class WlanContextImpl;


/**
* Common base class for classes implementing dynamic power mode transition 
* algorithm between PS and active mode in infrastructure mode 
*/
class WlanPowerModeMgrBase
    {
public:
    // Default frame payload length threshold value (in bytes) for U-APSD
    static const TUint32 KDefaultUapsdRxFrameLengthThreshold = 400;

public:

    /** Dtor */
    virtual ~WlanPowerModeMgrBase();

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
        TBool aIgnoreThisFrame ) = 0;

    /** 
    * To be called when accepting an Rx frame
    *
    * @since S60 3.1
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
        TDaType aDaType ) = 0;

    /**
    * To be called upon receiving the PS Mode Error indication
    * Determines the need to make a power mode transition
    *
    * @return To which power management mode to change; if any at all
    */
    virtual TPowerMgmtModeChange OnPsModeErrorIndication();
    
    /**
    * To be called upon Active to Light PS timer timeout
    *
    * @since S60 v5.1
    * @return ETrue if power mode transition should be done, 
    *         EFalse otherwise
    */
    virtual TBool OnActiveToLightPsTimerTimeout();

    /**
    * To be called upon Light PS to Active timer timeout
    *
    * @since S60 v5.1
    * @param aCtxImpl global statemachine context
    * @return ETrue if power mode transition should be done, 
    *         EFalse otherwise
    */
    virtual TBool OnLightPsToActiveTimerTimeout( WlanContextImpl& aCtxImpl );

    /**
    * To be called upon Light PS to Deep PS timer timeout
    *
    * @since S60 v5.1
    * @return ETrue if power mode transition should be done, 
    *         EFalse otherwise
    */
    virtual TBool OnLightPsToDeepPsTimerTimeout();
    
    /** 
    * Resets the state of the object 
    *
    * @since S60 3.1
    */
    void Reset();

    /** 
    * Resets the state of the derived object 
    *
    * @since S60 3.1
    */
    virtual void DoReset() = 0;

protected:

    /** Ctor */
    WlanPowerModeMgrBase();

    /**
    * Determines the need to increment frame counter
    *
    * @since S60 3.1
    * @param aCtxImpl
    * @param aAccessCategory
    * @param aPayloadLength
    * @param aUapsdRxFrameLengthThreshold
    * @param aDaType DA type (Unicast, Multicast or Broadcast) of the frame
    * @return ETrue if the frame should increment relevant frame counter, 
    *         EFalse otherwise
    */
    static TBool CountThisFrame(     
        WlanContextImpl& aCtxImpl,
        WHA::TQueueId aAccessCategory,
        TUint16 aEtherType,
        TBool aIgnoreThisFrame,
        TUint aPayloadLength,
        TUint16 aUapsdRxFrameLengthThreshold,
        TDaType aDaType );

private:

    // Prohibit copy constructor
    WlanPowerModeMgrBase( const WlanPowerModeMgrBase& );
    // Prohibit assigment operator
    WlanPowerModeMgrBase& operator= ( const WlanPowerModeMgrBase& );    
    
protected:  // Data

    /** 
    * Frame payload length threshold value (in bytes) for U-APSD
    */
    TUint16 iUapsdRxFrameLengthThreshold;    
    };

#endif // WLAN_POWER_MODE_MGR_BASE_H
