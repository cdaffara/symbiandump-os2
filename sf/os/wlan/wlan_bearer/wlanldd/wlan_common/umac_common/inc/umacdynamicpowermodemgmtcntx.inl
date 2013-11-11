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
* Description:   Implementation of WlanDynamicPowerModeMgmtCntx inline methods
*
*/

/*
* %version: 6 %
*/

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanDynamicPowerModeMgmtCntx::RestoreActiveModeParameters()
    {
    iToLightPsTimeout = iActiveParamsBackup.iToLightPsTimeout;
    iActiveModeCntx.SetParameters( 
        iActiveParamsBackup.iToLightPsFrameThreshold, 
        iActiveParamsBackup.iUapsdRxFrameLengthThreshold );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanDynamicPowerModeMgmtCntx::CancelTimeouts()
    {
    CancelToLightPsTimeout();
    CancelToActiveTimeout();
    CancelToDeepPsTimeout();    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline 
TBool WlanDynamicPowerModeMgmtCntx::StayInPsDespiteUapsdVoiceTraffic() const
    {    
    return ( iFlags & KStayInPsDespiteUapsdVoiceTraffic );
    }
             
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline 
TBool WlanDynamicPowerModeMgmtCntx::StayInPsDespiteUapsdVideoTraffic() const
    {    
    return ( iFlags & KStayInPsDespiteUapsdVideoTraffic );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TBool 
WlanDynamicPowerModeMgmtCntx::StayInPsDespiteUapsdBestEffortTraffic() const
    {    
    return ( iFlags & KStayInPsDespiteUapsdBestEffortTraffic );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TBool 
WlanDynamicPowerModeMgmtCntx::StayInPsDespiteUapsdBackgroundTraffic() const
    {    
    return ( iFlags & KStayInPsDespiteUapsdBackgroundTraffic );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline 
TBool WlanDynamicPowerModeMgmtCntx::StayInPsDespiteLegacyVoiceTraffic() const
    {    
    return ( iFlags & KStayInPsDespiteLegacyVoiceTraffic );
    }
             
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline 
TBool WlanDynamicPowerModeMgmtCntx::StayInPsDespiteLegacyVideoTraffic() const
    {    
    return ( iFlags & KStayInPsDespiteLegacyVideoTraffic );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TBool 
WlanDynamicPowerModeMgmtCntx::StayInPsDespiteLegacyBestEffortTraffic() const
    {    
    return ( iFlags & KStayInPsDespiteLegacyBestEffortTraffic );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TBool 
WlanDynamicPowerModeMgmtCntx::StayInPsDespiteLegacyBackgroundTraffic() const
    {    
    return ( iFlags & KStayInPsDespiteLegacyBackgroundTraffic );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanDynamicPowerModeMgmtCntx::SetPsModeErrorActiveModeParameters()
    {
    const TUint32 KToLightPsTimeout( 1000000 ); // 1 second
    const TUint16 KToLightPsFrameThreshold( 4 );
    const TUint16 KUapsdRxFrameLengthThreshold(
        WlanPowerModeMgrBase::KDefaultUapsdRxFrameLengthThreshold );
    
    iToLightPsTimeout = KToLightPsTimeout;
    iActiveModeCntx.SetParameters( 
        KToLightPsFrameThreshold, 
        KUapsdRxFrameLengthThreshold );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanDynamicPowerModeMgmtCntx::SetKeepAliveActiveModeParameters()
    {
    const TUint32 KToLightPsTimeout( 20000 ); // 20ms
    const TUint16 KToLightPsFrameThreshold( 1 );
    const TUint16 KUapsdRxFrameLengthThreshold(
        WlanPowerModeMgrBase::KDefaultUapsdRxFrameLengthThreshold );
    
    iToLightPsTimeout = KToLightPsTimeout;
    iActiveModeCntx.SetParameters( 
        KToLightPsFrameThreshold, 
        KUapsdRxFrameLengthThreshold );
    }
