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
* Description:   Implementation of the WlanDeepPsModePowerModeMgr class
*
*/

/*
* %version: 4 %
*/

#include "config.h"
#include "umacdeeppsmodepowermodemgr.h"
#include "UmacContextImpl.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanDeepPsModePowerModeMgr::WlanDeepPsModePowerModeMgr() 
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanDeepPsModePowerModeMgr::~WlanDeepPsModePowerModeMgr() 
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDeepPsModePowerModeMgr::SetParameters(
    TUint16 aUapsdRxFrameLengthThreshold )
    {
    iUapsdRxFrameLengthThreshold = aUapsdRxFrameLengthThreshold;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPowerMgmtModeChange WlanDeepPsModePowerModeMgr::OnFrameTx( 
    WlanContextImpl& aCtxImpl, 
    WHA::TQueueId /*aQueueId*/,
    TUint16 aEtherType,
    T802Dot11FrameControlTypeMask aDot11FrameType,
    TBool aIgnoreThisFrame )
    {
    TPowerMgmtModeChange powerMgmtModeChange( ENoChange );

    if ( aEtherType == KEapolType ||
         aEtherType == KWaiType ||
         aDot11FrameType == E802Dot11FrameTypeDataNull ||
         ( aEtherType == KArpType && 
           !(aCtxImpl.UapsdUsedForVoice() && aCtxImpl.InVoiceCallState()) ) ) 
        {
        powerMgmtModeChange = EToActive;

        OsTracePrint( KPwrStateTransition, (TUint8*)
            ("UMAC: WlanDeepPsModePowerModeMgr::OnFrameTx: EAPOL, WAI or keep "
             "alive frame or ARP in non- U-APSD WoWLAN call state; change to "
			 "Active") );
        }
    else if ( !aIgnoreThisFrame )
        {
        powerMgmtModeChange = EToLightPs; 

        OsTracePrint( KPwrStateTransition, (TUint8*)
            ("UMAC: WlanDeepPsModePowerModeMgr::OnFrameTx: change to Light"
             " PS") );            
        }
    else
        {
        // no action needed
        }
    
    return powerMgmtModeChange;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPowerMgmtModeChange WlanDeepPsModePowerModeMgr::OnFrameRx( 
    WlanContextImpl& aCtxImpl,
    WHA::TQueueId aAccessCategory,
    TUint16 aEtherType,
    TBool aIgnoreThisFrame,
    TUint aPayloadLength,
    TDaType aDaType ) 
    {
    TPowerMgmtModeChange powerMgmtModeChange( ENoChange );

    if ( aEtherType == KEapolType ||
         aEtherType == KWaiType )
        {
        powerMgmtModeChange = EToActive;

        OsTracePrint( KPwrStateTransition, (TUint8*)
            ("UMAC: WlanDeepPsModePowerModeMgr::OnFrameRx: EAPOL or WAI frame;"
             " change to Active") );            
        }
    else
        {
        if ( CountThisFrame( 
                aCtxImpl, 
                aAccessCategory,
                aEtherType,
                aIgnoreThisFrame, 
                aPayloadLength, 
                iUapsdRxFrameLengthThreshold,
                aDaType ) )
            {
            powerMgmtModeChange = EToLightPs;        

            OsTracePrint( KPwrStateTransition, (TUint8*)
                ("UMAC: WlanDeepPsModePowerModeMgr::OnFrameRx: change to Light"
                 " PS") );            
            }
        }
    
    return powerMgmtModeChange;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPowerMgmtModeChange WlanDeepPsModePowerModeMgr::OnPsModeErrorIndication()
    {
    return EToActive;
    }
