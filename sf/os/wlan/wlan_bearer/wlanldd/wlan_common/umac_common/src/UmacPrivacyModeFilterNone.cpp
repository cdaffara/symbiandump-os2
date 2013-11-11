/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanPrivacyModeFilterNone class
*
*/

/*
* %version: 11 %
*/

#include "config.h"
#include "UmacPrivacyModeFilterNone.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanPrivacyModeFilterNone::operator()( 
    const SDataFrameHeader& aFrameheader, 
    TBool aUserDataEnabled, 
    TUint16 aEthernetType,
    TBool /* aUnicastKeyExists */, 
    TBool /* aAesOrTkipOrWapiEncrypted */ ) const
    {
    TBool ret( EFalse );
           
    if ( ( // allow all that have WEP bit off
           !( aFrameheader.IsWepBitSet() )
           // AND are going to protocol stack 
           && aEthernetType != KEapolType
           // AND user data is enabled
           && aUserDataEnabled 
         )
         // OR are EAPOL frames
         || aEthernetType == KEapolType
         // OR are our test frames
         || aEthernetType == KBounceType
       )
        {
        ret = ETrue;
        }
    else
        {
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: WlanPrivacyModeFilterNone::operator: frame filtered") );
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: WlanPrivacyModeFilterNone::operator: wep bit: %d"), 
            aFrameheader.IsWepBitSet() );
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: WlanPrivacyModeFilterNone::operator: ether type: 0x%04x"), 
            aEthernetType );
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: WlanPrivacyModeFilterNone::operator: UserDataEnabled: %d"), 
            aUserDataEnabled );
        }

    return ret;
    }
