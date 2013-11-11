/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* software SHA2 implementation
* RFC 4634 (US Secure Hash Algorithms (SHA and HMAC-SHA))
* FIPS 180-2 (With change notice)
*
*/


/**
 @file
*/

#include <hash.h>
#include "sha224and256.h"
#include "sha384and512.h"

// Initial Hash Values of SHA2 algorithms
/**
 * Initial Hash Value for SHA-224
 * 
 * These words were obtained by taking the first thirty-two bits 
 * of the fractional parts of the square roots of the first eight
 * prime numbers.
 * 
 * FIPS 180-2 Appendix
 * FIPS 180-3 Section 5.3.2
 */
const TUint SHA224InitVals[] = 
    			{
    			0xc1059ed8, // A
    			0x367cd507, // B
    			0x3070dd17, // C
    			0xf70e5939, // D
    			0xffc00b31, // E
    			0x68581511, // F
    			0x64f98fa7, // G
    			0xbefa4fa4  // H
    			};

/**
 * Initial Hash Value for SHA-256
 * 
 * These words were obtained by taking the first thirty-two bits 
 * of the fractional parts of the square roots of the first eight
 * prime numbers.
 * 
 * FIPS 180-2 Section 5.3.2
 */
const TUint SHA256InitVals[] = 
    			{
    			0x6a09e667, // A
    			0xbb67ae85, // B
    			0x3c6ef372, // C
    			0xa54ff53a, // D
    			0x510e527f, // E
    			0x9b05688c, // F
    			0x1f83d9ab, // G
    			0x5be0cd19  // H
    			};

/**
 * Initial Hash Value for SHA-384
 * 
 *  These words were obtained by taking the first sixty-four bits 
 * of the fractional parts of the square roots of the first eight
 * prime numbers.
 * 
 * FIPS 180-2 Section 5.3.3
 */
const TUint64 SHA384InitVals[] = 
    			{
    			UI64LIT(0xcbbb9d5dc1059ed8), // A
    			UI64LIT(0x629a292a367cd507), // B
    			UI64LIT(0x9159015a3070dd17), // C
    			UI64LIT(0x152fecd8f70e5939), // D
    			UI64LIT(0x67332667ffc00b31), // E
    			UI64LIT(0x8eb44a8768581511), // F
    			UI64LIT(0xdb0c2e0d64f98fa7), // G
    			UI64LIT(0x47b5481dbefa4fa4)  // H
    			};

/**
 * Initial Hash Value for SHA-512
 * 
 * These words were obtained by taking the first sixty-four bits 
 * of the fractional parts of the square roots of the first eight
 * prime numbers.
 * 
 * FIPS 180-2 Section 5.3.4
 */
const TUint64 SHA512InitVals[] = 
    			{
    			UI64LIT(0x6a09e667f3bcc908), // A
    			UI64LIT(0xbb67ae8584caa73b), // B
    			UI64LIT(0x3c6ef372fe94f82b), // C
    			UI64LIT(0xa54ff53a5f1d36f1), // D
    			UI64LIT(0x510e527fade682d1), // E
    			UI64LIT(0x9b05688c2b3e6c1f), // F
    			UI64LIT(0x1f83d9abfb41bd6b), // G
    			UI64LIT(0x5be0cd19137e2179)  // H
    			};

	
EXPORT_C CSHA2* CSHA2::NewL(TSH2Algo aAlgorithmId)
	{
	CSHA2* self = CSHA2::NewLC(aAlgorithmId);
	CleanupStack::Pop(self);
	return self;						
	}
														
EXPORT_C CSHA2* CSHA2::NewLC(TSH2Algo aAlgorithmId)
	{
	CSHA2* self = new (ELeave) CSHA2();
	CleanupStack::PushL(self);
	self->ConstructL(aAlgorithmId);
	return self;						
	}
														
