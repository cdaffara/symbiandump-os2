/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <rc2.h>
#include "rc2shim.h"

CRC2::CRC2()
	{
	}

/* CRC2Encryptor */
EXPORT_C CRC2Encryptor* CRC2Encryptor::NewL(const TDesC8& aKey, TInt aEffectiveKeyLenBits)
	{
	return CRC2EncryptorShim::NewL(aKey, aEffectiveKeyLenBits);
	}

EXPORT_C CRC2Encryptor* CRC2Encryptor::NewLC(const TDesC8& aKey, TInt aEffectiveKeyLenBits)
	{
	return CRC2EncryptorShim::NewLC(aKey, aEffectiveKeyLenBits);
	}
	
CRC2Encryptor::CRC2Encryptor()
	{
	}

/* CRC2Decryptor */
EXPORT_C CRC2Decryptor* CRC2Decryptor::NewL(const TDesC8& aKey, TInt aEffectiveKeyLenBits)
	{
	return CRC2DecryptorShim::NewL(aKey, aEffectiveKeyLenBits);
	}

EXPORT_C CRC2Decryptor* CRC2Decryptor::NewLC(const TDesC8& aKey, TInt aEffectiveKeyLenBits)
	{
	return CRC2DecryptorShim::NewLC(aKey, aEffectiveKeyLenBits);
	}

CRC2Decryptor::CRC2Decryptor()
	{	
	}
	
// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

TInt CRC2::BlockSize() const
	{
	// Method replaced by shim 
	ASSERT(EFalse);			
	return 0;
	}

TInt CRC2::KeySize() const
	{
	// Method replaced by shim 
	ASSERT(EFalse);			
	return 0;
	}
	
void CRC2::Reset()
	{
	// Method replaced by shim 
	ASSERT(EFalse);			
	}	
	
void CRC2::SetKey(const TDesC8& /*aKey*/, TInt /*aEffectiveKeyLenBits*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);				
	}	

void CRC2Decryptor::Transform(TDes8& /*aBlock*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);	
	}		

void CRC2Encryptor::Transform(TDes8& /*aBlock*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);	
	}	

