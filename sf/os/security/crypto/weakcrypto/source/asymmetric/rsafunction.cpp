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
#include <asymmetrickeys.h>
#include "rsafunction.h"
#include "../bigint/mont.h"

//Public Encrypt
void RSAFunction::EncryptL(const CRSAPublicKey& aPublicKey, 
	const TInteger& aInput, RInteger& aOutput)
	{
	FunctionL(aPublicKey.N(), aPublicKey.E(), aInput, aOutput);
	}

//Private Decrypt
void RSAFunction::DecryptL(const CRSAPrivateKey& aPrivateKey, 
							const TInteger& aInput, RInteger& aOutput)
	{
	if (aPrivateKey.PrivateKeyType()==EStandard)
		{
		const CRSAPrivateKeyStandard* stdKey = 
			static_cast<const CRSAPrivateKeyStandard*>(&aPrivateKey);
		FunctionL(aPrivateKey.N(), stdKey->D(), aInput, aOutput);
		}
	else if (aPrivateKey.PrivateKeyType()==EStandardCRT)
		{
		FunctionCRTL(static_cast<const CRSAPrivateKeyCRT&>(aPrivateKey), aInput, 
			aOutput);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}

//Private Encrypt
void RSAFunction::SignL(const CRSAPrivateKey& aPrivateKey, 
	const TInteger& aInput, RInteger& aOutput)
	{
	if (aPrivateKey.PrivateKeyType()==EStandard)
		{
		const CRSAPrivateKeyStandard* stdKey = 
			static_cast<const CRSAPrivateKeyStandard*>(&aPrivateKey);
		FunctionL(aPrivateKey.N(), stdKey->D(), aInput, aOutput);
		}
	else if (aPrivateKey.PrivateKeyType()==EStandardCRT)
		{
		FunctionCRTL(static_cast<const CRSAPrivateKeyCRT&>(aPrivateKey), aInput, 
			aOutput);
		}
	else
	{
		User::Leave(KErrNotSupported);
	}
}

//Public Decrypt
void RSAFunction::VerifyL(const CRSAPublicKey& aPublicKey,
	const TInteger& aInput, RInteger& aOutput)
	{
	FunctionL(aPublicKey.N(), aPublicKey.E(), aInput, aOutput);
	}
	
//The RSA Trapdoor Function
void RSAFunction::FunctionL(const TInteger& aModulus, const TInteger& aExponent, 
							 const TInteger& aBase, RInteger& aOutput)
	{
	IsInputValidL(aBase, aModulus);

	aOutput = TInteger::ModularExponentiateL(aBase, aExponent, aModulus);
	}

//The CRT version of the RSA Trapdoor Function
void RSAFunction::FunctionCRTL(const CRSAPrivateKeyCRT& aPrivateKey,
								const TInteger& aInput, RInteger& aOutput)
	{
	IsInputValidL(aInput, aPrivateKey.N());

	CMontgomeryStructure* montP = CMontgomeryStructure::NewLC(aPrivateKey.P());
	CMontgomeryStructure* montQ = CMontgomeryStructure::NewLC(aPrivateKey.Q());
	
	const TInteger& p = aPrivateKey.P();
	const TInteger& q = aPrivateKey.Q();
	const TInteger& u = aPrivateKey.QInv();

	//m1 = c^(dP) mod(p)
	RInteger inputReduced = aInput.ModuloL(aPrivateKey.P());
	CleanupStack::PushL(inputReduced);
	const TInteger& m1 = montP->ExponentiateL(inputReduced, aPrivateKey.DP());
	CleanupStack::PopAndDestroy(&inputReduced);

	//m2 = c^(dQ) mod(Q)
	inputReduced = aInput.ModuloL(aPrivateKey.Q());
	CleanupStack::PushL(inputReduced);
	const TInteger& m2 = montQ->ExponentiateL(inputReduced, aPrivateKey.DQ());
	CleanupStack::PopAndDestroy(&inputReduced);
	
	//Calculate CRT
	//h = (m1-m2) qInv mod(p)
	RInteger h = m1.MinusL(m2);
	CleanupStack::PushL(h);
	h *= u;
	h %= p; 

	//m = m2 + q * h	
	h *= q;
	h += m2;

	aOutput = h;
	CleanupStack::Pop(&h);

	CleanupStack::PopAndDestroy(montQ);
	CleanupStack::PopAndDestroy(montP);
	}

