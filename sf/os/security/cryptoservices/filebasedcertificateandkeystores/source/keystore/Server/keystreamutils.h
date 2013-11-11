/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Serialization for public and private keys
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __KEYSTREAMUTILS_H__
#define __KEYSTREAMUTILS_H__

#include <s32strm.h>
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#include <s32std.h>
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#include "fsmarshaller.h"

class CRSAPublicKey;
class CRSAPrivateKey;
class CDSAPublicKey;
class CDSAPrivateKey;
class RInteger;

EXTERNALIZE_FUNCTION(CRSAPublicKey)
EXTERNALIZE_FUNCTION(CRSAPrivateKey)
EXTERNALIZE_FUNCTION(CDSAPublicKey)
EXTERNALIZE_FUNCTION(CDSAPrivateKey)

void ExternalizeL(const CRSAPublicKey& aData, RWriteStream& aStream);
void ExternalizeL(const CRSAPrivateKey& aData, RWriteStream& aStream);
void ExternalizeL(const CDSAPublicKey& aData, RWriteStream& aStream);
void ExternalizeL(const CDSAPrivateKey& aData, RWriteStream& aStream);

void CreateL(RReadStream& aStream, CRSAPublicKey*& aOut);
void CreateL(RReadStream& aStream, CRSAPrivateKey*& aOut);
void CreateL(RReadStream& aStream, CDSAPublicKey*& aOut);
void CreateL(RReadStream& aStream, CDSAPrivateKey*& aOut);

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
HBufC8* DecryptFromStreamL( RReadStream& aInStream, TPtrC8& aKey );
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#endif
