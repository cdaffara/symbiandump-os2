/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of WlanWsaAddKey inline methods.
*
*/

/*
* %version: 9 %
*/

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WlanWsaAddKey::~WlanWsaAddKey()
    {
    iKey = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanWsaAddKey::Set( 
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TKeyType aKeyType,
    const TAny* aKey,
    TUint8 aEntryIndex )
    {
    iKeyType = aKeyType;
    iKey = aKey;
    iEntryIndex  = aEntryIndex;
    }
