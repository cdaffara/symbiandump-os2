/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* crypto key pair definition - asymmetric key pair
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CRYPTOAPI_KEYPAIR_H__
#define __CRYPTOAPI_KEYPAIR_H__

#include <cryptospi/keys.h>

namespace CryptoSpi
	{
	
	/**
	The Asymmetric Key Pair Definition
	*/
	NONSHARABLE_CLASS(CKeyPair) : public CBase
		{
	public:

		/**
		Create a CKeyPair instance from a public and private key. The 
		CKeyPair takes ownership of both keys.
		@param aPublicKey	The public key in the key pair.
		@param aPrivateKey	The private key in the key pair.
		@return a pointer to a CKeyPair instance
		*/
		IMPORT_C static CKeyPair* NewL(CKey* aPublicKey, CKey* aPrivateKey);

		/**
		Retrieve the public key
		*/
		IMPORT_C const CKey& PublicKey() const;

		/**
		Retrieve the private key
		*/
		IMPORT_C const CKey& PrivateKey() const;

		~CKeyPair();

	private:
		/**
		constructor
		*/
		CKeyPair();

		/**
		2nd Phase Constructor
		*/
		void ConstructL(CKey*, CKey*);

	private:
		/**
		The public key in the key pair
		*/
		CKey* iPublicKey;

		/**
		The private key in the key pair
		*/
		CKey* iPrivateKey;
		};

	} // namespace CryptoSpi

#endif //__CRYPTOAPI_KEYPAIR_H__
