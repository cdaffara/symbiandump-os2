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
* Description:   Implementation of the TWlanStartScanningMode class.
*
*/

/*
* %version: 4 %
*/

#include "config.h"
#include "umacstartscanningmode.h"
#include "umacscanfsmcntx.h"
#include "umacscanfsmcb.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TWlanStartScanningMode::Entry( 
    WlanScanFsmCntx& aCnxt )
    {
    aCnxt.Cb().OnScanFsmEvent( MWlanScanFsmCb::EFSMSTARTSCANNINGMODE );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TWlanStartScanningMode::OnScanningModeStarted( 
    WlanScanFsmCntx& aCnxt )
    {
    ChangeState( aCnxt, aCnxt.ScanModeRunning() );
    }
