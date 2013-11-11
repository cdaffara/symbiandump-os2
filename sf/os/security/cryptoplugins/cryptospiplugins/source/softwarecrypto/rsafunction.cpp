/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <bigint.h>
#include "keys.h"
#include <asymmetrickeys.h>
#include <cryptospi/cryptospidef.h>
#include <cryptospi/cryptoparams.h>
#include "rsafunction.h"
#include "mont.h"

using namespace CryptoSpi;

// Public Encrypt
void RSAFunction::EncryptL(const CKey& aPublicKey,
	const TInteger& aInput, RInteger& aOutput)
	{
	const TInteger& N = aPublicKey.GetBigIntL(KRsaKeyParameterNUid);
	const TInteger& E = aPublicKey.GetBigIntL(KRsaKeyParameterEUid);
	FunctionL(N, E, aInput, aOutput);
	}

// Private Decrypt
void RSAFunction::DecryptL(const CKey& aPrivateKey, const TInteger& aInput, RInteger& aOutput)
	{
	if (aPrivateKey.KeyProperty().iKeyType == KRsaPrivateKeyStandardUid)
		{
		const TInteger& N = aPrivateKey.GetBigIntL(KRsaKeyParameterNUid);
		const TInteger& D = aPrivateKey.GetBigIntL(KRsaKeyParameterDUid);
		FunctionL(N, D, aInput, aOutput);
		}
	else if (aPrivateKey.KeyProperty().iKeyType == KRsaPrivateKeyCRTUid)
		{
		FunctionCRTL(aPrivateKey, aInput, aOutput);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}

// Private Encrypt
void RSAFunction::SignL(const CKey& aPrivateKey, const TInteger& aInput, RInteger& aOutput)
	{
	if (aPrivateKey.KeyProperty().iKeyType == KRsaPrivateKeyStandardUid)
		{
		const TInteger& N = aPrivateKey.GetBigIntL(KRsaKeyParameterNUid);
		const TInteger& D = aPrivateKey.GetBigIntL(KRsaKeyParameterDUid);
		FunctionL(N, D, aInput, aOutput);
		}
	else if (aPrivateKey.KeyProperty().iKeyType == KRsaPrivateKeyCRTUid)
		{
		FunctionCRTL(aPrivateKey, aInput, aOutput);
		}
	else
	{
		User::Leave(KErrNotSupported);
	}
}

// Public Decrypt
void RSAFunction::VerifyL(const CKey& aPublicKey,
	const TInteger& aInput, RInteger& aOutput)
	{
	const TInteger& N = aPublicKey.GetBigIntL(KRsaKeyParameterNUid);
	const TInteger& E = aPublicKey.GetBigIntL(KRsaKeyParameterEUid);
	FunctionL(N, E, aInput, aOutput);
	}
	
// The RSA Trapdoor Function
void RSAFunction::FunctionL(const TInteger& aModulus, const TInteger& aExponent, 
							 const TInteger& aBase, RInteger& aOutput)
	{
	IsInputValidL(aBase, aModulus);

	aOutput = TInteger::ModularExponentiateL(aBase, aExponent, aModulus);
	}

// The CRT version of the RSA Trapdoor Function
void RSAFunction::FunctionCRTL(const CKey& aPrivateKey,
								const TInteger& aInput, RInteger& aOutput)
	{
	const TInteger& N = aPrivateKey.GetBigIntL(KRsaKeyParameterNUid);
	IsInputValidL(aInput, N);

	const TInteger& P = aPrivateKey.GetBigIntL(KRsaKeyParameterPUid);
	const TInteger& Q = aPrivateKey.GetBigIntL(KRsaKeyParameterQUid);
	const TInteger& DP = aPrivateKey.GetBigIntL(KRsaKeyParameterDPUid);
	const TInteger& DQ = aPrivateKey.GetBigIntL(KRsaKeyParameterDQUid);
	const TInteger& QInv = aPrivateKey.GetBigIntL(KRsaKeyParameterQInvUid);

	CMontgomeryStructure* montP = CMontgomeryStructure::NewLC(P);
	CMontgomeryStructure* montQ = CMontgomeryStructure::NewLC(Q);
	
	// m1 = c^(dP) mod(p)
	RInteger inputReduced = aInput.ModuloL(P);
	CleanupStack::PushL(inputReduced);
	const TInteger& m1 = montP->ExponentiateL(inputReduced, DP);
	CleanupStack::PopAndDestroy(&inputReduced);

	// m2 = c^(dQ) mod(Q)
	inputReduced = aInput.ModuloL(Q);
	CleanupStack::PushL(inputReduced);
	const TInteger& m2 = montQ->ExponentiateL(inputReduced, DQ);
	CleanupStack::PopAndDestroy(&inputReduced);
	
	// Calculate CRT
	// h = (m1-m2) qInv mod(p)
	RInteger h = m1.MinusL(m2);
	CleanupStack::PushL(h);
	h *= QInv;
	h %= P;

	// m = m2 + q * h
	h *= Q;
	h += m2;

	aOutput = h;
	CleanupStack::Pop(&h);

	CleanupStack::PopAndDestroy(montQ);
	CleanupStack::PopAndDestroy(montP);
	}
