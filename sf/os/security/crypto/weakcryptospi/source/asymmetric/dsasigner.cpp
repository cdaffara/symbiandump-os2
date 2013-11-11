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

EXPORT_C CDSASigner* CDSASigner::NewL(const CDSAPrivateKey& aKey)
	{
	return CDsaSignerShim::NewL(aKey);
	}

EXPORT_C CDSASigner* CDSASigner::NewLC(const CDSAPrivateKey& aKey) 
	{
	return CDsaSignerShim::NewLC(aKey);
	}

CDSASigner::CDSASigner(const CDSAPrivateKey& aKey) 
	: iPrivateKey(aKey)
	{
	}

// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
TInt CDSASigner::MaxInputLength(void) const
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return 0;
	}

CDSASignature* CDSASigner::SignL(const TDesC8& /*aInput*/) const
	{
	// Method replaced by shim
	User::Leave(KErrNotSupported);
	return 0;
	}
