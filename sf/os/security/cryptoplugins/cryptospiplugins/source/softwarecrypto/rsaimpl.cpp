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


#include "rsaimpl.h"
#include "rsafunction.h"
#include "pluginconfig.h"
#include <cryptopanic.h>
#include <cryptostrength.h>
#include <securityerr.h>

using namespace SoftwareCrypto;

/* CRSAImpl */
CRSAImpl::CRSAImpl(
	TUid aCryptoMode,
	TUid aPadding) :
	CAsymmetricCipherImpl(aCryptoMode, aPadding)
	{
	}

CRSAImpl* CRSAImpl::NewL(const CKey& aKey, TUid aCryptoMode, TUid aPadding)
	{
	CRSAImpl* self = CRSAImpl::NewLC(aKey, aCryptoMode, aPadding);
	CleanupStack::Pop(self);
	return self;
	}
	
CRSAImpl* CRSAImpl::NewLC(const CKey& aKey, TUid aCryptoMode, TUid aPadding)
	{
	CRSAImpl* self = new(ELeave) CRSAImpl(aCryptoMode, aPadding);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}
	
CRSAImpl::~CRSAImpl()
	{
	}
	
TInt CRSAImpl::GetMaximumOutputLengthL() const
	{
	const TInteger& N = iKey->GetBigIntL(KRsaKeyParameterNUid);
	
	if (iCryptoMode.iUid == KCryptoModeDecrypt)
		return N.ByteCount() - iPadding->MinPaddingLength();
	else
		return N.ByteCount();
	}
	
TInt CRSAImpl::GetMaximumInputLengthL() const
	{
	const TInteger& N = iKey->GetBigIntL(KRsaKeyParameterNUid);
	
	if (iCryptoMode.iUid == KCryptoModeEncrypt)
		return N.ByteCount() - iPadding->MinPaddingLength();
	else
		return N.ByteCount();
	}
	
void CRSAImpl::ConstructL(const CKey& aKey)
	{
	const TInteger& N = aKey.GetBigIntL(KRsaKeyParameterNUid);
	TCrypto::IsAsymmetricWeakEnoughL(N.BitCount());
	CAsymmetricCipherImpl::ConstructL(aKey);
	
	if (! IsValidKeyLengthL(N.ByteCount()))
		{
		User::Leave(KErrKeySize);
		}
	}
	
CExtendedCharacteristics* CRSAImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}
	
const CExtendedCharacteristics* CRSAImpl::GetExtendedCharacteristicsL()
	{
	return CRSAImpl::CreateExtendedCharacteristicsL();
	}

TUid CRSAImpl::ImplementationUid() const
	{
	return KCryptoPluginRsaCipherUid;
	}
	
void CRSAImpl::EncryptL(const TDesC8& aInput, TDes8& aOutput) const
	{
	__ASSERT_DEBUG(aOutput.MaxLength() >= GetMaximumOutputLengthL(), User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));
	__ASSERT_DEBUG(aInput.Length() <= GetMaximumInputLengthL(), User::Panic(KCryptoPanic, ECryptoPanicInputTooLarge));
	
	HBufC8* buf = HBufC8::NewLC(GetMaximumOutputLengthL());
	TPtr8 ptr = buf->Des();
	
	iPadding->PadL(aInput, ptr);
	RInteger input = RInteger::NewL(ptr);
	CleanupStack::PushL(input);
	
	RInteger output;
	RSAFunction::EncryptL(*iKey, input, output);
	CleanupStack::PushL(output);
	
	aOutput.Append(*(output.BufferLC()));
	CleanupStack::PopAndDestroy(4, buf); //BufferLC, output, input, buf
	}

void CRSAImpl::DecryptL(const TDesC8& aInput, TDes8& aOutput) const
	{
	__ASSERT_DEBUG(aOutput.MaxLength() >= GetMaximumOutputLengthL(), User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));
	__ASSERT_DEBUG(aInput.Length() <= GetMaximumInputLengthL(), User::Panic(KCryptoPanic, ECryptoPanicInputTooLarge));
	
	RInteger input = RInteger::NewL(aInput);
	CleanupStack::PushL(input);
	
	RInteger output;
	
	RSAFunction::DecryptL(*iKey, input, output);
	CleanupStack::PushL(output);
	
	TPtrC8 ptr = *(output.BufferLC());
	iPadding->UnPadL(ptr, aOutput);
	
	CleanupStack::PopAndDestroy(3, &input); //BufferLC(), output, input
	}

void CRSAImpl::ProcessL(const TDesC8& aInput, TDes8& aOutput)
	{
	if (iCryptoMode.iUid == KCryptoModeEncrypt)
		{
		EncryptL(aInput, aOutput);
		}
	else
		{
		DecryptL(aInput, aOutput);
		}
	}

TBool CRSAImpl::IsValidKeyLengthL(TInt aKeyBytes) const
	{
	if (aKeyBytes < 1)
		return EFalse;
	
	switch (iCryptoMode.iUid)
		{
		case KCryptoModeEncrypt:
			// Check if GetMaximumInputLengthL() makes sense,
			// if not the key length must be too small
			if (GetMaximumInputLengthL() <= 0)
				return EFalse;
			break;
		
		case KCryptoModeDecrypt:
			// Check if GetMaximumOutputLengthL() makes sense,
			// if not the key length must be too small
			if (GetMaximumOutputLengthL() <= 0)
				return EFalse;
			break;
		}
	return ETrue;
	}

