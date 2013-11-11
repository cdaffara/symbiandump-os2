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
* Description:  Inline functions of CWlanControlClient class.
*
*/

/*
* %version: 3 %
*/

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline CWlanControlClient* CWlanControlClient::NewL()
    {
    const TUid KCWlanControlClientUid = { 0x10282e0f };

    TAny* interface = REComSession::CreateImplementationL(
        KCWlanControlClientUid,
        _FOFF( CWlanControlClient,
        iInstanceIdentifier ) );
    return reinterpret_cast<CWlanControlClient*>( interface );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline CWlanControlClient::~CWlanControlClient()
    {
    REComSession::DestroyedImplementation( iInstanceIdentifier );
    }
