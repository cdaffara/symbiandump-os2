/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of inline methods
*
*/

/*
* %version: 3 %
*/

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline WlanSignalPredictor::~WlanSignalPredictor()
    {
    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::~WlanSignalPredictor") );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanSignalPredictor::Start()
    {
    OsTracePrint( KRxFrame, (TUint8*)("UMAC: WlanSignalPredictor::Start") );
    iRunning = ETrue;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanSignalPredictor::Stop()
    {
    OsTracePrint( KRxFrame, (TUint8*)("UMAC: WlanSignalPredictor::Stop") );
    iRunning = EFalse;
    }
