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


#include "dsashim.h"

EXPORT_C CDSAVerifier* CDSAVerifier::NewL(const CDSAPublicKey& aKey)
	{
	return CDsaVerifierShim::NewL(aKey);
	}

EXPORT_C CDSAVerifier* CDSAVerifier::NewLC(const CDSAPublicKey& aKey)
	{
	return CDsaVerifierShim::NewLC(aKey);
	}

CDSAVerifier::CDSAVerifier(const CDSAPublicKey& aKey)  
	: iPublicKey(aKey)
	{
	}

// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
TInt CDSAVerifier::MaxInputLength(void) const
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return 0;
	}

TBool CDSAVerifier::VerifyL(const TDesC8& /*aInput*/, const CDSASignature& /*aSignature*/) const
	{
	// Method replaced by shim
	User::Leave(KErrNotSupported);
	return EFalse;
	}
