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
* Description:  inline implementation of WlanSpia
*
*/

/*
* %version: 4 %
*/

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline MWlanOsaExt& WlanSpia::OsaExtCb()
    {
    return *iOsaExt;
    }

inline void WlanSpia::Attach( MWlanOsaExt& aOsaExt )
    {
    iOsaExt = &aOsaExt;
    }