void CSHA2::ConstructL(const CSHA2& aSHA2)
	{
	iAlgorithmType = aSHA2.iAlgorithmType;
	iInitValues = aSHA2.iInitValues;
	iHashSize = aSHA2.iHashSize;
    switch(iAlgorithmType)
		{
		case E224Bit:
		case E256Bit:
			{
			const CSHA224And256* const impl = static_cast<CSHA224And256*>(aSHA2.iImplementation);
			iImplementation = new (ELeave) CSHA224And256(*impl);
			break;
			}
		case E384Bit:
		case E512Bit:
			{
			const CSHA384And512* const impl = static_cast<CSHA384And512*>(aSHA2.iImplementation);
			iImplementation = new (ELeave) CSHA384And512(*impl);
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	}

void CSHA2::ConstructL(TSH2Algo aAlgorithmId)
    {
    switch(aAlgorithmId)
    	{
    	case E224Bit:
    		{
    		iImplementation = CSHA224And256::NewL();
    		iInitValues = SHA224InitVals;
    		iAlgorithmType = E224Bit;
    		iHashSize = KSHA224HashSize;
    		break;
    		}
    	case E256Bit:
    		{
    		iImplementation = CSHA224And256::NewL();
    		iInitValues = SHA256InitVals;
    		iAlgorithmType = E256Bit;
    		iHashSize = KSHA256HashSize;
    		break;
    		}
    	case E384Bit:
    		{
    		iImplementation = CSHA384And512::NewL();
    		iInitValues = SHA384InitVals;
    		iAlgorithmType = E384Bit;
    		iHashSize = KSHA384HashSize;
    		break;
    		}
    	case E512Bit:
    		{
    		iImplementation = CSHA384And512::NewL();
    		iInitValues = SHA512InitVals;
    		iAlgorithmType = E512Bit;
    		iHashSize = KSHA512HashSize;
    		break;
    		}
    	default:
    		{
    		User::Leave(KErrNotSupported);
    		}
    	}
    
    Reset();
    }

EXPORT_C CSHA2::~CSHA2()
	{
	delete iImplementation;
	}

EXPORT_C CMessageDigest* CSHA2::ReplicateL()
	{	 
	return CSHA2::NewL(iAlgorithmType);
	}
	
EXPORT_C TPtrC8 CSHA2::Hash(const TDesC8& aMessage)
	{
	TPtrC8 ptr(KNullDesC8());
	iImplementation->Update(aMessage.Ptr(),aMessage.Size());
	iImplementation->StoreState();
	ptr.Set(iImplementation->Final().Ptr(), iHashSize);
	iImplementation->RestoreState();
	return ptr;
	}
	
EXPORT_C CMessageDigest* CSHA2::CopyL()
	{
	CSHA2* hash = new(ELeave) CSHA2();
	CleanupStack::PushL(hash);
	hash->ConstructL(*this);
	CleanupStack::Pop(hash);
	return hash;
	}

EXPORT_C TInt CSHA2::BlockSize(void)
	{
	TInt blockSize = KSHA256BlockSize;
	if(E384Bit == iAlgorithmType || E512Bit == iAlgorithmType)
		{
		blockSize = KSHA512BlockSize;
		}
	return blockSize;
	}

EXPORT_C TInt CSHA2::HashSize(void)
	{
	return iHashSize;
	}

EXPORT_C void CSHA2::Reset()
	{
	iImplementation->Reset(iInitValues);
	}

EXPORT_C void CSHA2::Update(const TDesC8& aMessage)
	{
	iImplementation->Update(aMessage.Ptr(),aMessage.Size());	
	}

EXPORT_C TPtrC8 CSHA2::Final(void)
	{
	TPtrC8 ptr(KNullDesC8());
	ptr.Set(iImplementation->Final().Ptr(), iHashSize);
	Reset();
	return ptr;
	}
	
EXPORT_C TPtrC8 CSHA2::Final(const TDesC8& aMessage)
	{
	iImplementation->Update(aMessage.Ptr(),aMessage.Size());			
	TPtrC8 ptr(KNullDesC8());
	ptr.Set(iImplementation->Final().Ptr(), iHashSize);
	Reset();
	return ptr;
	}

void CSHA2::RestoreState()
	{
	iImplementation->RestoreState();
	}

void CSHA2::StoreState()
	{
	iImplementation->StoreState();
	}

// Implemented in hmacimpl.cpp or softwarehashbase.cpp
// but required as derived from MHash. No coverage here.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

