/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "desextendimpl.h"

#include "destables.h"
#include "../../../source/common/inlines.h"
#include "../des.inl"
#include "pluginconfig.h"
#include "symmetriccipherimpl.h"
#include <cryptostrength.h>


//	bit 0 is left-most in byte
static const TInt bytebit[] = {0200,0100,040,020,010,04,02,01};

//Extended Charcteristics

static const TInt32 KExtendCharAttribute1 = 0x102ABCD1;
static const TUid KExtendCharAttribute1Uid ={KExtendCharAttribute1};

static const TInt32 KExtendCharAttribute2 = 0x102ABCD2;
static const TUid KExtendCharAttribute2Uid ={KExtendCharAttribute2};

static const TInt32 KExtendCharAttribute3 = 0x102ABCD3;
static const TUid KExtendCharAttribute3Uid ={KExtendCharAttribute3};

using namespace SoftwareCrypto;

/* CDesImpl */
CDesExtendImpl::CDesExtendImpl(
	TUid aImplementationUid,
	TUint8 aBlockBytes,
	TUid aCryptoMode,
	TUid aOperationMode,
	TUid aPadding) : 
	CSymmetricBlockCipherImpl(aBlockBytes, aCryptoMode, aOperationMode, aPadding),
	iImplementationUid(aImplementationUid)
	{
	}

CDesExtendImpl* CDesExtendImpl::NewL(TUid aImplementationUid, const CKey& aKey, TUid aCryptoMode, TUid aOperationMode, TUid aPadding)
	{
	CDesExtendImpl* self = CDesExtendImpl::NewLC(aImplementationUid, aKey, aCryptoMode, aOperationMode, aPadding);
	CleanupStack::Pop(self);
	return self;
	}
	
CDesExtendImpl* CDesExtendImpl::NewLC(TUid aImplementationUid, const CKey& aKey, TUid aCryptoMode, TUid aOperationMode, TUid aPadding)
	{
	CDesExtendImpl* self = new(ELeave) CDesExtendImpl(aImplementationUid, KDesBlockBytes, aCryptoMode, aOperationMode, aPadding);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	
	const TDesC8& keyContent = aKey.GetTDesC8L(KSymmetricKeyParameterUid);
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(keyContent.Size()) - keyContent.Size());
	return self;
	}
		
CDesExtendImpl::~CDesExtendImpl()
	{
	// make sure key information isn't visible to other processes if the
	// page is reused.
	delete iExtendChars;
	Mem::FillZ(&iK, sizeof(iK));
	}
	
void CDesExtendImpl::ConstructL(const CKey& aKey)
	{
	CSymmetricBlockCipherImpl::ConstructL(aKey);
	iExtendChars = CreateExtendedCharacteristicsL();
	SetKeySchedule();
	}
	
CExtendedCharacteristics* CDesExtendImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	
	//***************************************************************
	CExtendedCharacteristics* exChars = CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	CleanupStack::PushL(exChars);
	
	exChars->AddCharacteristicL(1234,KExtendCharAttribute1Uid);
	exChars->AddCharacteristicL(5678,KExtendCharAttribute2Uid);
	exChars->AddCharacteristicL(_L8("HAPPYDAYS"),KExtendCharAttribute3Uid);
	//**************************************************************
	CleanupStack::Pop(exChars);
	
	return exChars;
	}
	
const CExtendedCharacteristics* CDesExtendImpl::GetExtendedCharacteristicsL()
	{
	return iExtendChars;
	}		
	
TUid CDesExtendImpl::ImplementationUid() const
	{
	return iImplementationUid;
	}
	
TBool CDesExtendImpl::IsValidKeyLength(TInt aKeyBytes) const
	{
	return (aKeyBytes == KDesKeyBytes);
	}
	
TInt CDesExtendImpl::GetKeyStrength() const
	{
	// parity bits are excluded
	return BytesToBits(KDesKeyBytes - 8);
	}	
	
