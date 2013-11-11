/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanPrivacyModeFilters class.
*
*/

/*
* %version: 6 %
*/

#include "config.h"
#include "umacprivacymodefilters.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanPrivacyModeFilters::SetActiveFilter(     
    TEncryptionStatus aEncryptionStatus )
    {
    if ( aEncryptionStatus == EEncryptionDisabled )
        {
        OsTracePrint( KUmacProtocolState, (TUint8*)
            ("UMAC: WlanPrivacyModeFilters::SetActiveFilter: set none mode privacy filter") );
        iActiveFilter = &iNone;
        }
    else if ( aEncryptionStatus == EEncryptionWep )
        {
        OsTracePrint( KUmacProtocolState, (TUint8*)
            ("UMAC: WlanPrivacyModeFilters::SetActiveFilter: set WEP mode privacy filter") );
        iActiveFilter = &iWep;
        }
    else if ( aEncryptionStatus == EEncryption802dot1x )
        {
        OsTracePrint( KUmacProtocolState, (TUint8*)
            ("UMAC: WlanPrivacyModeFilters::SetActiveFilter: set 802dot1x mode privacy filter") );
        iActiveFilter = &i1x;
        }
    else if ( aEncryptionStatus == EEncryptionWpa )
        {
        OsTracePrint( KUmacProtocolState, (TUint8*)
            ("UMAC: WlanPrivacyModeFilters::SetActiveFilter: set WPA mode privacy filter") );
        iActiveFilter = &iWpa;
        }
    else if ( aEncryptionStatus == EEncryptionWepMixed )
        {
        OsTracePrint( KUmacProtocolState, (TUint8*)
            ("UMAC: WlanPrivacyModeFilters::SetActiveFilter: set WEP mixed mode privacy filter") );
        iActiveFilter = &iMixedModeWep;
        }    
    else if ( aEncryptionStatus == EEncryption802dot1xMixed )
        {
        OsTracePrint( KUmacProtocolState, (TUint8*)
            ("UMAC: WlanPrivacyModeFilters::SetActiveFilter: set 802.1x mixed mode privacy filter") );
        iActiveFilter = &iMixedMode1x;
        }
    else if ( aEncryptionStatus == EEncryptionWAPI )
        {
        OsTracePrint( KUmacProtocolState, (TUint8*)
            ("UMAC: WlanPrivacyModeFilters::SetActiveFilter: set WAPI mode privacy filter") );
        iActiveFilter = &iWapi;
        }
    else
        {
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanPrivacyModeFilters::SetActiveFilter: no active Rx filter set; aEncryptionStatus: %d"), 
            aEncryptionStatus );
        }
    }
