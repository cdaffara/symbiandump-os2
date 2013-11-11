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


#include "rc2impl.h"
#include "keys.h"

#include "rc2table.h"
#include "common/inlines.h"
#include "pluginconfig.h"
#include "symmetriccipherimpl.h"
#include <cryptostrength.h>

using namespace SoftwareCrypto;

CRc2Impl::CRc2Impl(
	TUid aCryptoMode,
	TUid aOperationMode,
	TUid aPadding) :
	CSymmetricBlockCipherImpl(KRc2BlockBytes, aCryptoMode, aOperationMode, aPadding)
	{
	}

CRc2Impl* CRc2Impl::NewL(
	const CKey& aKey,
	TUid aCryptoMode,
	TUid aOperationMode,
	TUid aPadding,
	TInt aEffectiveKeyLenBits)	
	{
	CRc2Impl* self = CRc2Impl::NewLC(aKey, aCryptoMode, aOperationMode, aPadding, aEffectiveKeyLenBits);
	CleanupStack::Pop(self);
	return self;
	}
	
CRc2Impl* CRc2Impl::NewLC(
	const CKey& aKey,
	TUid aCryptoMode,
	TUid aOperationMode,
	TUid aPadding,
	TInt aEffectiveKeyLenBits)
	{
	CRc2Impl* self = new(ELeave) CRc2Impl(aCryptoMode, aOperationMode, aPadding);
	CleanupStack::PushL(self);
	self->ConstructL(aKey, aEffectiveKeyLenBits);
	
	const TDesC8& keyContent = aKey.GetTDesC8L(KSymmetricKeyParameterUid);
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(keyContent.Size()) - keyContent.Size());
	return self;
	}
		
CRc2Impl::~CRc2Impl()
	{
	Mem::FillZ(&iK, sizeof(iK));
	}
	
void CRc2Impl::ConstructL(const CKey& aKey, TInt aEffectiveKeyLenBits)	
	{
	iEffectiveKeyLenBits = aEffectiveKeyLenBits;	
	CSymmetricBlockCipherImpl::ConstructL(aKey);
	SetKeySchedule();
	}
	
CExtendedCharacteristics* CRc2Impl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

const CExtendedCharacteristics* CRc2Impl::GetExtendedCharacteristicsL()
	{
	return CRc2Impl::CreateExtendedCharacteristicsL();
	}

TUid CRc2Impl::ImplementationUid() const
	{
	return KCryptoPluginRc2Uid;
	}

TBool CRc2Impl::IsValidKeyLength(TInt aKeyBytes) const
	{
	return ((aKeyBytes > 0 && aKeyBytes <= KRc2MaxKeySizeBytes) ? ETrue : EFalse);
	}	
	
TInt CRc2Impl::GetKeyStrength() const
	{
	return Min(iEffectiveKeyLenBits, BytesToBits(iKeyBytes));
	}

void CRc2Impl::SetKeySchedule()
	{				
	TUint keyLen = iKey->Length();
	
	TUint8 L[KRc2MaxKeySizeBytes];	
	Mem::Copy((TUint8*)&L[0], (TUint8*)&(*iKey)[0], keyLen);

	TInt i = keyLen;
	for (; i < KRc2MaxKeySizeBytes; i++)
		{
		L[i] = RC2_TABLE::PITABLE[(L[i-1] + L[i-keyLen]) & 255];
		}

	TUint T8 = (iEffectiveKeyLenBits+7) / 8;
	TUint8 TM = (TUint8)(255 >> ((8-(iEffectiveKeyLenBits%8))%8));
	L[128-T8] = RC2_TABLE::PITABLE[L[128-T8] & TM];

	for (i=127-T8; i>=0; i--)
		L[i] = RC2_TABLE::PITABLE[L[i+1] ^ L[i+T8]];

	for (i=0; i < KRc2ExpandedKeyLen; i++)
		iK[i] = (TUint16)(L[2*i] + (L[2*i+1] << 8));
	}

#pragma warning (disable : 4244)	//	conversion from 'int' to 'unsigned short', possible loss of data
void CRc2Impl::TransformEncrypt(
	TUint8* aBuffer,
	TUint aNumBlocks)
	{
	for (TInt blockNum = 0; blockNum < aNumBlocks; ++blockNum)
		{		
		ModeEncryptStart(aBuffer);
		
		TUint16 R0, R1, R2, R3;
		GetBlockLittleEndian(aBuffer, R0, R1, R2, R3);
		
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
		PutBlockLittleEndian(aBuffer, R0, R1, R2, R3);	
		ModeEncryptEnd(aBuffer);
		aBuffer += KRc2BlockBytes;
		}
	}
#pragma warning (default : 4244)	//	conversion from 'int' to 'unsigned short', possible loss of data


#pragma warning (disable : 4244)	//	conversion from 'int' to 'unsigned short', possible loss of data
void CRc2Impl::TransformDecrypt(
	TUint8* aBuffer,
	TUint aNumBlocks)
	{
	for (TInt blockNum = 0; blockNum < aNumBlocks; ++blockNum)
		{		
		ModeDecryptStart(aBuffer);

		TUint16 R0, R1, R2, R3;
		GetBlockLittleEndian(aBuffer, R0, R1, R2, R3);

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

		PutBlockLittleEndian(aBuffer, R0, R1, R2, R3);
		ModeDecryptEnd(aBuffer);
		aBuffer += KRc2BlockBytes;
		}
	}
#pragma warning (default : 4244)	//	conversion from 'int' to 'unsigned short', possible loss of data
