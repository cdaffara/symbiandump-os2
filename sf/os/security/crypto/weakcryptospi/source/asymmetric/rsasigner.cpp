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

/* CRSASigner */

EXPORT_C CRSASigner::CRSASigner(void)
	{
	}

/* CRSAPKCS1v15Signer */
EXPORT_C CRSAPKCS1v15Signer* CRSAPKCS1v15Signer::NewL(const CRSAPrivateKey& aKey)
	{
	return CRSAPKCS1v15SignerShim::NewL(aKey);
	}

EXPORT_C CRSAPKCS1v15Signer* CRSAPKCS1v15Signer::NewLC(const CRSAPrivateKey& aKey)
	{
	return CRSAPKCS1v15SignerShim::NewLC(aKey);
	}

CRSAPKCS1v15Signer::CRSAPKCS1v15Signer(const CRSAPrivateKey& aKey)
	: iPrivateKey(aKey)
	{
	}

CRSAPKCS1v15Signer::~CRSAPKCS1v15Signer(void)
	{
	}

// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
TInt CRSAPKCS1v15Signer::MaxInputLength(void) const
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return 0;
	}

TInt CRSAPKCS1v15Signer::MaxOutputLength(void) const
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return 0;
	}

CRSASignature* CRSAPKCS1v15Signer::SignL(const TDesC8& /*aInput*/) const
	{
	// Method replaced by shim
	User::Leave(KErrNotSupported);
	return 0;
	}

void CRSAPKCS1v15Signer::ConstructL(void)
	{
	// Method replaced by shim
	User::Leave(KErrNotSupported);
	}
