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

#include "sha2impl.h"

#include <cryptospi/hashplugin.h>
#include "pluginconfig.h"
#include "sha224and256impl.h"
#include "sha384and512impl.h"

using namespace SoftwareCrypto;

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

	
CSHA2Impl* CSHA2Impl::NewL(TInt32 aAlgorithmId)
	{
	CSHA2Impl* self = CSHA2Impl::NewLC(aAlgorithmId);
	CleanupStack::Pop(self);
	return self;						
	}
														
CSHA2Impl* CSHA2Impl::NewLC(TInt32 aAlgorithmId)
	{
	CSHA2Impl* self = new (ELeave) CSHA2Impl();
	CleanupStack::PushL(self);
	self->ConstructL(aAlgorithmId);
	return self;						
	}
														
void CSHA2Impl::ConstructL(const CSHA2Impl& aSHA2Impl)
	{
	iImplementationUid = aSHA2Impl.iImplementationUid;
	iInitValues = aSHA2Impl.iInitValues;
	iHashSize = aSHA2Impl.iHashSize;
    switch(iImplementationUid.iUid)
		{
		case KCryptoPluginSha224:
		case KCryptoPluginSha256:
			{
			const CSHA224And256Impl* const impl = static_cast<CSHA224And256Impl*>(aSHA2Impl.iImplementation);
			iImplementation = new (ELeave) CSHA224And256Impl(*impl);
			break;
			}
		case KCryptoPluginSha384:
		case KCryptoPluginSha512:
			{
			const CSHA384And512Impl* const impl = static_cast<CSHA384And512Impl*>(aSHA2Impl.iImplementation);
			iImplementation = new (ELeave) CSHA384And512Impl(*impl);
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	}

void CSHA2Impl::ConstructL(TInt32 aAlgorithmId)
    {
    switch(aAlgorithmId)
    	{
    	case KCryptoPluginSha224:
    		{
    		iImplementation = CSHA224And256Impl::NewL();
    		iInitValues = SHA224InitVals;
    		iImplementationUid = KCryptoPluginSha224Uid;
    		iHashSize = KSHA224HashSize;
    		break;
    		}
    	case KCryptoPluginSha256:
    		{
    		iImplementation = CSHA224And256Impl::NewL();
    		iInitValues = SHA256InitVals;
    		iImplementationUid = KCryptoPluginSha256Uid;
    		iHashSize = KSHA256HashSize;
    		break;
    		}
    	case KCryptoPluginSha384:
    		{
    		iImplementation = CSHA384And512Impl::NewL();
    		iInitValues = SHA384InitVals;
    		iImplementationUid = KCryptoPluginSha384Uid;
    		iHashSize = KSHA384HashSize;
    		break;
    		}
    	case KCryptoPluginSha512:
    		{
    		iImplementation = CSHA384And512Impl::NewL();
    		iInitValues = SHA512InitVals;
    		iImplementationUid = KCryptoPluginSha512Uid;
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

CSHA2Impl::~CSHA2Impl()
	{
	delete iImplementation;
	}

void CSHA2Impl::Reset()
	{
	iImplementation->Reset(iInitValues);
	}

void CSHA2Impl::Close()
	{
	delete this;	
	}

MHash* CSHA2Impl::ReplicateL()
	{	 
	return CSHA2Impl::NewL(iImplementationUid.iUid);
	}
	
MHash* CSHA2Impl::CopyL()
	{
	CSHA2Impl* hash = new(ELeave) CSHA2Impl();
	CleanupStack::PushL(hash);
	hash->ConstructL(*this);
	CleanupStack::Pop(hash);
	return hash;
	}

TUid CSHA2Impl::ImplementationUid()
	{
	return iImplementationUid; 
	}

void CSHA2Impl::GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics)
	{
	aPluginCharacteristics=NULL;
	TInt hashNum=sizeof(KHashCharacteristics)/sizeof(THashCharacteristics*);
	for (TInt i=0;i<hashNum;i++)
		{
		if (KHashCharacteristics[i]->cmn.iImplementationUID == ImplementationUid().iUid)
			{
			aPluginCharacteristics = KHashCharacteristics[i];
			break;
			}
		}	
	}

CExtendedCharacteristics* CSHA2Impl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

const CExtendedCharacteristics* CSHA2Impl::GetExtendedCharacteristicsL()
	{
	return CSHA2Impl::CreateExtendedCharacteristicsL();
	}	

TPtrC8 CSHA2Impl::Hash(const TDesC8& aMessage)
	{
	TPtrC8 ptr(KNullDesC8());
	iImplementation->Update(aMessage.Ptr(),aMessage.Size());
	iImplementation->StoreState();
	ptr.Set(iImplementation->Final().Ptr(), iHashSize);
	iImplementation->RestoreState();
	return ptr;
	}
	
void CSHA2Impl::Update(const TDesC8& aMessage)
	{
	iImplementation->Update(aMessage.Ptr(),aMessage.Size());	
	}
	
TPtrC8 CSHA2Impl::Final(const TDesC8& aMessage)
	{
	TPtrC8 ptr(KNullDesC8());
	if (aMessage!=KNullDesC8())
		{
		iImplementation->Update(aMessage.Ptr(),aMessage.Size());			
		}
	ptr.Set(iImplementation->Final().Ptr(), iHashSize);
	Reset();
	return ptr;
	}

void CSHA2Impl::RestoreState()
	{
	iImplementation->RestoreState();
	}

void CSHA2Impl::StoreState()
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

TAny* CSHA2Impl::GetExtension(TUid /*aExtensionId*/)
	{
	return NULL;	
	}

void CSHA2Impl::SetOperationModeL(TUid /*aOperationMode*/)
	{
	User::Leave(KErrNotSupported);
	}

void CSHA2Impl::SetKeyL(const CKey& /*aKey*/)
	{
	User::Leave(KErrNotSupported);
	}

