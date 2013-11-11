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
* Description:   Implementation of the WlanWsaComplexCommand class.
*
*/

/*
* %version: 7 %
*/

#include "config.h"
#include "UmacWsaComplexCommand.h"
#include "UmacContextImpl.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaComplexCommand::Act( 
    WlanContextImpl& aCtxImpl, 
    TUint32 aAct ) const
    {
    aCtxImpl.WhaCommandAct( aAct );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanWsaComplexCommand::DoErrorIndication( 
    WlanContextImpl& aCtxImpl, 
    WHA::TStatus /*aStatus*/ )
    {    
    OsTracePrint( KWarningLevel, 
        (TUint8*)("UMAC * umacwhacomplexcommand * abort") );

    // this is the one and only global error handler
    ChangeState( aCtxImpl, *this, aCtxImpl.iStates.iMacError );

    // signal with return value that a state transition occurred
    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaComplexCommand::OnWhaCommandResponse( 
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TCommandId /*aCommandId*/, 
    WHA::TStatus /*aStatus*/,
    const WHA::UCommandResponseParams& /*aCommandResponseParams*/,
    TUint32 /*aAct*/ )
    {
    // left intentionally empty
    }
