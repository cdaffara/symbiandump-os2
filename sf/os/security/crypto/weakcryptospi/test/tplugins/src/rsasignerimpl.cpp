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


#include "rsasignerimpl.h"
#include "pluginconfig.h"
#include "rsafunction.h"

using namespace SoftwareCrypto;

// Implementation of CRSASignerImpl 
CRSASignerImpl* CRSASignerImpl::NewL(TUid aImplementationUid, const CKey& aKey, TUid aPaddingMode)
	{
	CRSASignerImpl* self = CRSASignerImpl::NewLC(aImplementationUid, aKey, aPaddingMode);
	CleanupStack::Pop(self);
	return self;
	}

CRSASignerImpl* CRSASignerImpl::NewLC(TUid aImplementationUid,const CKey& aKey, TUid aPaddingMode)
	{
	CRSASignerImpl* self = new(ELeave) CRSASignerImpl(aImplementationUid, aPaddingMode);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}

CRSASignerImpl::CRSASignerImpl(TUid aImplementationUid, TUid aPaddingMode) 
	: iPaddingMode(aPaddingMode), iImplementationUid(aImplementationUid)
	{
	}

CRSASignerImpl::~CRSASignerImpl()
	{
	delete iPadding;
	}

void CRSASignerImpl::ConstructL(const CKey& aKey)
	{
	CSignerImpl::ConstructL(aKey);
	SetPaddingModeL(iPaddingMode);
	}

CExtendedCharacteristics* CRSASignerImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

const CExtendedCharacteristics* CRSASignerImpl::GetExtendedCharacteristicsL()
	{
	return CRSASignerImpl::CreateExtendedCharacteristicsL();
	}

TUid CRSASignerImpl::ImplementationUid() const
	{
	return iImplementationUid;
	}

void CRSASignerImpl::SetKeyL(const CKey& aPrivateKey) 
	{
	DoSetKeyL(aPrivateKey);
	Reset();
	}

void CRSASignerImpl::SetPaddingModeL(TUid aPaddingMode)
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

TInt CRSASignerImpl::GetMaximumInputLengthL() const
	{
	return GetMaximumOutputLengthL() - iPadding->MinPaddingLength();	
	}

TInt CRSASignerImpl::GetMaximumOutputLengthL() const
	{
	const TInteger& paramN = iKey->GetBigIntL(KRsaKeyParameterNUid);
	return paramN.ByteCount();	
	}

void CRSASignerImpl::SignL(const TDesC8& aInput, CCryptoParams& aSignature) 
	{
	HBufC8* buf = HBufC8::NewLC(GetMaximumOutputLengthL());
	TPtr8 ptr = buf->Des();
	
	//The following will panic if aInput is larger than MaxOutputLength() It is
	//likely that the caller has passed in something that has not been hashed.
	//This is a programming, and likely a security error, in client code, not a
	//problem here.
	iPadding->PadL(aInput, ptr);

	RInteger input = RInteger::NewL(ptr);
	CleanupClosePushL(input);
	RInteger output;

	RSAFunction::SignL(*iKey, input, output);
	CleanupClosePushL(output);

	aSignature.AddL(output, KRsaSignatureParameterSUid);
	CleanupStack::PopAndDestroy(3, buf); //input, buf
	}
