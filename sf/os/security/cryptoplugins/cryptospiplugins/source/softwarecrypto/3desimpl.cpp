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


#include "3desimpl.h"

#include "destables.h"
#include "common/inlines.h"
#include "des.inl"
#include "pluginconfig.h"
#include "symmetriccipherimpl.h"
#include <cryptostrength.h>

using namespace SoftwareCrypto;

/* C3DesImpl */
C3DesImpl::C3DesImpl(
	TUid aCryptoMode,
	TUid aOperationMode,
	TUid aPadding) :
	CDesImpl(KDesBlockBytes, aCryptoMode, aOperationMode, aPadding)
	{
	}

C3DesImpl* C3DesImpl::NewL(const CKey& aKey, TUid aCryptoMode, TUid aOperationMode, TUid aPadding)
	{
	C3DesImpl* self = C3DesImpl::NewLC(aKey, aCryptoMode, aOperationMode, aPadding);
	CleanupStack::Pop(self);
	return self;
	}
	
C3DesImpl* C3DesImpl::NewLC(const CKey& aKey, TUid aCryptoMode, TUid aOperationMode, TUid aPadding)
	{
	C3DesImpl* self = new(ELeave) C3DesImpl(aCryptoMode, aOperationMode, aPadding);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);

	const TDesC8& keyContent = aKey.GetTDesC8L(KSymmetricKeyParameterUid);
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(keyContent.Size()) - keyContent.Size());
	return self;
	}
		
C3DesImpl::~C3DesImpl()
	{
	// make sure key information isn't visible to other processes if the
	// page is reused.
	Mem::FillZ(&iK1, sizeof(iK1));
	Mem::FillZ(&iK2, sizeof(iK2));
	Mem::FillZ(&iK3, sizeof(iK3));
	}
	
void C3DesImpl::ConstructL(const CKey& aKey)
	{
	CDesImpl::ConstructL(aKey);
	SetKeySchedule();
	}	
	
CExtendedCharacteristics* C3DesImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}
	
const CExtendedCharacteristics* C3DesImpl::GetExtendedCharacteristicsL()
	{
	return C3DesImpl::CreateExtendedCharacteristicsL();
	}

TUid C3DesImpl::ImplementationUid() const
	{
	return KCryptoPlugin3DesUid;
	}
	
TBool C3DesImpl::IsValidKeyLength(TInt aKeyBytes) const
	{
	return (aKeyBytes == K3DesKeyBytes);
	}
	
TInt C3DesImpl::GetKeyStrength() const
	{
	// Exclude parity bits from each subkey
	return BytesToBits(K3DesKeyBytes - (3 * 8));
	}	
	
void C3DesImpl::TransformEncrypt(
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
		// The mode is applied to the entire operation and NOT 
		// for each DES transform
		TUid opMode = iOperationMode;
		iOperationMode = KOperationModeECBUid;
		DoTransform(l, r, iK1);
		DoTransform(r, l, iK2);
		DoTransform(l, r, iK3);
		iOperationMode = opMode;
		FPerm(l,r);

		// Put the portions back into the block as little endian
		PutBlockBigEndian(aBuffer, r, l);
		ModeEncryptEnd(aBuffer);
		aBuffer += KDesBlockBytes;
		}
	}
	
void C3DesImpl::TransformDecrypt(
	TUint8* aBuffer,
	const TUint aNumBlocks)
	{	
	for (TInt i = 0; i < aNumBlocks; ++i)
		{		
		ModeDecryptStart(aBuffer);
			
		TUint32 l, r;
		// Split the block into 2 word-sized big endian portions
		GetBlockBigEndian(aBuffer, l, r);

		IPerm(l,r);
		
		// The mode is applied to the entire operation and NOT 
		// for each DES transform
		TUid opMode = iOperationMode;
		iOperationMode = KOperationModeECBUid;
		DoTransform(l, r, iK1);
		DoTransform(r, l, iK2);
		DoTransform(l, r, iK3);
		iOperationMode = opMode;
		FPerm(l,r);

		// Put the portions back into the block as little endian
		PutBlockBigEndian(aBuffer, r, l);	
		ModeDecryptEnd(aBuffer);
		aBuffer += K3DesBlockBytes;
		}
	}	

void C3DesImpl::SetKeySchedule()
	{
	if (iCryptoMode.iUid == KCryptoModeEncrypt)
		{
		// Encrypt -> Decrypt -> Encrypt
		// Encryptor key	
		SetEncryptKeySchedule(iKey->Mid(0, KDesKeyBytes), iK1);
		
		// Decryptor key
		SetDecryptKeySchedule(iKey->Mid(KDesKeyBytes, 2 * KDesKeyBytes), iK2);
		
		// Encryptor key
		SetEncryptKeySchedule(iKey->Mid(2 * KDesKeyBytes), iK3);
		}
	else 
		{
		// Decrypt -> Encrypt -> Decrypt
		// Key order is reversed !
		
		ASSERT(iCryptoMode.iUid == KCryptoModeDecrypt);
		// Decryptor key	
		SetDecryptKeySchedule(iKey->Mid(0, KDesKeyBytes), iK3);
		
		// Encryptor key
		SetEncryptKeySchedule(iKey->Mid(KDesKeyBytes, 2 * KDesKeyBytes), iK2);
		
		// Decryptor key
		SetDecryptKeySchedule(iKey->Mid(2 * KDesKeyBytes), iK1);
		}	
	}		
