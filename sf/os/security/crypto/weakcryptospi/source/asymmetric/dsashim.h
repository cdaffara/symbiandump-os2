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
* DSA shim classes definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __DSASHIM_H__
#define __DSASHIM_H__

#include <asymmetric.h>

namespace CryptoSpi
	{
	class CSigner;
	class CVerifier;
	class CKey;
	}
	
NONSHARABLE_CLASS(CDsaSignerShim) : public CDSASigner
{
public:
	/**
	Creates an CDsaSignerShim object which has the same interface
	as CDSASigner but delegates all work to a Crypto SPI plug-in.
	
	@param aKey The signing key
	@return A pointer to a CDsaSignerShim instance
	*/
	static CDsaSignerShim* NewL(const CDSAPrivateKey& aKey);
	
	/**
	Creates an CDsaSignerShim object which has the same interface
	as CDSASigner but delegates all work to a Crypto SPI plug-in.
	
	A pointer to the new object is placed on the cleanup stack
	
	@param aKey The signing key
	@return A pointer to a CDsaSignerShim instance
	*/
	static CDsaSignerShim* NewLC(const CDSAPrivateKey& aKey);
	
	/**
	Digitally signs the specified input message 
	Note that in order to be interoperable and compliant with the DSS, aInput
	must be the result of a SHA-1 hash.

	@param aInput	A SHA-1 hash of the message to sign
	@return			A pointer to a new CSignature object
	@panic ECryptoPanicInputTooLarge	If aInput is larger than MaxInputLength(),
										which is likely to happen if the caller
										has passed in something that has not been hashed.
	*/
	virtual CDSASignature* SignL(const TDesC8& aInput) const;

	//From MSignatureSystem
	virtual TInt MaxInputLength() const;
	
	/// Destructor
	~CDsaSignerShim();
	
private:
	/// Constructor
	CDsaSignerShim(const CDSAPrivateKey& aKey);
	void ConstructL(const CDSAPrivateKey& aKey);
	
private:
	/// SPI delegate
	CryptoSpi::CSigner* iSignerImpl;
		
	/// SPI requires all key to passed as key-objects
	CryptoSpi::CKey* iKey;
};

NONSHARABLE_CLASS(CDsaVerifierShim) : public CDSAVerifier
{
public:
	/**
	Creates an CDsaVerifierShim object which has the same interface
	as CDSAVerifier but delegates all work to a Crypto SPI plug-in.
	
	@param aKey The verification key
	@return A pointer to a CDsaVerifierShim instance
	*/
	static CDsaVerifierShim* NewL(const CDSAPublicKey& aKey);
	
	/**
	Creates an CDsaVerifierShim object which has the same interface
	as CDSAVerifier but delegates all work to a Crypto SPI plug-in.

	A pointer to the new object is placed on the cleanup stack

	@param aKey The verification key
	@return A pointer to a CDsaVerifierShim instance
	*/
	static CDsaVerifierShim* NewLC(const CDSAPublicKey& aKey);

	//From CVerifier	
	virtual TBool VerifyL(const TDesC8& aInput, const CDSASignature& aSignature) const;

	//From MSignatureSystem
	virtual TInt MaxInputLength() const;

	// Destructor
	~CDsaVerifierShim();
	
private:
	// Constructor
	CDsaVerifierShim(const CDSAPublicKey& aKey);
	void ConstructL(const CDSAPublicKey& aKey);
	
private:
	/// SPI delegate
	CryptoSpi::CVerifier* iVerifierImpl;
	
	/// SPI requires all key to passed as key-objects
	CryptoSpi::CKey* iKey;
};

#endif // __DSASHIM_H__
