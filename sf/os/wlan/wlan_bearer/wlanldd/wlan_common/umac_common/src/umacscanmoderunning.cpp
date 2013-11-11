/*
* Copyright (c) 2005-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the TWlanScanModeRunning class.
*
*/

/*
* %version: 4 %
*/

#include "config.h"
#include "umacscanmoderunning.h"
#include "umacscanfsmcntx.h"
#include "umacscanfsmcb.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TWlanScanModeRunning::Entry( 
    WlanScanFsmCntx& aCnxt )
    {
    aCnxt.Cb().OnScanFsmEvent( MWlanScanFsmCb::EFSMSCANMODERUNNING );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TWlanScanModeRunning::StopScanningMode(
    WlanScanFsmCntx& aCnxt )
    {
    ChangeState( aCnxt, aCnxt.StopScanningMode() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TWlanScanModeRunning::OnScanningModeStopped( 
    WlanScanFsmCntx& aCnxt )
    {
    ChangeState( aCnxt, aCnxt.ScanModeStopped() );
    }
