/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline functions of agent-hotspot interface plugin
*
*/

/*
* %version: 3 %
*/

#ifndef WLANAGTHOTSPOTCLIENT_INL
#define WLANAGTHOTSPOTCLIENT_INL

// Static constructor.
inline CWlanAgtHotSpotClient* CWlanAgtHotSpotClient::NewL( )
    {
    const TUid KWlanAgtHotSpotClient = { 0x1028309d };

    TAny* interface = REComSession::CreateImplementationL( KWlanAgtHotSpotClient,
        _FOFF( CWlanAgtHotSpotClient, iInstanceIdentifier ) );
    return reinterpret_cast<CWlanAgtHotSpotClient*>( interface );
    }
    
// Destructor
inline CWlanAgtHotSpotClient::~CWlanAgtHotSpotClient()
    {
    REComSession::DestroyedImplementation( iInstanceIdentifier );
    }

#endif // WLANAGTHOTSPOTCLIENT_INL
