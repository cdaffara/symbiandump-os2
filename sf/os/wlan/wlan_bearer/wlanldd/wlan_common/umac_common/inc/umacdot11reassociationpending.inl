/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of WlanDot11ReassociationPending inline 
*                methods.
*
*/

/*
* %version: 4 %
*/

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline WlanDot11ReassociationPending::WlanDot11ReassociationPending() : 
    iState( EINIT ) 
    {
    };

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline WlanDot11ReassociationPending::~WlanDot11ReassociationPending()
    {
    };

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TBool WlanDot11ReassociationPending::Associated() const
    {
    return (iFlags & KReassocSuccess );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanDot11ReassociationPending::OnTxReassocFrameXferEvent( 
    WlanContextImpl& aCtxImpl )
    {
    ChangeInternalState( aCtxImpl, EWAIT4REASSOCIATIONRESPONSE );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanDot11ReassociationPending::ConfigureAc( 
    WlanContextImpl& aCtxImpl )
    {
    ConfigureAcParams( aCtxImpl );
    }

