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
* Description:  Inline functions of CWlanEapolClient class.
*
*/

/*
* %version: 4 %
*/

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline CWlanEapolClient* CWlanEapolClient::NewL(
    TInt aUid,
    MWlanEapolCallbackInterface * aPartner )
    {
    const TUid KTMPUid = { aUid };

    TAny* interface = REComSession::CreateImplementationL(
        KTMPUid,
        _FOFF( CWlanEapolClient, iInstanceIdentifier ),
        aPartner );

    return reinterpret_cast<CWlanEapolClient*>( interface );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline CWlanEapolClient::~CWlanEapolClient()
    {
    REComSession::DestroyedImplementation( iInstanceIdentifier );
    }
