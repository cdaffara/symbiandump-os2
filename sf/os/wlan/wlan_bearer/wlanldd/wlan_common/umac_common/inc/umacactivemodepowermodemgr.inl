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
* Description:   Implementation of WlanActiveModePowerModeMgr inline methods
*
*/

/*
* %version: 5 %
*/

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanActiveModePowerModeMgr::SetParameters(
    TUint aToLightPsFrameThreshold,
    TUint16 aUapsdRxFrameLengthThreshold )
    {
    OsTracePrint( KPwrStateTransition, (TUint8*)
        ("UMAC: WlanActiveModePowerModeMgr::SetParameters: "
         "aToLightPsFrameThreshold: %d"),
        aToLightPsFrameThreshold );
    OsTracePrint( KPwrStateTransition, (TUint8*)
        ("UMAC: WlanActiveModePowerModeMgr::SetParameters: "
         "aUapsdRxFrameLengthThreshold: %d"),
        aUapsdRxFrameLengthThreshold );

    iToLightPsFrameThreshold = aToLightPsFrameThreshold;
    iUapsdRxFrameLengthThreshold = aUapsdRxFrameLengthThreshold;    
    }
