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


#include "rc2.h"
#include "rc2table.h"
#include "../common/inlines.h"
#include <cryptostrength.h>

const TInt KRC2BlockBytes = 8;

/* CRC2Encryptor */

void CRC2::SetKey(const TDesC8& aKey, TInt aEffectiveKeyLenBits)
	{
	TUint keyLen = (TUint)aKey.Size();

	iKey.Copy(aKey);
	iEffectiveKeyLenBits = aEffectiveKeyLenBits;

	TUint8 L[KRC2MaxKeySizeBytes];	
	Mem::Copy((TUint8*)&L[0], (TUint8*)&aKey[0], keyLen);

	TInt maxKeySizeBytes = (TInt)KRC2MaxKeySizeBytes;
	TInt expandedKeyLen = (TInt)KRC2ExpandedKeyLen;
	TInt i = keyLen;
	for (; i < maxKeySizeBytes; i++)
		{
		L[i] = RC2_TABLE::PITABLE[(L[i-1] + L[i-keyLen]) & 255];
		}

	TUint T8 = (aEffectiveKeyLenBits+7) / 8;
	TUint8 TM = (TUint8)(255 >> ((8-(iEffectiveKeyLenBits%8))%8));
	L[128-T8] = RC2_TABLE::PITABLE[L[128-T8] & TM];

	for (i=127-T8; i>=0; i--)
		L[i] = RC2_TABLE::PITABLE[L[i+1] ^ L[i+T8]];

	for (i=0; i < expandedKeyLen; i++)
		iK[i] = (TUint16)(L[2*i] + (L[2*i+1] << 8));
	}

void CRC2::Reset()
	{
	SetKey(iKey, iEffectiveKeyLenBits);
	}

TInt CRC2::BlockSize() const
	{
	return KRC2BlockBytes;
	}

TInt CRC2::KeySize() const
	{
	return iKey.Size();
	}

CRC2::CRC2(void)
	{
	}

/* CRC2Encryptor */

EXPORT_C CRC2Encryptor* CRC2Encryptor::NewL(const TDesC8& aKey, 
	TInt aEffectiveKeyLenBits)
	{
	CRC2Encryptor* me = CRC2Encryptor::NewLC(aKey, aEffectiveKeyLenBits);
	CleanupStack::Pop(me);
	return (me);
	}

EXPORT_C CRC2Encryptor* CRC2Encryptor::NewLC(const TDesC8& aKey, 
	TInt aEffectiveKeyLenBits)
	{
	CRC2Encryptor* me = new (ELeave) CRC2Encryptor;
	CleanupStack::PushL(me);	//	Does not leave but function requires it be Push-ed
	me->SetKey(aKey, aEffectiveKeyLenBits);	
	// weak enough if either aKey or aEffectiveKeyLenBits is weak
	TInt minKeySize = Min(aEffectiveKeyLenBits, BytesToBits(aKey.Size()));
	TCrypto::IsSymmetricWeakEnoughL(minKeySize);
	return (me);
	}

#pragma warning (disable : 4244)	//	conversion from 'int' to 'unsigned short', possible loss of data
void CRC2Encryptor::Transform(TDes8& aBlock)
	{
	assert(aBlock.Size() == KRC2BlockBytes);
	
	TUint16 R0, R1, R2, R3;
	GetBlockLittleEndian((TUint8*)&aBlock[0], R0, R1, R2, R3);
	
	TInt i = 0;
	for (; i < 16; i++)
		{
		R0 += (R1 & ~R3) + (R2 & R3) + iK[4*i+0];
		R0 = rotlFixed(R0, 1);

		R1 += (R2 & ~R0) + (R3 & R0) + iK[4*i+1];
		R1 = rotlFixed(R1, 2);

		R2 += (R3 & ~R1) + (R0 & R1) + iK[4*i+2];
		R2 = rotlFixed(R2, 3);

		R3 += (R0 & ~R2) + (R1 & R2) + iK[4*i+3];
		R3 = rotlFixed(R3, 5);

		if (i == 4 || i == 10)
			{
			R0 += iK[R3 & 63];
			R1 += iK[R0 & 63];
			R2 += iK[R1 & 63];
			R3 += iK[R2 & 63];
			}
		}

	PutBlockLittleEndian((TUint8*)&aBlock[0], R0, R1, R2, R3);	
	}
#pragma warning (default : 4244)	//	conversion from 'int' to 'unsigned short', possible loss of data

CRC2Encryptor::CRC2Encryptor(void)
	{
	}

/* CRC2Decryptor */

EXPORT_C CRC2Decryptor* CRC2Decryptor::NewL(const TDesC8& aKey, 
	TInt aEffectiveKeyLenBits)
	{
	CRC2Decryptor* me = CRC2Decryptor::NewLC(aKey, aEffectiveKeyLenBits);
	CleanupStack::Pop(me);
	return (me);
	}

EXPORT_C CRC2Decryptor* CRC2Decryptor::NewLC(const TDesC8& aKey, 
	TInt aEffectiveKeyLenBits)
	{
	CRC2Decryptor* me = new (ELeave) CRC2Decryptor;
	CleanupStack::PushL(me);	//	Does not leave but function requires it be Push-ed
	me->SetKey(aKey, aEffectiveKeyLenBits);
	// weak enough if either aKey or aEffectiveKeyLenBits is weak
	TInt minKeySize = Min(aEffectiveKeyLenBits, BytesToBits(aKey.Size()));
	TCrypto::IsSymmetricWeakEnoughL(minKeySize);
	return (me);
	}

#pragma warning (disable : 4244)	//	conversion from 'int' to 'unsigned short', possible loss of data
void CRC2Decryptor::Transform(TDes8& aBlock)
	{
	assert(aBlock.Size() == KRC2BlockBytes);

	TUint16 R0, R1, R2, R3;
	GetBlockLittleEndian((TUint8*)&aBlock[0], R0, R1, R2, R3);

	TInt i = 15;
	for (; i >= 0; i--)
		{
		if (i == 4 || i == 10)
			{
			R3 -= iK[R2 & 63];
			R2 -= iK[R1 & 63];
			R1 -= iK[R0 & 63];
			R0 -= iK[R3 & 63];
			}

		R3 = rotrFixed(R3, 5);
		R3 -= (R0 & ~R2) + (R1 & R2) + iK[4*i+3];

		R2 = rotrFixed(R2, 3);
		R2 -= (R3 & ~R1) + (R0 & R1) + iK[4*i+2];

		R1 = rotrFixed(R1, 2);
		R1 -= (R2 & ~R0) + (R3 & R0) + iK[4*i+1];

		R0 = rotrFixed(R0, 1);
		R0 -= (R1 & ~R3) + (R2 & R3) + iK[4*i+0];
		}

	PutBlockLittleEndian((TUint8*)&aBlock[0], R0, R1, R2, R3);
	}

#pragma warning (default : 4244)	//	conversion from 'int' to 'unsigned short', possible loss of data

CRC2Decryptor::CRC2Decryptor(void)
	{
	}
