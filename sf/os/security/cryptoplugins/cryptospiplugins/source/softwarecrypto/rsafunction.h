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


/**
 @file
 @internalComponent
 @released
*/

#ifndef __RSAFUNCTION_H__
#define __RSAFUNCTION_H__

#include <e32base.h>
#include "keys.h"

using namespace CryptoSpi;

class TInteger;

class RSAFunction
	{
public:
	static inline TBool IsInputValid(const TInteger& aInput, 
		const TInteger& aModulus);
	static inline void IsInputValidL(const TInteger& aInput, 
		const TInteger& aModulus);
	static void EncryptL(const CKey& aPublicKey,
		const TInteger& aInput, RInteger& aOutput);
	static void DecryptL(const CKey& aPrivateKey,
		const TInteger& aInput, RInteger& aOutput);
	static void SignL(const CKey& aPrivateKey,
		const TInteger& aInput, RInteger& aOutput);
	static void VerifyL(const CKey& aPublicKey,
		const TInteger& aInput, RInteger& aOutput);
private:
	static void FunctionL(const TInteger& aModulus, const TInteger& aExponent,
		const TInteger& aBase, RInteger& aOutput);
	static void FunctionCRTL(const CKey& aPrivateKey,
		const TInteger& aInput, RInteger& aOutput);
private:
	RSAFunction(void);
	};

/** Computes whether a given message representative is within the valid bounds
 * for a given modulus, i.e. whether the message is representative within [0,n-1].
 * @param aInput The message representative.
 * @param aModulus The modulus.
 * @return TBool representing whether or not the message representative is
 * valid.
 */
TBool RSAFunction::IsInputValid(const TInteger& aInput, 
	const TInteger& aModulus)
	{
	//See HAC 8.3 1.b
	//Message (input) must be in the interval [0,n-1] (inclusive)
	if( aInput.IsNegative() || aInput >= aModulus )
		return EFalse;
	else
		return ETrue;
	}

void RSAFunction::IsInputValidL(const TInteger& aInput,
	const TInteger& aModulus)
	{
	if(!IsInputValid(aInput, aModulus))
		User::Leave(KErrArgument);
	}

#endif

