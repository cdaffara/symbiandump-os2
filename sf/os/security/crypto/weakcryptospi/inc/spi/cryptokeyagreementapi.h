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
* crypto key agreement application interface
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CRYPTOAPI_KEYAGREEMENTAPI_H__
#define __CRYPTOAPI_KEYAGREEMENTAPI_H__

#include <e32base.h>
#include <cryptospi/cryptobaseapi.h>

namespace CryptoSpi
	{
	class MKeyAgreement;
	class MAsyncKeyAgreement;
	class CKey;
	class CCryptoParams;

	/**
	Synchronous Key Agreement, which wraps a synchronous Key Agreement plugin implementation
	*/
	NONSHARABLE_CLASS(CKeyAgreement) : public CCryptoBase
		{
	public:

		/**
		 * @internalComponent
		 *
		 * Create a CKeyAgreement instance from the given MKeyAgreement instance
		 * @param aKeyAgreement	The key agreement plugin instance
		 * @return A pointer to a CKeyAgreement instance
		 */
		static CKeyAgreement* NewL(MKeyAgreement* aKeyAgreement, TInt aHandle);

		/**
		Destructor
		*/
		IMPORT_C ~CKeyAgreement();

		/**
		Set the private key for this keyagreement. Reset() is called to reinitialized the keyAgreement
		@param aSelfPrivateKey	The private key of one of the parties
		@param aParams	The parameters shared between both parties
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.		
		*/
		IMPORT_C void SetKeyL(const CKey& aSelfPrivateKey, const CCryptoParams* aParams);

		/**
		Performs the key agreement operation.
		@param aOtherPublicKey	The public key of the other party
		@param aParams	The parameters shared between both parties
		@return A pointer to a CKey instance containing the agreed key
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C CKey* AgreeL(const CKey& aOtherPublicKey, const CCryptoParams* aParams);

	private:
		/**
		Constructor
		*/
		CKeyAgreement(MKeyAgreement* aKeyAgreement, TInt aHandle);
		};

	/**
	Asynchronous Key Agreement, which wraps a asynchronous Key Agreement plugin implementation
	*/
	NONSHARABLE_CLASS(CAsyncKeyAgreement) : public CCryptoBase
		{
	public:
		/**
		 * @internalComponent
		 *
		 * Create a CKeyAgreement instance from the given MKeyAgreement instance
		 * @param aKeyAgreement The async key agreement plugin instance
		 * @return A pointer to a CKeyAgreement instance
		 */
		static CAsyncKeyAgreement* NewL(MAsyncKeyAgreement* aKeyAgreement, TInt aHandle);
		
		/**
		Destructor
		*/
		IMPORT_C ~CAsyncKeyAgreement();
		
		/**
		Set the private key for this keyagreement. Reset() is called to reinitialized the keyAgreement
		@param aSelfPrivateKey	The private key of one of the parties
		@param aParams	The parameters shared between both parties
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void SetKeyL(const CKey& aSelfPrivateKey, const CCryptoParams* aParams);
		
		
		/**
		Performs the key agreement operation.
		@param aOtherPublicKey	The public key of the other party
		@param aKey A pointer to a CKey instance containing the agreed key
		@param aParams	The parameters shared between both parties
		@param aRequestStatus
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void AgreeL(const CKey& aOtherPublicKey, CKey& aKey, const CCryptoParams* aParams, TRequestStatus& aRequestStatus);

	private:
		/**
		Constructor
		*/
		CAsyncKeyAgreement(MAsyncKeyAgreement* aKeyAgreement, TInt aHandle);
		};

	/**
	the Factory to create synchronous and asynchronous keyagreement instances
	*/
	class CKeyAgreementFactory
		{
	public:
		/**
		Creates a new instance of a key agreement system.

		@param aKeyAgreement A reference to a pointer that should be set to point to the new key agreement object.
		@param aAlgorithmUid The algorithm to use
		@param aPrivateKey The private key to combine with the other parties public key during the agreement.
		@param aAlgorithmParams The parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@return KErrNone if successful; otherwise, a system wide error code.
		*/
		IMPORT_C static void CreateKeyAgreementL(CKeyAgreement*& aKeyAgreement,
												TUid aAlgorithmUid,
												const CKey& aPrivateKey,
												const CCryptoParams* aAlgorithmParams);

		/**
		Creates a new asynchronous instance of a key agreement system.

		@param aKeyAgreement A reference to a pointer that should be set to point to the new key agreement object.
		@param aAlgorithmUid The algorithm to use
		@param aPrivateKey The private key to combine with the other parties public key during the agreement.
		@param aAlgorithmParams The parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@return KErrNone if successful; otherwise, a system wide error code.
		*/
		IMPORT_C static void CreateAsyncKeyAgreementL(CAsyncKeyAgreement*& aKeyAgreement,
													TUid aAlgorithmUid,
													const CKey& aPrivateKey,
													const CCryptoParams* aAlgorithmParams);
		};
	}

#endif //__CRYPTOAPI_KEYAGREEMENTAPI_H__
