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
* Description:   Implementation of WlanWsaWriteMib inline methods.
*
*/

/*
* %version: 9 %
*/

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WlanWsaWriteMib::~WlanWsaWriteMib()
    {
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanWsaWriteMib::Set( 
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TMib aMib,
    TUint32 aLength,
    const TAny* aData )
    {
    iMib = aMib;
    iLength = aLength;
    iData = aData;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanWsaWriteMib::DfcPending( TBool aValue )
    {
    iDfcPending = aValue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanWsaWriteMib::DfcPending() const
    {
    return iDfcPending;
    }
