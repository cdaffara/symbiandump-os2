/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of ProtocolStackSideUmacCb class
*
*/

/*
* %version: 13 %
*/

#include "config.h"
#include "UmacProtocolStackSideUmacCb.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanProtocolStackSideUmacCb::SetProtocolStackTxOffset( 
    TUint32 aEthernetFrameTxOffset )
    {
    if ( iSelf )
        {
        iSelf->SetProtocolStackTxOffset( 
            aEthernetFrameTxOffset );
        }
    else
        {
        // protocol stack side client is not present so we do nothing
        // (This is the case for the 1st connect and in that case the 
        // frame Tx offset will be set elsewhere)
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanProtocolStackSideUmacCb::OnTxProtocolStackDataComplete( 
    TInt aCompletionCode,
    TDataBuffer* aMetaHeader )
    {
    if ( iSelf )
        {
        iSelf->OnTxProtocolStackDataComplete( aCompletionCode, aMetaHeader );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanProtocolStackSideUmacCb::OnOtherTxDataComplete()
    {
    if ( iSelf )
        {
        iSelf->OnOtherTxDataComplete();
        }    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanProtocolStackSideUmacCb::OnTxDataSent()
    {
    if ( iSelf )
        {
        iSelf->OnTxDataSent();
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanProtocolStackSideUmacCb::ProtocolStackDataReceiveComplete( 
    TDataBuffer*& aBufferStart, 
    TUint32 aNumOfBuffers )
    {
    TBool status ( EFalse );
    
    if ( iSelf )
        {
        status = iSelf->ProtocolStackDataReceiveComplete( aBufferStart, aNumOfBuffers );
        }
        
    return status;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanProtocolStackSideUmacCb::ProtocolStackSideClientReady() const
    {
    TBool status ( EFalse );
    
    if ( iSelf )
        {
        status = iSelf->ProtocolStackSideClientReady();
        }
        
    return status;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanProtocolStackSideUmacCb::UserDataReEnabled()
    {
    if ( iSelf )
        {
        iSelf->UserDataReEnabled();
        }
    }
