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
* Description:  Inline functions of CWlanScanInfo class.
*
*/

/*
* %version: 6 %
*/

#ifndef WLANSCANINFO_INL
#define WLANSCANINFO_INL

// Static constructor.
inline CWlanScanInfo* CWlanScanInfo::NewL()
    {
    const TUid KCWlanScanInfoUid = { 0x101f8f01 };

    TAny* interface = REComSession::CreateImplementationL(
        KCWlanScanInfoUid,
        _FOFF( CWlanScanInfo,
        iInstanceIdentifier ) );
    return reinterpret_cast<CWlanScanInfo*>( interface );
    }
    
// Destructor
inline CWlanScanInfo::~CWlanScanInfo()
    {
    REComSession::DestroyedImplementation( iInstanceIdentifier );
    }

#endif // WLANSCANINFO_INL
            
// End of File
