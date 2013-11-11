/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanWsaKeyIndexMapper class
*
*/

/*
* %version: 11 %
*/

#include "config.h"
#include "UmacWsaKeyIndexMapper.h"

// ============================ MEMBER FUNCTIONS ===============================


inline TUint32 WlanWsaKeyIndexMapper::HandleWepPairwiseKey()
    {
    return KWepPairWiseKey;
    }

inline TUint32 WlanWsaKeyIndexMapper::HandleWepGroupKey( 
    TUint32 aDefaultKeyNumber )
    {
    return aDefaultKeyNumber;
    }

inline TUint32 WlanWsaKeyIndexMapper::HandleTkipGroupKey()
    {
    return KTkipGroupKey;
    }

inline TUint32 WlanWsaKeyIndexMapper::HandleTkipPairWiseKey()
    {
    return KTkipPairWiseKey;
    }

inline TUint32 WlanWsaKeyIndexMapper::HandleAesPairwiseKey()
    {
    return KAesPairWiseKey;
    }

inline TUint32 WlanWsaKeyIndexMapper::HandleAesGroupKey()
    {
    return KAesGroupKey;
    }

inline TUint32 WlanWsaKeyIndexMapper::HandleWapiPairwiseKey()
    {
    return KWapiPairWiseKey;
    }

inline TUint32 WlanWsaKeyIndexMapper::HandleWapiGroupKey()
    {
    return KWapiGroupKey;
    }

TUint8 WlanWsaKeyIndexMapper::Extract( 
    WHA::TKeyType aKeyType, 
    TUint32 aDefaultKeyNumber )
    {
    TUint32 ret(0);

    if ( aKeyType == WHA::EWepGroupKey )
        {
        ret = HandleWepGroupKey( aDefaultKeyNumber );
        }
    else if ( aKeyType == WHA::EWepPairWiseKey )
        {
        ret = HandleWepPairwiseKey();
        }
    else if ( aKeyType == WHA::ETkipGroupKey )
        {
        ret = HandleTkipGroupKey();
        }
    else if ( aKeyType == WHA::ETkipPairWiseKey )
        {
        ret = HandleTkipPairWiseKey();
        }
    else if ( aKeyType == WHA::EAesGroupKey )
        {
        ret = HandleAesGroupKey();
        }
    else if ( aKeyType == WHA::EAesPairWiseKey )
        {
        ret = HandleAesPairwiseKey();
        }
    else if ( aKeyType == WHA::EWapiGroupKey )
        {
        ret = HandleWapiGroupKey();
        }
    else if ( aKeyType == WHA::EWapiPairWiseKey )
        {
        ret = HandleWapiPairwiseKey();
        }
    else
        {
        OsTracePrint( KErrorLevel, (TUint8*)
            ("UMAC: WlanWsaKeyIndexMapper::Extract(): aKeyType: %d"), aKeyType );    
        OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
        }

    return ret;
    };
