/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanDot11Connecting class
*
*/

/*
* %version: 15 %
*/

#include "config.h"
#include "UmacDot11Connecting.h"
#include "UmacContextImpl.h"

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Connecting::IsClass2Frame( const TUint8 aFrameType )
    {
    return ( aFrameType == E802Dot11FrameTypeAssociationReq
        || aFrameType == E802Dot11FrameTypeAssociationResp
        || aFrameType == E802Dot11FrameTypeReassociationReq
        || aFrameType == E802Dot11FrameTypeReassociationResp
        || aFrameType == E802Dot11FrameTypeDisassociation );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Connecting::IsClass3Frame( const TUint8 aFrameType )
    {
    return ( aFrameType & E802Dot11FrameTypeData ); // is any data type frame
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAny* WlanDot11Connecting::RequestForBuffer( 
    WlanContextImpl& aCtxImpl,
    TUint16 aLength )
    {
    return aCtxImpl.GetRxBuffer( aLength );
    }
