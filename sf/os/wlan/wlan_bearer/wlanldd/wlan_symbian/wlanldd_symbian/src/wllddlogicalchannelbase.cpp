/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the DWlanLogicalChannelBase class.
*
*/

/*
* %version: 3 %
*/

#include "WlLddWlanLddConfig.h"
#include "wllddlogicalchannelbase.h"
#include <kern_priv.h>

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
DWlanLogicalChannelBase::DWlanLogicalChannelBase()
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
DWlanLogicalChannelBase::~DWlanLogicalChannelBase()
    {
    }

// ---------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalChannelBase::Request( TInt aReqNo, TAny* a1, TAny* a2 )
    {
    if ( aReqNo < static_cast<TInt>(EMinRequestId) )
        {
        Kern::PanicCurrentThread( 
            _L("DWlanLogicalChannelBase::Request"), 
            ERequestNoInvalid );
        
        return KErrGeneral;
        }
    else if ( aReqNo >= EMinControlFast && aReqNo < EMaxControlFast)
        {
        // Execute fast control commands in the context of the client's 
        // thread in supervisor mode.
        return reinterpret_cast<TInt>(DoControlFast( aReqNo , a1 ));
        }
    else
        {
        TThreadMessage& m = Kern::Message();
        m.iValue = aReqNo;
        m.iArg[0] = a1;
        if ( aReqNo < 0 )
            {
            kumemget32( &m.iArg[1], a2, 2 * sizeof( TAny* ) );
            }
        else
            {
            m.iArg[1] = a2;
            }
        return m.SendReceive( &iMsgQ );
        }
    }
