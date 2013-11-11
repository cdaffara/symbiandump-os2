/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* MCTKeystoreUIDs.h
*
*/


/**
 @file 
 @publishedPartner
 @released
*/
 
#ifndef __MCTKEYSTOREUIDS_H__
#define __MCTKEYSTOREUIDS_H__

#include <e32std.h>

/** The UID of a key store interface */
const TInt KInterfaceKeyStore = 0x101F7334;

/** The type ID of CCTKeyInfo objects */
const TUid KRSARepudiableSignerUID = {0x101F7A3C};
const TUid KDSARepudiableSignerUID = {0x101F7A3D};
const TUid KPrivateDecryptorUID = {0x101F7A3E};
const TUid KKeyAgreementUID = {0x101F7A3F};


#endif // __MCTKEYSTOREUIDS_H__
