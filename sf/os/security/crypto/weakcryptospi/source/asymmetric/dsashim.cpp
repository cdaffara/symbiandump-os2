/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "dsashim.h"
#include <cryptospi/cryptosignatureapi.h>
#include <cryptospi/cryptospidef.h>
#include <cryptospi/keys.h>
#include "keyconverter.h"

#include "../common/inlines.h"


using namespace CryptoSpi;

// CDsaSignerShim ////////////////////////////////////////////////////////

CDsaSignerShim* CDsaSignerShim::NewL(const CDSAPrivateKey& aKey)
	{
	CDsaSignerShim* self = CDsaSignerShim::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}

CDsaSignerShim* CDsaSignerShim::NewLC(const CDSAPrivateKey& aKey)
	{
	CDsaSignerShim* self = new (ELeave) CDsaSignerShim(aKey);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}

CDsaSignerShim::CDsaSignerShim(const CDSAPrivateKey& aKey)
	: CDSASigner(aKey)
	{
	}

CDsaSignerShim::~CDsaSignerShim()
	{
	delete iSignerImpl;
	delete iKey;
	}

void CDsaSignerShim::ConstructL(const CDSAPrivateKey& aKey)
	{
	iKey = KeyConverter::CreateKeyL(aKey);
	CSignatureFactory::CreateSignerL(
									iSignerImpl,
									KDsaSignerUid,
									*iKey,
									KPaddingModeNoneUid,
									NULL);
	}


CDSASignature* CDsaSignerShim::SignL(const TDesC8& aInput) const
	{
	//Sign the input data
	CCryptoParams* signature = CCryptoParams::NewLC();
	iSignerImpl->SignL(aInput, *signature);
	
	//Retrieve the R&S in DSA signature from the array 
	const TInteger& cR=signature->GetBigIntL(KDsaSignatureParameterRUid);
	const TInteger& cS=signature->GetBigIntL(KDsaSignatureParameterSUid);
	
	
	//Make copies of the DSA signature
	RInteger r=RInteger::NewL(cR);
	CleanupClosePushL(r);
	RInteger s=RInteger::NewL(cS);
	CleanupClosePushL(s);	
	
	//Create the DSA signature object, the ownership of r&s is transfered to dsaSig
	CDSASignature* dsaSig=CDSASignature::NewL(r, s);
	
	//Cleanup
	CleanupStack::Pop(2, &r); 	
	CleanupStack::PopAndDestroy(signature);
	
	return dsaSig;
	}

TInt CDsaSignerShim::MaxInputLength() const
	{
	TInt maxInputLength=0; 
	TRAPD(err, maxInputLength=iSignerImpl->GetMaximumInputLengthL())
	if (err==KErrNone)
		{
		return maxInputLength;
		}
	else
		{
		return err;
		}
	}


// CDsaVerifierShim ////////////////////////////////////////////////////////
CDsaVerifierShim* CDsaVerifierShim::NewL(const CDSAPublicKey& aKey)
	{
	CDsaVerifierShim* self = CDsaVerifierShim::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}

CDsaVerifierShim* CDsaVerifierShim::NewLC(const CDSAPublicKey& aKey)
	{
	CDsaVerifierShim* self = new (ELeave) CDsaVerifierShim(aKey);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}

CDsaVerifierShim::CDsaVerifierShim(const CDSAPublicKey& aKey)
	: CDSAVerifier(aKey)
	{
	}

CDsaVerifierShim::~CDsaVerifierShim()
	{
	delete iVerifierImpl;
	delete iKey;
	}

void CDsaVerifierShim::ConstructL(const CDSAPublicKey& aKey)
	{
	iKey = KeyConverter::CreateKeyL(aKey);
	CSignatureFactory::CreateVerifierL(
									iVerifierImpl,
									KDsaVerifierUid,
									*iKey,
									KPaddingModeNoneUid,
									NULL);
											
	}

TBool CDsaVerifierShim::VerifyL(const TDesC8& aInput, const CDSASignature& aSignature) const
	{
	//create the array format dsa signature for the new crypto spi
	CCryptoParams* dsaSig = CCryptoParams::NewLC();

	dsaSig->AddL(aSignature.R(), KDsaSignatureParameterRUid);
	dsaSig->AddL(aSignature.S(), KDsaSignatureParameterSUid);

	//pass the signature and input to the new crypto spi to be verified
	TBool verificationResult=EFalse;
	iVerifierImpl->VerifyL(aInput, *dsaSig, verificationResult);

	//Cleanup the array
	CleanupStack::PopAndDestroy(dsaSig);

	return verificationResult;
	}

TInt CDsaVerifierShim::MaxInputLength() const
	{
	TInt maxInputLength=0; 
	TRAPD(err, maxInputLength=iVerifierImpl->GetMaximumInputLengthL())
	if (err==KErrNone)
		{
		return maxInputLength;
		}
	else
		{
		return err;
		}
	}

