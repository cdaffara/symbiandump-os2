/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of WlanScanFsmCntx inline 
*                methods.
*
*/

/*
* %version: 7 %
*/

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline WlanScanFsmCntx::WlanScanFsmCntx( MWlanScanFsmCb& aCb ) : 
    iCb( aCb ),
    // to keep static code analysers happy. Actual initialization below
    iCurrentState( NULL )
    {
    iCurrentState = &iWlanScanModeStopped;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TWlanScanFsm& WlanScanFsmCntx::Current()
    {
    return *iCurrentState;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanScanFsmCntx::Current( 
    TWlanScanFsm& aNewCurrent )
    {
    iCurrentState = &aNewCurrent;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline MWlanScanFsmCb& WlanScanFsmCntx::Cb()
    {
    return iCb;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TWlanScanFsm& WlanScanFsmCntx::ScanModeRunning()
    {
    return iWlanScanModeRunning;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TWlanScanFsm& WlanScanFsmCntx::ScanModeStopped()
    {
    return iWlanScanModeStopped;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TWlanScanFsm& WlanScanFsmCntx::StartScanningMode()
    {
    return iWlanStartScanningMode;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TWlanScanFsm& WlanScanFsmCntx::StopScanningMode()
    {
    return iWlanStopScanningMode;
    }
