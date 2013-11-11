/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of WlanLightPsModePowerModeMgr inline methods
*
*/

/*
* %version: 3 %
*/

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanLightPsModePowerModeMgr::SetParameters(
    TUint aToActiveFrameThreshold,
    TUint aToDeepPsFrameThreshold,
    TUint16 aUapsdRxFrameLengthThreshold )
    {
    iToActiveFrameThreshold = aToActiveFrameThreshold;
    iToDeepPsFrameThreshold = aToDeepPsFrameThreshold;
    iUapsdRxFrameLengthThreshold = aUapsdRxFrameLengthThreshold;
    }