void CDesExtendImpl::TransformEncrypt(
	TUint8* aBuffer,
	TUint aNumBlocks)
	{
	for (TInt i = 0; i < aNumBlocks; ++i)
		{		
		ModeEncryptStart(aBuffer);
		TUint32 l, r;
		// Split the block into 2 word-sized big endian portions
		GetBlockBigEndian(aBuffer, l, r);
		IPerm(l,r);
		DoTransform(l, r, iK);		
		FPerm(l,r);

		// Put the portions back into the block as little endian
		PutBlockBigEndian(aBuffer, r, l);

		ModeEncryptEnd(aBuffer);
		aBuffer += KDesBlockBytes;
		}
	}	
	
void CDesExtendImpl::TransformDecrypt(
	TUint8* aBuffer,
	TUint aNumBlocks)
	{
	for (TInt i = 0; i < aNumBlocks; ++i)
		{		
		ModeDecryptStart(aBuffer);

		TUint32 l, r;
		// Split the block into 2 word-sized big endian portions
		GetBlockBigEndian(aBuffer, l, r);

		IPerm(l,r);
		DoTransform(l, r, iK);		
		FPerm(l,r);

		// Put the portions back into the block as little endian
		PutBlockBigEndian(aBuffer, r, l);

		ModeDecryptEnd(aBuffer);
		aBuffer += KDesBlockBytes;
		}
	}

void CDesExtendImpl::SetKeySchedule()
	{
	if (iCryptoMode.iUid == KCryptoModeEncrypt)
		{
		SetEncryptKeySchedule(*iKey, iK);
		}
	else 
		{
		ASSERT(iCryptoMode.iUid == KCryptoModeDecrypt);
		SetDecryptKeySchedule(*iKey, iK);
		}	
	}		

void CDesExtendImpl::DoTransform(TUint32& l, TUint32& r, const TUint32* aKeySchedule)
	{
	TInt i = 0;
	for (; i<8; i++)
		{
		TUint32 work = rotrFixed(r, 4U) ^ aKeySchedule[4*i+0];
		l ^= DES_TABLE::sbox[6][(work) & 0x3f]
		  ^  DES_TABLE::sbox[4][(work >> 8) & 0x3f]
		  ^  DES_TABLE::sbox[2][(work >> 16) & 0x3f]
		  ^  DES_TABLE::sbox[0][(work >> 24) & 0x3f];
		work = r ^ aKeySchedule[4*i+1];
		l ^= DES_TABLE::sbox[7][(work) & 0x3f]
		  ^  DES_TABLE::sbox[5][(work >> 8) & 0x3f]
		  ^  DES_TABLE::sbox[3][(work >> 16) & 0x3f]
		  ^  DES_TABLE::sbox[1][(work >> 24) & 0x3f];

		work = rotrFixed(l, 4U) ^ aKeySchedule[4*i+2];
		r ^= DES_TABLE::sbox[6][(work) & 0x3f]
		  ^  DES_TABLE::sbox[4][(work >> 8) & 0x3f]
		  ^  DES_TABLE::sbox[2][(work >> 16) & 0x3f]
		  ^  DES_TABLE::sbox[0][(work >> 24) & 0x3f];
		work = l ^ aKeySchedule[4*i+3];
		r ^= DES_TABLE::sbox[7][(work) & 0x3f]
		  ^  DES_TABLE::sbox[5][(work >> 8) & 0x3f]
		  ^  DES_TABLE::sbox[3][(work >> 16) & 0x3f]
		  ^  DES_TABLE::sbox[1][(work >> 24) & 0x3f];
		}
	}	

void CDesExtendImpl::SetEncryptKeySchedule(const TDesC8& aKey, TUint32* aKeySchedule)
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
		(*(aKeySchedule+(2*i))) = ((TUint32)ks[0] << 24)
			| ((TUint32)ks[2] << 16)
			| ((TUint32)ks[4] << 8)
			| ((TUint32)ks[6]);
		
		(*(aKeySchedule+(2*i+1))) = ((TUint32)ks[1] << 24)
			| ((TUint32)ks[3] << 16)
			| ((TUint32)ks[5] << 8)
			| ((TUint32)ks[7]);
		}		
	}

void CDesExtendImpl::SetDecryptKeySchedule(const TDesC8& aKey, TUint32* aKeySchedule)
	{
	SetEncryptKeySchedule(aKey, aKeySchedule);
	ReverseKeySchedule(aKeySchedule);
	}
