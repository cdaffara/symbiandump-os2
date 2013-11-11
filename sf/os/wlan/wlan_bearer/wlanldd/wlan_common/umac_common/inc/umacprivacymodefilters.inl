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
* Description:   Implementation of WlanPrivacyModeFilters inline methods.
*
*/

/*
* %version: 7 %
*/

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WlanPrivacyModeFilters::WlanPrivacyModeFilters() 
    : iActiveFilter( NULL ) 
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WlanPrivacyModeFilters::~WlanPrivacyModeFilters() 
    {
    iActiveFilter = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanPrivacyModeFilters::ExecuteFilter( 
    const SDataFrameHeader& aFrameheader, 
    TBool aUserDataEnabled, 
    TUint16 aEthernetType,
    TBool aUnicastKeyExists, 
    TBool aAesOrTkipOrWapiEncrypted ) const    
    {
    return (*iActiveFilter)( 
        aFrameheader, 
        aUserDataEnabled, 
        aEthernetType, 
        aUnicastKeyExists, 
        aAesOrTkipOrWapiEncrypted ); 
    }
