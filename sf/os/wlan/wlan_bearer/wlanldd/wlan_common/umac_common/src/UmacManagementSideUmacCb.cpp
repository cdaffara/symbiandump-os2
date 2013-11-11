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
* Description:   Implementation of the WlanManagementSideUmacCb class.
*
*/

/*
* %version: 20 %
*/

#include "config.h"
#include "UmacManagementSideUmacCb.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanManagementSideUmacCb::~WlanManagementSideUmacCb() 
    {
    iSelf = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::Attach( 
    WlanManagementSideUmacCb& aSelf )
    {
    iSelf = &aSelf;

    OsTracePrint( KInitLevel, 
        (TUint8*)("UMAC: WlanManagementSideUmacCb::Attach: address: 0x%08x"), 
        reinterpret_cast<TUint32>(iSelf) );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::Detach()
    {
    OsTracePrint( KInitLevel, 
        (TUint8*)("UMAC: WlanManagementSideUmacCb::Detach: address: 0x%08x"), 
        reinterpret_cast<TUint32>(iSelf) );    

    iSelf = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::OnOidCompleted( 
    TInt aReason, 
    SOidOutputData& OidOutputData )
    {
    if ( iSelf )
        {
        iSelf->OnOidCompleted( aReason, OidOutputData );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::SetMgmtSideTxOffsets( 
    TUint32 aEthernetFrameTxOffset,
    TUint32 aDot11FrameTxOffset,
    TUint32 aSnapFrameTxOffset )
    {
    OsTracePrint( KWsaTxDetails, 
        (TUint8*)("UMAC: WlanManagementSideUmacCb::SetMgmtSideTxOffsets: iSelf address: 0x%08x"), 
        reinterpret_cast<TUint32>(iSelf) );    

    if ( iSelf )
        {
        iSelf->SetMgmtSideTxOffsets(
            aEthernetFrameTxOffset,
            aDot11FrameTxOffset,
            aSnapFrameTxOffset );
        }    
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint8* WlanManagementSideUmacCb::GetBufferForRxData( 
    TUint aLengthinBytes )
    {
    return (iSelf ? iSelf->GetBufferForRxData( aLengthinBytes ) : NULL);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint8* WlanManagementSideUmacCb::DmaPrivateTxMemory()
    {
    return (iSelf ? iSelf->DmaPrivateTxMemory() : NULL);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::MgmtDataReceiveComplete( 
    TDataBuffer*& aBufferStart, 
    TUint32 aNumOfBuffers )
    {
    if ( iSelf )
        {
        iSelf->MgmtDataReceiveComplete( aBufferStart, aNumOfBuffers );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::MgmtPathWriteComplete (TInt aErr)
    {
    if ( iSelf )
        {
        iSelf->MgmtPathWriteComplete( aErr );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::OnInDicationEvent( 
    TIndication aIndication )
    {
    if ( iSelf )
        {
        iSelf->OnInDicationEvent( aIndication );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::MarkRxBufFree( TUint8* aBufferToFree )
    {
    if ( iSelf )
        {
        iSelf->MarkRxBufFree( aBufferToFree );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::RegisterTimeout( 
    TUint32 aTimeoutInMicroSeconds,
    TWlanTimer aTimer )
    {
    if ( iSelf )
        {
        iSelf->RegisterTimeout( aTimeoutInMicroSeconds, aTimer );
        }
    else
        {
        // implementation error; catch it
        OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::CancelTimeout( TWlanTimer aTimer )
    {
    if ( iSelf )
        {
        iSelf->CancelTimeout( aTimer );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::RegisterDfc( TAny* aCntx )
    {
    if ( iSelf )
        {
        iSelf->RegisterDfc( aCntx );
        }
    else
        {
        // implementation error; catch it
        OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::CancelDfc()
    {
    if ( iSelf )
        {
        iSelf->CancelDfc();
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDataBuffer* WlanManagementSideUmacCb::GetRxFrameMetaHeader()
    {
    return (iSelf ? iSelf->GetRxFrameMetaHeader() : NULL);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanManagementSideUmacCb::FreeRxFrameMetaHeader( TDataBuffer* aMetaHeader )
    {
    if ( iSelf )
        {
        iSelf->FreeRxFrameMetaHeader( aMetaHeader );
        }
    }
