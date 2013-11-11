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


#include "rsashim.h"

/* CRSAVerifier */

EXPORT_C CRSAVerifier::CRSAVerifier(void)
	{
	}

/* CRSAPKCS1v15Verifier */
EXPORT_C CRSAPKCS1v15Verifier* CRSAPKCS1v15Verifier::NewL(const CRSAPublicKey& aKey)
	{
	return CRSAPKCS1v15VerifierShim::NewL(aKey);
	}

EXPORT_C CRSAPKCS1v15Verifier* CRSAPKCS1v15Verifier::NewLC(const CRSAPublicKey& aKey)
	{
	return CRSAPKCS1v15VerifierShim::NewLC(aKey);
	}

CRSAPKCS1v15Verifier::~CRSAPKCS1v15Verifier(void)
	{
	}

CRSAPKCS1v15Verifier::CRSAPKCS1v15Verifier(const CRSAPublicKey& aKey)
	: iPublicKey(aKey)
	{
	}

// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
TInt CRSAPKCS1v15Verifier::MaxInputLength(void) const
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return 0;
	}

TInt CRSAPKCS1v15Verifier::MaxOutputLength(void) const
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return 0;
	}

HBufC8* CRSAPKCS1v15Verifier::InverseSignLC(const CRSASignature& /*aSignature*/) const
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return 0;
	}

void CRSAPKCS1v15Verifier::ConstructL(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	}

// InverseSignLC is replaced by shim. So, VerifyL also can not be used properly, hence excluded from coverage.
EXPORT_C TBool CRSAVerifier::VerifyL(const TDesC8& aInput, const CRSASignature& aSignature) const
	{
	TBool retval = EFalse;
	HBufC8* inverseSign = InverseSignLC(aSignature);
	
	if (inverseSign->Compare(aInput)==0)
		{
		retval = ETrue;
		}
	CleanupStack::PopAndDestroy(inverseSign);
	return retval;	
	}
