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
* Description:   Implementation of the WlanPowerModeMgrBase class
*
*/

/*
* %version: 9 %
*/

#include "config.h"
#include "umacpowermodemgrbase.h"
#include "UmacContextImpl.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanPowerModeMgrBase::WlanPowerModeMgrBase() : 
    iUapsdRxFrameLengthThreshold( KDefaultUapsdRxFrameLengthThreshold )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanPowerModeMgrBase::~WlanPowerModeMgrBase() 
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPowerMgmtModeChange WlanPowerModeMgrBase::OnPsModeErrorIndication()
    {
    return ENoChange;
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanPowerModeMgrBase::OnActiveToLightPsTimerTimeout()
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanPowerModeMgrBase::OnLightPsToActiveTimerTimeout(
    WlanContextImpl& /*aCtxImpl*/ )
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanPowerModeMgrBase::OnLightPsToDeepPsTimerTimeout()
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanPowerModeMgrBase::Reset()
    {
    DoReset();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanPowerModeMgrBase::CountThisFrame(     
    WlanContextImpl& aCtxImpl,
    WHA::TQueueId aAccessCategory,
    TUint16 aEtherType,
    TBool aIgnoreThisFrame,
    TUint aPayloadLength,
    TUint16 aUapsdRxFrameLengthThreshold,
    TDaType aDaType )
    {
    TBool countThisFrame( ETrue );

    if ( !( aEtherType == KEapolType ||
            aEtherType == KWaiType ) )
        {
        if ( !aIgnoreThisFrame )
            {
            // this is not a frame to be ignored based on WLAN mgmt client
            // settings. However, check if it should be ignored for other
            // reasons
    
            if ( aDaType == EBroadcastAddress )
                {
                countThisFrame = EFalse;            
    
                OsTracePrint( KPwrStateTransition, (TUint8*)
                    ("UMAC: WlanPowerModeMgrBase::CountThisFrame: no; a bcast frame") );
                }
            else if ( aDaType == EMulticastAddress )
                {
                if ( !aCtxImpl.MulticastAddressCount() )
                    {
                    // no multicast MAC filters defined, so no downlink 
                    // multicast data stream expected => don't count the frame
                    countThisFrame = EFalse;
    
                    OsTracePrint( KPwrStateTransition, (TUint8*)
                        ("UMAC: WlanPowerModeMgrBase::CountThisFrame: no; a mcast frame and no mcast MAC filters defined") );
                    }
                else
                    {
                    // at least one multicast MAC filter defined so a downlink
                    // multicast data stream may be expected => count the frame
                    OsTracePrint( KPwrStateTransition, (TUint8*)
                        ("UMAC: WlanPowerModeMgrBase::CountThisFrame: yes; a mcast frame and at least one mcast MAC filter defined") );
                    }            
                }
            else
                {
                // a unicast frame
                
                if ( aCtxImpl.UapsdUsedForBestEffort() &&
                     aAccessCategory == WHA::ELegacy &&
                     aPayloadLength < aUapsdRxFrameLengthThreshold )
                    {
                    // U-APSD is being used for Best Effort (Legacy) AC (queue)
                    // and access category of this frame == WHA::ELegacy 
                    // and payload is shorter than the defined threshold =>
                    // don't count this frame
                    countThisFrame = EFalse;
    
                    OsTracePrint( KPwrStateTransition, (TUint8*)
                        ("UMAC: WlanPowerModeMgrBase::CountThisFrame: no, as uapsd in use for legacy, AC == legacy and payload < thr"));
                    }
                else
                    {
                    OsTracePrint( KPwrStateTransition, (TUint8*)
                        ("UMAC: WlanPowerModeMgrBase::CountThisFrame: yes"));            
                    }
                }            
            }
        else
            {
            // this is a frame to be ignored based on WLAN mgmt client AC settings
            
            countThisFrame = EFalse;
        
            OsTracePrint( KPwrStateTransition, (TUint8*)
                ("UMAC: WlanPowerModeMgrBase::CountThisFrame: no, as frames are ignored for this queue/AC: %d"),
                aAccessCategory );
            }
        }
    else
        {
        // EAPOL and WAI frames we always count. Not further actions needed
        
        OsTracePrint( KPwrStateTransition, (TUint8*)
            ("UMAC: WlanPowerModeMgrBase::CountThisFrame: yes; EAPOL or WAI frame"));            
        }
        
    return countThisFrame;
    }
