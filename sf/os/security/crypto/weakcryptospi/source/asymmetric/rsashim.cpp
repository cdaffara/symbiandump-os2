/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "rsashim.h"
#include <cryptospi/cryptoasymmetriccipherapi.h>
#include <cryptospi/cryptosignatureapi.h>
#include <cryptospi/cryptospidef.h>
#include "keyconverter.h"
#include <cryptospi/keys.h>

#include "../common/inlines.h"


using namespace CryptoSpi;

// CRSAPKCS1v15EncryptorShim ////////////////////////////////////////////////////////

CRSAPKCS1v15EncryptorShim* CRSAPKCS1v15EncryptorShim::NewL(const CRSAPublicKey& aKey)
	{
	CRSAPKCS1v15EncryptorShim* self = CRSAPKCS1v15EncryptorShim::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}

CRSAPKCS1v15EncryptorShim* CRSAPKCS1v15EncryptorShim::NewLC(const CRSAPublicKey& aKey)
	{
	CRSAPKCS1v15EncryptorShim* self = new (ELeave) CRSAPKCS1v15EncryptorShim(aKey);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}

CRSAPKCS1v15EncryptorShim::CRSAPKCS1v15EncryptorShim(const CRSAPublicKey& aKey)
	: CRSAPKCS1v15Encryptor(aKey)
	{
	}

CRSAPKCS1v15EncryptorShim::~CRSAPKCS1v15EncryptorShim()
	{
	delete iAsymmetricCipherImpl;
	delete iKey;
	}

void CRSAPKCS1v15EncryptorShim::ConstructL(const CRSAPublicKey& aKey)
	{
	iKey = KeyConverter::CreateKeyL(aKey);
	CAsymmetricCipherFactory::CreateAsymmetricCipherL(
												iAsymmetricCipherImpl,
												KRsaCipherUid,
												*iKey,
												KCryptoModeEncryptUid,
												KPaddingModePkcs1_v1_5_EncryptionUid,
												NULL);
	}

void CRSAPKCS1v15EncryptorShim::EncryptL(const TDesC8& aInput, TDes8& aOutput) const
	{
	iAsymmetricCipherImpl->ProcessL(aInput, aOutput);
	}

TInt CRSAPKCS1v15EncryptorShim::MaxInputLength(void) const
	{
	TInt maxInputLength=0; 
	TRAPD(err, maxInputLength=iAsymmetricCipherImpl->GetMaximumInputLengthL())
	if (err==KErrNone)
		{
		return maxInputLength;
		}
	else
		{
		return err;
		}
	}

TInt CRSAPKCS1v15EncryptorShim::MaxOutputLength(void) const
	{
	TInt maxOutputLength=0; 
	TRAPD(err, maxOutputLength=iAsymmetricCipherImpl->GetMaximumOutputLengthL())
	if (err==KErrNone)
		{
		return maxOutputLength;
		}
	else
		{
		return err;
		}
	}

// CRSAPKCS1v15DecryptorShim ////////////////////////////////////////////////////////
CRSAPKCS1v15DecryptorShim* CRSAPKCS1v15DecryptorShim::NewL(const CRSAPrivateKey& aKey)
	{
	CRSAPKCS1v15DecryptorShim* self = CRSAPKCS1v15DecryptorShim::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}


CRSAPKCS1v15DecryptorShim* CRSAPKCS1v15DecryptorShim::NewLC(const CRSAPrivateKey& aKey)
	{
	CRSAPKCS1v15DecryptorShim* self = new (ELeave) CRSAPKCS1v15DecryptorShim(aKey);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}

CRSAPKCS1v15DecryptorShim::CRSAPKCS1v15DecryptorShim(const CRSAPrivateKey& aKey)
	: CRSAPKCS1v15Decryptor(aKey)
	{
	}

CRSAPKCS1v15DecryptorShim::~CRSAPKCS1v15DecryptorShim()
	{
	delete iAsymmetricCipherImpl;
	delete iKey;
	}

void CRSAPKCS1v15DecryptorShim::ConstructL(const CRSAPrivateKey& aKey)
	{
	iKey = KeyConverter::CreateKeyL(aKey);
	CAsymmetricCipherFactory::CreateAsymmetricCipherL(
												iAsymmetricCipherImpl,
												KRsaCipherUid,
												*iKey,
												KCryptoModeDecryptUid,
												KPaddingModePkcs1_v1_5_EncryptionUid,
												NULL);
	}

void CRSAPKCS1v15DecryptorShim::DecryptL(const TDesC8& aInput, TDes8& aOutput) const
	{
	iAsymmetricCipherImpl->ProcessL(aInput, aOutput);
	}

TInt CRSAPKCS1v15DecryptorShim::MaxInputLength(void) const
	{
	TInt maxInputLength=0; 
	TRAPD(err, maxInputLength=iAsymmetricCipherImpl->GetMaximumInputLengthL())
	if (err==KErrNone)
		{
		return maxInputLength;
		}
	else
		{
		return err;
		}
	}

TInt CRSAPKCS1v15DecryptorShim::MaxOutputLength(void) const
	{
	TInt maxOutputLength=0; 
	TRAPD(err, maxOutputLength=iAsymmetricCipherImpl->GetMaximumOutputLengthL())
	if (err==KErrNone)
		{
		return maxOutputLength;
		}
	else
		{
		return err;
		}
	}

// CRSAPKCS1v15SignerShim ////////////////////////////////////////////////////////
CRSAPKCS1v15SignerShim* CRSAPKCS1v15SignerShim::NewL(const CRSAPrivateKey& aKey)
	{
	CRSAPKCS1v15SignerShim* self = CRSAPKCS1v15SignerShim::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}

