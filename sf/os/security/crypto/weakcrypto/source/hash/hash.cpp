/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include <e32std.h>
#include <hash.h>
#include "hashinc.h"
#define EXPANDLOOP

EXPORT_C CMessageDigest::CMessageDigest(void):CBase()
{}

EXPORT_C CMessageDigest::CMessageDigest(const CMessageDigest& /*aMD*/):CBase()
{}

EXPORT_C CMessageDigest::~CMessageDigest(void)
{}

/* Note that the shifts here are NOT guaranteed to work if s == 0, and it is
 * assumed in R() (and hence in all inline) that a is unsigned. */
// CMD_R - rotate a left by n bits; the main bottleneck of the algorithm 
// implementation
/*
C++ version compiles to:

CMD_R__FUiUi:
	add	r3, r0, #0
	lsl	r3, r3, r1
	mov	r2, #32
	sub	r2, r2, r1
	lsr	r0, r0, r2
	add	r3, r3, r0
	add	r0, r3, #0
	bx	lr

*/

//////////////////////////////////////////////////////////////////
//	Factory class to create CMessageDigest derived objects
//////////////////////////////////////////////////////////////////
EXPORT_C CMessageDigest* CMessageDigestFactory::NewDigestL(CMessageDigest::THashId aHashId)
{
	CMessageDigest* hash = NULL;
	switch (aHashId)
	{
	case (CMessageDigest::EMD2):
		{
			hash = CMD2::NewL();
			break;
		}
	case (CMessageDigest::EMD5):
		{
			hash = CMD5::NewL();
			break;
		}
	case (CMessageDigest::ESHA1):
		{
			hash = CSHA1::NewL();
			break;
		}
        case (CMessageDigest::EMD4):
		{
			hash = CMD4::NewL();
			break;
		}
	case (CMessageDigest::ESHA224):
		{
			hash = CSHA2::NewL(E224Bit);
			break;
		}
	case (CMessageDigest::ESHA256):
		{
			hash = CSHA2::NewL(E256Bit);
			break;
		}
	case (CMessageDigest::ESHA384):
		{
			hash = CSHA2::NewL(E384Bit);
			break;
		}
	case (CMessageDigest::ESHA512):
		{
			hash = CSHA2::NewL(E512Bit);
			break;
		}
	case (CMessageDigest::HMAC):
	default:	
		User::Leave(KErrNotSupported);
	}

	return (hash);
}

EXPORT_C CMessageDigest* CMessageDigestFactory::NewDigestLC(CMessageDigest::THashId aHashId)
{
	CMessageDigest* hash = CMessageDigestFactory::NewDigestL(aHashId);
	CleanupStack::PushL(hash);
	return (hash);
}

EXPORT_C CMessageDigest* CMessageDigestFactory::NewHMACL(CMessageDigest::THashId aHashId, const TDesC8& aKey)
{
	CMessageDigest* hash = CMessageDigestFactory::NewDigestLC(aHashId);
	CMessageDigest* hmac = CHMAC::NewL(aKey, hash);
	CleanupStack::Pop(hash);	//	Now owned by hmac
	return (hmac);
}

EXPORT_C CMessageDigest* CMessageDigestFactory::NewHMACLC(CMessageDigest::THashId aHashId, const TDesC8& aKey)
{
	CMessageDigest* hmac = CMessageDigestFactory::NewHMACL(aHashId, aKey);
	CleanupStack::PushL(hmac);
	return (hmac);
}
