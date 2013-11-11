/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Part of WLAN HAL API
*
*/

/*
* %version: 4 %
*/

inline void Wha::Attach( MWhaCb& aWhaCb )
    {
    iWhaCb = &aWhaCb; 
    }

inline MWhaCb& Wha::WhaCb()
    {
    return *iWhaCb;
    }

inline WlanHpa& Wha::Hpa()
    {
    return iHpa;
    }

inline MWlanOsa& Wha::OsaCb()
    {
    return iOsa;
    }

inline WlanSpia& Wha::Spia()
    {
    return iSpia;
    }
