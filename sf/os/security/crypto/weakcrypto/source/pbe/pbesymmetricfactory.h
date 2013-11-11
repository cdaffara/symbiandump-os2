/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* PBE symmetric factory
*
*/


/**
 @file 
 @internalComponent 
*/
 
#ifndef __PBESYMMETRICFACTORY_H__
#define __PBESYMMETRICFACTORY_H__

#include <symmetric.h>
#include <pbe.h>

const TUint KAESBlockBytes = 16;
const TUint KAESKeyBytes128 = 16;
const TUint KAESKeyBytes192 = 24;
const TUint KAESKeyBytes256 = 32;

const TUint KDESBlockBytes = 8;
const TUint KDESKeyBytes = 8;
const TUint K3DESKeyBytes = 24;

const TUint K2Key3DESKeyBytes = 16;
const TUint  KRC4KeyBytes128 = 16;
const TUint  KRC4KeyBytes40 = 5;

const TUint KRC2BlockBytes = 8;
const TUint KRC2KeyBytes40 = 5;
const TUint KRC2KeyBytes128 = 16;

const TPBECipher KPBEDefaultStrongCipher = ECipherAES_CBC_128;
const TPBECipher KPBEDefaultWeakCipher = ECipherDES_CBC;
const TUint KPBEDefaultSaltBytes = 16; //pkcs5 recommends min 64 bits (8 bytes);

const TUint KPBEMaxCipherIVBytes = KAESBlockBytes;
const TUint KPBEMaxCipherKeyBytes = KAESKeyBytes256;
const TUint KPBEMaxSaltBytes = 16; //pkcs5 recommends min 64 bits (8 bytes);

class PBE
	{
public:
	static TUint GetKeyBytes(TPBECipher aCipher);
	static TUint GetBlockBytes(TPBECipher aCipher);

	static CSymmetricCipher* MakeEncryptorL(TPBECipher aCipher, 
		const TDesC8& aKey, const TDesC8& aIV);
	static CSymmetricCipher* MakeDecryptorL(TPBECipher aCipher, 
		const TDesC8& aKey, const TDesC8& aIV);
private:
	PBE(void);
	PBE(const PBE&);
	PBE& operator=(const PBE&);	
	};

#endif
