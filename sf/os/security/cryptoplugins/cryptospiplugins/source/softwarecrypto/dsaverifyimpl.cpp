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


#include "dsaverifyimpl.h"
#include "pluginconfig.h"

using namespace SoftwareCrypto;

// Implementation of CDSAVerifierImpl
CDSAVerifierImpl* CDSAVerifierImpl::NewL(const CKey& aKey)
	{
	CDSAVerifierImpl* self = CDSAVerifierImpl::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}
	
CDSAVerifierImpl* CDSAVerifierImpl::NewLC(const CKey& aKey)
	{
	CDSAVerifierImpl* self = new(ELeave) CDSAVerifierImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}
	
CDSAVerifierImpl::CDSAVerifierImpl() 
	{
	}

CDSAVerifierImpl::~CDSAVerifierImpl()
	{
	}
	
void CDSAVerifierImpl::ConstructL(const CKey& aKey)
	{
	CVerifierImpl::ConstructL(aKey);
	}
	
CExtendedCharacteristics* CDSAVerifierImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}
	
const CExtendedCharacteristics* CDSAVerifierImpl::GetExtendedCharacteristicsL()
	{
	return CDSAVerifierImpl::CreateExtendedCharacteristicsL();
	}
	
TUid CDSAVerifierImpl::ImplementationUid() const
	{
	return KCryptoPluginDsaVerifierUid;
	}
	
void CDSAVerifierImpl::SetKeyL(const CKey& aPublicKey)
	{
	DoSetKeyL(aPublicKey);
	Reset();	
	}

TInt CDSAVerifierImpl::GetMaximumInputLengthL() const
	{
	return KSha1HashLength;
	}

void CDSAVerifierImpl::VerifyL(const TDesC8& aInput, const CCryptoParams& aSignature, TBool& aVerificationResult)
	{
	//Retrieve the parameter Q from the key	
	const TInteger& tQ=iKey->GetBigIntL(KDsaKeyParameterQUid);

	//see HAC 11.56 or DSS section 6
	//I'll follow HAC as I like the description better

	// a) Obtain A's authenticate public key

	// b) Verify that 0 < r < q and 0 < s < q; if not reject signature

	//Retrieve the R&S in DSA signature from the array

	const TInteger& tR=aSignature.GetBigIntL(KDsaSignatureParameterRUid);
	const TInteger& tS=aSignature.GetBigIntL(KDsaSignatureParameterSUid);

	if (tR <= 0 || tR >= tQ)
		{
		aVerificationResult=EFalse;
		return;
		}
	if (tS <= 0 || tS >= tQ)
		{
		aVerificationResult=EFalse;
		return;
		}
		
		
	// c) Compute w = s^(-1) mod q and h(m)
	RInteger w = tS.InverseModL(tQ);
	CleanupStack::PushL(w);
	// Note that in order to be interoperable, compliant with the DSS, and
	// secure, aInput must be the result of a SHA-1 hash
	RInteger hm = RInteger::NewL(aInput);
	CleanupStack::PushL(hm);

	// d) Compute u1 = w * hm mod q and u2 = r * w mod q
	RInteger u1 = TInteger::ModularMultiplyL(w, hm, tQ);
	CleanupStack::PushL(u1);

	RInteger u2 = TInteger::ModularMultiplyL(tR, w, tQ);
	CleanupStack::PushL(u2);

	// e) Compute v = ((g^u1 * y^u2) mod p) mod q
	
	const TInteger& tG=iKey->GetBigIntL(KDsaKeyParameterGUid);
	const TInteger& tY=iKey->GetBigIntL(KDsaKeyParameterYUid);
	const TInteger& tP=iKey->GetBigIntL(KDsaKeyParameterPUid);

	RInteger temp = TInteger::ModularExponentiateL(tG, u1, tP);
	CleanupStack::PushL(temp);
	RInteger temp1 = TInteger::ModularExponentiateL(tY, u2, tP);
	CleanupStack::PushL(temp1);
	RInteger v = TInteger::ModularMultiplyL(temp, temp1, tP);
	CleanupStack::PushL(v);
	v %= tQ;

	// f) Accept the signature if v == r
	if(v == tR)
		{
		aVerificationResult = ETrue;
		}

	CleanupStack::PopAndDestroy(7, &w);
	}


// Methods which are not supported can be excluded from the coverage.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

void CDSAVerifierImpl::InverseSignL(HBufC8*& /*aOutput*/, const CCryptoParams& /*aSignature*/)
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	}
