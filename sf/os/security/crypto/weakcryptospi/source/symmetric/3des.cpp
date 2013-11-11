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


#include "3des.h"
#include "3desshim.h"

const TInt K3DESBlockBytes = 8;
const TInt K3DESKeyBytes = 24;

C3DES::C3DES()
	{
	}

/* C3DESEncryptor */
EXPORT_C C3DESEncryptor* C3DESEncryptor::NewL(const TDesC8& aKey)
	{
	return C3DESEncryptorShim::NewL(aKey);
	}

EXPORT_C C3DESEncryptor* C3DESEncryptor::NewLC(const TDesC8& aKey)
	{
	return C3DESEncryptorShim::NewLC(aKey);
	}

/* C3DESDecryptor */
EXPORT_C C3DESDecryptor* C3DESDecryptor::NewL(const TDesC8& aKey)
	{
	return C3DESDecryptorShim::NewL(aKey);
	}

EXPORT_C C3DESDecryptor* C3DESDecryptor::NewLC(const TDesC8& aKey)
	{
	return C3DESDecryptorShim::NewLC(aKey);
	}
	
// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

void C3DES::Transform(TDes8& /*aBlock*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);		
	}
	
void C3DES::Reset()
	{
	// Method replaced by shim 
	ASSERT(EFalse);			
	}
	
TInt C3DES::BlockSize() const
	{
	// Method replaced by shim 
	ASSERT(EFalse);
	return K3DESBlockBytes;			
	}	

TInt C3DES::KeySize() const
	{
	// Method replaced by shim 
	ASSERT(EFalse);
	return K3DESKeyBytes;		
	}
	
void C3DES::DoSetKey(const TDesC8& /*aKey*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);				
	}
	
void C3DES::ConstructL(const TDesC8& /*aKey*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);			
	}	

void C3DESEncryptor::DoSetKey(const TDesC8& /*aKey*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);				
	}	

void C3DESDecryptor::DoSetKey(const TDesC8& /*aKey*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);				
	}		