CRSAPKCS1v15SignerShim* CRSAPKCS1v15SignerShim::NewLC(const CRSAPrivateKey& aKey)
	{
	CRSAPKCS1v15SignerShim* self = new (ELeave) CRSAPKCS1v15SignerShim(aKey);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}

CRSASignature* CRSAPKCS1v15SignerShim::SignL(const TDesC8& aInput) const
	{
	//Sign the input data
	CCryptoParams* signature = CCryptoParams::NewLC();
	iSignerImpl->SignL(aInput, *signature);

	//Retrieve the S in RSA signature from the array
	const TInteger& cS=signature->GetBigIntL(KRsaSignatureParameterSUid);

	//Make copies of the RSA signature
	RInteger s=RInteger::NewL(cS);
	CleanupClosePushL(s);	

	//Create the RSA signature object, the ownership of s is transfered to rsaSig
	CRSASignature* rsaSig=CRSASignature::NewL(s);

	//Cleanup
	CleanupStack::Pop(&s);
	CleanupStack::PopAndDestroy(signature);
	return rsaSig;
	}

TInt CRSAPKCS1v15SignerShim::MaxInputLength(void) const
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
	
TInt CRSAPKCS1v15SignerShim::MaxOutputLength(void) const
	{
	TInt maxOutputLength=0; 
	TRAPD(err, maxOutputLength=iSignerImpl->GetMaximumOutputLengthL())
	if (err==KErrNone)
		{
		return maxOutputLength;
		}
	else
		{
		return err;
		}
	}

CRSAPKCS1v15SignerShim::~CRSAPKCS1v15SignerShim(void)
	{
	delete iSignerImpl;
	delete iKey;
	}

CRSAPKCS1v15SignerShim::CRSAPKCS1v15SignerShim(const CRSAPrivateKey& aKey)
	: CRSAPKCS1v15Signer(aKey)
	{
	}
	
void CRSAPKCS1v15SignerShim::ConstructL(const CRSAPrivateKey& aKey)
	{
	iKey = KeyConverter::CreateKeyL(aKey);
	CSignatureFactory::CreateSignerL(
									iSignerImpl,
									KRsaSignerUid,
									*iKey,
									KPaddingModePkcs1_v1_5_SignatureUid,
									NULL);
	}

// CRSAPKCS1v15VerifierShim ////////////////////////////////////////////////////////
CRSAPKCS1v15VerifierShim* CRSAPKCS1v15VerifierShim::NewL(const CRSAPublicKey& aKey)
	{
	CRSAPKCS1v15VerifierShim* self = CRSAPKCS1v15VerifierShim::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}

CRSAPKCS1v15VerifierShim* CRSAPKCS1v15VerifierShim::NewLC(const CRSAPublicKey& aKey)
	{
	CRSAPKCS1v15VerifierShim* self = new (ELeave) CRSAPKCS1v15VerifierShim(aKey);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}
	
TBool CRSAPKCS1v15VerifierShim::VerifyL(const TDesC8& aInput, const CRSASignature& aSignature) const
	{
	//create the array format rsa signature for the new crypto spi
	CCryptoParams* rsaSig = CCryptoParams::NewLC();

	rsaSig->AddL(aSignature.S(), KRsaSignatureParameterSUid);

	//pass the signature and input to crypto spi to be verified
	TBool verificationResult = EFalse;	
	iVerifierImpl->VerifyL(aInput, *rsaSig, verificationResult);

	//Cleanup the array
	CleanupStack::PopAndDestroy(rsaSig);
	return verificationResult;
	}

HBufC8* CRSAPKCS1v15VerifierShim::InverseSignLC(const CRSASignature& aSignature) const
	{
	//create the array format rsa signature for the new crypto spi
	CCryptoParams* rsaSig = CCryptoParams::NewLC();

	rsaSig->AddL(aSignature.S(), KRsaSignatureParameterSUid);

	//pass the signature and input to crypto spi to be verified
	HBufC8* output = NULL;
	iVerifierImpl->InverseSignL(output, *rsaSig);

	//Cleanup the array
	CleanupStack::PopAndDestroy(rsaSig);

	// leave output on the cleanup stack
	CleanupStack::PushL(output);
	return output;
	}

TInt CRSAPKCS1v15VerifierShim::MaxInputLength(void) const
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
	
TInt CRSAPKCS1v15VerifierShim::MaxOutputLength(void) const
	{
	TInt maxOutputLength=0; 
	TRAPD(err, maxOutputLength=iVerifierImpl->GetMaximumOutputLengthL())
	if (err==KErrNone)
		{
		return maxOutputLength;
		}
	else
		{
		return err;
		}
	}

CRSAPKCS1v15VerifierShim::~CRSAPKCS1v15VerifierShim(void)
	{
	delete iVerifierImpl;
	delete iKey;
	}

CRSAPKCS1v15VerifierShim::CRSAPKCS1v15VerifierShim(const CRSAPublicKey& aKey)
	: CRSAPKCS1v15Verifier(aKey)
	{
	}

void CRSAPKCS1v15VerifierShim::ConstructL(const CRSAPublicKey& aKey)
	{
	iKey = KeyConverter::CreateKeyL(aKey);
	CSignatureFactory::CreateVerifierL(
									iVerifierImpl,
									KRsaVerifierUid,
									*iKey,
									KPaddingModePkcs1_v1_5_SignatureUid,
									NULL);
	}

