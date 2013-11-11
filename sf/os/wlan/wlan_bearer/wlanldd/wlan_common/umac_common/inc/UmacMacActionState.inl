/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of WlanMacActionState inline methods.
*
*/

/*
* %version: 7 %
*/

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline WlanMacActionState::~WlanMacActionState() 
    {
    iDot11HistoryState = NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline WlanDot11State& WlanMacActionState::Dot11History()
    {
    return *iDot11HistoryState;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanMacActionState::Dot11History( 
    WlanDot11State& aState )
    {
    iDot11HistoryState = &aState;
    }
