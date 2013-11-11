/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  inline implementation of WlanHpa
*
*/

/*
* %version: 4 %
*/

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanHpa::Attach( MWlanHpaCb& aCb )
    {
    iHpaCb = &aCb;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline MWlanHpaCb& WlanHpa::HpaCb()
    {
    return *iHpaCb;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline MWlanOsaExt& WlanHpa::OsaExtCb()
    {
    return iOsaExt;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline MWlanOsa& WlanHpa::OsaCb()
    {
    return iOsa;
    }
