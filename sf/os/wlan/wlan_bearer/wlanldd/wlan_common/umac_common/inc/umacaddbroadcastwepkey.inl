/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of WlanAddBroadcastWepKey inline methods.
*
*/

/*
* %version: 6 %
*/

inline WlanAddBroadcastWepKey::~WlanAddBroadcastWepKey()
    {
    iKey = NULL;
    iMemory = NULL;
    }

inline TBool WlanAddBroadcastWepKey::UseAsDefaultKey() const
    {
    return iFlags & KUseAsDefaultKey;
    };

inline TBool WlanAddBroadcastWepKey::UseAsPairwiseKey() const
    {
    return iFlags & KUseAsPairwiseKey;
    };
