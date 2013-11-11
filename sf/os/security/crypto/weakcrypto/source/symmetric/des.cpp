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


#include "des.h"
#include "destables.h"
#include "../common/inlines.h"
#include "des.inl"
#include <cryptostrength.h>

const TInt KDESBlockBytes = 8;
const TInt KDESKeyBytes = 8;

//	bit 0 is left-most in byte
static const TInt bytebit[] = {0200,0100,040,020,010,04,02,01};

void CDES::Transform(TDes8& aBlock)
	{
	assert(aBlock.Size() == KDESBlockBytes);

	TUint32 l, r;
	// Split the block into 2 word-sized big endian portions
	GetBlockBigEndian((TUint8*)&aBlock[0], l, r);

	IPerm(l,r);

	CDES::DoTransform(l, r, iK1);
		
	FPerm(l,r);

	// Put the portions back into the block as little endian
	PutBlockBigEndian((TUint8*)&aBlock[0], r, l);
	}

void CDES::DoTransform(TUint32& l, TUint32& r, const TUint32* aKey)
	{
	TInt i = 0;
	for (; i<8; i++)
		{
		TUint32 work = rotrFixed(r, 4U) ^ aKey[4*i+0];
		l ^= DES_TABLE::sbox[6][(work) & 0x3f]
		  ^  DES_TABLE::sbox[4][(work >> 8) & 0x3f]
		  ^  DES_TABLE::sbox[2][(work >> 16) & 0x3f]
		  ^  DES_TABLE::sbox[0][(work >> 24) & 0x3f];
		work = r ^ aKey[4*i+1];
		l ^= DES_TABLE::sbox[7][(work) & 0x3f]
		  ^  DES_TABLE::sbox[5][(work >> 8) & 0x3f]
		  ^  DES_TABLE::sbox[3][(work >> 16) & 0x3f]
		  ^  DES_TABLE::sbox[1][(work >> 24) & 0x3f];

		work = rotrFixed(l, 4U) ^ aKey[4*i+2];
		r ^= DES_TABLE::sbox[6][(work) & 0x3f]
		  ^  DES_TABLE::sbox[4][(work >> 8) & 0x3f]
		  ^  DES_TABLE::sbox[2][(work >> 16) & 0x3f]
		  ^  DES_TABLE::sbox[0][(work >> 24) & 0x3f];
		work = l ^ aKey[4*i+3];
		r ^= DES_TABLE::sbox[7][(work) & 0x3f]
		  ^  DES_TABLE::sbox[5][(work >> 8) & 0x3f]
		  ^  DES_TABLE::sbox[3][(work >> 16) & 0x3f]
		  ^  DES_TABLE::sbox[1][(work >> 24) & 0x3f];
		}
	}


TInt CDES::BlockSize() const
	{
	return KDESBlockBytes;
	}

TInt CDES::KeySize() const
	{
	return KDESKeyBytes;
	}

CDES::~CDES()
	{
	delete iKey;
	}

void CDES::ConstructL(const TDesC8& aKey, TBool /*aCheckWeakKey*/)
	{
	assert(aKey.Size() == KDESKeyBytes);

	iKey = aKey.AllocL();
	SetKey(aKey, iK1);
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

void CDES::SetKey(const TDesC8& aKey, TUint32* aKeyBuffer)
	{
	TInt i=0, j=0, l=0, m=0;

//	Form a byte array from aKey, taking endianess into account (little->big)	
	TUint8 key[8];								//	For big endian byte array	
	Mem::Copy(&key, &aKey[0], 8);

	TUint8 buffer[56+56+8];
	TUint8* const pc1m = &buffer[0];			/* place to modify pc1 into */
	TUint8* const pcr = pc1m + 56;				/* place to rotate pc1 into */
	TUint8* const ks = pcr + 56;

	for (j=0; j<56; j++) 
		{/* convert pc1 to bits of key */
		l = DES_TABLE::pc1[j]-1;				/* integer bit location  */
		m = l & 07;								/* find bit              */
		pc1m[j]=(key[l>>3] &					/* find which key byte l is in */
			bytebit[m])							/* and which bit of that byte */
			? (TUint8)1 : (TUint8)0;			/* and store 1-bit result */
		}

	for (i=0; i<16; i++) 
		{/* key chunk for each iteration */
		Mem::FillZ(ks,8);							/* Clear key schedule */
		for (j=0; j<56; j++)
		/*	rotate pc1 the right amount */
			pcr[j] = pc1m[(l=j+DES_TABLE::totrot[i])<(j<28? 28 : 56) ? l: l-28];
		
		/* rotate left and right halves independently */
		
		for (j=0; j<48; j++)
			{/* select bits individually */
			/* check bit that goes to ks[j] */
			if (pcr[DES_TABLE::pc2[j]-1])
				{/* mask it in if it's there */
				l= j % 6;
				ks[j/6] |= bytebit[l] >> 2;
				}
			}

		/* Now convert to odd/even interleaved form for use in F */
		(*(aKeyBuffer+(2*i))) = ((TUint32)ks[0] << 24)
			| ((TUint32)ks[2] << 16)
			| ((TUint32)ks[4] << 8)
			| ((TUint32)ks[6]);
		
		(*(aKeyBuffer+(2*i+1))) = ((TUint32)ks[1] << 24)
			| ((TUint32)ks[3] << 16)
			| ((TUint32)ks[5] << 8)
			| ((TUint32)ks[7]);
		}
	}

void CDES::Reset()
	{
	SetKey(*iKey, iK1);
	}


/* CDESEncryptor */

EXPORT_C CDESEncryptor* CDESEncryptor::NewL(const TDesC8& aKey, 
	TBool aCheckWeakKey)
	{
	CDESEncryptor* me = CDESEncryptor::NewLC(aKey, aCheckWeakKey);
	CleanupStack::Pop(me);
	return (me);
	}

EXPORT_C CDESEncryptor* CDESEncryptor::NewLC(const TDesC8& aKey, 
	TBool aCheckWeakKey)
	{
	CDESEncryptor* me = new (ELeave) CDESEncryptor();
	CleanupStack::PushL(me);
	me->ConstructL(aKey, aCheckWeakKey);
	// DES only used 7 bits out of every key byte
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()) - aKey.Size());
	return (me);
	}

CDESEncryptor::CDESEncryptor()
	{
	}

/* CDESDecryptor */

EXPORT_C CDESDecryptor* CDESDecryptor::NewL(const TDesC8& aKey, 
	TBool aCheckWeakKey)
	{
	CDESDecryptor* me = CDESDecryptor::NewLC(aKey, aCheckWeakKey);
	CleanupStack::Pop(me);
	return (me);
	}

EXPORT_C CDESDecryptor* CDESDecryptor::NewLC(const TDesC8& aKey, 
	TBool aCheckWeakKey)
	{
	CDESDecryptor* me = new (ELeave) CDESDecryptor();
	CleanupStack::PushL(me);
	me->ConstructL(aKey, aCheckWeakKey);
	// DES only used 7 bits out of every key byte
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()) - aKey.Size());
	return (me);
	}


CDESDecryptor::CDESDecryptor()
	{
	}

void CDESDecryptor::SetKey(const TDesC8& aKey, TUint32* aKeyBuffer)
	{
	CDES::SetKey(aKey, aKeyBuffer);

	ReverseKeySchedule(iK1);
	}

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

