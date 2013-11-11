/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* ARC4 shim classes definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef ARC4SHIM_H
#define ARC4SHIM_H

#include <arc4.h>

namespace CryptoSpi
	{
	class CCryptoParams;
	class CSymmetricCipher;
	class CKey;
	}
	
NONSHARABLE_CLASS(CArc4Shim) : public CARC4
{
public:
	/**
	Creates a CArc4Shim object which has the same interface as CARC4 
	but delegates all work to a Crypto SPI plug-in.
	@param aKey					The key to use. aKey must be less 
								than or equal to KRC4MaxKeySizeBytes.  
	@param aDiscardBytes		The number of bytes to drop from the 
								beginning of the key stream.
	@return						A pointer to a CArc4Shim instance
	@leave KErrKeyNotWeakEnough	If the key size is larger than that 
								allowed by the cipher strength 
								restrictions of the crypto library.
								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	static CArc4Shim* NewL(const TDesC8& aKey, TUint aDiscardBytes);
	
	/**
	Creates a CArc4Shim object which has the same interface as CARC4,
	and leave it on the cleanup stack. but delegates all work to a 
	Crypto SPI plug-in.
	@param aKey					The key to use. aKey must be less 
								than or equal to KRC4MaxKeySizeBytes.  
	@param aDiscardBytes		The number of bytes to drop from the 
								beginning of the key stream.
	@return						A pointer to a CArc4Shim instance
	@leave KErrKeyNotWeakEnough	If the key size is larger than that 
								allowed by the cipher strength 
								restrictions of the crypto library.
								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	static CArc4Shim* NewLC(const TDesC8& aKey, TUint aDiscardBytes);

	/**
	Destructor
	*/
	~CArc4Shim();

	// From CSymmetricCipher class
	void Reset();
	TInt BlockSize() const;
	TInt KeySize() const;		
	void Process(const TDesC8& aInput, TDes8& aOutput);
	void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);
	TInt MaxOutputLength(TInt aInputLength) const;
	TInt MaxFinalOutputLength(TInt aInputLength) const;

private:
	/** 
	Constructor
	*/
	CArc4Shim();

	/**
	Second Phase Constructor
	*/
	void ConstructL(const TDesC8& aKey, TUint aDiscardBytes);
		
private:
	/// SPI delegate
	CryptoSpi::CSymmetricCipher* iSymmetricCipherImpl;
		
	/// SPI requires all key to passed as key-objects
	CryptoSpi::CKey* iKey;
	
	CryptoSpi::CCryptoParams* iAlgorithmParams;
};

#endif // ARC4SHIM_H
