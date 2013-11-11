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
* crypto key pair generator application interface
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CRYPTOAPI_KEYPAIRGENERATORAPI_H__
#define __CRYPTOAPI_KEYPAIRGENERATORAPI_H__

#include <cryptospi/cryptobaseapi.h>

namespace CryptoSpi
	{
	class MKeyPairGenerator;
	class MAsyncKeyPairGenerator;
	class CCryptoParams;
	class CKeyPair;
	
	/**
	Synchronous Key pair generator, which wraps a synchronous Key pair generator plugin implementation
	*/
	NONSHARABLE_CLASS(CKeyPairGenerator) : public CCryptoBase
		{
	public:
		/**
		Create a CKeyPairGenerator instance from the given MKeyPairGenerator instance
		@param aKeyPairGenerator The key pair generator plugin instance
		@return A pointer to a CKeyPairGenerator instance
		*/		
		static CKeyPairGenerator* NewL(MKeyPairGenerator* aKeyPairGenerator, TInt aHandle);

		/**
		Generates a key pair
		@param aKeySize 	The algorithm-specific metric in number of bits
		@param aKeyParameters	The list of algorithm-specific parameters used by the generator to generate the key pair
		@param aKeyPair	The generated key pair
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void GenerateKeyPairL(TInt aKeySize, const CCryptoParams& aKeyParameters, CKeyPair*& aKeyPair);

		/**
		Destructor
		*/
		IMPORT_C ~CKeyPairGenerator();
	
	private:
		
		/**
		Constructor
		*/
		CKeyPairGenerator(MKeyPairGenerator* aKeyPairGenerator, TInt aHandle);
		};


	/**
	Asynchronous Key pair generator, which wraps a asynchronous Key pair generator plugin implementation
	*/
	NONSHARABLE_CLASS(CAsyncKeyPairGenerator) : public CCryptoBase
		{
	public:

		/**
		Create a CAsyncKeyPairGenerator instance from the given MAsyncKeyPairGenerator instance
		@param aKeyPairGenerator	The aync key pair generator plugin instance
		@return A pointer to a CAsyncKeyPairGenerator instance
		*/
		static CAsyncKeyPairGenerator* NewL(MAsyncKeyPairGenerator* aKeyPairGenerator, TInt aHandle);

		/**
		Destructor
		*/
		IMPORT_C ~CAsyncKeyPairGenerator();

		/**
		Generates a key pair
		@param aKeySize	The algorithm-specific metric in number of bits
		@param aKeyParameters	The list of algorithm-specific parameters used by the generator to generate the key pair
		@param aKeyPair	The generated key pair
		@param aRequestStatus
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void GenerateKeyPairL(TInt aKeySize, const CCryptoParams& aKeyParameters, CKeyPair*& aKeyPair, TRequestStatus& aRequestStatus);

		/**
		Cancel the outstanding request
		*/
		IMPORT_C void Cancel();

	private:

		/**
		Constructor
		*/
		CAsyncKeyPairGenerator(MAsyncKeyPairGenerator* aKeyPairGenerator, TInt aHandle);
		};


	/**
	the Factory to create synchronous and asynchronous Key pair generator instances
	*/
	class CKeyPairGeneratorFactory
		{
	public:

		/**
		Creates a new instance of a key pair generator.

		@param aKeyPairGenerator A reference to a pointer that should be set to point to the new asymmetric key pair generator object.
		@param aKeyAlgorithmUid	The algorithm UID
		@param aAlgorithmParams	The parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@return KErrNone if successful; otherwise, a system wide error code.
		*/
		IMPORT_C static void CreateKeyPairGeneratorL(CKeyPairGenerator*& aKeyPairGenerator,
													TUid aKeyAlgorithmUid,
													const CCryptoParams* aAlgorithmParams);

		/**
		Creates a new asynchronous instance of a key pair generator.

		@param aKeyPairGenerator A reference to a pointer that should be set to point to the new asymmetric key pair generator object.
		@param aKeyAlgorithmUid	The algorithm UID
		@param aAlgorithmParams	The parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@return KErrNone if successful; otherwise, a system wide error code.
		*/
		IMPORT_C static void CreateAsyncKeyPairGeneratorL(CAsyncKeyPairGenerator*& aKeyPairGenerator,
														TUid aKeyAlgorithmUid,
														const CCryptoParams* aAlgorithmParams);
		};
	}

#endif //__CRYPTOAPI_KEYPAIRGENERATORAPI_H__
