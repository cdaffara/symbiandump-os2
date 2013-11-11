/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <des.h>
#include "desshim.h"

const TInt KDESBlockBytes = 8;
const TInt KDESKeyBytes = 8;

CDES::~CDES()
	{
	}

CDES::CDES()
	{
	}

typedef TUint8 TKeyDES[KDESKeyBytes];
const TInt KKnownWeakKeysCount = 16;

const TKeyDES weak_keys[KKnownWeakKeysCount] =
	{
	/* weak keys */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE},
	{0x1E,0x1E,0x1E,0x1E,0x0E,0x0E,0x0E,0x0E},
	{0xE0,0xE0,0xE0,0xE0,0xF0,0xF0,0xF0,0xF0},
	{0x00,0xFE,0x00,0xFE,0x00,0xFE,0x00,0xFE},
	{0xFE,0x00,0xFE,0x00,0xFE,0x00,0xFE,0x00},
	{0x1E,0xE0,0x1E,0xE0,0x0E,0xF0,0x0E,0xF0},
	{0xE0,0x1E,0xE0,0x1E,0xF0,0x0E,0xF0,0x0E},
	{0x00,0xE0,0x00,0xE0,0x00,0xF0,0x00,0xF0},
	{0xE0,0x00,0xE0,0x00,0xF0,0x00,0xF0,0x00},
	{0x1E,0xFE,0x1E,0xFE,0x0E,0xFE,0x0E,0xFE},
	{0xFE,0x1E,0xFE,0x1E,0xFE,0x0E,0xFE,0x0E},
	{0x00,0x1E,0x00,0x1E,0x00,0x0E,0x00,0x0E},
	{0x1E,0x00,0x1E,0x00,0x0E,0x00,0x0E,0x00},
	{0xE0,0xFE,0xE0,0xFE,0xF0,0xFE,0xF0,0xFE},
	{0xFE,0xE0,0xFE,0xE0,0xFE,0xF0,0xFE,0xF0}
	};
	
EXPORT_C TBool CDES::IsWeakKey(const TDesC8& aKey)
	{
	TKeyDES key;
	TInt index = 0;
	//Reset parity bits
	for(; index < KDESKeyBytes; index++)
		{
		key[index] = aKey[index] & 0xFE;
		}
		
	TBool weak = EFalse;	
	//Compare key with potential weak keys without parity	
	for (index=0; index < KKnownWeakKeysCount; index++)
		{
		if (Mem::Compare(weak_keys[index], KDESKeyBytes, &key[0], KDESKeyBytes)==0)
			{
			weak = ETrue;
			break;
			}
		}
	return weak;
	}

/* CDESEncryptor */
EXPORT_C CDESEncryptor* CDESEncryptor::NewL(const TDesC8& aKey, TBool /*aCheckWeakKey*/)
	{
	return CDESEncryptorShim::NewL(aKey);
	}

EXPORT_C CDESEncryptor* CDESEncryptor::NewLC(const TDesC8& aKey, TBool /*aCheckWeakKey*/)
	{
	return CDESEncryptorShim::NewLC(aKey);
	}

CDESEncryptor::CDESEncryptor()
	{
	}

/* CDESDecryptor */
EXPORT_C CDESDecryptor* CDESDecryptor::NewL(const TDesC8& aKey, TBool /*aCheckWeakKey*/)
	{
	return CDESDecryptorShim::NewL(aKey);
	}

EXPORT_C CDESDecryptor* CDESDecryptor::NewLC(const TDesC8& aKey, TBool /*aCheckWeakKey*/)
	{
	return CDESDecryptorShim::NewLC(aKey);
	}

CDESDecryptor::CDESDecryptor()
	{	
	}
	
// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

void CDES::Transform(TDes8& /*aBlock*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);	
	}

TInt CDES::BlockSize() const
	{
	// Method replaced by shim 
	ASSERT(EFalse);			
	return KDESBlockBytes;
	}

TInt CDES::KeySize() const
	{
	// Method replaced by shim 
	ASSERT(EFalse);			
	return KDESKeyBytes;
	}
	
void CDES::Reset()
	{
	// Method replaced by shim 
	ASSERT(EFalse);			
	}	
	
void CDES::SetKey(const TDesC8& /*aKey*/, TUint32* /*aKeyBuffer*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);				
	}
	
void CDES::ConstructL(const TDesC8& /*aKey*/, TBool /*aCheckWeakKey*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);			
	}

void CDESDecryptor::SetKey(const TDesC8& /*aKey*/, TUint32* /*aKeyBuffer*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);	
	}
