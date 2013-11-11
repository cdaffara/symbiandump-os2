/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline functions of CWlanMgmtClient class.
*
*/

/*
* %version: 5 %
*/

#ifndef WLANMGMTCLIENT_INL
#define WLANMGMTCLIENT_INL

// ---------------------------------------------------------
// CWlanScanRequest::NewL
// ---------------------------------------------------------
//
inline CWlanMgmtClient* CWlanMgmtClient::NewL()
    {
    const TUid KCWlanMgmtClientUid = { 0x101f8eff };

    TAny* interface = REComSession::CreateImplementationL(
        KCWlanMgmtClientUid,
        _FOFF( CWlanMgmtClient,
        iInstanceIdentifier ) );
    return reinterpret_cast<CWlanMgmtClient*>( interface );
    }
    
// ---------------------------------------------------------
// CWlanScanRequest::~CWlanMgmtClient
// ---------------------------------------------------------
//
inline CWlanMgmtClient::~CWlanMgmtClient()
    {
    REComSession::DestroyedImplementation( iInstanceIdentifier );
    }

#endif // WLANMGMTCLIENT_INL
            
// End of File
