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
* Description:   Implementation of the TWlanScanFsm class.
*
*/

/*
* %version: 6 %
*/

#include "config.h"
#include "umacscanfsm.h"
#include "umacscanfsmcntx.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TWlanScanFsm::StartScanningMode( 
    WlanScanFsmCntx& /*aCnxt*/ )
    {
    // implementation error
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TWlanScanFsm::StopScanningMode( 
    WlanScanFsmCntx& /*aCnxt*/ )
    {
    // implementation error
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TWlanScanFsm::OnScanningModeStarted( 
    WlanScanFsmCntx& /*aCnxt*/ )
    {
    // implementation error
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TWlanScanFsm::OnScanningModeStopped( 
    WlanScanFsmCntx& /*aCnxt*/ )
    {
    // implementation error
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TWlanScanFsm::ChangeState( 
    WlanScanFsmCntx& aCnxt, 
    TWlanScanFsm& aNewState ) const
    {
    aCnxt.Current( aNewState );
    aCnxt.Current().Entry( aCnxt );
    }
