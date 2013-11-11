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


#include "rsaverifyimpl.h"
#include "pluginconfig.h"
#include "rsafunction.h"

using namespace SoftwareCrypto;

// Implementation of CRSAVerifierImpl
CRSAVerifierImpl* CRSAVerifierImpl::NewL(const CKey& aKey, TUid aPaddingMode)
	{
	CRSAVerifierImpl* self = CRSAVerifierImpl::NewLC(aKey, aPaddingMode);
	CleanupStack::Pop(self);
	return self;
	}
	
CRSAVerifierImpl* CRSAVerifierImpl::NewLC(const CKey& aKey, TUid aPaddingMode)
	{
	CRSAVerifierImpl* self = new(ELeave) CRSAVerifierImpl(aPaddingMode);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}

CRSAVerifierImpl::CRSAVerifierImpl(TUid aPaddingMode)
	: iPaddingMode(aPaddingMode)
	{
	}

CRSAVerifierImpl::~CRSAVerifierImpl()
	{
	delete iPadding;
	}
	
void CRSAVerifierImpl::ConstructL(const CKey& aKey)
	{
	CVerifierImpl::ConstructL(aKey);
	SetPaddingModeL(iPaddingMode);
	}
	
CExtendedCharacteristics* CRSAVerifierImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

const CExtendedCharacteristics* CRSAVerifierImpl::GetExtendedCharacteristicsL()
	{
	return CRSAVerifierImpl::CreateExtendedCharacteristicsL();
	}

TUid CRSAVerifierImpl::ImplementationUid() const
	{
	return KCryptoPluginRsaVerifierUid;
	}

void CRSAVerifierImpl::SetPaddingModeL(TUid aPaddingMode) 
	{
	CPadding* padding(0);
	switch (aPaddingMode.iUid)
		{
		case KPaddingModeNone:
			padding = CPaddingNone::NewL(GetMaximumOutputLengthL());
			break;
		case KPaddingModePkcs1_v1_5_Signature:
			padding = CPaddingPKCS1Signature::NewL(GetMaximumOutputLengthL());
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	delete iPadding;
	iPadding = padding;
	iPaddingMode = aPaddingMode;
	Reset();	
	}

void CRSAVerifierImpl::SetKeyL(const CKey& aPublicKey)
	{
	DoSetKeyL(aPublicKey);
	Reset();	
	}

TInt CRSAVerifierImpl::GetMaximumInputLengthL() const
	{
	return GetMaximumOutputLengthL() - iPadding->MinPaddingLength();	
	}

TInt CRSAVerifierImpl::GetMaximumOutputLengthL() const
	{
	const TInteger& paramN = iKey->GetBigIntL(KRsaKeyParameterNUid);
	return paramN.ByteCount();	
	}

void CRSAVerifierImpl::VerifyL(const TDesC8& aInput, const CCryptoParams& aSignature, TBool& aVerificationResult)
	{
	HBufC8* output = NULL;
	InverseSignL(output, aSignature);
	CleanupStack::PushL(output);

	// is the original hash the same as the hash extracted from the signature
	aVerificationResult = EFalse;
	if (!output->Compare(aInput))
		{
		aVerificationResult = ETrue;
		}
	CleanupStack::PopAndDestroy(output);
	}

void CRSAVerifierImpl::InverseSignL(HBufC8*& aOutput, const CCryptoParams& aSignature)
	{
	// extract the original hash from the signature
	const TInteger& signature = aSignature.GetBigIntL(KRsaSignatureParameterSUid);
	RInteger output;
	RSAFunction::VerifyL(*iKey, signature, output);
	CleanupClosePushL(output);

	// format the extracted hash so it can be compared with the original hash
	HBufC8* paddedHashPtr = output.BufferLC();
	aOutput = HBufC8::NewLC(GetMaximumOutputLengthL());
	TPtr8 unpaddedHash = aOutput->Des();

	iPadding->UnPadL(*paddedHashPtr, unpaddedHash);

	CleanupStack::Pop(aOutput);
	CleanupStack::PopAndDestroy(2, &output);
	}
