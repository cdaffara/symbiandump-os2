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


#include "dsasignerimpl.h"
#include "pluginconfig.h"

using namespace SoftwareCrypto;

// Implementation of CDSASignerImpl 
CDSASignerImpl* CDSASignerImpl::NewL(const CKey& aKey)
	{
	CDSASignerImpl* self = CDSASignerImpl::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}
	
CDSASignerImpl* CDSASignerImpl::NewLC(const CKey& aKey)
	{
	CDSASignerImpl* self = new(ELeave) CDSASignerImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}

CDSASignerImpl::CDSASignerImpl() 
	{
	}
	
CDSASignerImpl::~CDSASignerImpl()
	{
	}
	
void CDSASignerImpl::ConstructL(const CKey& aKey)
	{
	CSignerImpl::ConstructL(aKey);
	}

CExtendedCharacteristics* CDSASignerImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

const CExtendedCharacteristics* CDSASignerImpl::GetExtendedCharacteristicsL()
	{
	return CDSASignerImpl::CreateExtendedCharacteristicsL();
	}

TUid CDSASignerImpl::ImplementationUid() const
	{
	return KCryptoPluginDsaSignerUid;
	}
	
void CDSASignerImpl::SetKeyL(const CKey& aPrivateKey) 
	{
	DoSetKeyL(aPrivateKey);
	Reset();
	}

TInt CDSASignerImpl::GetMaximumInputLengthL() const 
	{
	return KSha1HashLength;
	}

void CDSASignerImpl::SignL(const TDesC8& aInput, CCryptoParams& aSignature) 
	{
	
	//see HAC 11.56 or DSS section 5
	//I'll follow HAC as I like its description better
	//We don't check that r and s are non both non-null like the DSS
	//states you _optionally_ can.  The chances of this are _incredibly_ small.
	//You've got a much better chance of a bit failure ocurring in the hardware
	//than this.

	const TInteger& tQ=iKey->GetBigIntL(KDsaKeyParameterQUid);
		
	// a) Select a random secret integer (k | 0 < k < q)
	RInteger qminus1 = RInteger::NewL(tQ);
	CleanupStack::PushL(qminus1);
	--qminus1;
	RInteger k = RInteger::NewRandomL(TInteger::One(), qminus1);
	CleanupStack::PopAndDestroy(&qminus1);
	CleanupStack::PushL(k);
	
	
	// b) compute r = (g^k mod p) mod q
	
	const TInteger& tG=iKey->GetBigIntL(KDsaKeyParameterGUid);
	const TInteger& tP=iKey->GetBigIntL(KDsaKeyParameterPUid);
	RInteger r = TInteger::ModularExponentiateL(tG, k, tP);
	CleanupStack::PushL(r);
	r %=tQ;
	
	
	// c) compute k^(-1) mod q

	RInteger kinv = k.InverseModL(tQ);
	CleanupStack::PushL(kinv);


	// d) compute s = k^(-1) * {h(m) + xr} mod q
	// Note that in order to be interoperable, compliant with the DSS, and
	// secure, aInput must be the result of a SHA-1 hash

	RInteger hm = RInteger::NewL(aInput);
	CleanupStack::PushL(hm);
	
	const TInteger& tX=iKey->GetBigIntL(KDsaKeyParameterXUid);	
	RInteger s = tX.TimesL(r);
	CleanupStack::PushL(s);
	s += hm;
	s *= kinv;
	s %= tQ;


	// e) signature for m is the pair (r,s)
	aSignature.AddL(r, KDsaSignatureParameterRUid);
	aSignature.AddL(s, KDsaSignatureParameterSUid);
	
	CleanupStack::PopAndDestroy(5, &k);
	}
